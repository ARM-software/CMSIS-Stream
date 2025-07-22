# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
# Include definition of the nodes
from nodes import * 

# Define the datatype we are using for all the IOs in this
# example
floatType=CType(F32)

def mkGraph(event_only=False):
    the_graph = Graph()

    if event_only:
        # If we only want to use events, we can use the
        # event system to connect the nodes
        src=EvtSource("source")
        sink=EvtSink("sink")
        the_graph.connect(src[0],sink[0])
    else:
        src=Source("source",floatType,5)
        processing=ProcessingNode("processing",floatType,7,7)
        sink=Sink("sink",floatType,5)
        evtsink=EvtSink("evtsink")
        the_graph.connect(src.o,processing.i)
        the_graph.connect(processing.o,sink.i)
        the_graph.connect(src[0],sink[0])
        the_graph.connect(src[0],evtsink[0])

    return the_graph

