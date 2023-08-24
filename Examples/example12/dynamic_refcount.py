from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.yaml import *
from create import mkGraph

sharedPtr=CCustomType("std::shared_ptr<buffer>",8)
g=mkGraph(sharedPtr)

print("Generate graphviz and code")

conf=Configuration()
conf.debugLimit=1
conf.schedName="dynamic_scheduler"
conf.prefix="dynamic_"
conf.schedulerCFileName = "dynamic_scheduler"

conf.cOptionalArgs=[]

export_graph(g,"graph_dynamic.yml")
export_config(conf,"config_dynamic.yml")

with open("pre_schedule_dynamic.dot","w") as f:
    g.graphviz(f)

sched = g.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode("dynamic",conf)

with open("dynamic.dot","w") as f:
    sched.graphviz(f)

