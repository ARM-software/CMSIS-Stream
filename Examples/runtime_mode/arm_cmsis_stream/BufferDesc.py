# automatically generated by the FlatBuffers compiler, do not modify

# namespace: arm_cmsis_stream

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class BufferDesc(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAs(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = BufferDesc()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def GetRootAsBufferDesc(cls, buf, offset=0):
        """This method is deprecated. Please switch to GetRootAs."""
        return cls.GetRootAs(buf, offset)
    # BufferDesc
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # BufferDesc
    def Length(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint32Flags, o + self._tab.Pos)
        return 0

def BufferDescStart(builder): builder.StartObject(1)
def Start(builder):
    return BufferDescStart(builder)
def BufferDescAddLength(builder, length): builder.PrependUint32Slot(0, length, 0)
def AddLength(builder, length):
    return BufferDescAddLength(builder, length)
def BufferDescEnd(builder): return builder.EndObject()
def End(builder):
    return BufferDescEnd(builder)