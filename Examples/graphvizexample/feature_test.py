from cmsis_stream.cg.scheduler import *
import subprocess
import re

# This file is to generate a graph for any setting
# that can be changed in the style of a graph.
# It is for testing purpose.
# If you just need top experiment with styling, please
# look at graph.py

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




#conf.memoryOptimization=True
#conf.dumpSchedule = True 

style = {
    # Graph global settings
     "graph_background"      : "gray"       # Color of graph background
    ,"graph_font"            : "Courier" # Font used in the graph

    # Node settings
    ,"node_color"            : "lightcyan2"        # Fill color of any node
    ,"node_boundary_color"   : "red"       # Boundary color of any node
    ,"node_label_size"       : "24.0"        # Font size of node labels
    ,"node_label_color"      : "blue"       # Color of node labels

    # Special node settings (Constant node and delay box)
    ,"special_node_border"   : "4"           # Thickness of node boundary

    # Node port settings
    ,"port_sample_color"     : "red"        # Color of number of samples
    ,"port_sample_font_size" : "18.0"        # Font size of number of samples

    ,"port_font_color"       : "red"       # Color of a port
    ,"port_font_size"        : "18.0"        # Font size of a port

    # Edge settings
    ,"edge_color"            : "red"       # Color of any edge
    ,"edge_label_size"       : "18.0"        # Font size of any edge label
    ,"edge_label_color"      : "red"       # Color of any edge label
    ,"edge_style"            : "dashed"       # Edge style (dashed, dotted ...)
    ,"arrow_size"            : 1.0           # Arrow size at end of edge
}

class MyStyle(Style):
    ############################
    #
    # NORMAL NODE CUSTOMIZATION
    #

    def node_color(self,node):
        if node.isPureNode:
           return("burlywood1")
        return super().node_color(node)

    def node_boundary_color(self,node):
        if node.isPureNode:
           return("fuchsia")
        return super().node_boundary_color(node)

    def node_label_color(self,node):
        if node.isPureNode:
           return("purple")
        return super().node_label_color(node)

    def node_label_size(self,node):
        if node.isPureNode:
           return("20")
        return super().node_label_size(node)

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

    def delay_boundary_color(self,delay_value):
        return("red")

    def delay_edge_color(self,nb_samples):
        if nb_samples==10:
           return("limegreen")
        return(super().delay_edge_color(nb_samples))

    def delay_label_color(self,node):
        return("red")

    def delay_label_size(self,node):
        return("20")
       
    ############################
    #
    # CONST NODE CUSTOMIZATION
    #

    def const_border(self,const_name):
        return("2")

    def const_color(self,const_name):
        return("red")

    def const_boundary_color(self,const_name):
        return("red")

   
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
       
        return super().port_sample_color(nb_sample)

    def port_sample_font_size(self,nb_sample):
        if nb_sample>=320:
            return("18")
       
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
        nb = self.fifoLength(edge) 
        if (nb>160):
           return("red")
        super().edge_label_color(edge)

    def edge_label_size(self,edge):
        
        nb = self.fifoLength(edge) 
        if (nb>160):
            return("25")
        return("14")

    def edge_color(self,edge):
        s = self.edgeSrcNode(edge)
        d = self.edgeDstNode(edge)
        
        if s.isPureNode and d.isPureNode:
           return("red")
        
        return(super().edge_color(edge))

    def edge_style(self,edge):
        s = self.edgeSrcNode(edge)
        d = self.edgeDstNode(edge)
        
        if s.isPureNode and d.isPureNode:
           return("dashed")
        
        return(super().edge_style(edge))

    def edge_label(self,fifo,typeName,length):
        s = self.edgeSrcNode(fifo)
        d = self.edgeDstNode(fifo)

        if s.isPureNode and d.isPureNode:
           return f"{typeName}<BR/>({length})"

        return(super().edge_label(fifo,typeName,length))


    def delay_edge_color(self,srcPort,nb_samples):
        return("red")

    def delay_edge_style(self,srcPort,nb_samples):
        return("dashed")


    def const_edge_color(self,name,dstPort):
        if name == "HALF":
           return("cyan")
        return(super().const_edge_color(name))


    def const_edge_style(self,name,dstPort):
        return("dashed")

class VStyle:
    def __init__(self,method):
        self._s = MyStyle()
        self._o = Style()
        self._m = method

    def __getattr__(self,x):
        if x == self._m:
            return(getattr(self._s,x))
        else:
            return(getattr(self._o,x))

    def __getitem__(self,x):
        return(self._o[x])



def mkGraph(k,i,s):
    ### Define nodes
    half=Constant("HALF")
    src=StereoSource("src",AUDIO_INTERRUPT_LENGTH)
    toMono=Unzip("toMono",floatType, AUDIO_INTERRUPT_LENGTH)
    
    sa=Dsp("scale",floatType,AUDIO_INTERRUPT_LENGTH)
    sb=Dsp("scale",floatType,AUDIO_INTERRUPT_LENGTH)
    
    add=Dsp("add",floatType,AUDIO_INTERRUPT_LENGTH)
    
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
    
    #print("Generate graphviz and code")
    
    conf=Configuration()
    conf.debugLimit=1
    conf.cOptionalArgs="int opt1,int opt2"
    conf.sinkPriority  = True
    conf.CMSISDSP = True
    
    myStyle = s
    #myStyle=Style.dark_style()
    #myStyle=Style.default_style()
    
    with open(f"dots/pre_schedule_feature_{i}.dot","w") as f:
        g.graphviz(f,style=myStyle)
        
    sched = g.computeSchedule(conf)
    
    with open(f"dots/feature_{i}.dot","w") as f:
        sched.graphviz(f,style=myStyle)

    subprocess.run(["dot","-Tpng","-o",f"feature/pre_schedule_feature_{k}.png",f"dots/pre_schedule_feature_{i}.dot"])
    subprocess.run(["dot","-Tpng","-o",f"feature/feature_{k}.png",f"dots/feature_{i}.dot"])

for i,k in enumerate(style.keys()):
   print(k)
   s = Style._DEFAULT_STYLE.copy()
   s[k] = style[k]
   #print(s)

   mkGraph(k,i,Style(s))

exceptions=["default_style"
           ,"dark_style"
           ,"isFIFO"
           ,"isFIFO"
           ,"fifoLength"
           ,"edgeSrcNode"
           ,"edgeDstNode"
           ,"edgeSrcPort"
           ,"edgeDstPort"
           ,"getPort"
           ]

def accept_method(name):
    return(not re.match(r'^_',name) and not name in exceptions)

def get_methods(object, spacing=20):
  methodList = []
  for method_name in dir(object):
    try:
        if callable(getattr(object, method_name)):
            name = str(method_name)
            if accept_method(name):
               methodList.append(name)
    except Exception:
        name = str(method_name)
        if accept_method(name):
               methodList.append(name)
  return(methodList)
  
#object_methods = [method_name for method_name in dir(MyStyle)
#                  if callable(getattr(object, method_name))]

all_methods = get_methods(MyStyle)
for i,m in enumerate(all_methods):
   s = VStyle(m)
   print(m)
   #print(s['node_boundary_color'])
   #print(m)
   #print(hasattr(s,"node_boundary_color"))

   #print(s.const_border(10))

   mkGraph("func_"+m,i,s)
