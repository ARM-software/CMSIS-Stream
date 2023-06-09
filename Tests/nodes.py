# Include definitions from the Python package
from cmsis_stream.cg.scheduler import *

### Define new types of Nodes 

class ProcessingNode(GenericNode):
    """
    Definition of a ProcessingNode for the graph

    Parameters
    ----------
    name : str
         Name of the C variable identifying this node 
         in the C code
    theType : CGStaticType
            The datatype for the input and output
    inLength : int
             The number of samples consumed by input
    outLength : int 
              The number of samples produced on output
    """
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ProcessingNode"

class Sink(GenericSink):
    """
    Definition of a Sink node for the graph

    Parameters
    ----------
    name : str
         Name of the C variable identifying this node 
         in the C code
    theType : CGStaticType
            The datatype for the input
    inLength : int
             The number of samples consumed by input
    """
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Sink"

class Source(GenericSource):
    """
    Definition of a Source node for the graph

    Parameters
    ----------
    name : str
         Name of the C variable identifying this node 
         in the C code
    theType : CGStaticType
            The datatype for the output
    outLength : int 
              The number of samples produced on output
    """
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Source"

class ProcessingNodeA(GenericManyToManyNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericManyToManyNode.__init__(self,name)
        self.addManyInput(theType,inLength,3)
        self.addManyOutput(theType,outLength,2)


    @property
    def typeName(self):
        return "ProcessingNodeA"

class ProcessingNodeB(GenericToManyNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericToManyNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addManyOutput(theType,outLength,2)


    @property
    def typeName(self):
        return "ProcessingNodeB"

class ProcessingNodeC(GenericFromManyNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericFromManyNode.__init__(self,name)
        self.addManyInput(theType,inLength,3)
        self.addOutput("o",theType,outLength)


    @property
    def typeName(self):
        return "ProcessingNodeC"