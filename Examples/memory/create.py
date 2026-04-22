from cmsis_stream.cg.scheduler import *
import os 

from cmsis_stream.cg.yaml import *
import os

NBLEN = 100
NB = 1
NBBRANCH = 2
        
### Define new types of Nodes 

class ProcessingNode(GenericNode):
    def __init__(self,name):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),NBLEN)
        self.addOutput("o",CType(F32),NBLEN)

    @property
    def typeName(self):
        return "ProcessingNode"
    
class ProcessingNode12(GenericNode):
    def __init__(self,name):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),NBLEN)
        self.addOutput("oa",CType(F32),NBLEN)
        self.addOutput("ob",CType(F32),NBLEN)

    @property
    def typeName(self):
        return "ProcessingNode12"
    
class ProcessingNode21(GenericNode):
    def __init__(self,name):
        GenericNode.__init__(self,name)
        self.addInput("ia",CType(F32),NBLEN)
        self.addInput("ib",CType(F32),NBLEN)
        self.addOutput("o",CType(F32),NBLEN)

    @property
    def typeName(self):
        return "ProcessingNode21"

class Sink(GenericSink):
    def __init__(self,name):
        GenericSink.__init__(self,name)
        self.addInput("i",CType(F32),NBLEN)

    @property
    def typeName(self):
        return "Sink"

class Source(GenericSource):
    def __init__(self,name):
        GenericSource.__init__(self,name)
        self.addOutput("o",CType(F32),NBLEN)

    @property
    def typeName(self):
        return "Source"


def openingBranches(g,k,layer,nb_branch):
    res = []
    for n in layer:
       p = ProcessingNode12(f"p_start_{k}")
       g.connect(n,p.i)
       res.append(p.oa)
       res.append(p.ob)
       k = k + 1
    if nb_branch == 1:
        return res
    return openingBranches(g,k,res,nb_branch-1)

def closingBranch(g,k,endNodes):
    connections = []
    for i in range(0,len(endNodes),2):
        p = ProcessingNode21(f"p_end_{k}")
        g.connect(endNodes[i],p.ia)
        g.connect(endNodes[i+1],p.ib)
        k = k + 1
        connections.append(p.o)
    if len(connections) == 1:
        return connections
    else:
        return closingBranch(g,k,connections)

def mkGraph():
    src=Source("source")
    dst =Sink("sink")
    
    
    g = Graph()
    l = openingBranches(g,0,[src.o],NBBRANCH)
   

    endConnection = []

    for branch,n in enumerate(l):
        oldp = None
        for k in range(NB):
            p = ProcessingNode(f"p{branch}_{k}")
            if k == 0:
                g.connect(n,p.i)
            else:
                g.connect(oldp.o,p.i)
            oldp = p
        endConnection.append(p.o)

   
    r = closingBranch(g,0,endConnection)
    g.connect(r[0],dst.i)
    
    return g


conf=Configuration()

conf.memoryOptimization=False
sched = mkGraph().computeSchedule(conf)
print("Without memory optimization")
print("Schedule length = %d" % sched.scheduleLength)
print(f"Memory usage {sched.memory} bytes")

conf.memoryOptimization=True
sched = mkGraph().computeSchedule(conf)
print("\nWith memory optimization")
print("Schedule length = %d" % sched.scheduleLength)
print(f"Memory usage {sched.memory} bytes")

sched.ccode("generated",conf)

with open("test.dot","w") as f:
    sched.graphviz(f,config=conf)

os.system("dot -Tpng test.dot -o test.png")