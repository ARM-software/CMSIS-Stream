from cmsis_stream.cg.scheduler import *

# Creation of a graph for testing styling.
# Look below for the style definition

AUDIO_INTERRUPT_LENGTH = 160
MFCCFEATURESSIZE=10
NBOUTMFCC=50
OVERLAPMFCC=25
floatType=CType(F32,cmsis_dsp=True)


### Define new types of Nodes 

class Node(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "Node"

class StereoSource(GenericSource):
    def __init__(self,name,inLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",floatType,2*inLength)

    @property
    def typeName(self):
        return "StereoSource"

# This is a fake TFLite node just for illustration
class TFLite(GenericSink):
    def __init__(self,name):
        GenericSink.__init__(self,name)
        self.addInput("i",floatType,NBOUTMFCC*MFCCFEATURESSIZE)

    @property
    def typeName(self):
        return "TFLite"

# This is a fake MFCC just to illustrate how it could be used in a graph.
# For a real MFCC example, look at example5
class MFCC(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)

        self.addInput("i",floatType,inLength)
        self.addOutput("o",floatType,MFCCFEATURESSIZE)

    @property
    def typeName(self):
        return "MFCC"


### Define nodes
half=Constant("HALF")
src=StereoSource("src",AUDIO_INTERRUPT_LENGTH)
toMono=Unzip("toMono",floatType, AUDIO_INTERRUPT_LENGTH)

sa=Binary("arm_scale_f32",floatType,AUDIO_INTERRUPT_LENGTH)
sb=Binary("arm_scale_f32",floatType,AUDIO_INTERRUPT_LENGTH)

add=Binary("arm_add_f32",floatType,AUDIO_INTERRUPT_LENGTH)

audioWindow=SlidingBuffer("audioWin",floatType,640,320)
mfcc=MFCC("mfcc",640)

mfccWindow=SlidingBuffer("mfccWind",floatType,NBOUTMFCC*MFCCFEATURESSIZE,OVERLAPMFCC*MFCCFEATURESSIZE)
tf=TFLite("TFLite")

g = Graph()

g.connectWithDelay(src.o, toMono.i,10)
g.connect(toMono.o1,sa.ia)
g.connect(toMono.o2,sb.ia)

# A constant node as no output
# So it is connected directly
g.connect(half,sa.ib)
g.connect(half,sb.ib)

g.connect(sa.o,add.ia)
g.connect(sb.o,add.ib)

g.connect(add.o,audioWindow.i)
g.connect(audioWindow.o, mfcc.i)
g.connect(mfcc.o,mfccWindow.i)
g.connect(mfccWindow.o,tf.i)

print("Generate graphviz and code")

conf=Configuration()
conf.debugLimit=1
conf.cOptionalArgs="int opt1,int opt2"
conf.sinkPriority  = True
conf.CMSISDSP = True

#conf.memoryOptimization=True
#conf.dumpSchedule = True 

############################
#
# Definition of a new style
#

# Global style settings

style = {
     "graph_background"      : "gray"
    ,"graph_font"            : "courier"

    ,"node_color"            : "ghostwhite"
    ,"node_boundary_color"   : "yellow"

    ,"port_font_size"        : "12.0"

    ,"port_sample_color"     : "blue"
    ,"port_sample_font_size" : "12.0"

    ,"edge_color"            : "green"
    ,"arrow_size"            : 1.0

}

# New style object to customize some nodes, edges, ports.
class MyStyle(Style):
    ############################
    #
    # NORMAL NODE CUSTOMIZATION
    #

    def node_color(self,node):
        # pure node is a pure function with no state
        # like some CMSIS-DSP ones
        if node.isPureNode:
           return("burlywood1")
        else:
            return("ghostwhite")

    def node_boundary_color(self,node):
        return("fuchsia")

    def node_label_color(self,node):
        return("purple")

    def node_label_size(self,node):
        return("20")

    def node_label(self,node):
        if node.nodeName == "arm_scale_f32":
            return "*"
        return super().node_label(node)

    
    ############################
    #
    # DELAY NODE CUSTOMIZATION
    #

    def delay_border(self,delay_value):
        return("2")

    def delay_color(self,delay_value):
        return("bisque")

    def delay_edge_color(self,nb_samples):
        if nb_samples==10:
           return("limegreen")
        return(super().delay_edge_color(nb_samples))

    def delay_label_color(self,node):
        return("black")

    def delay_label_size(self,node):
        return("20")
       
    ############################
    #
    # CONST NODE CUSTOMIZATION
    #

    def const_border(self,const_name):
        return("2")

    def const_color(self,const_name):
        return("darkseagreen")

    def const_edge_color(self,name):
        if name == "HALF":
           return("cyan")
        return(super().const_edge_color(name))

    def const_label_color(self,node):
        return("red")

    def const_label_size(self,node):
        return("9")


    ############################
    #
    # NODE PORT CUSTOMIZATION
    #

    def port_sample_color(self,nb_sample):
        if nb_sample>=320:
            return("red")
        else:
            return super().port_sample_color(nb_sample)

    def port_sample_font_size(self,nb_sample):
        if nb_sample>=320:
            return("18")
        else:
            return super().port_sample_font_size(nb_sample)


    def port_font_color(self,item,i,input=False):
        port = self.getPort(item,i,input)
        if port.owner.isPureNode:
            if port.name == "o":
                return("red")

        return super().port_font_color(item,i,input)

    def port_font_size(self,item,i,input=False):
        port = self.getPort(item,i,input)
        if port.owner.isPureNode:
            if port.name == "o":
                return("20")

        return super().port_font_size(item,i,input)

    ############################
    #
    # NORMAL EDGE CUSTOMIZATION
    #

    def edge_label_color(self,edge):
        return("red")

    def edge_label_size(self,edge):
        if not self.isFIFO(edge):
           return("14")
        else:
            nb = self.fifoLength(edge) 
            if (nb>160):
                return("25")
        return("14")

    def edge_color(self,edge):
        s = self.edgeSrcNode(edge)
        d = self.edgeDstNode(edge)
        
        if s.isPureNode and d.isPureNode:
           return("white")
        else:
           return(super().edge_color(edge))

    def edge_style(self,edge):
        s = self.edgeSrcNode(edge)
        d = self.edgeDstNode(edge)
        
        if s.isPureNode and d.isPureNode:
           return("dashed")
        else:
           return(super().edge_style(edge))

    def edge_label(self,typeName,length):
        return f"{typeName}<BR/>({length})"


myStyle = MyStyle(style)
#myStyle=Style.dark_style()
#myStyle=Style.default_style()

with open("pre_schedule_test.dot","w") as f:
    g.graphviz(f,style=myStyle)
    
sched = g.computeSchedule(conf)
#print(sched.schedule)
print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)
#
# No CPP code is generated from the scheduling as this
# example is only about the graph styling



with open("test.dot","w") as f:
    sched.graphviz(f,style=myStyle)

