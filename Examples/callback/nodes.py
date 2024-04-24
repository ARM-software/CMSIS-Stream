# Include definitions from the Python package
from cmsis_stream.cg.scheduler import F32,CType,ArgLength,GenericFunction,GenericNode,GenericSink,GenericSource

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

class Sink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

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

# Node for following C function
# void myfunc(float *       i0,
#             int           nb_samples0,
#             custom_type_t testVar,
#             float *       i1,
#             int           nb_bytes1,
#             float *       o,
#             int           nb_samples2,
#             int           someInt,
#             const char*   someStr);
#
# There are two ways to describe the someInt
# argument.
# It can be described as an additional argument
# using the addLiteralArg
# It can be described as a virtual FIFO connected
# to a constant node in the graph. In that
# case it will appear in the graphical representation
# but the generated code will be the same
class MyFunction(GenericFunction):
    def __init__(self, l):
        GenericFunction.__init__(self,"myfunc",
            ["i"
            ,"o"
            ,ArgLength("o")
            ])

        self.addInput("i",CType(F32),l)
        self.addOutput("o",CType(F32),l)