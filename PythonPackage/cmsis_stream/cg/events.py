import struct 
import numpy as np
import os 
import weakref 
import mmap 

class Priority:
    kLowPriority = 0
    kNormalPriority = 1
    kHighPriority = 2
   
class DataType:
    kNone = 0
    kInt8 = 1
    kInt16 = 2
    kInt32 = 3
    kInt64 = 4
    kFloat = 5
    kDouble = 6
    kUInt8 = 7
    kUInt16 = 8
    kUInt32 = 9
    kUInt64 = 10
    kAny = 11
    kStr = 12
    kTensor = 13
    kCombined = 14

class BufferType:
    kCopyBuffer = 0
    kSharedBuffer = 1


class SharedBuffer:
    def __init__(self,fd,size,global_id,memserver):
        self.fd = fd
        self.size = size
        self.data = None
        self.global_id = global_id;
        self.memserver = memserver
        self_ref = weakref.ref(self)
        self._finalizer = weakref.finalize(self, SharedBuffer._cleanup, self_ref)

    def map(self):
        if self.data is None:
           self.data = mmap.mmap(self.fd, self.size, access=mmap.ACCESS_WRITE)

    def close(self):
        SharedBuffer._cleanup(self) 
   
   
    @staticmethod
    def _cleanup(self):
        if self.data:
            self.data.close()
            self.data = None
        if self.fd:
           os.close(self.fd)
           self.fd = -1
           if self.memserver is not None:
               if self.global_id != -1:
                  self.memserver.release(self.global_id)
           self.global_id = -1

    
class MappedArray:
    def __init__(self,shared,dims,dtype):
        self.shared = shared 
        shared.map()
        self.numpy = np.frombuffer(shared.data, dtype=dtype).reshape(dims)

    def close(self):
        self.numpy = None
        if self.shared is not None:
            self.shared.close()
            self.shared = None
        

class Event:
    def __init__(self, event_id, priority,data=None):
        self.event_id = event_id
        self.priority = priority
        self.data = data  # Placeholder for event data

    def __repr__(self):
        return f"Event(event_id={self.event_id}, priority={self.priority},data={self.data})"


