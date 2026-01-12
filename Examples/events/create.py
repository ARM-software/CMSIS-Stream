# Include definition of the nodes
from nodes import * 
# Include definition of the graph
from graph import * 

from cmsis_stream.cg.yaml import *



# Create a configuration object
conf=Configuration()
conf.CMSISDSP = False
conf.asynchronous = False
conf.horizontal=True
conf.nodeIdentification = True
conf.schedName = "scheduler"

the_graph = mkGraph(event_only=False)

export_graph(the_graph,"graph.yml")
export_config(conf,"config.yml")

# Compute a static scheduling of the graph 
# The size of FIFO is also computed

with open("pre_schedule_simple.dot","w") as f:
    the_graph.graphviz(f)

scheduling = the_graph.computeSchedule(config=conf)

# Print some statistics about the compute schedule
# and the memory usage
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)


# Generate a graphviz representation of the graph
with open("schedule.dot","w") as f:
    scheduling.graphviz(f)

# Generate the C++ code for the static scheduler
scheduling.ccode(".",conf)
scheduling.genJsonIdentification(".",conf)
scheduling.genJsonSelectors(".",conf)
scheduling.genJsonSelectorsInit(".",conf)


newconf=Configuration()
# Reuse selectors ID definitions from previous graph
newconf.selectorsID = scheduling.selectorsID
newconf.CMSISDSP = False
newconf.asynchronous = False
newconf.horizontal=True
newconf.nodeIdentification = True
newconf.schedName = "simple"
newconf.schedulerCFileName = "simple_scheduler"

the_graph = mkGraph(event_only=True)
scheduling = the_graph.computeSchedule(config=newconf,oldSelectorsInit=["scheduler_selectors_inits.json"])

# Generate a graphviz representation of the graph
with open("events.dot","w") as f:
    scheduling.graphviz(f)

scheduling.ccode(".",newconf)
scheduling.genJsonIdentification(".",newconf)
scheduling.genJsonSelectors(".",newconf)
scheduling.genJsonSelectorsInit(".",newconf)
