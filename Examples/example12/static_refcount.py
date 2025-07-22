from cmsis_stream.cg.scheduler import *
import os 

from cmsis_stream.cg.yaml import *

def try_remove(path):
    if os.path.isfile(path):
        os.remove(path)
        
from create import mkGraph

bufferType=CCustomType("buffer",4)
sharedBufferType=SharedType(bufferType)
g=mkGraph(sharedBufferType)

print("Generate graphviz and code")

conf=Configuration()
conf.debugLimit=1
conf.schedName="static_scheduler"
conf.prefix="static_"
conf.schedulerCFileName = "static_scheduler"


conf.cOptionalArgs=[]

export_graph(g,"graph_static.yml")
export_config(conf,"config_static.yml")

with open("pre_schedule_static.dot","w") as f:
    g.graphviz(f)

sched = g.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode("static",conf)

class MyStyle(Style):
    def edge_color(self,edge):
        inputPort=self.edgeSrcPort(edge)
        if inputPort.theType.shared:
            return("crimson")
        else:
            return(super().edge_color(edge))


with open("static.dot","w") as f:
    sched.graphviz(f,style=MyStyle())

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