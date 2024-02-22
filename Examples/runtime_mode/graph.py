# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
# Include definition of the nodes
from nodes import * 

NB_SOURCE = 8
NB_PROCESSING = 4
NB_OF_SRC = 16

# Define the datatype we are using for all the IOs in this
# example
floatType=CType(F32)

# Instantiate a Source node with a float datatype and
# working with packet of 5 samples (each execution of the
# source in the C code will generate 5 samples)
# "source" is the name of the C variable that will identify
# this node

the_sources=[]
for i in range(NB_OF_SRC):
   src=Source(f"source{i}",floatType,NB_SOURCE)
   src.identified=False 
   the_sources.append(src)




# Instantiate a Processing node using a float data type for
# both the input and output. The number of samples consumed
# on the input and produced on the output is 7 each time
# the node is executed in the C code
# "processing" is the name of the C variable that will identify
# this node
processing=ProcessingNode("processing",floatType,NB_PROCESSING,NB_PROCESSING,v=10)
processing.identified=True 

# Instantiate a Sink node with a float datatype and consuming
# 5 samples each time the node is executed in the C code
# "sink" is the name of the C variable that will identify
# this node
sinka=Sink("sinka",floatType,NB_SOURCE)
sinka.identified=False 
sinkb=Sink("sinkb",floatType,NB_SOURCE)
sinkb.identified=False 

# Create a Graph object
the_graph = Graph()

NB=0
def recurse(s):
    global NB
    if len(s)==2:
        NB = NB + 1
        adder=AdderNode(f"adder{NB}",floatType,NB_SOURCE)
        the_graph.connect(s[0].o,adder.ia)
        the_graph.connect(s[1].o,adder.ib)
        return(adder)
    else:
        nb=len(s) >> 1
        sa=s[:nb]
        sb=s[nb:]
        oa = recurse(sa)
        ob = recurse(sb)
        NB = NB + 1
        adder=AdderNode(f"adder{NB}",floatType,NB_SOURCE)
        the_graph.connect(oa.o,adder.ia)
        the_graph.connect(ob.o,adder.ib)
        return(adder)

res = recurse(the_sources)
        
the_graph.connect(res.o,processing.i)
the_graph.connect(processing.o,sinka.i)
the_graph.connect(processing.o,sinkb.i)


