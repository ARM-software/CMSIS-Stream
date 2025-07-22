# Include definitions from the Python package
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource

### Define new types of Nodes 

class ProcessingNode(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name,selectors=["reset","increment"])
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ProcessingNode"

class Sink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name,selectors=["increment"])
        self.addInput("i",theType,inLength)
        self.addEventInput()

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Sink"

class Source(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name,selectors=["increment"])
        self.addOutput("o",theType,outLength)
        self.addEventOutput()

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Source"


# Event only nodes with no data processing 

class EvtSource(GenericSource):
    def __init__(self,name):
        GenericSource.__init__(self,name,selectors=["increment"])
        self.addEventOutput()

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "EvtSource"
    
class EvtSink(GenericSink):
    def __init__(self,name):
        GenericSink.__init__(self,name,selectors=["increment","value"])
        self.addEventInput()

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "EvtSink"