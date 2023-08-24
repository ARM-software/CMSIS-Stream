from cmsis_stream.cg.scheduler import *
from copy import deepcopy 

class InPlace(GenericNode):
    def __init__(self,name,theType):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,1)
        self.addOutput("o",theType,1)

    @property
    def typeName(self):
        return "InPlace"

class BufferCopy(GenericNode):
    def __init__(self,name,theType):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,1)
        self.addOutput("o",theType.unique(),1)

    @property
    def typeName(self):
        return "BufferCopy"

class Processing(GenericNode):
    def __init__(self,name,theType,on_shared=False):
        GenericNode.__init__(self,name)
        self.addInput("b",theType,1)
        if on_shared:
           self.addInput("i",theType.share(),1)
        else:
           self.addInput("i",theType,1)
        self.addOutput("o",theType,1)

    @property
    def typeName(self):
        return "Processing"

class BufferSink(GenericSink):
    def __init__(self,name,theType):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,1)

    @property
    def typeName(self):
        return "BufferSink"

class BufferSource(GenericSource):
    def __init__(self,name,theType):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,1)

    @property
    def typeName(self):
        return "BufferSource"



def mkGraph(the_type):
    buf1=BufferSource("buf1",the_type)
    buf2=BufferSource("buf2",the_type)
    processing=Processing("processing",the_type,on_shared=True)
    bufCopy=BufferCopy("bufCopy",the_type.share())
    inplace=InPlace("inplace",the_type)

    sinkA=BufferSink("sinkA",the_type)
    sinkB=BufferSink("sinkB",the_type)

    g = Graph()

    g.connect(buf1.o,processing.i)
    g.connect(buf2.o,processing.b)
    g.connect(buf1.o,bufCopy.i)
    g.connect(bufCopy.o,inplace.i)

    g.connect(processing.o,sinkA.i)
    g.connect(inplace.o,sinkB.i)
    return(g)
