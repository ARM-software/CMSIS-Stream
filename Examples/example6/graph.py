import numpy as np 

from cmsis_stream.cg.scheduler import *
import os 

from cmsis_stream.cg.yaml import *

def try_remove(path):
    if os.path.isfile(path):
        os.remove(path)

from sharedconfig import *



floatType = CType(F32)

### The feature computed by the graph is one second of MFCCs.
### So the nbMFCCOutputs is computed from this and with the additional
### assumption that it must be even.
### Because the MFCC slising window is sliding by half a second
### each time (half number of MFCCs)
src=FileSource("src",NBCHANNELS*AUDIO_INTERRUPT_LENGTH)
src.addLiteralArg("input_example6.txt")


slidingAudio=SlidingBuffer("audioWin",floatType,FFTSize,FFTSize>>1)
slidingMFCC=SlidingBuffer("mfccWin",floatType,2*numOfDctOutputs,numOfDctOutputs)

mfcc=MFCC("mfcc",floatType,FFTSize,numOfDctOutputs)
mfcc.addVariableArg("mfccConfig")

sink=FileSink("sink",numOfDctOutputs)
sink.addLiteralArg("output_example6.txt")

g = Graph()

g.connect(src.o, slidingAudio.i)

g.connect(slidingAudio.o, mfcc.i)

g.connect(mfcc.o,slidingMFCC.i)
g.connect(slidingMFCC.o,sink.i)

print("Generate graphviz and code")

conf=Configuration()
#conf.dumpSchedule = True 

conf.debugLimit=1
conf.cOptionalArgs="arm_mfcc_instance_f32 *mfccConfig"


export_graph(g,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f)
    
sched = g.computeSchedule(conf)
print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)
#

sched.ccode("generated",config=conf)

with open("test.dot","w") as f:
    sched.graphviz(f)

try_remove("StreamNode.hpp")
try_remove("GenericNodes.hpp")
try_remove("EventQueue.hpp")
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