# Include definition of the nodes
from nodes import * 
# Include definition of the graph
from graph import * 

from cmsis_stream.cg.yaml import *

# Create a configuration object
conf=Configuration()
# The number of schedule iteration is limited to 1
# to prevent the scheduling from running forever
# (which should be the case for a stream computation)
conf.debugLimit=1
# Enable inclusion of CMSIS-DSP headers
conf.CMSISDSP = True

export_graph(the_graph,"graph.yml")
export_config(conf,"config.yml")

with open("pre_schedule_simpledsp.dot","w") as f:
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

# Generate a graphviz representation of the graph
with open("simpledsp.dot","w") as f:
    scheduling.graphviz(f)

