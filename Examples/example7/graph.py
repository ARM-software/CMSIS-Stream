import numpy as np 

from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.yaml import *


class Processing(GenericNode):
    def __init__(self,name,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(Q15),outLength)
        self.addOutput("o",CType(Q15),outLength)

    @property
    def typeName(self):
        return "Processing"


BUFSIZE=128
### Define nodes
src=VHTSource("src",BUFSIZE,0)
processing=Processing("proc",BUFSIZE)
sink=VHTSink("sink",BUFSIZE,0)


g = Graph()

g.connect(src.o, processing.i)
g.connect(processing.o, sink.i)



print("Generate graphviz and code")



conf=Configuration()
conf.CMSISDSP = True

export_graph(g,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f)
    
sched = g.computeSchedule(conf)
print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)


sched.pythoncode(".",config=conf)

with open("test.dot","w") as f:
    sched.graphviz(f)

