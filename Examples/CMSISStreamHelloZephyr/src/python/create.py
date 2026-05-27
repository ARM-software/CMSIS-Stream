from cmsis_stream.cg.scheduler import *
from pathlib import Path
import subprocess
from nodes import *

# Get current working directory
cwd = Path.cwd()

# Build relative path (e.g., ../src)
target = cwd / "../hello_graph"

# Resolve to absolute path (optional but cleaner)
target = target.resolve()

if not target.exists():
    print(f"The script must be launched from the python folder but you launched it from {cwd}.")



the_graph = Graph()

src = DebugSource("src",CType(F32),10)
process = DebugProcess("process",CType(F32),10)
sink = DebugSink("sink",CType(F32))
evtSink = DebugEvtSink("evtSink")

the_graph.connect(src.o,process.i)
the_graph.connect(process.o,sink.i)
the_graph.connect(sink["oev0"],evtSink["iev0"])

conf = Configuration()
conf.CMSISDSP = False
conf.asynchronous = False

conf.horizontal=True
conf.nodeIdentification = True
conf.schedName = f"scheduler_hello"
conf.schedulerCFileName = f"scheduler_hello"
conf.memoryOptimization = True

conf.appConfigCName = f"app_config.hpp"
conf.cOptionalInitArgs = [f"helloParams_t *params"]
conf.appNodesCName = f"AppNodes_hello.hpp"

# Buffer prefix
conf.prefix = f"stream_hello_"


scheduling = the_graph.computeSchedule(config=conf)
    
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)

scheduling.ccode(f"../hello_graph",conf)
scheduling.genJsonIdentification(f"../hello_graph/json",conf)
scheduling.genJsonSelectorsInit(f"../hello_graph/json",conf)
    
class MyStyle(Style):
        
        def edge_color(self,edge):
            nb = self.fifoLength(edge) 
            if nb is None:
                nb = 0
            s = self.edgeSrcNode(edge)
            d = self.edgeDstNode(edge)
            
            if d.nodeName ==  "display":
               return("magenta")
            else: 
                if (nb > 512):
                    return("orange")
                return(super().edge_color(edge))
    
myStyle = MyStyle()

with open(f"../hello_graph/hello.dot","w") as f:
        scheduling.graphviz(f,style=myStyle)
    
subprocess.run(["dot","-Tpng",f"../hello_graph/hello.dot","-o",f"../hello_graph/hello.png"])
