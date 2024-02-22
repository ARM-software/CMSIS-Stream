# Include definitions from the Python package
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource

import struct

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
    def __init__(self,name,theType,inLength,outLength,v=1):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)
        self._v = v

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ProcessingNode"

    @property
    def uuid(self):
        return "3ff62b0c9ad8445dbbe9208d87423446"

    @property
    def node_data(self):
        return(struct.pack('<i', self._v))
    
class AdderNode(GenericNode):
    def __init__(self,name,theType,ioLength):
        GenericNode.__init__(self,name)
        self.addInput("ia",theType,ioLength)
        self.addInput("ib",theType,ioLength)
        self.addOutput("o",theType,ioLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Adder"

    @property
    def uuid(self):
        return "6a73381ccd114f13ba9634757c2c4a59"

    @property
    def node_data(self):
        return None

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

    @property
    def uuid(self):
        return "c30ea9eae9c34638bbc6021fa3549d93"

    @property
    def node_data(self):
        return None

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

    @property
    def uuid(self):
        return "c0089f592f334ec4902330f69f0f4833"

    @property
    def node_data(self):
        return None

