# Include definitions from the Python package
from cmsis_stream.cg.scheduler import *

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
    def __init__(self, length_a
                     , length_b
                     , length_c
                     , with_const = False):

        if with_const:
            # With constant node, the someInt is
            # connected to a FIFO through a new port
            # ic
            # Note that there is no ArgLength argument
            # for this FIFO in the corresponding
            # C API description because it will be connected
            # to a constant node and the FIFO won't
            # really exist in the generated code
            someInt = "ic"
            someStr = 1
        else:
            # Otherwise someInt is the second additional
            # argument
            someInt = 1
            someStr = 2
        GenericFunction.__init__(self,"myfunc",
            ["ia"
            ,ArgLength("ia")
            , 0
            ,"ib"
            ,ArgLength("ib",sample_unit=False)
            ,"o"
            ,ArgLength("o")
            ,someInt,someStr
            ])


        self.addInput("ia",CType(F32),length_a)
        self.addInput("ib",CType(F32),length_b)
        if with_const:
            # Create an input port for the someInt
            # argument. This port will be connected
            # to a constant node in the graph
            self.addInput("ic",CType(SINT32),length_b)
        self.addOutput("o",CType(Q15),length_c)

        self.addVariableArg("testVar")
        if with_const:
           self.addLiteralArg("5")
        else:
           self.addLiteralArg(4,"5")