class Pack:
    def __init__(self,memserver=None):
        self.data = bytearray()
        self.memserver = memserver

    def write_int8(self, value):
        self.data.extend(struct.pack('<b', value))
    def write_int16(self, value):
        self.data.extend(struct.pack('<h', value))
    def write_int32(self, value):
        self.data.extend(struct.pack('<i', value))
    def write_int64(self, value):
        self.data.extend(struct.pack('<q', value))
    
    def write_uint8(self, value):
        self.data.extend(struct.pack('<B', value))
    def write_uint16(self, value):
        self.data.extend(struct.pack('<H', value))
    def write_uint32(self, value):
        self.data.extend(struct.pack('<I', value))
    def write_uint64(self, value):
        self.data.extend(struct.pack('<Q', value))

    def write_float(self, value):
        self.data.extend(struct.pack('<f', value))
    def write_double(self, value):
        self.data.extend(struct.pack('<d', value))

    def write_array_int8(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}b", *values))
    def write_array_int16(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}h", *values))
    def write_array_int32(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}i", *values))
    def write_array_int64(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}q", *values))

    def write_array_uint8(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}B", *values))
    def write_array_uint16(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}H", *values))
    def write_array_uint32(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}I", *values))
    def write_array_uint64(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}Q", *values))
        
    def write_array_float(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}f", *values))
    def write_array_double(self, values):
        self.write_uint32(len(values))
        self.data.extend(struct.pack(f"<{len(values)}d", *values))

    @property
    def bytes(self):
        return self.data
    
    def _write_shared_buffer(self,value,network):
        self.write_uint8(DataType.kInt8)
        self.write_uint8(BufferType.kSharedBuffer)
        if value.shared.fd is not None:
            self.write_uint32(value.numpy.nbytes)
            # global ID
            self.write_int32(value.shared.global_id)
            if not network and self.memserver is not None:
               self.memserver.acquire(value.shared.global_id)
        else:
            self.write_uint32(0)
            self.write_int32(-1)
    
    def _pack(self,id,value,network):
        if value is None:
            self.write_uint8(DataType.kNone)
        elif isinstance(value, int):
                if -128 <= value <= 127:
                    self.write_uint8(DataType.kInt8)
                    self.write_int8(value)
                elif -32768 <= value <= 32767:
                    self.write_uint8(DataType.kInt16)
                    self.write_int16(value)
                elif -2147483648 <= value <= 2147483647:
                    self.write_uint8(DataType.kInt32)
                    self.write_int32(value)
                else:
                    self.write_uint8(DataType.kInt64)
                    self.write_int64(value)
        elif isinstance(value, float):
                self.write_uint8(DataType.kFloat)
                self.write_float(value)
        elif isinstance(value, str):
                self.write_uint8(DataType.kStr)
                b = value.encode("utf-8")
                self.write_array_uint8(b)
        elif isinstance(value, bytes):
                self.write_uint8(DataType.kAny)
                self.write_uint8(BufferType.kCopyBuffer)
                self.write_array_uint8(value)
        elif isinstance(value, SharedBuffer):
                self.write_uint8(DataType.kAny)
                self.write_uint8(BufferType.kSharedBuffer)
                if value.fd is not None:
                    self.write_uint32(value.size)
                    self.write_int32(value.global_id)
                    if not network and self.memserver is not None:
                       self.memserver.acquire(value.global_id)
                else:
                    self.write_uint32(0)
                    self.write_int32(-1)
        elif isinstance(value,MappedArray):
            self.write_uint8(DataType.kTensor)
            self.write_uint8(len(value.numpy.shape))
            s = list(value.numpy.shape)
            s += [0] * (4 - len(s))
            self.write_array_uint32(s)
            if value.numpy.dtype == np.int8:
                self._write_shared_buffer(DataType.kInt8,value,network)
            if value.numpy.dtype == np.int16:
                self._write_shared_buffer(DataType.kInt16,value,network)
            if value.numpy.dtype == np.int32:
                self._write_shared_buffer(DataType.kInt32,value,network)
            if value.numpy.dtype == np.int64:
                self._write_shared_buffer(DataType.kInt64,value,network)
            if value.numpy.dtype == np.float32:
                self._write_shared_buffer(DataType.kFloat,value,network)
            if value.numpy.dtype == np.float64:
                self._write_shared_buffer(DataType.kDouble,value,network)
            if value.numpy.dtype == np.uint8:
                self._write_shared_buffer(DataType.kUInt8,value,network)
            if value.numpy.dtype == np.uint16:
                self._write_shared_buffer(DataType.kUInt16,value,network)
            if value.numpy.dtype == np.uint32:
                self._write_shared_buffer(DataType.kUInt32,value,network)
            if value.numpy.dtype == np.uint64:
                self._write_shared_buffer(DataType.kUInt64,value,network)
        elif isinstance(value, np.ndarray):
                self.write_uint8(DataType.kTensor)
                self.write_uint8(len(value.shape))
                s = list(value.shape)
                s += [0] * (4 - len(s))
                self.write_array_uint32(s)
                if value.dtype == np.int8:
                    self.write_uint8(DataType.kInt8)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_int8(value.flatten())
                elif value.dtype == np.int16:
                    self.write_uint8(DataType.kInt16)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_int16(value.flatten())
                elif value.dtype == np.int32:
                    self.write_uint8(DataType.kInt32)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_int32(value.flatten())
                elif value.dtype == np.int64:
                    self.write_uint8(DataType.kInt64)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_int64(value.flatten())
                elif value.dtype == np.float32:
                    self.write_uint8(DataType.kFloat)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_float(value.flatten())
                elif value.dtype == np.float64:
                    self.write_uint8(DataType.kDouble)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_double(value.flatten())
                elif value.dtype == np.uint8:
                    self.write_uint8(DataType.kUInt8)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_uint8(value.flatten())
                elif value.dtype == np.uint16:
                    self.write_uint8(DataType.kUInt16)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_uint16(value.flatten())
                elif value.dtype == np.uint32:
                    self.write_uint8(DataType.kUInt32)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_uint32(value.flatten())
                elif value.dtype == np.uint64:
                    self.write_uint8(DataType.kUInt64)
                    self.write_uint8(BufferType.kCopyBuffer)
                    self.write_array_uint64(value.flatten())

    def pack(self,nodeid,event,network=False):
        self.write_uint32(nodeid)
        self.write_uint32(event.event_id)
        self.write_uint8(event.priority)
        if isinstance(event.data, list) and len(event.data) == 1 :
           event.data = event.data[0]
        if isinstance(event.data, list) and len(event.data) == 0:
           event.data = None
        
        if not isinstance(event.data, list):
            # Value not combined
            self.write_uint8(0)
            self._pack(0,event.data,network)
        else:
            self.write_uint8(1)
            self.write_uint32(len(event.data))
            for k,e in enumerate(event.data):
                self._pack(k,e,network)
            

