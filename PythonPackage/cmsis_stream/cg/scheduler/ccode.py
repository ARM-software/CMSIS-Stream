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

def mkNodeIdentifications(config,sched):
   idents=[]
   nb = 0
   for n in sched.nodes:
      if n.identified:
         name = f"{config.prefix.upper()}{n.nodeName.upper()}_ID"
         n.identificationName = name
         idents.append((name,nb))
         nb = nb + 1

   return(idents)


def gencode(sched,directory,config):


    env = Environment(
       loader=PackageLoader("cmsis_stream.cg.scheduler"),
       autoescape=select_autoescape(),
       trim_blocks=True
    )
    
    schedDescription=""

    if isinstance(config.cOptionalArgs,list):
       spc = " " * 30
       config.cOptionalArgs = f",\n{spc}".join(config.cOptionalArgs)

    # Asychronous implies code array and switchCase
    if config.asynchronous:
       config.switchCase = True
       config.memoryOptimization = False

    identifiedNodes = []
    if config.nodeIdentification:
       config.heapAllocation = True
       r = mkNodeIdentifications(config,sched)
       identifiedNodes = r

    if config.switchCase:
       ctemplate = env.get_template("codeSwitch.cpp")
       nb = 0
       for s in sched.schedule:
         schedDescription = schedDescription + ("%d," % sched.nodes[s].codeID)
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
    if len(sched.schedule) <=255:
       schedSwitchDataType = "uint8_t"

    with open(cfile,"w") as f:
         print(ctemplate.render(fifos=sched._graph._allFIFOs,
            nbFifos=nbFifos,
            nbNodes=len(sched.nodes),
            nodes=sched.nodes,
            schedule=sched.schedule,
            schedLen=len(sched.schedule),
            schedSwitchDataType=schedSwitchDataType,
            config=config,
            sched=sched,
            schedDescription=schedDescription,
            identifiedNodes=identifiedNodes
            ),file=f)

    with open(hfile,"w") as f:
         print(htemplate.render(fifos=sched._graph._allFIFOs,
            nbFifos=nbFifos,
            nodes=sched.nodes,
            schedule=sched.schedule,
            config=config,
            sched=sched,
            identifiedNodes=identifiedNodes
            ),file=f)

   