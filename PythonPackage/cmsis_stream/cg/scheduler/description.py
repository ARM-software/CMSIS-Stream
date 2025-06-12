###########################################
# Project:      CMSIS DSP Library
# Title:        description.py
# Description:  Schedule generation
# 
# 
# Target Processor: Cortex-M and Cortex-A cores
# -------------------------------------------------------------------- */
# 
# Copyright (C) 2021-2025 ARM Limited or its affiliates. All rights reserved.
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
"""Description of the graph"""

import networkx as nx
import numpy as np 
import math 

from sympy import Matrix
from sympy.core.numbers import ilcm,igcd

from .graphviz import gengraph,gen_precompute_graph,Style
from .ccode import gencode as c_gencode
from .pythoncode import gencode as p_gencode

from .node import *
from .config import *
from .standard import Duplicate

from ..types import *

# To debug graph coloring for memory optimization
#import matplotlib.pyplot as plt

class IncompatibleIO(Exception):
    def __init__(self,namea,nameb,ioa,iob,srcType,dstType):
       Exception.__init__(self)
       self._srcType=srcType
       self._dstType=dstType
       self._namea = namea
       self._nameb = nameb
       self._ioa = ioa
       self._iob = iob

    def __str__(self):
        return(f"{self._namea}.{self._ioa}:{self._srcType.ctype} -> {self._nameb}.{self._iob}:{self._dstType.ctype}")


class GraphIsNotConnected(Exception):
    pass

class NotSchedulableError(Exception):
    pass

class DeadlockError(Exception):
    pass

class CannotDelayConstantError(Exception):
    pass

class TooManyNodesOnOutput(Exception):
    pass

class FIFOClassCannotBeString(Exception):
    def __init__(self,s):
        self._s = s 

    def __str__(self):
        return f"""
The FIFO class customization cannot be a string anymore. A Python class must 
be provided.
The Python class is used to describe the C++ FIFO (C++ class and additional
constructor arguments). The Python class must follow API of StreamFIFO.
The error occured when trying to use string "{self._s}" for a FIFO class"""

# Trying to use a FIFO as a real FIFO (no array)
# when a buffer is assigned to the FIFO and the buffer must be used
# # as an array
class FIFOWithCustomBufferMustBeUsedAsArray(Exception):
    def __init__(self,fifo):
        self._fifo = fifo

    def __str__(self):
        return(f"{self._fifo.src.owner.nodeName} -> {self._fifo.dst.owner.nodeName}")


# Trying to connect two IO with incompatible buffer constraints
class BufferConstraintOnIOAreIncompatibles(Exception):
    def __init__(self,src,dst):
        self._src = src
        self._dst = dst

    def __str__(self):
        return(f"{self._src.owner.nodeName} -> {self._dst.owner.nodeName}")

# Trying to insert a duplicate node when there is a constraint on the
# FIFO that is not coming from an IO
# Then, it is not possible to know where the constraint must be applied 
# once the duplicate node has been inserted
class CantHaveBufferConstraintOnFIFOWhenDuplicateIsInserted(Exception):
    def __init__(self,src,dst):
        self._src = src
        self._dst = dst

    def __str__(self):
        return(f"{self._src.owner.nodeName} -> {self._dst.owner.nodeName}")

class CannotReuseCustomBufferMoreThanOnce(Exception):
    def __init__(self,name):
        self._name = name

    def __str__(self):
        return(f"custom buffer: {self._name}")

class DupDestination:
    def __init__(self,
                 node,
                 delay,
                 fifoClass,
                 fifoScale,
                 fifoAsyncLength,
                 fifoWeak):
        self.node = node
        self.delay = delay 
        self.fifoClass = fifoClass 
        self.fifoScale = fifoScale 
        self.fifoAsyncLength = fifoAsyncLength 
        self.fifoWeak = fifoWeak 


# Description of how to generate the FIFO in the C++ code 
class StreamFIFO:
    def __init__(self,the_type,length):
        self._ctype = the_type
        self._length = length 

    @property
    def cname(self):
        return "FIFO"
    
    @property
    def args(self):
        # No additional arguments by default
        return []

class FifoBuffer:
    """Buffer used by a FIFO"""
    def __init__(self,bufferID,theType,length):
        self._length=length 
        self._theType=theType 
        self._bufferID=bufferID

class FIFODesc:
    """A FIFO connecting two nodes"""
    def __init__(self,fifoid,fifoClass,fifoScale,fifoAsyncLength=0,weak=False,customBuffer=None):
        # The FIFO is in fact just an array
        self.isArray=False 
        # FIFO length
        self.length=0
        # FIFO type
        self.theType=None 
        # Buffer used by FIFO (allocated by the scheduler)
        self.buffer=None 
        # Custom buffer assigned to FIFO 
        self.customBuffer = customBuffer
        # Used for plot in graphviz
        self.bufferID=-1
        self._fifoID=fifoid 
        # Source IO
        self.src = None 
        # Dest IO
        self.dst = None 
        # FIFO delay
        self.delay=0
        self.fifoClass = fifoClass
        self.fifoScale = fifoScale
        # Used for fully asynchronous mode
        self.fifoAsyncLength = fifoAsyncLength
        self.weak=weak

        # Used for liveliness analysis
        # To share buffers between FIFO in memory optimization
        # mode, we need to know when a FIFO is in use.
        # We compute the maximum extent : so the biggest interval
        # and not a disconnected union of intervals
        # This could be improved. We could use
        # a disjoint union of intervals but they should be mapped
        # to the same node in the interference graph
        self._liveInterval=(-1,-1)

        # shared buffer number not yet allocated
        self.sharedNB=-1

        # When true, this fifo is ignore in the output
        # arguments of the C duplicate node
        self._skip_for_duplicate = False

        # FIFO class description used for code generation
        # By default an instance of StreamFIFO
        # Create and memoized when generating the C code from the
        # Jinja template
        self._fifo_desc_for_code = None


    def _mk_fifo_desc(self):
        if self._fifo_desc_for_code is None:
            if isinstance(self.fifoClass,str):
               raise FIFOClassCannotBeString(self.fifoClass)

            self._fifo_desc_for_code = self.fifoClass(self.theType,self.length)


    

    # For c code generation 
    @property
    def isArrayAsInt(self):
        if self.isArray:
            return(1)
        else:
            return(0)

    @property
    def hasDelay(self):
        return(self.delay>0)

    def dump(self):
        
        print("array %d len %d %s id %d src %s:%s dst %s:%s  " % 
            (self.isArray,
             self.length,
             self.theType.ctype,
             self.fifoID,
             self.src.owner.nodeID,
             self.src.name,
             self.dst.owner.nodeID,
             self.dst.name))

    @property
    def fifoID(self):
        return self._fifoID
    
    def recordWrite(self,theTime):
        start,stop=self._liveInterval  
        if start==-1:
            self._liveInterval=(theTime,stop)

    def recordRead(self,theTime):
        start,stop=self._liveInterval 
        if (theTime > stop):
            self._liveInterval=(start,theTime)

    def bufName(self,config):
        if not self.customBuffer is None:
            if self.customBuffer.assignedByNode:
                return("nullptr")
            else:
               return(self.customBuffer.name)
        if config.bufferAllocation:
           return(f"buffers.buf{self.buffer._bufferID}")
        else:
           return(f"{config.prefix}buf{self.buffer._bufferID}")

    @property
    def fifo_class_str (self):
        self._mk_fifo_desc()
        return f"{self._fifo_desc_for_code.cname}"

    @property
    def fifo_additional_args (self):
        self._mk_fifo_desc()
        if not self._fifo_desc_for_code.args:
            return ""
        else:
            r = ",".join(self._fifo_desc_for_code.args)
            return f",{r}"

    # When there are additional args, we need to generate
    # the delay argument even when it has its default value of 0
    # So, this is checked in the Jijna template
    @property 
    def hasAdditionalArgs(self):
        self._mk_fifo_desc()
        return len(self._fifo_desc_for_code.args)>0


