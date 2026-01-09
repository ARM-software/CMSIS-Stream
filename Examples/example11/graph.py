from cmsis_stream.cg.scheduler import *
import os 

from cmsis_stream.cg.yaml import *

def try_remove(path):
    if os.path.isfile(path):
        os.remove(path)
### Define new types of Nodes 



class SinkAsync(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        return "SinkAsync"

class SourceOdd(GenericSource):
    def __init__(self,name,theType,inLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "SourceOdd"

class SourceEven(GenericSource):
    def __init__(self,name,theType,inLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "SourceEven"

class ProcessingOddEven(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("ia",theType,inLength)
        self.addInput("ib",theType,inLength)
        self.addInput("ic",theType,outLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "ProcessingOddEven"



### Define nodes
dataType=CType(SINT16)
odd=SourceOdd("sourceOdd",dataType,1)
even=SourceEven("sourceEven",dataType,1)

proc=ProcessingOddEven("proc",dataType,1,1)
comp=Unary("compute",dataType,1)

sinka=SinkAsync("sinka",dataType,1)
sinkb=SinkAsync("sinkb",dataType,1)

debug=NullSink("debug",dataType,1)

NBDEBUG_SINK = 4
debugSinks=[]
for i in range(NBDEBUG_SINK):
    debugSinks.append(NullSink(f"debug{i}",dataType,1))

g = Graph()

# Option to customize the default class
# to use for Duplicate and FIFO
# FIFO class can also be changed in the connect
# function to change the class for a specific
# connection
g.defaultFIFOClass = StreamFIFO
g.duplicateNodeClassName = "Duplicate"

g.connect(odd.o,proc.ia,fifoAsyncLength=1)
g.connect(even.o,proc.ib,fifoAsyncLength=1)
# Just for checking duplicate nodes
# with scaling factor are working.
# In practice, all edge of a duplicate nodes
# should have same FIFO size
g.connect(odd.o,debug.i,fifoAsyncLength=2)

g.connect(proc.o,comp.i,fifoAsyncLength=2)

g.connect(comp.o,proc.ic,fifoAsyncLength=2,weak=True)


g.connect(comp.o,sinka.i,fifoAsyncLength=2)
g.connect(comp.o,sinkb.i,fifoAsyncLength=2)
for i in range(NBDEBUG_SINK):
    g.connect(comp.o,debugSinks[i].i,fifoAsyncLength=2)


print("Generate graphviz and code")

conf=Configuration()
conf.debugLimit=10
conf.cOptionalArgs=[]
conf.CMSISDSP = False 

conf.heapAllocation = False

#conf.switchCase = False

# Asynchronous mode enable. It implies
# switchCase  true
conf.fullyAsynchronous = True 


#conf.displayFIFOSizes=True
# Prefix for global FIFO buffers
#conf.prefix="sched1"

#conf.dumpSchedule = True 

export_graph(g,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f)
    
sched = g.computeSchedule(config=conf)
#print(sched.schedule)
print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)
#


#conf.postCustomCName = "post.h"
#conf.CAPI = True
#conf.prefix="global"
#conf.dumpFIFO = True
#conf.CMSISDSP = False
#conf.switchCase = False

# For pure functions (like CMSIS-DSP) which are not
# packaged in a C++ class, we have to decide of the
# asynchronous policy. What must be done in case of
# FIFO overflow or underflow.
# By default, the execution is skipped.
conf.asyncDefaultSkip = True 

sched.ccode("./generated",conf)

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