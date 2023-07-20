import numpy as np 

from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.yaml import *


from sharedconfig import *



q15Type = CType(Q15)

### The feature computed by the graph is one second of MFCCs.
### So the nbMFCCOutputs is computed from this and with the additional
### assumption that it must be even.
### Because the MFCC slising window is sliding by half a second
### each time (half number of MFCCs)
src=WavSource("src",NBCHANNELS*AUDIO_INTERRUPT_LENGTH)
src.addLiteralArg(True)
src.addLiteralArg("test_stereo.wav")

toMono=InterleavedStereoToMono("toMono",q15Type,AUDIO_INTERRUPT_LENGTH)

slidingAudio=SlidingBuffer("audioWin",q15Type,FFTSize,AudioOverlap)
slidingMFCC=SlidingBuffer("mfccWin",q15Type,numOfDctOutputs*nbMFCCOutputs,numOfDctOutputs*nbMFCCOutputs>>1)

mfcc=MFCC("mfcc",q15Type,FFTSize,numOfDctOutputs)
mfcc.addVariableArg("mfccConfig")

sink=NumpySink("sink",q15Type,nbMFCCOutputs * numOfDctOutputs)
sink.addVariableArg("dispbuf")

g = Graph()

g.connect(src.o, toMono.i)

g.connect(toMono.o, slidingAudio.i)

g.connect(slidingAudio.o, mfcc.i)

g.connect(mfcc.o,slidingMFCC.i)
g.connect(slidingMFCC.o,sink.i)

print("Generate graphviz and code")

conf=Configuration()
#conf.dumpSchedule = True 

conf.debugLimit=12
conf.pyOptionalArgs="mfccConfig,dispbuf"

export_graph(g,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f)
    
sched = g.computeSchedule(conf)
print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)
#


sched.pythoncode(".",config=conf)

with open("test.dot","w") as f:
    sched.graphviz(f)