def analyzeStep(vec,allFIFOs,theTime):
    """Analyze an evolution step to know which FIFOs are read and written to"""
    fifoID = 0 
    for fifo in (vec > 0):
        if fifo:
            allFIFOs[fifoID].recordWrite(theTime) 
        fifoID = fifoID + 1

    fifoID = 0 
    for fifo in (vec < 0):
        if fifo:
            allFIFOs[fifoID].recordRead(theTime) 
        fifoID = fifoID + 1

class Graph():

    def __init__(self):
        self._nodes={}
        self._edges={}
        self._delays={}
        self._constantEdges={}
        self._g = nx.Graph()
        self._sortedNodes=None
        self._totalMemory=0
        self._allFIFOs = None 
        self._allBuffers = None
        self._FIFOClasses = {}
        # In async mode, scaling factor for a given
        # FIFO to override the global scaling factor
        self._FIFOScale = {}
        # In fully asynchronous the fifo length
        # and weak state
        self._FIFOAsyncLength = {}
        self._FIFOWeak = {}
        # If FIFO must be mapped on a custom buffer
        # It prevents memory optimization from bein applied to
        # this FIFO
        self._FIFOCustomBuffer = {}
        # If FIFO constraint is inherited from an IO then duplicate node
        # can be inserted since we know how to reapply the constraint
        self._InheritedFIFOConstraint = {}
        # Topological sorting of nodes
        # computed during topology matrix
        # and used for some scheduling
        # (prioritizing the scheduling of nodes
        # close to the graph output)
        self._topologicalSort=[]
        self.defaultFIFOClass = StreamFIFO

         # Prefix used to generate the class names
        # of the duplicate nodes like Duplicate2,
        # Duplicate3 ...
        self.duplicateNodeClassName = "Duplicate"


    def graphviz(self,f,config=Configuration(),style=Style.default_style()):
        """Write graphviz into file f""" 
        gen_precompute_graph(self,f,config,style=style)

    def computeSourceTopologicalSortOfNodes(self):
        remaining = self._sortedNodes.copy()

        while len(remaining)>0:
            toremove = []
            for n in remaining:
                if n.nbInputsForTopologicalSort == 0:
                    toremove.append(n)

            if (len(toremove)==0):
                # There are some loops so topological sort is
                # not possible 
                self._topologicalSort = [] 
                return


            self._topologicalSort.append(toremove)
            for x in toremove:
                remaining.remove(x)

            # Update input count for predecessors of the
            # removed nodes 
            for x in toremove:
                for i in x._outputs:
                    theFifo = x._outputs[i].fifo
                    #  Fifo empty means connection to a
                    # constant node so should be ignoredf
                    # for topological sort
                    if len(theFifo)>0:
                       theOutputNodeInput = theFifo[1]
                       theOutputNode = theOutputNodeInput.owner
                       theOutputNode.nbInputsForTopologicalSort = theOutputNode.nbInputsForTopologicalSort - 1
        #print(self._topologicalSort)

    def computeSinkTopologicalSortOfNodes(self):
        remaining = self._sortedNodes.copy()

        while len(remaining)>0:
            toremove = []
            for n in remaining:
                if n.nbOutputsForTopologicalSort == 0:
                    toremove.append(n)

            if (len(toremove)==0):
                # There are some loops so topological sort is
                # not possible 
                self._topologicalSort = [] 
                return


            self._topologicalSort.append(toremove)
            for x in toremove:
                remaining.remove(x)

            # Update output count for predecessors of the
            # removed nodes 
            for x in toremove:
                for i in x._inputs:
                    theFifo = x._inputs[i].fifo
                    #  Fifo empty means connection to a
                    # constant node so should be ignoredf
                    # for topological sort
                    if len(theFifo)>0:
                       theInputNodeOutput = theFifo[0]
                       theInputNode = theInputNodeOutput.owner
                       theInputNode.nbOutputsForTopologicalSort = theInputNode.nbOutputsForTopologicalSort - 1
        #print(self._topologicalSort)


    def connectDup(self,destination,outputIO,theId):
        if (destination[theId].delay!=0):
            self.connectWithDelay(outputIO,destination[theId].node,
                                           destination[theId].delay,
                                           dupAllowed=False,
                                           fifoClass=destination[theId].fifoClass,
                                           fifoScale=destination[theId].fifoScale,
                                           fifoAsyncLength=destination[theId].fifoAsyncLength,
                                           weak=destination[theId].fifoWeak)
        else:
            self.connect(outputIO,destination[theId].node,
                                  dupAllowed=False,
                                  fifoClass=destination[theId].fifoClass,
                                  fifoScale=destination[theId].fifoScale,
                                  fifoAsyncLength=destination[theId].fifoAsyncLength,
                                  weak=destination[theId].fifoWeak
                                  )



    def insertDuplicates(self,config):
        # Insert dup nodes (Duplicate2 and Duplicate3) to
        # ensure that an output is connected to only one input
        dupNb = 0
        # Scan all nodes
        allNodes = list(self._g)
        for n in allNodes:
            # For each nodes, get the IOs
            for k in n._outputs.keys():
                output = n._outputs[k]
                fifo = output.fifo
                # Check if the FIFO list has only 1 element
                # In this case, we convert it into a value
                # since code using the graph is expecting an
                # IO to be connected to one and only one other IO
                # so the FIFO must be a value and not a list
                if len(fifo)==1:
                    # Extract first element of list
                    fifo = fifo[0]
                    fifo[0].fifo = fifo 
                    fifo[1].fifo = fifo
                # If the FIFO has more elements, we need to
                # restructure the graph and add Duplicate nodes
                else:
                    
                    dup = None 
                    # We extract from the IO the nb of produced
                    # samples and the data type
                    # Duplicate will use the same
                    inputSize = output.nbSamples
                    theType = output.theType


                    # We create a duplicate node
                    dup = Duplicate("dup%d" % dupNb,theType,inputSize,len(fifo),className=self.duplicateNodeClassName)

                    #print(dup)



                    dupNb = dupNb + 1
                    # We disconnect all the fifo element (a,b)
                    # and remember the destination b
                    # We must rewrite the edges of self._g
                    # self._edges 
                    # self._nodes
                    # the node fifo 
                    destinations = []
                    delays = []


                    self._sortedNodes = None
                    self._sortedEdges = None
                    for f in fifo:
                        # IO connected to the FIFOs
                        # nodea is the IO belowing to nodea
                        # but not the node itself
                        nodea = f[0]
                        nodeb = f[1]

                        fifoClass = self.defaultFIFOClass
                        fifoScale = 1.0
                        fifoAsyncLength = 0
                        fifoWeak = False
                        
                        if (nodea,nodeb) in self._FIFOClasses:
                            fifoClass = self._FIFOClasses[(nodea,nodeb)]

                        if (nodea,nodeb) in self._FIFOScale:
                            fifoScale = self._FIFOScale[(nodea,nodeb)]

                        if (nodea,nodeb) in self._FIFOAsyncLength:
                            fifoAsyncLength = self._FIFOAsyncLength[(nodea,nodeb)]

                        if (nodea,nodeb) in self._FIFOWeak:
                            fifoWeak = self._FIFOWeak[(nodea,nodeb)]

                        if (nodea,nodeb) in self._FIFOCustomBuffer:
                            if not (nodea,nodeb) in self._InheritedFIFOConstraint or not self._InheritedFIFOConstraint[(nodea,nodeb)]:
                               raise CantHaveBufferConstraintOnFIFOWhenDuplicateIsInserted(nodea,nodeb)
                            #else:
                            #    print(f"{nodea.owner.nodeName} -> {nodeb.owner.nodeName}")

                        if (nodea,nodeb) in self._delays:
                           delay = self._delays[(nodea,nodeb)]
                        else:
                           delay = 0

                        destination=DupDestination(nodeb,
                            delay,
                            fifoClass,
                            fifoScale,
                            fifoAsyncLength,
                            fifoWeak)

                        destinations.append(destination)

                        nodea.fifo=None 
                        nodeb.fifo=None
                        # Since we are not using a multi graph
                        # and there no parallel edges, it will
                        # remove all edges between two nodes.
                        # But some edges may come from other IO
                        # and be valid.
                        # Anyway, those edges are not used
                        # in the algorithm at all
                        # Instead we have our own graph with self._edges
                        # (This script will have to be rewritten in a much
                        # cleaner way)
                        if self._g.has_edge(nodea.owner,nodeb.owner):
                           self._g.remove_edge(nodea.owner,nodeb.owner)

                        del self._edges[(nodea,nodeb)]
                        if (nodea,nodeb) in self._FIFOClasses:
                            del self._FIFOClasses[(nodea,nodeb)]
                        if (nodea,nodeb) in self._FIFOScale:
                            del self._FIFOScale[(nodea,nodeb)]

                        # If we reach this line without an exception it means
                        # the FIFO constraint is inherited from an IO
                        # so it can be removed from the FIFO
                        # and by connection of the nodes to the duplicate one
                        # new constraints on new FIFOs will be created
                        if (nodea,nodeb) in self._FIFOCustomBuffer:
                            del self._FIFOCustomBuffer[(nodea,nodeb)]

                        if (nodea,nodeb) in self._FIFOAsyncLength:
                            del self._FIFOAsyncLength[(nodea,nodeb)]
                        if (nodea,nodeb) in self._FIFOWeak:
                            del self._FIFOWeak[(nodea,nodeb)]
                        

                        if (nodea,nodeb) in self._delays:
                           del self._delays[(nodea,nodeb)]

                        
                    
                    # Now for each fifo destination b_i we need 
                    # create a new
                    # connection dup -> b_i
                    # And we create a new connection a -> dup

                    # a -> dup
                    if config.asynchronous:
                        maxScale = np.max(np.array([dest.fifoScale for dest in destinations]))
                        self.connect(output,dup.i,dupAllowed=False,
                        fifoScale = maxScale)
                    elif config.fullyAsynchronous:
                        maxAsyncLength = np.max(np.array([dest.fifoAsyncLength for dest in destinations]))
                        self.connect(output,dup.i,dupAllowed=False,
                        fifoAsyncLength = maxAsyncLength)
                    else:
                       self.connect(output,dup.i,dupAllowed=False)

                    
                    for i in range(len(destinations)):
                       name = dup.outputNameFromIndex(i)
                       self.connectDup(destinations,dup[name],i)
                        

               

    def connect(self,nodea,nodeb,
        dupAllowed=True,
        fifoClass=None,
        fifoScale = 1.0,
        fifoAsyncLength=0,
        weak=False,
        buffer=None,
        customBufferMustBeArray=True):
        if fifoClass is None:
            fifoClass = self.defaultFIFOClass
        # When connecting to a constant node we do nothing
        # since there is no FIFO in this case
        # and it does not participate to the scheduling.
        if (isinstance(nodea,Constant)):
            nodeb.constantNode = nodea
            self._constantEdges[(nodea,nodeb)]=True
        else:

            if nodea.compatible(nodeb):
                if (not nodea._bufferConstraint is None) and (not nodeb._bufferConstraint is None):
                    if not nodea._bufferConstraint.compatibleWith(nodeb._bufferConstraint):
                        raise BufferConstraintOnIOAreIncompatibles(nodea,nodeb)
                

                self._sortedNodes = None
                self._sortedEdges = None
                self._g.add_edge(nodea.owner,nodeb.owner)
    
                if dupAllowed:
                   nodea.fifo.append((nodea,nodeb))
                   nodeb.fifo.append((nodea,nodeb))
                else:
                   nodea.fifo=(nodea,nodeb)
                   nodeb.fifo=(nodea,nodeb)
                self._edges[(nodea,nodeb)]=True

                self._FIFOClasses[(nodea,nodeb)] = fifoClass
                self._FIFOScale[(nodea,nodeb)] = fifoScale
                self._FIFOAsyncLength[(nodea,nodeb)] = fifoAsyncLength
                self._FIFOWeak[(nodea,nodeb)] = weak
                if not nodea._bufferConstraint is None:
                    self._FIFOCustomBuffer[(nodea,nodeb)] = nodea._bufferConstraint
                    self._InheritedFIFOConstraint[(nodea,nodeb)] = True
                    #print(f"{nodea.owner.nodeName} -> {nodeb.owner.nodeName}")
                elif not nodeb._bufferConstraint is None:
                    self._FIFOCustomBuffer[(nodea,nodeb)] = nodeb._bufferConstraint
                    self._InheritedFIFOConstraint[(nodea,nodeb)] = True
                    #print(f"{nodea.owner.nodeName} -> {nodeb.owner.nodeName}")
                elif not buffer is None:
                    self._FIFOCustomBuffer[(nodea,nodeb)] = BufferConstraint(name=buffer,mustBeArray=customBufferMustBeArray)
                    self._InheritedFIFOConstraint[(nodea,nodeb)] = False


                if not (nodea.owner in self._nodes):
                   self._nodes[nodea.owner]=True
                if not (nodeb.owner in self._nodes):
                   self._nodes[nodeb.owner]=True
 
            else:
                raise IncompatibleIO(nodea.owner.nodeName,
                                     nodeb.owner.nodeName,
                                     nodea.name,nodeb.name,
                                     nodea.theType,nodeb.theType)

    def connectWithDelay(self,nodea,nodeb,delay,
        dupAllowed=True,
        fifoClass=None,
        fifoScale=1.0,
        fifoAsyncLength=0,
        weak=False,
        buffer=None,
        customBufferMustBeArray=True):
        if fifoClass is None:
            fifoClass = self.defaultFIFOClass
        # We cannot connect with delay to a constant node
        if (isinstance(nodea,Constant)):
            raise CannotDelayConstantError
        else:
            self.connect(nodea,nodeb,
                dupAllowed=dupAllowed,
                fifoClass=fifoClass,
                fifoScale = fifoScale,
                fifoAsyncLength=fifoAsyncLength,
                weak=weak,
                buffer=buffer,customBufferMustBeArray=customBufferMustBeArray)
            self._delays[(nodea,nodeb)] = delay
    
    def __str__(self):
        res=""
        for (a,b) in self._edges: 
            nodea = a.owner
            nodeb = b.owner 

            res += ("%s.%s -> %s.%s\n" % (nodea.nodeID,a.name, nodeb.nodeID,b.name))

        return(res)

    def no_exception_for_duplicate(self,config,k,f):
        if config.disableDuplicateOptimization:
            return(True)
        if not k.isArray:
            return(True)
        if not f.isArray:
            return(True)


        

        # We check if k and f are related to same duplicate
        if k.src.owner == f.src.owner and isinstance(k.src.owner,Duplicate):
            # No interference between k and f FIFO buffers
            # They both are output FIFO of same duplicate node
            # In this case we must also ensure that the input FIFO of duplicate node
            # is working in array mode otherwise we still need to make a copy.
            
            srcNode = k.src.owner
            inputFifo = srcNode._inputs['i']._fifo
            fifoDescInput = self._edgeToFIFO[inputFifo]

            duplicate_exception = fifoDescInput.isArray

            # We must check they don't interfere with input
            # stop <= start
            no_src_inteferenceA = fifoDescInput._liveInterval[1] <= k._liveInterval[0]
            duplicate_exception = duplicate_exception and no_src_inteferenceA

            no_src_inteferenceB = fifoDescInput._liveInterval[1] <= f._liveInterval[0]
            duplicate_exception = duplicate_exception and no_src_inteferenceB

            #print(f"{fifoDescInput.src.owner.nodeName} -> dup -> {k.dst.owner.nodeName},{f.dst.owner.nodeName} : {not duplicate_exception}")

            return(not duplicate_exception)

        if k.dst.owner == f.src.owner and isinstance(k.dst.owner,Duplicate):
            # No interference between k and f FIFO buffers
            # They are both connected to Duplicate node and don't overlap in time
            duplicate_exception = k._liveInterval[1] <= f._liveInterval[0]
            #print(f"{f.dst.owner.nodeName} <- dup <- {k.src.owner.nodeName} : {not duplicate_exception}")

            return(not duplicate_exception)
        if k.src.owner == f.dst.owner and isinstance(f.dst.owner,Duplicate):
            # No interference between k and f FIFO buffers
            # They are both connected to Duplicate node and don't overlap in time
            duplicate_exception = f._liveInterval[1] <= k._liveInterval[0]
            #print(f"{f.src.owner.nodeName} -> dup -> {k.dst.owner.nodeName} : {not duplicate_exception}")

            return(not duplicate_exception)

        #print(f"{k.src.owner.nodeName} -> {k.dst.owner.nodeName}")
        #print(f"  {f.src.owner.nodeName} -> {f.dst.owner.nodeName}")
        return(True)

    def initializeFIFODescriptions(self,config,allFIFOs, fifoLengths,maxTime):
        """Initialize FIFOs datastructure""" 

        # Before we start we check that no custom buffer is used more than once

        customBufferNames = {}
        for edge in self._FIFOCustomBuffer:
            custom = self._FIFOCustomBuffer[edge]
            if not custom.name is None:
                if custom.name in customBufferNames:
                    raise CannotReuseCustomBufferMoreThanOnce(custom.name)
                else:
                    customBufferNames[custom.name] = True

        for fifo in allFIFOs:
            edge = self._sortedEdges[fifo.fifoID]
            if config.asynchronous:
               if edge in self._FIFOScale:
                  fifoScale = self._FIFOScale[edge]
                  fifo.fifoScale = fifoScale
               
               if fifoScale != 1.0:
                  scale = fifoScale
               else:
                 scale = 1.0
                 if type(config.FIFOIncrease) == float:
                    scale = config.FIFOIncrease
                 else:
                    scale = (1.0 + 1.0*config.FIFOIncrease/100)
               fifo.length = int(math.ceil(fifoLengths[fifo.fifoID] * scale))
            elif config.fullyAsynchronous:
                if edge in self._FIFOAsyncLength:
                  fifoAsyncLength = self._FIFOAsyncLength[edge]
                  fifo.length = fifoAsyncLength
                if edge in self._FIFOWeak:
                  fifo.weak = self._FIFOWeak[edge]
            else:
               fifo.length = fifoLengths[fifo.fifoID]
            src,dst = edge
            if edge in self._FIFOClasses:
               fifoClass = self._FIFOClasses[edge]
               fifo.fifoClass = fifoClass
            fifo.src=src
            fifo.dst=dst 

            if edge in self._FIFOCustomBuffer:
                fifo.customBuffer = self._FIFOCustomBuffer[edge]

            # To ensure dst node use same FIFO type as source
            # node. There is no implicit typecast of pointer
            # But the C++ template can typecast on elements
            # so the shared status is imposed by the source
            # and the destination node will adapt through C++
            # when it make senses (like a const input can also 
            # receive a non const pointer)
            dst.theType._shared = src.theType._shared

            fifo.delay=self.getDelay(edge)
            # When a FIFO is working as an array then its buffer may
            # potentially be shared with other FIFOs working as arrays
            # If fifo length is bigger it means we may have
            # double write, double read and so the FIFO is not used
            # as an array.
            # The scheduling try to interleave read / write so this
            # case may nto occur too often.
            if src.nbSamples == dst.nbSamples and src.nbSamples == fifo.length:
                if fifo.delay==0:
                   if not config.asynchronous and not config.fullyAsynchronous:
                      fifo.isArray = True 
            # Type of FIFO is coming from the SRC.
            # It is an important property for the
            # change of type in duplicate insertion
            fifo.theType = src.theType
            #fifo.dump()


        for fifo in allFIFOs:
            if not fifo.customBuffer is None:
                if fifo.customBuffer.mustBeArray and not fifo.isArray:
                    raise FIFOWithCustomBufferMustBeUsedAsArray(fifo)

        bufferID=-1
        allBuffers={}

        # Compute a graph describing when FIFOs are used at the same time
        # Then use graph coloring to allocate buffer to those FIFOs.
        # Then size the buffer based on the longest FIFO using it
        # Memory optimizations are disabled by the asynchronous mode
        # because the execution is no more periodic.
        # So anything deduced from a synchronous period
        # can't be used.
        if config.memoryOptimization and not config.asynchronous and not config.fullyAsynchronous:
            G = nx.Graph()

            for fifo in allFIFOs: 
                    # FIFOs that are not array do not participate in the
                    # memory optimization
                    if fifo.isArray:
                        G.add_node(fifo)

            # FIFO with an assigned external buffer
            # may be interfering with other FIFOs
            # for reasons not related to the scheduling
            for fifo in allFIFOs: 
                # Special interference constraints for FIFO having a specified
                # an imposed buffer
                if not fifo.customBuffer is None:
                    #  If FIFO is not an array it does not participate in
                    # the memory optimization
                    if fifo.isArray:
                        current_length_in_bytes = fifo.theType.bytes * fifo.length
                        for other in allFIFOs:
                            other_length_in_bytes = other.theType.bytes * other.length

                            # If not used as an array the fifo is already interfering
                            # so no need to add an edge
                            # A FIFO also doesn't add an edge to itself
                            if other != fifo and other.isArray:
                                # In case of custom buffer, fifos are not interfering only if
                                # they have same buffer size and other fifo is not using a custom buffer
                                # or is using the same custom buffer and if the buffer
                                # is not assigned by the node.
                                # If it is assigned by the node, then other fifo would not have
                                # access to this buffer.
                                if  (
                                        # If buffer allocated by node other fifos can't access it
                                        # so we can't share this buffer
                                        fifo.customBuffer.assignedByNode or
                                        # If user does not want to share this buffer then we add interference 
                                        not fifo.customBuffer.canBeShared 
                                        # Assigned buffer cannot be resized so we only accept to share
                                        # with buffers of same size
                                        or (current_length_in_bytes != other_length_in_bytes) 
                                        # If other fifo declare a constraint it must be the same buffer
                                        or (other.customBuffer and (fifo.customBuffer != other.customBuffer))
                                    ):
                                    if not G.has_edge(fifo,other) and not G.has_edge(other,fifo):
                                         # In any other case we add an interference edge
                                         # The two FIFOs cannot share a buffer
                                         G.add_edge(fifo,other)

            # Create the interference graph due to scheduling constraints

            # Dictionary of active FIFOs at a given time.
            # The time is a scheduling step
            active={}
            currentTime=0
            while currentTime<=maxTime:
                # Remove fifo no more active.
                # Their stop time < currentTime
                toDelete=[]
                for k in active:
                    start,stop=k._liveInterval 
                    if stop<currentTime:
                        toDelete.append(k)
                for k in toDelete:
                    del active[k]   
    
                # Check FIFOs becoming active.
                # They are added to the active list
                # and an interference edge is added between thus FIFO
                # and all the FIFOs active at same time.
                for fifo in allFIFOs: 
                    if fifo.isArray:
                        start,stop=fifo._liveInterval
                        # If a src -> node -> dst
                        # At time t, node will read for src and the stop time
                        # will be currentTime t because once read
                        # the buffer can be reused.
                        # And it will write to dst and the start time will be
                        # currentTime because once written the buffer
                        # cannot be used again until it has been read.
                        # So, src and dst are both live at this time.
                        # Which means the condition on the stop time must be 
                        # stop >= currentTime and not a strict comparison
                        if start<=currentTime and stop >= currentTime:
                            if not (fifo in active):
                                for k in active:
                                    # Interference edge between two FIFOs is they
                                    # are active at same time
                                    # with some exception for duplicate nodes
                                    if not G.has_edge(k,fifo) and not G.has_edge(fifo,k):
                                       # if the connection is with output of a duplicate node
                                       # and if fifo isArray then we do not add an interference edge
                                       # in all cases
                                       if self.no_exception_for_duplicate(config,k,fifo):
                                          G.add_edge(k,fifo)
                                          #print(f"{k.src.owner.nodeName} -> {k.dst.owner.nodeName} ; {fifo.src.owner.nodeName} -> {fifo.dst.owner.nodeName}")
                                active[fifo]=True 
    
                currentTime = currentTime + 1

            # To debug and display the interference graph
            if False:
               import matplotlib.pyplot as plt
               labels={}
               for n in G.nodes:
                  labels[n]="%s -> %s" % (n.src.owner.nodeName,n.dst.owner.nodeName)
       
               pos = nx.spring_layout(G, seed=3113794652)
               #subax1 = plt.subplot(121)
               nx.draw_networkx_nodes(G, pos)

               nx.draw_networkx_edges(G, pos, width=1.0, alpha=0.5)
               
               nx.draw_networkx_labels(G, pos, labels, font_size=10)
               plt.show()
               quit()

        
            # Graph coloring
            d = nx.coloring.greedy_color(G, strategy=config.memStrategy)

            # Allocate the colors (buffer ID) to the FIFO
            # and keep track of the max color number
            # Since other buffers (for real FIFOs) will have their
            # numbering start after this one.
            # In case of buffer constraint, buffer ID is not used
            # and instead the custom buffer name is used
            
            # Remap to continuous buffer number starting from 0
            # since the Jinja templates assume that buffers are
            # continuous
            continuousNumber = 0
            # May contain an integer when buffer allocated by CMSIS-Stream
            # Otherwise contains a BufferConstraint when buffer is allocated
            # outside of stream
            sharedToContinuous={}
            # Scan first for custom buffers
            for fifo in d:
                if not fifo.customBuffer is None:
                    if not d[fifo] in sharedToContinuous:
                       sharedToContinuous[d[fifo]] = fifo.customBuffer
                       fifo.sharedNB=sharedToContinuous[d[fifo]]

            # Scan for other fifos
            for fifo in d:
                if fifo.customBuffer is None:
                   if not d[fifo] in sharedToContinuous:
                       sharedToContinuous[d[fifo]] = continuousNumber
                       continuousNumber = continuousNumber + 1
                   
                   # If this color ID is already in the dictionary and is an
                   # int then we select the numbered buffer allocated by
                   # CMSIS-STream
                   if isinstance(sharedToContinuous[d[fifo]], int):
                      fifo.sharedNB=sharedToContinuous[d[fifo]]
                      bufferID=max(bufferID,fifo.sharedNB)
                   # otherwise we select th custom buffer
                   else:
                      fifo.customBuffer = sharedToContinuous[d[fifo]]
                   
            # Compute the max size for each shared buffer allocated by CMSIS-Stream
            maxSizes={} 
            for fifo in d:
                # No custom buffer assigned so can use a shared buffer
                if fifo.customBuffer is None:
                   lengthInBytes = fifo.theType.bytes * fifo.length
                   if fifo.sharedNB in maxSizes:
                       maxSizes[fifo.sharedNB] = max(maxSizes[fifo.sharedNB],lengthInBytes) 
                   else:
                       maxSizes[fifo.sharedNB]=lengthInBytes

            # Create the buffers
            for theID in maxSizes:
              sharedA = FifoBuffer(theID,CType(UINT8),maxSizes[theID])
              allBuffers[theID]=sharedA

        # bufferID must start after all shared buffers
        bufferID = bufferID + 1
        for fifo in allFIFOs:
            # Use shared buffer if memory optimization
            if fifo.isArray and (config.memoryOptimization and not config.asynchronous and not config.fullyAsynchronous):
                # If fifo is not using a custom buffer then it uses
                # a buffer allocated by CMSIS-Stream
                if fifo.customBuffer is None:
                      fifo.buffer=allBuffers[fifo.sharedNB] 
                      fifo.bufferID=fifo.sharedNB
            # Create a new buffer for a real FIFO
            # Use bufferID which is starting after the numbers allocated
            # to shared buffers
            else:
                # It applies when no memory optimization or when the
                # fifo is not used as an array and so was not handled
                # in the buffer allocations above.
                if fifo.customBuffer is None:
                   buf = FifoBuffer(bufferID,fifo.theType,fifo.length)
                   allBuffers[bufferID]=buf
                   fifo.buffer=buf
                   fifo.bufferID = bufferID
                   bufferID = bufferID + 1

        allBuffers = allBuffers.values()

        # Compute the total memory used in bytes (not including custom buffers
        # that are not allocated by CMSIS-Stream)
        self._totalMemory = 0
        for buf in allBuffers:
            self._totalMemory = self._totalMemory + buf._theType.bytes * buf._length

        # Now we scan the duplicate nodes and check when output fifo is
        # sharing buffer with input fifo
        # In this case, we mark this fifo for skipping in the duplicate node
        # argument generation
        # We have also the case when several output fifos are sharing the same
        # buffer but it is not the input buffer.
        # In this case we should only copy one fifo (not yet done)
        if not config.disableDuplicateOptimization:
            for n in self._sortedNodes:
                if isinstance(n,Duplicate):
                    inputEdge = n._inputs['i']._fifo
                    inputFIFO = self._edgeToFIFO[inputEdge]
                    inputBuf = inputFIFO.buffer
    
                    list_to_process = list(n._outputs).copy()
                    for o in n._outputs:
                        outputEdge = n._outputs[o]._fifo
                        outputFIFO = self._edgeToFIFO[outputEdge]
                        outputBuf = outputFIFO.buffer
                        if outputBuf == inputBuf:
                            outputFIFO._skip_for_duplicate = True
                            list_to_process.remove(o)
    
                    # Now we organize remaining list per buffer and  the first element
                    # of each group will do the copy.
                    buf_groups = {}
                    for a in list_to_process:
                        outputEdge = n._outputs[a]._fifo
                        outputFIFO = self._edgeToFIFO[outputEdge]
                        outputBuf = outputFIFO.buffer
                        if outputBuf in buf_groups:
                            buf_groups[outputBuf].append(outputFIFO)
                        else:
                            buf_groups[outputBuf]=[outputFIFO]
                    # Now we skip for duplicate all elements of each group except the first
                    for k in buf_groups:
                        for l in buf_groups[k][1:]:
                            l._skip_for_duplicate = True
    
        #for fifo in allFIFOs:
        #    fifo.dump()
        return(allBuffers)




    @property
    def constantEdges(self):
        return list(self._constantEdges.keys())
    
    @property
    def nodes(self):
        return list(self._nodes.keys())

    @property
    def edges(self):
        return list(self._edges.keys())
    

    def hasDelay(self,edge):
        return(edge in self._delays)

    def getDelay(self,edge):
        if self.hasDelay(edge):
           return(self._delays[edge])
        else:
           return(0)

    def checkGraph(self):
        if not nx.is_connected(self._g):
            raise GraphIsNotConnected

    def topologyMatrix(self):
        self.checkGraph()
        # For cyclo static scheduling : compute the node periods
        for n in self.nodes:
            n.computeCyclePeriod()

        rows=[]
        # This is used in schedule generation
        # and all functions must use the same node ordering
        self._sortedNodes = sorted(self.nodes, key=lambda x: x.nodeID)
        # Arbitrary order but used for now
        self._sortedEdges = self.edges.copy()
        #for x in self._sorted:
        #    print(x.nodeID)

        # Compute sorted node ID
        nID = 0
        for node in self._sortedNodes:
            node.sortedNodeID = nID
            node.nbOutputsForTopologicalSort = node.nbOutputs

            node.nbInputsForTopologicalSort = node.nbInputs
            # Remove weak input edges
            # (Used to break loops for topological order)
            nbWeak = 0
            for i in node._inputs:
                edge = node._inputs[i].fifo
                # If it is  a connection to a constant node 
                # or a weak edge it will be ignored for the 
                # topological sort so we don't count it
                if len(edge)==0 or self._FIFOWeak[edge]:
                       nbWeak = nbWeak + 1
            
            node.nbInputsForTopologicalSort = node.nbInputsForTopologicalSort - nbWeak 

            nID = nID + 1

        for edge in self._sortedEdges: 
            na,nb = edge
            currentRow=[0] * len(self._sortedNodes) 

            ia=self._sortedNodes.index(na.owner)
            ib=self._sortedNodes.index(nb.owner)

            # Produced by na on the edge
            # for execution of one cycle of the na.owner node
            totalProduced = na.cycleTotal * na.owner.cyclePeriod // na.cyclePeriod
            currentRow[ia] = totalProduced

            # Consumed by nb on the edge
            # for execution of a full cycle of the node
            totalProduced = nb.cycleTotal * nb.owner.cyclePeriod // nb.cyclePeriod
            currentRow[ib] = -totalProduced

            rows.append(currentRow)

        return(np.array(rows))

    def nullVector(self,m):
        #print("Null vector")
        # m is topology matrix computed with toplogyMatrix
        r=Matrix(m).nullspace()
        if len(r) != 1:
           raise NotSchedulableError
        result=list(r[0])
        #print(result)
        denominators = [x.q for x in result]
        # Remove denominators
        ppcm = ilcm(*denominators)
        #print(ppcm)
        intValues = [x * ppcm for x in result]
        # Convert intValues to the smallest possible values
        gcd = igcd(*intValues)
        res = [x // gcd for x in intValues]
        # Convert SymPy Integer to Python int
        res = np.array([int(x) for x in res],dtype=int)

        return(res)

    @property
    def initEvolutionVector(self):
        """Initial FIFO state taking into account delays"""
        return(np.array([self.getDelay(x) for x in self.edges]))

    def evolutionVectorForNode(self,nodeID,test=True):
        """Return the evolution vector corresponding to a selected node"""
        # For a simple static scheduling, the topology matrix T
        # is enough. If we know which node is executed, we have
        # a node vector V where there is a 1 at the position of the
        # execute node.
        # And the data generated on the fifos is:
        # T . V so the new fifo vector B' is given with
        # B' = T .V + B
        # But in case of cyclo static scheduling the topology
        # matrix is for a full period of the node
        # so 1 or several periods of each IO.
        # And we don't have the granularity corresponding to 
        # one node execution.
        # For one node execution, we need to know where we are
        # in the cycle on each IO
        # and where we are in the cycle for the node to
        # track how many full eriods of the node execution have been
        # done.
        # So this function is not just returning the node vector
        # and letting the main function updating the fifos.
        # This function is returning the new fifo state
        # Node vector would have been
        # v = np.zeros(len(self._sortedNodes))
        # v[nodeID] = 1 
        # for cyclo static scheduling
        n = self._sortedNodes[nodeID]
        #print(n)
        v = np.zeros(len(self._sortedEdges))

        # In test mode we are testing several nodes
        # to find the best one to schedule.
        # So we should not advance the cycle
        # of the IOs
        for i in n._inputs:
            io = n._inputs[i]
            edge = io._fifo
            # If 0, it is a connection to a constant node
            # so there is no FIFO
            if len(edge)>0:
               pos = self._sortedEdges.index(edge)
               v[pos] = -io.cycleValue
               if not test:
                  io.advanceCycle()
            

        for o in n._outputs:
            io = n._outputs[o]
            edge = io._fifo
            # If 0 it is a connection to a constant edge
            # so there is no FIFO
            if len(edge)>0:
               pos = self._sortedEdges.index(edge)
               v[pos] = io.cycleValue
               if not test:
                  io.advanceCycle()
            

        #print(v)
        return(v)

    def computeTopologicalOrderSchedule(self,normV,allFIFOs,initB,bMax,initN,config):
        b = np.array(initB)
        n = np.array(initN)


        schedule=[]

        zeroVec = np.zeros(len(self._sortedNodes))
        evolutionTime = 0
        #print(self._sortedNodes)
        # While there are remaining node periods to schedule
        while (n != zeroVec).any():
            #print("")
            #print(n)
            # Look for the best node to schedule
            # which is the one giving the minimum FIFO increase
            
            # None selected
            selected = -1

            # Min FIFO size found

            for layer in self._topologicalSort:
                minVal = 10000000
                selected = -1
                for node in layer:
                    # If the node can be scheduled
                    if n[node.sortedNodeID] > 0:
                       # Evolution vector for static scheduling
                       # v = self.evolutionVectorForNode(nodeID)
                       # for cyclo static we need the new fifo state
                       newB = self.evolutionVectorForNode(node.sortedNodeID) + b
                       # New fifos size after this evolution
                       # For static scheduling, fifo update would have been
                       # newB = np.dot(t,v) + b
                       #print(newB)
                
                       # Check that there is no FIFO underflow or overfflow:
                       if np.all(newB >= 0) and np.all(newB <= bMax):
                          # Total FIFO size for this possible execution
                          # We normalize to get the occupancy number as explained above
                          theMin = (1.0*np.array(newB) / normV).max()
                          # If this possible evolution is giving smaller FIFO size
                          # (measured in occupancy number) then it is selected
                          if theMin < minVal:
                             minVal = theMin
                             selected = node.sortedNodeID 
    
                       if selected != -1:
                          # We put the selected node at the end of the layer
                          # so that we do not always try the same node
                          # each time we analyze this layer
                          # If several nodes can be selected it should allow
                          # to select them with same probability
                          layer.remove(node)
                          layer.append(node)
                          break
                # For breaking from the outer loop too
                else:
                    continue
                break
                # No node could be scheduled because of not enough data
                # in the FIFOs. It should not occur if there is a null
                # space of dimension 1. So, it is probably a bug if
                # this exception is raised
            if selected < 0:
               raise DeadlockError
            
            # Now  we have evaluated all schedulable nodes for this run
            # and selected the one giving the smallest FIFO increase
    
            # Implementation for static scheduling
            # Real evolution vector for selected node
            # evol =  self.evolutionVectorForNode(selected)
            # Keep track that this node has been schedule
            # n = n - evol
            # Compute new fifo state
            # fifoChange = np.dot(t,evol)
            # b = fifoChange + b
    
            # Implementation for cyclo static scheduling
            #print("selected")
            fifoChange = self.evolutionVectorForNode(selected,test=False)
            b = fifoChange + b
            # For cyclo static, we create an evolution vector 
            # which contains a 1
            # at a node position only if this node has executed
            # its period.
            # Otherwise the null vector is not decreased
            v = np.zeros(len(self._sortedNodes))
            v[selected] = self._sortedNodes[selected].executeNode() 
            n = n - v
    
    
            if config.displayFIFOSizes:
               print(b)
                
            schedule.append(selected)
    
            # Analyze FIFOs to know if a FIFOs write is
            # followed immediately by a FIFO read of same size
            analyzeStep(fifoChange,allFIFOs,evolutionTime)
            evolutionTime = evolutionTime + 1
        return(schedule)

    def _computeFullyAsynchronousSchedule(self,config):
        # First we must rewrite the graph and insert duplication
        # nodes when an ouput is connected to several inputs.
        # After this transform, each output should be connected to
        # only one output.
        self.insertDuplicates(config)

        networkMatrix = self.topologyMatrix()

        t = np.array(networkMatrix)
        #print(t)
        #print(n)

        # Define the list of FIFOs objects
        nbFIFOS = t.shape[0]
        allFIFOs = [] 
        self._edgeToFIFO = {}
        for i in range(nbFIFOS):
            desc = FIFODesc(i,self.defaultFIFOClass,1.0)
            allFIFOs.append(desc)
            self._edgeToFIFO[self._sortedEdges[i]] = desc

       

        self.computeSourceTopologicalSortOfNodes()

        schedule=[]
        for layer in self._topologicalSort:
            for node in layer:
                schedule.append(node.sortedNodeID)

        # In fully asynchronous the fifoLength array and maxTime are
        # not used
        allBuffers=self.initializeFIFODescriptions(config,allFIFOs,[],0)
        self._allFIFOs = allFIFOs 
        self._allBuffers = allBuffers

        return(Schedule(self,self._sortedNodes,self._sortedEdges,schedule,config))




    def computeSchedule(self,config=Configuration()):
        if config.fullyAsynchronous:
            return(self._computeFullyAsynchronousSchedule(config))
        # First we must rewrite the graph and insert duplication
        # nodes when an ouput is connected to several inputs.
        # After this transform, each output should be connected to
        # only one output.
        self.insertDuplicates(config)

        networkMatrix = self.topologyMatrix()
        #print(networkMatrix)

        if config.sinkPriority:
           self.computeSinkTopologicalSortOfNodes()

        mustDoSinkPrioritization = config.sinkPriority and len(self._topologicalSort)>0
        if config.sinkPriority and not mustDoSinkPrioritization:
            print("Sink prioritization has been disabled. The graph has some loops")

        # Init values
        initB = self.initEvolutionVector
        initN = self.nullVector(networkMatrix)
        #print(initB)
        #print(initN)
        #print(np.dot(networkMatrix,initN))

        # nullVector is giving the number of repetitions
        # for a node cycle.
        # So the number of iteration of the node must be multiplied
        # by the cycle period for this node.
        #for nodeID in range(len(self._sortedNodes)):
        #    initN[nodeID] = initN[nodeID] * self._sortedNodes[nodeID].cyclePeriod


        # Current values (copys)
        b = np.array(initB)
        n = np.array(initN)

        if config.displayFIFOSizes:
           for edge in self._sortedEdges:
             print("%s:%s -> %s:%s" % (edge[0].owner.nodeID,edge[0].name,edge[1].owner.nodeID,edge[1].name))
           print(b)

        # Topology matrix
        t = np.array(networkMatrix)
        #print(t)
        #print(n)

        # Define the list of FIFOs objects
        nbFIFOS = t.shape[0]
        allFIFOs = [] 
        self._edgeToFIFO = {}
        for i in range(nbFIFOS):
            desc = FIFODesc(i,self.defaultFIFOClass,1.0)
            allFIFOs.append(desc)
            self._edgeToFIFO[self._sortedEdges[i]] = desc

        # Normalization vector
        # For static scheduling it is
        #normV =  1.0*np.apply_along_axis(abs,1,t).max(axis=1)
        # For cyclo static scheduling we need the max per execution
        # and not the accumulated value for the run of a node
        # period which may be several periods of an IO
        #print(normV)
        normV = np.zeros(len(self._sortedEdges)) 
        for e in range(len(self._sortedEdges)):
            (src,dst) = self._sortedEdges[e]
            normV[e] = max(src.cycleMax, dst.cycleMax)

        #print(normV)

        # bMax below is used to track maximum FIFO size 
        # occuring during a run of the schedule
        #
        # The heuristric is:
        #
        # First we compute on each edge the maximum absolute value
        # It is the minimum amount of data an edge must contain
        # for the system to work either because it is produced
        # by a node or consumed by another.
        # We use this value as an unit of measurement for the edge.
        # So, we normalize the FIFO lengths by this size.
        # If this occupancy number is > 1 then it means
        # that enough data is available on the FIFO for the
        # consumer to consume it.
        # When we select a node for scheduling later we try
        # to minimize the occupancy number of all FIFOs by
        # selecting the scheduling which is giving the
        # minimum maximum occupancy number after the run.
        bMax = 1.0*np.array(initB)


        schedule=[]

        zeroVec = np.zeros(len(self._sortedNodes),dtype=int)
        evolutionTime = 0        
        #print(self._sortedNodes)
        # While there are remaining node periods to schedule
        while (n != zeroVec).any():
            #print("")
            #print(n)
            # Look for the best node to schedule
            # which is the one giving the minimum FIFO increase
            
            # None selected
            selected = -1

            # Min FIFO size found
            minVal = 10000000

            
            for node in self._sortedNodes:
                # If the node can be scheduled
                if n[node.sortedNodeID] > 0:
                   # Evolution vector for static scheduling
                   # v = self.evolutionVectorForNode(nodeID)
                   # for cyclo static we need the new fifo state
                   newB = self.evolutionVectorForNode(node.sortedNodeID) + b
                   # New fifos size after this evolution
                   # For static scheduling, fifo update would have been
                   # newB = np.dot(t,v) + b
                   #print(newB)
            
                   # Check that there is no FIFO underflow:
                   if np.all(newB >= 0):
                      # Total FIFO size for this possible execution
                      # We normalize to get the occupancy number as explained above
                      theMin = (1.0*np.array(newB) / normV).max()
                      # If this possible evolution is giving smaller FIFO size
                      # (measured in occupancy number) then it is selected
                      
                      if theMin <= minVal:
                         minVal = theMin
                         selected = node.sortedNodeID 


            # No node could be scheduled because of not enough data
            # in the FIFOs. It should not occur if there is a null
            # space of dimension 1. So, it is probably a bug if
            # this exception is raised
            if selected < 0:
               raise DeadlockError
            # Now  we have evaluated all schedulable nodes for this run
            # and selected the one giving the smallest FIFO increase

            # Implementation for static scheduling
            # Real evolution vector for selected node
            # evol =  self.evolutionVectorForNode(selected)
            # Keep track that this node has been schedule
            # n = n - evol
            # Compute new fifo state
            # fifoChange = np.dot(t,evol)
            # b = fifoChange + b

            # Implementation for cyclo static scheduling
            #print("selected")
            fifoChange = self.evolutionVectorForNode(selected,test=False)
            b = fifoChange + b
            # For cyclo static, we create an evolution vector 
            # which contains a 1
            # at a node position only if this node has executed
            # its period.
            # Otherwise the null vector is not decreased
            v = np.zeros(len(self._sortedNodes))
            v[selected] = self._sortedNodes[selected].executeNode() 
            n = n - v


            if config.displayFIFOSizes and not mustDoSinkPrioritization:
               print(b)
            
            bMax = np.maximum(b,bMax)
            schedule.append(selected)

            # Analyze FIFOs to know if a FIFOs write is
            # followed immediately by a FIFO read of same size
            if not mustDoSinkPrioritization:
               analyzeStep(fifoChange,allFIFOs,evolutionTime)
            evolutionTime = evolutionTime + 1

        fifoMax=np.floor(bMax).astype(np.int32)

        if mustDoSinkPrioritization:
           schedule = self.computeTopologicalOrderSchedule(normV,allFIFOs,initB,bMax,initN,config)
        
        allBuffers=self.initializeFIFODescriptions(config,allFIFOs,fifoMax,evolutionTime)
        self._allFIFOs = allFIFOs 
        self._allBuffers = allBuffers

        if config.dumpSchedule:
            print("Schedule")
            for nodeID in schedule:
                print(self._sortedNodes[nodeID].nodeName)
            print("")
        return(Schedule(self,self._sortedNodes,self._sortedEdges,schedule,config))


class Schedule:
    def __init__(self,g,sortedNodes,sortedEdges,schedule,config):
        self._sortedNodes=sortedNodes
        self._sortedEdges=sortedEdges
        self._schedule = schedule 
        self._graph = g
        self._config = config
        self._edgeToFIFO = g._edgeToFIFO
        # Nodes containing pure functions (no state) like some
        # CMSIS-DSP functions.
        
        nodeCodeID = 0
        pureClassID = 1

        # Because heapAllocation must be set as a scheduling option (which is an error)
        # So if we detect callback mode in the config, we set the
        # options enforced by callback.
        if config.callback:
           config.switchCase = True
           config.heapAllocation = True


        for n in self.nodes:
            n.codeID = nodeCodeID
            nodeCodeID = nodeCodeID + 1
            # Constant nodes are ignored since they have
            # no arcs, and are connected to no FIFOs
            
            pureClassID=n.createArgsWithSchedulerFifoID(pureClassID,self._config,self.fifoID,self._edgeToFIFO)
            

            

    def hasDelay(self,edge):
        return(self._graph.hasDelay(edge))

    def getDelay(self,edge):
        return(self._graph.getDelay(edge))


    @property
    def constantEdges(self):
        return self._graph.constantEdges

    @property
    def nodes(self):
        return self._sortedNodes

    @property
    def edges(self):
        return self._sortedEdges

    @property
    def schedule(self):
        return self._schedule

    #@property
    #def fifoLengths(self):
    #    return self._fifos

    @property 
    def scheduleLength(self):
        return len(self.schedule)

    @property 
    def memory(self):
        #theBytes=[x[0].theType.bytes for x in self.edges]
        #theSizes=[x[0]*x[1] for x in zip(self.fifoLengths,theBytes)]
        #return(np.sum(theSizes))
        return(self._graph._totalMemory)

    @property
    def graph(self):
        return self._graph

    def fifoID(self,edge):
        return(self.edges.index(edge))

    def outputFIFOs(self,node):
        outs=[]
        for io in node.outputNames:
            x = node._outputs[io]
            fifo=(self.fifoID(x.fifo),io)
            outs.append(fifo)
            
        return(outs)

    def ccode(self,directory,config=Configuration()):
        """Write graphviz into file f""" 
        c_gencode(self,directory,config)

    def pythoncode(self,directory,config=Configuration()):
        """Write graphviz into file f""" 
        p_gencode(self,directory,config)

    def graphviz(self,f,config=Configuration(),style=Style.default_style()):
        """Write graphviz into file f""" 
        gengraph(self,f,config,style=style)
    
    
    
    