class Unpack:
    def __init__(self,data,memserver=None):
        self.data = data
        self.memserver = memserver

    def read_int8(self):
        value = struct.unpack('<b', self.data[:1])[0]
        self.data = self.data[1:]
        return value
    def read_int16(self):
        value = struct.unpack('<h', self.data[:2])[0]
        self.data = self.data[2:]
        return value
    def read_int32(self):
        value = struct.unpack('<i', self.data[:4])[0]
        self.data = self.data[4:]
        return value
    def read_int64(self):
        value = struct.unpack('<q', self.data[:8])[0]
        self.data = self.data[8:]
        return value
    
    def read_uint8(self):
        value = struct.unpack('<B', self.data[:1])[0]
        self.data = self.data[1:]
        return value
    def read_uint16(self):
        value = struct.unpack('<H', self.data[:2])[0]
        self.data = self.data[2:]
        return value
    def read_uint32(self):
        value = struct.unpack('<I', self.data[:4])[0]
        self.data = self.data[4:]
        return value
    def read_uint64(self):
        value = struct.unpack('<Q', self.data[:8])[0]
        self.data = self.data[8:]
        return value
    
    def read_float(self):
        value = struct.unpack('<f', self.data[:4])[0]
        self.data = self.data[4:]
        return value
    def read_double(self):
        value = struct.unpack('<d', self.data[:8])[0]
        self.data = self.data[8:]
        return value
    
    def read_array_int8(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}b", self.data[:nb])
        self.data = self.data[nb:]
        return values
    
    def read_array_int16(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}h", self.data[:nb*2])
        self.data = self.data[nb*2:]
        return values
    
    def read_array_int32(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}i", self.data[:nb*4])
        self.data = self.data[nb*4:]
        return values
    
    def read_array_int64(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}q", self.data[:nb*8])
        self.data = self.data[nb*8:]
        return values
   
    
    def read_array_uint8(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}B", self.data[:nb])
        self.data = self.data[nb:]
        return values
    def read_array_uint16(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}H", self.data[:nb*2])
        self.data = self.data[nb*2:]
        return values
    def read_array_uint32(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}I", self.data[:nb*4])
        self.data = self.data[nb*4:]
        return values
    def read_array_uint64(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}Q", self.data[:nb*8])
        self.data = self.data[nb*8:]
        return values
    
    def read_array_float(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}f", self.data[:nb*4])
        self.data = self.data[nb*4:]
        return values
    def read_array_double(self,nb=None):
        if nb is None:
           nb = self.read_uint32()
        values = struct.unpack(f"<{nb}d", self.data[:nb*8])
        self.data = self.data[nb*8:]
        return values
    
    def _read_maybe_shared_tensor(self,dims,dtype):
        buf_type = self.read_uint8()
        if buf_type == BufferType.kCopyBuffer:
            if dtype == np.int8:
               tensor_data = self.read_array_int8()
               return np.array(tensor_data, dtype=np.int8).reshape(dims)
            if dtype == np.int16:
               tensor_data = self.read_array_int16()
               return np.array(tensor_data, dtype=np.int16).reshape(dims)
            if dtype == np.int32:
               tensor_data = self.read_array_int32()
               return np.array(tensor_data, dtype=np.int32).reshape(dims)
            if dtype == np.int64:
               tensor_data = self.read_array_int64()
               return np.array(tensor_data, dtype=np.int64).reshape(dims)
            if dtype == np.float32:
               tensor_data = self.read_array_float()
               return np.array(tensor_data, dtype=np.float32).reshape(dims)
            if dtype == np.float64:
               tensor_data = self.read_array_double()
               return np.array(tensor_data, dtype=np.float64).reshape(dims)
            if dtype == np.uint8:
               tensor_data = self.read_array_uint8()
               return np.array(tensor_data, dtype=np.uint8).reshape(dims)
            if dtype == np.uint16:
               tensor_data = self.read_array_uint16()
               return np.array(tensor_data, dtype=np.uint16).reshape(dims)
            if dtype == np.uint32:
               tensor_data = self.read_array_uint32()
               return np.array(tensor_data, dtype=np.uint32).reshape(dims)
            if dtype == np.uint64:
               tensor_data = self.read_array_uint64()
               return np.array(tensor_data, dtype=np.uint64).reshape(dims)
            return None
        else:
            str_size = self.read_uint32()
            global_id = self.read_int32()
            if self.memserver is None:
                return None
            if str_size == 0:
                return None 
            s = self.memserver.get_buffer(global_id)
            self.memserver.release(global_id)
            
            m = MappedArray(s,dims,dtype)
            return s
           
            
    def _read_maybe_shared_bytestream(self):
        buf_type = self.read_uint8()
        if buf_type == BufferType.kCopyBuffer:
           str_size = self.read_uint32()
           value = self.data[:str_size-1]
           self.data = self.data[str_size:]
           return value
        else:
           str_size = self.read_uint32()
           global_id = self.read_int32()
           if self.memserver is None:
                return None
           if str_size == 0:
                return None 
           s = self.memserver.get_buffer(global_id)
           self.memserver.release(global_id)
           return s
    
    def maybe(self,k):
        if self.data:
            if int(self.data[0]) == k:
                self.data = self.data[1:]
                return True
        return False
        
    def unpack_value(self):
        if self.maybe(DataType.kNone):
            return None
        elif self.maybe(DataType.kInt8):
            return self.read_int8()
        elif self.maybe(DataType.kInt16):
            return self.read_int16()
        elif self.maybe(DataType.kInt32):
            return self.read_int32()
        elif self.maybe(DataType.kInt64):
            return self.read_int64()
        elif self.maybe(DataType.kFloat):
            return self.read_float()
        elif self.maybe(DataType.kDouble):
            return self.read_double()
        elif self.maybe(DataType.kUInt8):
            return self.read_uint8()
        elif self.maybe(DataType.kUInt16):
            return self.read_uint16()
        elif self.maybe(DataType.kUInt32):
            return self.read_uint32()
        elif self.maybe(DataType.kUInt64):
            return self.read_uint64()
        elif self.maybe(DataType.kStr):
            str_size = self.read_uint32()
            value = self.data[:str_size-1].decode('utf-8')
            self.data = self.data[str_size:]
            return value
        elif self.maybe(DataType.kAny):
            return self._read_maybe_shared_bytestream()

        elif self.maybe(DataType.kTensor):
            nbdims = self.read_uint8()
            dims = self.read_array_uint32()
            dims = dims[:nbdims]
            
            if self.maybe(DataType.kInt8):
                return self._read_maybe_shared_tensor(dims,np.int8)
            elif self.maybe(DataType.kInt16):
                return self._read_maybe_shared_tensor(dims,np.int16)
            elif self.maybe(DataType.kInt32):
                return self._read_maybe_shared_tensor(dims,np.int32)
            elif self.maybe(DataType.kInt64):
                return self._read_maybe_shared_tensor(dims,np.int64)
            elif self.maybe(DataType.kFloat):
                return self._read_maybe_shared_tensor(dims,np.float32)
            elif self.maybe(DataType.kDouble):
                return self._read_maybe_shared_tensor(dims,np.double)
            elif self.maybe(DataType.kUInt8):
                return self._read_maybe_shared_tensor(dims,np.uint8)
            elif self.maybe(DataType.kUInt16):
                return self._read_maybe_shared_tensor(dims,np.uint16)
            elif self.maybe(DataType.kUInt32):
                return self._read_maybe_shared_tensor(dims,np.uint32)
            elif self.maybe(DataType.kUInt64):
                return self._read_maybe_shared_tensor(dims,np.uint64)
            return None

        return None 
    
    def unpack_event(self):
        nodeid, eventid, priority, datacase = struct.unpack('<IIcc', self.data[:10])
        evt = Event(eventid,priority[0])
    
        if datacase == b'\x00':
            self.data = self.data[10:]
            evt.data = self.unpack_value()
        else:
            self.data = self.data[10:]
            nb_vals = self.read_uint32()
            res = []
            for _ in range(nb_vals):
                val = self.unpack_value()
                res.append(val)
            evt.data = res
        return(nodeid,evt)


