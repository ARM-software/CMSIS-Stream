###########################################
# Project:      CMSIS DSP Library
# Title:        graphviz.py
# Description:  Graphviz generation for the CG Static scheduler
# 
# 
# Target Processor: Cortex-M and Cortex-A cores
# -------------------------------------------------------------------- */
# 
# Copyright (C) 2021-2023 ARM Limited or its affiliates. All rights reserved.
# 
# SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
############################################
from jinja2 import Environment, PackageLoader, select_autoescape
import pathlib
import os.path



class Style:
    _DEFAULT_STYLE = {
    # Graph global settings
     "graph_background"      : "white"       # Color of graph background
    ,"graph_font"            : "Times-Roman" # Font used in the graph

    # Node settings
    ,"node_color"            : "none"        # Fill color of any node
    ,"node_boundary_color"   : "black"       # Boundary color of any node
    ,"node_label_size"       : "14.0"        # Font size of node labels
    ,"node_label_color"      : "black"       # Color of node labels

    # Special node settings (Constant node and delay box)
    ,"special_node_border"   : "1"           # Thickness of node boundary

    # Node port settings
    ,"port_sample_color"     : "blue"        # Color of number of samples
    ,"port_sample_font_size" : "12.0"        # Font size of number of samples

    ,"port_font_color"       : "black"       # Color of a port
    ,"port_font_size"        : "12.0"        # Font size of a port

    # Edge settings
    ,"edge_color"            : "black"       # Color of any edge
    ,"edge_label_size"       : "12.0"        # Font size of any edge label
    ,"edge_label_color"      : "black"       # Color of any edge label
    ,"edge_style"            : "solid"       # Edge style (dashed, dotted ...)
    ,"arrow_size"            : 0.5           # Arrow size at end of edge

    }

    _DARK_STYLE = {
         "graph_background"      : "black"
        ,"node_boundary_color"   : "white"
        ,"node_label_color"      : "white"

        ,"port_font_color"       : "white"

        ,"port_sample_color"     : "green"

        ,"edge_color"            : "white"
        ,"edge_label_color"      : "white"

    }

    def __getitem__(self,name):
        """Present styles from the dictionary as keys"""
        if name in self._s:
           return(self._s[name])
        raise IndexError 

    # Set default styles when a valuie for a style
    # is not available in the current style
    def _cleanStyle(self):
        for k in Style._DEFAULT_STYLE:
            if not k in self._s:
                self._s[k] = Style._DEFAULT_STYLE[k]

    def __init__(self,style=_DEFAULT_STYLE):
        self._s = style 
        # Force missing values to default values
        self._cleanStyle()

    # Create a style object with the default style
    @classmethod
    def default_style(cls):
        return cls(style=cls._DEFAULT_STYLE)

    # Create a style object with the dark style
    @classmethod
    def dark_style(cls):
        return cls(style=cls._DARK_STYLE)

    
    # Check if an edge is a FIFO (post-schedule graph)
    # or a pair of ios (pre-schedule graph)
    def isFIFO(self,edge):
        return (not (type(edge) is tuple))

    # Return the length of a FIFO or none
    # if the edge is not a FIFO (pre-schedule)
    # graph
    # But, in pre-schedule graph this function is
    # never called
    def fifoLength(self,edge):
        if self.isFIFO(edge):
            return(edge.length)
        else:
            return None

    # Get the src node of an edge
    def edgeSrcNode(self,edge):
        if not (self.isFIFO(edge)):
            return(edge[0].owner)
        else:
           return(edge.src.owner)

    # Get the dst node of and egde
    def edgeDstNode(self,edge):
        if not (self.isFIFO(edge)):
            return(edge[1].owner)
        else:
           return(edge.dst.owner)

    # Get the src port of an edge
    def edgeSrcPort(self,edge):
        if not (self.isFIFO(edge)):
            return(edge[0])
        else:
           return(edge.src)

    # Get the dst port of and egde
    def edgeDstPort(self,edge):
        if not (self.isFIFO(edge)):
            return(edge[1])
        else:
           return(edge.dst)

    # Get port ID on item (input or output)
    def getPort(self,item,i,input=False):
        if input:
            return(item.inputPortFromID(i))
        else:
            return(item.outputPortFromID(i))
          

    ############################
    #
    # NORMAL NODE CUSTOMIZATION
    #

    # fill color value
    def node_color(self,node):
        return(self["node_color"])

    # Boundary color value
    def node_boundary_color(self,node):
        return(self["node_boundary_color"])

    # Color of node label
    def node_label_color(self,node):
        return(self["node_label_color"])

    # Font size of node label
    def node_label_size(self,node):
        return(self["node_label_size"])

    # Node label
    def node_label(self,node):
        return node.graphvizName

    ############################
    #
    # DELAY NODE CUSTOMIZATION
    #

    # fill color value
    def delay_color(self,delay_value):
        return(self["node_color"])

    # delay boundary thickness
    def delay_border(self,delay_value):
        return(self["special_node_border"])

    # delay boundary color
    def delay_boundary_color(self,delay_value):
        return(self["node_boundary_color"])

    # delay label color
    def delay_label_color(self,delay_value):
        return(self["node_label_color"])

    # delay label size
    def delay_label_size(self,delay_value):
        return(self["node_label_size"])

    ############################
    #
    # CONST NODE CUSTOMIZATION
    #

    # fill color value
    def const_color(self,const_name):
        return(self["node_color"])

    # const node boundary thickness
    def const_border(self,const_name):
        return(self["special_node_border"])

    # const node boundary color
    def const_boundary_color(self,const_name):
        return(self["node_boundary_color"])

    # const label color
    def const_label_color(self,const_name):
        return(self["node_label_color"])

    # const label font size
    def const_label_size(self,const_name):
        return(self["node_label_size"])

    
    ############################
    #
    # NODE PORT CUSTOMIZATION
    #

    # Color of number of samples produced or
    # consumed by a port
    def port_sample_color(self,nb_sample):
        return(self["port_sample_color"])

    # Font size of number of samples produced or
    # consumed by a port
    def port_sample_font_size(self,nb_sample):
        return(self["port_sample_font_size"])

    # Color of a port
    def port_font_color(self,item,i,input=False):
        # item.outputNames[0]
        return(self["port_font_color"])

    # Font size of a port
    def port_font_size(self,item,i,input=False):
        return(self["port_font_size"])

    ############################
    #
    # NORMAL EDGE CUSTOMIZATION
    #

    # edge color
    def edge_color(self,edge):
        return(self["edge_color"])

    # edge label color
    def edge_label_color(self,edge):
        return(self["edge_label_color"])

    # edge label font size
    def edge_label_size(self,edge):
        return(self["edge_label_size"])

    # edge style (dashed, dotted ...)
    def edge_style(self,edge):
        return(self["edge_style"])

    # Label on the edge
    # (By default datatype and number of samples)
    def edge_label(self,fifo,typeName,length):
        if length > 1:
           return f"{typeName}({length})"
        else:
           return f"{typeName}"

    ############################
    #
    # DELAY EDGE CUSTOMIZATION
    #

    # edge color for the edge entering a delay box
    def delay_edge_color(self,srcPort,nb_samples):
        return(self["edge_color"])

    # edge style for the edge entering the delay box
    # (dashed, dotted ...)
    def delay_edge_style(self,srcPort,nb_samples):
        return(self["edge_style"])

    ############################
    #
    # CONST EDGE CUSTOMIZATION
    #

    # Color of edge connecting a constant node
    def const_edge_color(self,name,dstPort):
        return(self["edge_color"])

    # Style of edge connecting a constant node
    # (dashed, dotted)
    def const_edge_style(self,name,dstPort):
        return(self["edge_style"])

