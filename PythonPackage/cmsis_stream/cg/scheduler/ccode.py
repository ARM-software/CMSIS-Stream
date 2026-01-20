###########################################
# Project:      CMSIS DSP Library
# Title:        ccode.py
# Description:  C++ code generator for scheduler
# 
# $Date:        29 July 2021
# $Revision:    V1.10.0
# 
# Target Processor: Cortex-M and Cortex-A cores
# -------------------------------------------------------------------- */
# 
# Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
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
import os.path
import pathlib
from .config import *

def selector_define_name(sel):
    return f"SEL_{sel.upper()}_ID"

def selectors_for_node(node):
   r = []
   for selector in node.selectors:
      r.append(selector_define_name(selector))
   return r 

def init_args(sched,n):
   args = n.args
   if args:
      return f"({args})"
   else:
      return ""
   
def mk_selector_inits(sched):
   sels=dict(sched._selector_inits)
   
   l = []
   for s in sels:
      r = sels[s]
      if r["selectors"]:
         selString = ", ".join([str(x) for x in r["selectors"]])
         if r["isTemplate"]:
            l.append(f"template<>\nstd::array<uint16_t,{len(r["selectors"])}> {s}::selectors = {{{selString}}};")
         else:
            l.append(f"std::array<uint16_t,{len(r["selectors"])}> {s}::selectors = {{{selString}}};")
   return "\n".join(l)
   #print(sels)


def selector_defines(sched):
   r = []
   for selector in sched.selectorsID:
         name = selector_define_name(selector)
         r.append(f"#define {name} {sched.selectorsID[selector]} ")
   return "\n".join(r)

def mkPublishers(config,sched):
   p = []
   n = {}
   for id,node in enumerate(sched.allNodes):
      n[node] = id 
   
   for event_edge in sched._eventConnections:
      src_port = event_edge[0]
      dst_port = event_edge[1]
      src_node = src_port.owner
      p.append(src_node)
   return(n,p)


def gencode(sched,directory,config):


    env = Environment(
       loader=PackageLoader("cmsis_stream.cg.scheduler"),
       autoescape=select_autoescape(),
       trim_blocks=True
    )
    
    schedDescription=""

    if isinstance(config.cOptionalInitArgs,list):
       spc = " " * 30
       config.cOptionalInitArgs = f",\n{spc}".join(config.cOptionalInitArgs)

    if isinstance(config.cOptionalExecutionArgs,list):
       spc = " " * 30
       config.cOptionalExecutionArgs = f",\n{spc}".join(config.cOptionalExecutionArgs)

    if isinstance(config.cOptionalFreeArgs,list):
       spc = " " * 30
       config.cOptionalFreeArgs = f",\n{spc}".join(config.cOptionalFreeArgs)

    
    # Asynchronous implies code array and switchCase
    if config.asynchronous or config.fullyAsynchronous:
       config.memoryOptimization = False
       # Switch case forced otherwise we cannot
       # do a pre-check for FIFO overflow or 
       # underflow in async mode
       config.switchCase = True

   # callback implies switchcase since we need to be able
   # to restore / save the state machine
   # Since the scheduler state machine is called several times
   # the FIFO and objects must be created before
    if config.callback:
       config.switchCase = True
       config.heapAllocation = True

    identifiedNodes = []
    if config.nodeIdentification:
       config.heapAllocation = True
       identifiedNodes = sched.nodeIdentification

    if config.switchCase:
       ctemplate = env.get_template("codeSwitch.cpp")
       nb = 0
       for s in sched.schedule:
         schedDescription = schedDescription + ("%d," % sched.allNodes[s].codeID)
         nb = nb + 1
         if nb == 40:
            nb=0 
            schedDescription = schedDescription + "\n"
    else:
       ctemplate = env.get_template("code.cpp")
    htemplate = env.get_template("code.h")


    cfile=os.path.join(directory,"%s.cpp" % config.schedulerCFileName)
    hfile=os.path.join(directory,"%s.h" % config.schedulerCFileName)

    nbFifos = len(sched._graph._allFIFOs)

    schedSwitchDataType = "uint16_t"
    if len(sched.schedule) <= 255:
       schedSwitchDataType = "uint8_t"

    node_to_id,publishers = mkPublishers(config,sched)

    selector_inits = mk_selector_inits(sched)


    with open(cfile,"w") as f:
         print(ctemplate.render(fifos=sched._graph._allFIFOs,
            nbFifos=nbFifos,
            nbAllNodes=len(sched.allNodes),
            nbStreamNodes=len(sched.streamNodes),
            nbEventNodes=len(sched.eventNodes),
            allNodes=sched.allNodes,
            streamNodes=sched.streamNodes,
            eventNodes=sched.eventNodes,
            schedule=sched.schedule,
            schedLen=len(sched.schedule),
            schedSwitchDataType=schedSwitchDataType,
            config=config,
            sched=sched,
            schedDescription=schedDescription,
            identifiedNodes=identifiedNodes,
            node_to_id=node_to_id,
            publishers=publishers,
            init_args=init_args,
            selector_inits=selector_inits,
            eventConnections=sched._eventConnections
            ),file=f)

    with open(hfile,"w") as f:
         print(htemplate.render(fifos=sched._graph._allFIFOs,
            nbFifos=nbFifos,
            allNodes=sched.allNodes,
            schedule=sched.schedule,
            config=config,
            sched=sched,
            schedLen=len(sched.schedule),
            identifiedNodes=identifiedNodes,
            selector_defines=selector_defines(sched),
            ),file=f)

   