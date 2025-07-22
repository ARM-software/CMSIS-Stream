# Include definition of the nodes
from nodes import * 
# Include definition of the graph
from graph import * 

import os 

from cmsis_stream.cg.yaml import *

def try_remove(path):
    if os.path.isfile(path):
        os.remove(path)

import gen_flat as flat

# Create a configuration object
conf=Configuration()
# The number of schedule iteration is limited to 1
# to prevent the scheduling from running forever
# (which should be the case for a stream computation)
conf.debugLimit=1
# Disable inclusion of CMSIS-DSP headers so that we don't have
# to recompile CMSIS-DSP for such a simple example
conf.CMSISDSP = False
conf.CAPI = True
conf.asynchronous = False

conf.nodeIdentification = True

conf.memoryOptimization=True

export_graph(the_graph,"graph.yml")
export_config(conf,"config.yml")

# Compute a static scheduling of the graph 
# The size of FIFO is also computed

with open("pre_schedule_runtime_mode.dot","w") as f:
    the_graph.graphviz(f)

scheduling = the_graph.computeSchedule(config=conf)

# Print some statistics about the compute schedule
# and the memory usage
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)


buf = flat.gen(scheduling,conf)
flat.create_bin(buf,"sched_flat")


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


# Generate a graphviz representation of the graph
with open("runtime_mode.dot","w") as f:
    scheduling.graphviz(f)
