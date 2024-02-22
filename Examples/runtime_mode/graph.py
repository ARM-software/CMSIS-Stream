# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
# Include definition of the nodes
from nodes import * 

NBA = 5
NBB = 7

# Define the datatype we are using for all the IOs in this
# example
floatType=CType(F32)

# Instantiate a Source node with a float datatype and
# working with packet of 5 samples (each execution of the
# source in the C code will generate 5 samples)
# "source" is the name of the C variable that will identify
# this node
src=Source("source",floatType,NBA)
src.identified=False 
# Instantiate a Processing node using a float data type for
# both the input and output. The number of samples consumed
# on the input and produced on the output is 7 each time
# the node is executed in the C code
# "processing" is the name of the C variable that will identify
# this node
processinga=ProcessingNode("processinga",floatType,NBB,NBB,v=10)
processinga.identified=False 

# Instantiate a Sink node with a float datatype and consuming
# 5 samples each time the node is executed in the C code
# "sink" is the name of the C variable that will identify
# this node
sinka=Sink("sinka",floatType,NBA)
sinka.identified=False 
sinkb=Sink("sinkb",floatType,NBA)
sinkb.identified=False 

# Create a Graph object
the_graph = Graph()

# Connect the source to the processing node
the_graph.connect(src.o,processinga.i)
# Connect the processing node to the sink
the_graph.connect(processinga.o,sinka.i)

def crazy(nb,r):
    for i in range(nb):
        processingb=ProcessingNode(f"processing{i+1}",floatType,NBB,NBB)
        processingb.identified=False 
        the_graph.connect(r.o,processingb.i)
        r = processingb
    return(r)


processingb = crazy(10,processinga)

processinga.identified=True 
processingb.identified=True 

the_graph.connect(processingb.o,sinkb.i)