if __name__ == "__main__":
    # data 4.5 2
    combined=b'\x00\x00\x00\x00\x01\x00\x00\x00\x01\x01\x02\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x12\x40\x03\x02\x00\x00\x00'
    # data 4.5
    simple=b'\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x06\x00\x00\x00\x00\x00\x00\x12\x40'
    # simple str Hello
    simplestr=b'\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x0c\x06\x00\x00\x00\x48\x65\x6c\x6c\x6f\x00'
    # Tensor [2 3] ->  0.1 1.1 2.1 3.1 4.1 5.1
    tensor = b'\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x0d\x02\x04\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x05\x06\x00\x00\x00\xcd\xcc\xcc\x3d\xcd\xcc\x8c\x3f\x66\x66\x06\x40\x66\x66\x46\x40\x33\x33\x83\x40\x33\x33\xa3\x40'
    fromc =b'\x00\x00\x00\x00\x06\x00\x00\x00\x01\x00\x0d\x02\x04\x00\x00\x00\x05\x00\x00\x00\x02\x00\x00\x00\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x05\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x3f\x00\x00\x00\x40\x00\x00\x40\x40\x00\x00\x80\x40\x00\x00\xa0\x40\x00\x00\xc0\x40\x00\x00\xe0\x40\x00\x00\x00\x41\x00\x00\x10\x41'
    print("Unpacking")
    unpack = Unpack(fromc)
    nodeid,evt= unpack.unpack_event()
    print(nodeid,evt)

    print("\nPacking")
    pack = Pack()
    pack.pack(nodeid, evt)
    #print(pack.bytes)

    print("\nUnpacking packed data")
    new_unpack = Unpack(pack.bytes)
    new_nodeid,new_evt= new_unpack.unpack_event()
    print(new_nodeid,new_evt)