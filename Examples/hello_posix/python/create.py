from cmsis_stream.cg.scheduler import *
from pathlib import Path
import subprocess
from nodes import *

cwd = Path.cwd()
target = (cwd / "../hello_graph").resolve()

if not target.exists():
    print(f"The script must be launched from the python folder but you launched it from {cwd}.")

the_graph = Graph()

src = DebugSource("src", CType(F32), 10)
process = DebugProcess("process", CType(F32), 10)
sink = DebugSink("sink", CType(F32))
evtSink = DebugEvtSink("evtSink")

the_graph.connect(src.o, process.i)
the_graph.connect(process.o, sink.i)
the_graph.connect(sink["oev0"], evtSink["iev0"])

conf = Configuration()
conf.CMSISDSP = False
conf.asynchronous = False

conf.horizontal = True
conf.nodeIdentification = True
conf.schedName = "scheduler_hello"
conf.schedulerCFileName = "scheduler_hello"
conf.memoryOptimization = True

conf.appConfigCName = "app_config.hpp"
conf.cOptionalInitArgs = ["helloParams_t *params"]
conf.appNodesCName = "AppNodes_hello.hpp"
conf.prefix = "stream_hello_"

scheduling = the_graph.computeSchedule(config=conf)

print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)

scheduling.ccode("../hello_graph", conf)
scheduling.genJsonIdentification("../hello_graph/json", conf)
scheduling.genJsonSelectorsInit("../hello_graph/json", conf)

class MyStyle(Style):
    def edge_color(self, edge):
        nb = self.fifoLength(edge)
        if nb is None:
            nb = 0
        d = self.edgeDstNode(edge)

        if d.nodeName == "display":
            return "magenta"
        if nb > 512:
            return "orange"
        return super().edge_color(edge)

myStyle = MyStyle()

with open("../hello_graph/hello.dot", "w") as f:
    scheduling.graphviz(f, style=myStyle)

subprocess.run(["dot", "-Tpng", "../hello_graph/hello.dot", "-o", "../hello_graph/hello.png"])
