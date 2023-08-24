from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.yaml import *

### Define new types of Nodes 

class ProcessingNode(GenericManyToManyNode):
    def __init__(self,name,theType_in,theType_out,inLength,outLength,nb_in=2,nb_out=2):
        GenericNode.__init__(self,name)
        self.addManyInput(theType_in,inLength,nb_in)
        self.addManyOutput(theType_out,outLength,nb_out)

    @property
    def typeName(self):
        return "ProcessingNode"

class ToSink(GenericFromManyNode):
    def __init__(self,name,theType,inLength,nb=2):
        GenericFromManyNode.__init__(self,name)
        self.addManyInput(theType,inLength,nb)
        self.addOutput("o",theType,1)

    @property
    def typeName(self):
        return "ToSink"
    
class Sink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        return "Sink"

class FromSource(GenericToManyNode):
    def __init__(self,name,theType,outLength,nb=2):
        GenericToManyNode.__init__(self,name)
        self.addInput("i",theType,1)
        self.addManyOutput(theType,outLength,nb)

    @property
    def typeName(self):
        return "FromSource"
    
class Source(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "Source"

   
class SharedSource(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "SharedSource"

class SharedSink(GenericSink):
    def __init__(self,name,theTypeA,theTypeB,inLength):
        GenericSink.__init__(self,name)
        self.addInput("ia",theTypeA,inLength)
        self.addInput("ib",theTypeB,inLength)

    @property
    def typeName(self):
        return "SharedSink"

### Define nodes
complexType=CStructType("complex",8)

bufferType=CCustomType("buffer",4)
sharedBufferType=SharedType(bufferType)

srcType = CType(F32)
dstType = complexType #CType(Q15)
fromSrc=FromSource("fromSource",srcType,5,nb=5)
src=Source("source",srcType,1)
processing=ProcessingNode("processing",srcType,dstType,7,5,nb_in=5,nb_out=7)
toSink=ToSink("toSink",dstType,5,nb=7)
sink=Sink("sink",dstType,1)

sharedSrc=Source("sharedSource",sharedBufferType,1)
sharedSink=SharedSink("sharedSink",dstType,sharedBufferType,1)


g = Graph()

g.connect(src.o,fromSrc.i)

g.connect(fromSrc.oa,processing.ia)
g.connect(fromSrc.ob,processing.ib)
g.connect(fromSrc.oc,processing.ic)
g.connect(fromSrc.od,processing.id)
g.connect(fromSrc.oe,processing.ie)


g.connect(processing.oa,toSink.ia)
g.connect(processing.ob,toSink.ib)
g.connect(processing.oc,toSink.ic)
g.connect(processing.od,toSink.id)
g.connect(processing.oe,toSink.ie)
g.connect(processing.of,toSink["if"])
g.connect(processing.og,toSink.ig)

g.connect(toSink.o,sink.i)

g.connect(toSink.o,sharedSink.ia)
g.connect(sharedSrc.o,sharedSink.ib)

print("Generate graphviz and code")

conf=Configuration()
conf.debugLimit=1
conf.cOptionalArgs=["int someVariable"
                   ]
export_graph(g,"graph.yml")
export_config(conf,"config.yml")


with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f)

sched = g.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode(".",conf)

with open("test.dot","w") as f:
    sched.graphviz(f)

testg = import_graph("graph.yml")
export_graph(testg,"graph2.yml")


testc = import_config("config.yml")
export_config(testc,"config2.yml")