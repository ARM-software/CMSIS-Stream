from cmsis_stream.cg.scheduler import *

class ProcessingNode(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "ProcessingNode"

class Sink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        return "Sink"

class Source(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "Source"

# USe float instead of float32_t type name
floatType=CType(F32,cmsis_dsp=False)

src=Source("source",floatType,5)
processing=ProcessingNode("processing",floatType,7,7)
sink=Sink("sink",floatType,5)

the_graph = Graph()

the_graph.connect(src.o,processing.i)
the_graph.connect(processing.o,sink.i)

# Create a configuration object
conf=Configuration()

conf.debugLimit=1

conf.CMSISDSP = False
conf.asynchronous = False

scheduling = the_graph.computeSchedule(config=conf)

# Print some statistics about the compute schedule
# and the memory usage
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)

scheduling.ccode(".",conf)

# GenericNodes.h is created in the folder "generated"
generateGenericNodes(".")

# cg_status.h is created in the folder "generated"
generateCGStatus(".")

# Generate a graphviz representation of the graph
with open("simple.dot","w") as f:
    scheduling.graphviz(f)
