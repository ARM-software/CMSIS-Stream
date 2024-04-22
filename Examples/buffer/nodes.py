# Include definitions from the Python package
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource

### Define new types of Nodes 

class ProcessingNode(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ProcessingNode"

# Compatible constraint with src
class ProcessingNodeCC(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)
        self.i.setBufferConstraint(name="Test",mustBeArray=True,assignedByNode=False)


    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ProcessingNodeCC"


# Incompatible constraint with src
class ProcessingNodeIC(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)
        self.i.setBufferConstraint(name="Test2",mustBeArray=True,assignedByNode=False)


    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ProcessingNodeIC"

class Sink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addLiteralArg(name)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Sink"

class Source(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Source"

# Impose constraint with external buffer
class SourceC1(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)
        self.o.setBufferConstraint(name="Test",mustBeArray=True,assignedByNode=False)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "SourceC1"

# Impose constraint with internal buffer
class SourceC2(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)
        self.o.setBufferConstraint(name="Test",mustBeArray=True)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "SourceC2"

# External array can be used as fifo
class SourceC3(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)
        self.o.setBufferConstraint(name="Test",mustBeArray=False,assignedByNode=False)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "SourceC3"