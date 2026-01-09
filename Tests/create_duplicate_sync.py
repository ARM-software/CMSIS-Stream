# Include definition of the nodes
from nodes import * 
# Include definition of the graph
from graph_duplicate_sync import * 

# Create a configuration object
conf=Configuration()
# The number of schedule iteration is limited to 1
# to prevent the scheduling from running forever
# (which should be the case for a stream computation)
conf.debugLimit=1
conf.appConfigCName = "custom.h"
conf.appNodesCName = "DuplicateAppNodes.h"
conf.cOptionalArgs=["float* inputArray",
                    "float* outputArray"
                   ]
conf.CMSISDSP = False
# Compute a static scheduling of the graph 
# The size of FIFO is also computed
scheduling = the_graph.computeSchedule(config=conf)

# Print some statistics about the compute schedule
# and the memory usage
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)

# Generate the C++ code for the static scheduler
scheduling.ccode("duplicate_sync",conf)

# Generate a graphviz representation of the graph
with open("dot/duplicate_sync.dot","w") as f:
    scheduling.graphviz(f)

