# Include definition of the nodes
from nodes import * 
# Include definition of the graph
from graph import * 

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
generateGenericNodes("generated")

# cg_status.h is created in the folder "generated"
generateCGStatus("generated")

# Generate a graphviz representation of the graph
with open("event_recorder.dot","w") as f:
    scheduling.graphviz(f)

