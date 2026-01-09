from cmsis_stream.cg.scheduler import *
import os 

from cmsis_stream.cg.yaml import *

def try_remove(path):
    if os.path.isfile(path):
        os.remove(path)
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

try_remove("StreamNode.hpp")
try_remove("GenericNodes.hpp")
try_remove("EventQueue.hpp")
try_remove("EventDisplay.hpp")
try_remove("cg_queue.hpp")
try_remove("cg_queue.cpp")
try_remove("cg_enums.h")
try_remove("posix_thread.cpp")
try_remove("posix_thread.hpp")
try_remove("cstream_node.h")
try_remove("IdentifiedNode.hpp")
try_remove("cg_pack.hpp")

try_remove("StreamNode.h")
try_remove("GenericNodes.h")
try_remove("EventQueue.h")
try_remove("cg_queue.h")
try_remove("cg_enums.h")
try_remove("posix_thread.h")
try_remove("cstream_node.h")
try_remove("IdentifiedNode.h")
try_remove("cg_pack.h")
generateGenericNodes(".")