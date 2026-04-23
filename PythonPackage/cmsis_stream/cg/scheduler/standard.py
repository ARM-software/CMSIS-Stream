###########################################
# Project:      CMSIS DSP Library
# Title:        standard.py
# Description:  Standard nodes to describe a network
# 
# 
# Target Processor: Cortex-M and Cortex-A cores
# -------------------------------------------------------------------- */
# 
# Copyright (C) 2021-2026 ARM Limited or its affiliates. All rights reserved.
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
"""Standard nodes available to describe a network in addition to the generic nodes"""

from ..types import *
from .node import GenericNode,GenericToManyNode,GenericSource,GenericSink, joinit
from copy import deepcopy

floatType=CType(F32)

# Generic duplicate node with list output to
# support as many outputs as required.
# All outputs have same type and data length

def numberToBase(n, b):
    if n == 0:
        return [0]
    digits = []
    while n:
        digits.append(int(n % b))
        n //= b
    return digits[::-1]



# Duplicate node is working with a list of output nodes
# in the C generated code so that we don't have to provide
# different templates for different number of outputs.
# As consequence, the formatting of the arguments in the
# generated code must be modified.
# The template only use one arguments covering all the list
# And finally the arguments naming is following alphabetical
# order
class Duplicate(GenericToManyNode):
    def __init__(self,name,theType,inLength,nb,className="Duplicate"):
        GenericToManyNode.__init__(self,name,identified=False)

        self._className = className

        outputType = theType.share()

        self.addInput("i",theType,inLength)
        self.addManyOutput(outputType,inLength,nb)

    # Naming of output from index
    # They are in alphabetical order
    # so that the index used here is also the
    # index in the C code
    def outputNameFromIndex(self,i):
        r="".join([chr(x+ord('a')) for x in numberToBase(i,26)]).rjust(6,"a")
        return(r)


    @property 
    def isDuplicateNode(self) -> bool:
        return True

    @property
    def typeName(self):
        return ("%s" % self._className)














        