import numpy as np 

from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.yaml import *


FS=16000
# You can try with 120
AUDIO_INTERRUPT_LENGTH = 192
WINSIZE=256
floatType=CType(F32,cmsis_dsp=True)


### Define nodes
src=FileSource("src",AUDIO_INTERRUPT_LENGTH)
src.addLiteralArg("input_example3.txt")
sliding=SlidingBuffer("audioWin",floatType,256,128)
overlap=OverlapAdd("audioOverlap",floatType,256,128)
window=Binary("arm_mult_f32",floatType,WINSIZE)

toCmplx=ToComplex("toCmplx",floatType,WINSIZE)
toReal=ToReal("toReal",floatType,WINSIZE)
fft=CFFT("cfft",floatType,WINSIZE)
ifft=ICFFT("icfft",floatType,WINSIZE)

hann=Constant("HANN")
sink=FileSink("sink",AUDIO_INTERRUPT_LENGTH)
sink.addLiteralArg("output_example3.txt")

g = Graph()

g.connect(src.o, sliding.i)

# Windowing
g.connect(sliding.o, window.ia)
g.connect(hann,window.ib)

# FFT
g.connect(window.o,toCmplx.i)
g.connect(toCmplx.o,fft.i)
g.connect(fft.o,ifft.i)
g.connect(ifft.o,toReal.i)


# Overlap add
g.connect(toReal.o,overlap.i)
g.connect(overlap.o,sink.i)


print("Generate graphviz and code")

conf=Configuration()
conf.debugLimit=40
conf.CMSISDSP = True

#conf.memoryOptimization=True

#conf.dumpSchedule = True 

export_graph(g,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f)
    
sched = g.computeSchedule(conf)
#print(sched.schedule)
print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)
#


#conf.dumpFIFO=True
sched.ccode("generated",config=conf)

with open("test.dot","w") as f:
    sched.graphviz(f)

