# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
# Include definition of the nodes
from nodes import * 

# Replace the someInt argument by a virtual FIFO
# an connect it to a Constant node to provide the value
WITH_CONST = False

# Define the datatype we are using for all the IOs in this
# example. Use CMSIS-DSP naming (float32_t instead of float)
floatType=CType(F32,cmsis_dsp=True)

# Instantiate a Source node with a float datatype and
# working with packet of 5 samples (each execution of the
# source in the C code will generate 5 samples)
# "source" is the name of the C variable that will identify
# this node
srcA=Source("sourceA",floatType,7)
srcB=Source("sourceB",floatType,7)

if WITH_CONST:
    c = Constant("SomeConst")

# Instantiate a Processing node using a float data type for
# both the input and output. The number of samples consumed
# on the input and produced on the output is 7 each time
# the node is executed in the C code
# "processing" is the name of the C variable that will identify
# this node
processing=MyFunction(7,7,5,with_const = WITH_CONST )
# Instantiate a Sink node with a float datatype and consuming
# 5 samples each time the node is executed in the C code
# "sink" is the name of the C variable that will identify
# this node
sink=Sink("sink",CType(Q15),5)

# Create a Graph object
the_graph = Graph()

# Connect the source to the processing node
the_graph.connect(srcA.o,processing.ia)
the_graph.connect(srcB.o,processing.ib)

if WITH_CONST:
   # Connect a constant node to the port ic
   # to transform it into a virtual FIFO
   # The value of the corresponding argument
   # will be the value of the constant node
   the_graph.connect(c,processing.ic)

# Connect the processing node to the sink
the_graph.connect(processing.o,sink.i)


