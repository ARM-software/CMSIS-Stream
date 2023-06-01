# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
from nodes_bench import * 

# You can try with 120
AUDIO_INTERRUPT_LENGTH = 192
floatType=CType(F32,cmsis_dsp=False)

NBSINK = 5

### Define nodes
src=ArraySource("src",floatType,AUDIO_INTERRUPT_LENGTH)
src.addVariableArg("inputArray")

sink=[]
for i in range(NBSINK):
   s=ArraySink(f"sink{i}",floatType,AUDIO_INTERRUPT_LENGTH)
   s.addVariableArg(f"&outputArray[{AUDIO_INTERRUPT_LENGTH*i}]")
   sink.append(s)

the_graph = Graph()

for i in range(NBSINK):
    the_graph.connect(src.o, sink[i].i)

