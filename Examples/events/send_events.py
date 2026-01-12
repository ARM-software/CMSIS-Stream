import socket
import struct
import json
import time
import numpy as np
from cmsis_stream.cg.events import *

class EventID:
      kNoEvent = 0
      kDo = 1
      kPause = 2
      kResume = 3
      kGetParam = 5
      kValue = 6
      kStopGraph = 7
      kDebug = 8
      kStartNodeInitialization = 9   
      kNodeWasInitialized = 10  
      kNodeWillBeDestroyed = 11
      kNodeReadyToBeDestroyed = 12
      kError = 13
      kLongRun = 14      
      kSelector = 100


with open("scheduler_ident.json", "r") as f:
    ident = json.load(f)

with open("scheduler_selectors.json", "r") as f:
    selectors = json.load(f)

print(selectors)
# Default selectors


HOST = 'localhost'  # Server's IP address
PORT = 8100         # The same port as used by the server



def sendMsg(s, id,selector,data):
    if isinstance(id, str):
       id = ident[id]
    
    if isinstance(selector, str):
       selector = selectors[selector]

    
    evt = Event(selector, 1,data)
    pack = Pack()
    pack.pack(id, evt)
    
    nv = struct.pack("<I", len(pack.bytes))
    data = nv + pack.bytes
    s.sendall(data)


def read_msg(b):
    #print(b)
    unpack = Unpack(b)
    nodeid,evt= unpack.unpack_event()
    return (nodeid,evt)


class StreamNode:
    def __init__(self, name):
        self.name = name

class StreamNetwork:
    pass

def make_selector(action):
    def method(self,s,*args):
        #print(f"{self.name}.{action}{list(args)}")
        sendMsg(s, ident[self.name],selectors[action],list(args))
    return method



for n in selectors:
    setattr(StreamNode,n,make_selector(n))

for n in ident: 
    setattr(StreamNetwork,n,StreamNode(n))



#StreamNetwork.sink.increment(1,2)
#StreamNetwork.source.increment(3,4,5)
#StreamNetwork.processing.reset(6,7)

#exit(0)

def recv_messages(sock):
    """
    Generator that yields complete messages from a socket.
    Each message starts with a 4-byte unsigned int (big-endian)
    indicating the length of the message payload.
    """
    buffer = bytearray()
    msg_len = None

    while True:
        try:
            data = sock.recv(4096)
            #print(f"Received data:{len(data)} bytes")
        except socket.error:
            raise RuntimeError("Socket error or connection closed")

        if not data:
            print("Connection closed by the server.")
            break  # connection closed
        buffer.extend(data)

        while True:
            if msg_len is None:
                if len(buffer) >= 4:
                    msg_len = struct.unpack("<I", buffer[:4])[0]
                    #print("Message length:", msg_len)
                    buffer = buffer[4:]
                else:
                    break  # not enough data for message length

            if len(buffer) >= msg_len:
                msg = bytes(buffer[:msg_len])
                buffer = buffer[msg_len:]
                msg_len = None
                yield msg
            else:
                break  # not enough data for full message yet

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    gen = recv_messages(s)
    nb = 0
    #s.sendall(b'Hello from Python client!')
    while True:
          StreamNetwork.sink.do(s)
          client_command = next(gen)
          print(read_msg(client_command))
         
          StreamNetwork.sink.value(s,nb)
          nb = nb + 1
          client_command = next(gen)
          print(read_msg(client_command))
    #sendStop(s,"sink")
    #print("Message sent to server.")