# Generate a graph before schedule computation
# It is working with a graph that ha sno FIFOs but only
# edges (pair of input / output ports)
def gen_precompute_graph(g,f,config,style=Style.default_style()):
    env = Environment(
       loader=PackageLoader("cmsis_stream.cg.scheduler"),
       autoescape=select_autoescape(),
       trim_blocks=True
    )

    constObjs = list(set([x[0] for x in g.constantEdges]))
    template = env.get_template("precompute_dot_template.dot")

    nbFifos = len(g.edges)

    print(template.render(style=style,
      graph=g,
      nodes=g.nodes,
      edges=g.edges,
      fifos=g.edges,
      nbFifos=nbFifos,
      constEdges=g.constantEdges,
      nbConstEdges=len(g.constantEdges),
      constObjs=constObjs,
      config=config
      ),file=f)

# Work with a graph after schedule computation
# It has access to schedule information and FIFOs
def gengraph(sched,f,config,style=Style.default_style()):

    env = Environment(
       loader=PackageLoader("cmsis_stream.cg.scheduler"),
       autoescape=select_autoescape(),
       trim_blocks=True
    )
    
    constObjs = list(set([x[0] for x in sched.constantEdges]))
    template = env.get_template("dot_template.dot")

    nbFifos = len(sched._graph._allFIFOs)

    print(template.render(style=style,
      graph=sched,
      nodes=sched.nodes,
      edges=sched.edges,
      fifos=sched._graph._allFIFOs,
      nbFifos=nbFifos,
      constEdges=sched.constantEdges,
      nbConstEdges=len(sched.constantEdges),
      constObjs=constObjs,
      config=config
      ),file=f)