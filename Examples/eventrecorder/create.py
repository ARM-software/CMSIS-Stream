# Include definition of the nodes
from nodes import * 
# Include definition of the graph
from graph import * 

import os 

from cmsis_stream.cg.yaml import *

def try_remove(path):
    if os.path.isfile(path):
        os.remove(path)

# Create a configuration object
conf=Configuration()
# The number of schedule iteration is limited to 1
# to prevent the scheduling from running forever
# (which should be the case for a stream computation)
conf.debugLimit=1
# Disable inclusion of CMSIS-DSP headers so that we don't have
# to recompile CMSIS-DSP for such a simple example
conf.CMSISDSP = False

# Enable generation of event recorder calls in the C++
conf.eventRecorder = True

export_graph(the_graph,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_event_recorder.dot","w") as f:
    the_graph.graphviz(f)

# Compute a static scheduling of the graph 
# The size of FIFO is also computed
scheduling = the_graph.computeSchedule(config=conf)

# Print some statistics about the compute schedule
# and the memory usage
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)

# Generate the C++ code for the static scheduler
scheduling.ccode("generated",conf)

# GenericNodes.h is created in the folder "generated"
try_remove("generated/StreamNode.hpp")
try_remove("generated/GenericNodes.hpp")
try_remove("generated/EventQueue.hpp")
try_remove("generated/cg_queue.hpp")
try_remove("generated/cg_queue.cpp")
try_remove("generated/cg_enums.h")
try_remove("generated/posix_thread.cpp")
try_remove("generated/posix_thread.hpp")
try_remove("generated/cstream_node.h")
try_remove("generated/IdentifiedNode.hpp")
try_remove("generated/cg_pack.hpp")

try_remove("generated/StreamNode.h")
try_remove("generated/GenericNodes.h")
try_remove("generated/EventQueue.h")
try_remove("generated/cg_queue.h")
try_remove("generated/cg_enums.h")
try_remove("generated/posix_thread.h")
try_remove("generated/cstream_node.h")
try_remove("generated/IdentifiedNode.h")
try_remove("generated/cg_pack.h")
generateGenericNodes("generated")

# Generate a graphviz representation of the graph
with open("event_recorder.dot","w") as f:
    scheduling.graphviz(f)

