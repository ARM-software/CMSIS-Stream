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
# Disable inclusion of CMSIS-DSP headers so that we don't have
# to recompile CMSIS-DSP for such a simple example
conf.CMSISDSP = False
#conf.fullyAsynchronous = True
#conf.asynchronous = True
conf.callback = True
# Some arguments used to save the state of the state machine
# Here the state is the state variable defined in macro CG_BEFORE_SCHEDULE
# The state of objects is automatically preserved since objects are
# kept between calls of the schedule
conf.cOptionalArgs="int *someVariable"


# Compute a static scheduling of the graph 
# The size of FIFO is also computed

with open("pre_schedule_callback.dot","w") as f:
    the_graph.graphviz(f)

scheduling = the_graph.computeSchedule(config=conf)

# Print some statistics about the compute schedule
# and the memory usage
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)

# Generate the C++ code for the static scheduler
scheduling.ccode(".",conf)

# The generated code is including GenericNodes.h and 
# cg_enums.h
# Those files can either be used from the CMSIS-Stream 
# repository or they can be generated from the Python 
# package so that it is easier to start using CMSIS-Stream

# GenericNodes.h is created in the folder "generated"
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
with open("callback.dot","w") as f:
    scheduling.graphviz(f)
