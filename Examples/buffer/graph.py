# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
# Include definition of the nodes
from nodes import * 

# In TEST, duplicate is sharing all output buffers with input buffers
# Initialization code in CPP will be:
# Duplicate<float,5,float,5> dup0(fifo3,{});
TEST1 = 1
# Test2 : One output FIFO of duplicate is assigned to a specific buffer
# so duplicate must copy to this FIFO 
# Initialization code in CPP will be:
# Duplicate<float,5,float,5> dup0(fifo3,{&fifo5});
TEST2 = 2
# Test3: One output FIFO is a real FIFO and is not used as an array
# We must copy to this FIFO 
# Initialization code in CPP will be:
# Duplicate<float,5,float,5> dup0(fifo3,{&fifo4}); 
TEST3 = 3 

TEST = TEST1
# Define the datatype we are using for all the IOs in this
# example
floatType=CType(F32)

# Instantiate a Source node with a float datatype and
# working with packet of 5 samples (each execution of the
# source in the C code will generate 5 samples)
# "source" is the name of the C variable that will identify
# this node
src=Source("source",floatType,5)
# Instantiate a Processing node using a float data type for
# both the input and output. The number of samples consumed
# on the input and produced on the output is 7 each time
# the node is executed in the C code
# "processing" is the name of the C variable that will identify
# this node
processing1=ProcessingNode("processing1",floatType,5,5)
processing2=ProcessingNode("processing2",floatType,5,5)
processing3=ProcessingNode("processing3",floatType,5,5)

# Instantiate a Sink node with a float datatype and consuming
# 5 samples each time the node is executed in the C code
# "sink" is the name of the C variable that will identify
# this node
sink1=Sink("sink1",floatType,5)

if TEST == TEST3:
   sink2=Sink("sink2",floatType,7)
else:
   sink2=Sink("sink2",floatType,5)

# Create a Graph object
the_graph = Graph()

# Connect the source to the processing node
the_graph.connect(src.o,processing1.i,buffer="myBuffer")

the_graph.connect(processing1.o,sink2.i)
    
if TEST == TEST2:
    the_graph.connect(processing1.o,processing2.i,buffer="myBufferB")
else:
    the_graph.connect(processing1.o,processing2.i)
the_graph.connect(processing2.o,processing3.i)
# Connect the processing node to the sink
the_graph.connect(processing3.o,sink1.i)


