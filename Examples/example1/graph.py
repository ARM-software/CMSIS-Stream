from cmsis_stream.cg.scheduler import *
import os 

from cmsis_stream.cg.yaml import *

### Define new types of Nodes 

class Node(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

class Sink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        return "Sink"

class Source(GenericSource):
    def __init__(self,name,theType,inLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "Source"

class ProcessingNode(Node):
    @property
    def typeName(self):
        return "ProcessingNode"



### Define nodes
floatType=CType(F32)
src=Source("source",floatType,5)
processing=ProcessingNode("processing",floatType,7,5)
processing.addLiteralArg(4,"testString")
processing.addVariableArg("someVariable")
sink=Sink("sink",floatType,5)

g = Graph()

g.connect(src.o,processing.i)
g.connect(processing.o,sink.i)

print("Generate graphviz and code")

conf=Configuration()
conf.debugLimit=1
conf.cOptionalInitArgs=["int someVariable"]
conf.cOptionalExecutionArgs=["int someVariable"]
export_graph(g,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f)

sched = g.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode("generated",conf)

with open("test.dot","w") as f:
    sched.graphviz(f)

