###########################################
# Project:      CMSIS DSP Library
# Title:        node.py
# Description:  Node class for description of dataflow graph
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
"""Description of the basic types used to build a dataflow graph"""
from jinja2 import Environment, FileSystemLoader, PackageLoader,select_autoescape
import pathlib
import os.path
import numpy as np

from sympy.core.numbers import ilcm
from .args import *

class NoFunctionArrayInPython(Exception):
    pass

class NoAsynchronousModeInPython(Exception):
    pass

def camelCase(st):
    output = ''.join(x for x in st.title() if x.isalnum())
    return output[0].lower() + output[1:]

def joinit(iterable, delimiter):
    it = iter(iterable)
    yield next(it)
    for x in it:
        yield delimiter
        yield x

### Definition of the IOs

class IO:
    """Class of input / outputs"""
    def __init__(self,owner,name,theType,nbSamples):
        self._theType = theType
        # For cycle static scheduling it may also be
        # a list of samples for each iteration of a cycle
        self._nbSamples = nbSamples 
        self._owner = owner
        self._name = name
        self._fifo = [] 
        self.constantNode = None

        # For cyclo static scheduling nbSamples is a list
        # and when simulating the schedule we need to know
        # where we currently are in the list
        self._cyclePosition = 0 

    # For cyclo static scheduling we advance the position in the
    # cycle
    def advanceCycle(self):
        if isinstance(self.nbSamples,int):
            pass 
        else:
            self._cyclePosition = self._cyclePosition + 1 
            if self._cyclePosition == len(self.nbSamples):
               self._cyclePosition = 0

    # For cyclo static scheduling, get the value in the current
    # cycle position
    @property
    def cycleValue(self):
        if isinstance(self.nbSamples,int):
            return(self.nbSamples)
        else:
            return(self.nbSamples[self._cyclePosition])

    # For cyclo static scheduling: we need
    # the length of the cycle 
    @property
    def cyclePeriod(self):
        if isinstance(self.nbSamples,int):
            return(1)
        else:
            return(len(self.nbSamples))

    # For cyclo static scheduling we need the total
    # data generated by a run of the cycle
    @property
    def cycleTotal(self):
        if isinstance(self.nbSamples,int):
            return(self.nbSamples)
        else:
            return(np.sum(self.nbSamples))

    # For cyclo static we need the max in a period top use
    # as a normalization factor to identify the most filled
    # FIFO 
    @property
    def cycleMax(self):
        if isinstance(self.nbSamples,int):
            return(self.nbSamples)
        else:
            return(np.max(self.nbSamples))
    
    


    @property
    def fifo(self):
        return self._fifo

    ## the attribute name and the method name must be same which is used to set the value for the attribute
    @fifo.setter
    def fifo(self, var):
        self._fifo = var

   

    def compatible(self,other):
        return(self._theType == other._theType)

    @property
    def owner(self):
        return self._owner

    @property
    def name(self):
        return self._name

    @property
    def ctype(self):
        """ctype string """
        return self._theType.ctype

    @property
    def nptype(self):
        """ctype string """
        return self._theType.nptype

    @property
    def theType(self):
        return self._theType

    @property
    def dspExtension(self):
        return self._theType.dspExtension

    @property
    def graphViztype(self):
        return self._theType.graphViztype

    @property
    def nbSamples(self):
        return self._nbSamples
    
    

class Input(IO):
     """Node input"""
     pass

class Output(IO):
     """Node output"""
     pass

### Definition of the nodes types

class Constant:
    """ Represent a constant object.

        A constant object is ignored for the scheduling.
        But it can be connected to CMSIS-DSP inputs.
        It is generated as DEFINE
    """
    def __init__(self,name):
        self._name = name 

    @property
    def name(self):
        return self._name

    @property
    def isConstantNode(self):
        return True
    
    
class SchedArg:
    """Class for arguments of the scheduler functions.
       They can either be a literal arg like string, boolean
       or number or they can be a variable name"""
    
    def __init__(self,name):
          self._name=name 

class ArgLiteral(SchedArg):
     def __init__(self,name):
        super().__init__(name)

     @property
     def arg(self):
        if isinstance(self._name,str):
             return("\"%s\"" % self._name) 
        elif isinstance(self._name,bool):
             if self._name:
                return("true")
             else:
                return("false")
        else:
             return(str(self._name))

class VarLiteral(SchedArg):
     def __init__(self,name):
        super().__init__(name)

     @property 
     def arg(self):
        return(self._name)

class BaseNode:
    """Root class for all Nodes of a dataflow graph.
       To define a new kind of node, inherit from this class"""

    def __init__(self,name,identified=True):
        """Create a new kind of Node.

        name :: The name of the node which is used as
        a C variable in final code"""
        self._nodeName = name
        self._nodeID = name
        self._inputs={}
        self._outputs={}
        # For code generations
        # The fifo args
        self._args=""
        # Literal arguments
        self.schedArgs=None 

        # For cyclo static scheduling
        # It is the LCM of the cycles of all
        # connected edges.
        # After this period, the node and its edges
        # should be back to the original state
        self.cyclePeriod = 1

        self._positionInCycle = 0

        self.sortedNodeID = 0
        # Updated during toplogical sorts to find the current
        # sinks of the truncated graph
        self.nbOutputsForTopologicalSort = 0

        self._identified = identified

    def __getattr__(self,name):
        """Present inputs / outputs as attributes"""
        if name in self._inputs:
           return(self._inputs[name])
        if name in self._outputs:
           return(self._outputs[name])
        raise AttributeError

    def __getitem__(self,name):
        """Present inputs / outputs as keys"""
        if name in self._inputs:
           return(self._inputs[name])
        if name in self._outputs:
           return(self._outputs[name])
        raise IndexError 

    @property
    def identified(self):
        return self._identified

    @identified.setter
    def identified(self, value):
        self._identified = value
    
    # For cyclo static scheduling we need to compute
    # the cycle period for this node based upon the
    # period of the IOs 
    def computeCyclePeriod(self):
        allLengths = []
        for k in self._inputs:
            allLengths.append(self._inputs[k].cyclePeriod)
        for k in self._outputs:
            allLengths.append(self._outputs[k].cyclePeriod)
        if len(allLengths)>1:
           self.cyclePeriod = ilcm(*allLengths)
        else:
           self.cyclePeriod = allLengths[0]

    # For cyclo static scheduling, we need to track the number
    # of period execution 
    # We return 1 when a period has been executed 
    # and this is used to decrease the period count in
    # the null vector of the toplogy matrix
    def executeNode(self):
        self._positionInCycle = self._positionInCycle + 1 
        if (self._positionInCycle == self.cyclePeriod):
            self._positionInCycle = 0 
            return(1)
        else:
            return(0)

    def _addLiteralItem(self,item):
        if self.schedArgs:
            self.schedArgs.append(ArgLiteral(item))
        else:
            self.schedArgs=[ArgLiteral(item)]

    def addLiteralArg(self,*ls):
        for l in ls:
            if isinstance(l, list):
                for i in l:
                    self._addLiteralItem(i)
            else:
                self._addLiteralItem(l)

    def _addVariableItem(self,item):
        if self.schedArgs:
            self.schedArgs.append(VarLiteral(item))
        else:
            self.schedArgs=[VarLiteral(item)]

    def addVariableArg(self,*ls):
        for l in ls:
            if isinstance(l, list):
               for i in l:
                  self._addVariableItem(i)
            else:
              self._addVariableItem(l)
           

    @property
    def isConstantNode(self):
        return False

    @property 
    def isDuplicateNode(self):
        return False

    @property
    def hasState(self):
        """False if the node is a pure functiom with no state
           and no associated C++ object
        """
        return(True)

    @property
    def typeName(self):
        return "void"
    
    
    @property
    def nodeID(self):
        """Node ID to uniquely identify a node"""
        return self._nodeID

    @property
    def nodeName(self):
        """Node name displayed in graph

           It could be the same for different nodes if the
           node is just a function with no state.
        """
        return self._nodeName

    # For code generation



    def createArgsWithSchedulerFifoID(self,pureClassID,config,fifoID):
        """Get list of IO objects for inputs and outputs"""
        i=[] 
        o=[]
        theArgs=[] 
        theArgTypes=[]
        # Use orderd io names
        for io in self.inputNames:
            x = self._inputs[io]
            i.append(x)

        for io in self.outputNames:
            x = self._outputs[io]
            o.append(x)

        
        for io in i:
                # An io connected to a constant node has no fifo 
                if len(io.fifo) > 0:
                   theArgs.append(fifoID(io.fifo))
                   theArgTypes.append(io.ctype)
                else:
                # Instead the arg is the name of a constant node
                # instead of being a fifo ID
                   theArgs.append(io.constantNode.name)
                   theArgTypes.append(io.constantNode.name)
        for io in o:
                theArgs.append(fifoID(io.fifo))
                theArgTypes.append(io.ctype)

        if not self.hasState:
           # For pure function, analyze how many
           # real arguments (FIFO) we have
           self.analyzeConstantArgs()

        self.setArgsWith(theArgs,config)

        # Used to know how to call the run function of the
        # node in the CRun functions.
        if config.heapAllocation:
            self.nodeVariable=ArgsPtrObj(self.nodeName,owner="nodes")
        else:
            self.nodeVariable=ArgsObject(self.nodeName)
       

        return(pureClassID)

    
    # Compute how many real inputs and outputs we have
    # Other are constant ndoes
    def analyzeConstantArgs(self):
        inputid=0
        outputid=0
        
        # Use ordered io names
        for io in self.inputNames:
            x = self._inputs[io]
            if not x.constantNode:
               inputid = inputid + 1

        for io in self.outputNames:
            x = self._outputs[io]
            if not x.constantNode:
               outputid = outputid + 1
               

        self._realInputs = inputid
        self._realOutputs = outputid
        

    def getCTemplateArgumentsFor(self,orderedNames,ioDesc):
        ios=[] 
        # Use ordered io names
        for io in orderedNames:
            # Get IO description for this name
            x = ioDesc[io]
            # For cyclo static scheduling, we may have a list
            # of samples
            if isinstance(x.nbSamples,int):
               ios.append("%s,%d" % (x.ctype,x.nbSamples))
            else:
                # In case of a list of samples
                # thne templaet is receiving only the
                # max value
                m = np.max(x.nbSamples)
                ios.append("%s,%d" % (x.ctype,m))
        return(ios)

    def ioTemplate(self):
        """Template arguments for C
           input type, input size ...
           output type, output size ...

           Some nodes may customize it
        """
        ios=[] 
        ios += self.getCTemplateArgumentsFor(self.inputNames,self._inputs)
        ios += self.getCTemplateArgumentsFor(self.outputNames,self._outputs)
        
        return("".join(joinit(ios,",")))

    def pythonIoTemplate(self):
        """Template arguments for Python
           input type, input size ...
           output type, output size ...

           Some nodes may customize it
        """
        ios=[] 
        # Use ordered io names
        # For cyclo static scheduling, the nbSamples
        # may be a list and we use the max value
        # for the code generayion
        for io in self.inputNames:
            x = self._inputs[io]
            if isinstance(x.nbSamples,int):
               ios.append("%d" % x.nbSamples)
            else:
                m = np.max(x.nbSamples)
                ios.append("%d" % m)

        for io in self.outputNames:
            x = self._outputs[io]
            if isinstance(x.nbSamples,int):
               ios.append("%d" % x.nbSamples)
            else:
                m = np.max(x.nbSamples)
                ios.append("%d" % m)

        
        return("".join(joinit(ios,",")))

    def cRun(self,config,ctemplate=True):
        """Run function

           Some nodes may customize it
        """
        if ctemplate:
           return (f"cgStaticError = {self.nodeVariable.access}run();")
        else:
           return (f"cgStaticError = {self.nodeVariable.access}run()")

    def cFunc(self,ctemplate=True):
        """Function call for code array scheduler

           Some nodes may customize it
        """
        if ctemplate:
           return ("(runNode)&%s<%s>::run" % (self.typeName,self.ioTemplate()))
        else:
           raise NoFunctionArrayInPython
    
    
    @property
    def listOfargs(self):
        """List of fifos args for object initialization"""
        return self._args


    @property
    def args(self):
        """String of fifo args for object initialization
            with literal argument and variable arguments"""
        allArgs=[x.reference for x in self.listOfargs]
        # Add specific argrs after FIFOs
        if self.schedArgs:
            for lit in self.schedArgs:
                allArgs.append(lit.arg)
        return "".join(joinit(allArgs,","))
    
    def setArgsWith(self,fifoIDs,config):
        res=[]
        # Template args is used only for code array
        # scheduler when we create on the fly a new class
        # for a function.
        # In this case, the arguments of the template must only be
        # fifos and not constant.
        templateargs=[]
        for x in fifoIDs:
          # If args is a FIFO we generate a name using fifo ids
          # They have already been ordered (in calling function)
          # using alphabetical order of input and output names
          if isinstance(x,int):
             if config.heapAllocation:
                fifoArg = FifoPtrID(x,owner="fifos")
             else:
                fifoArg = FifoID(x)
             res.append(fifoArg)
             templateargs.append(fifoArg)
          # If args is a constant node, we just use the constant node name
          # (Defined in C code)
          else:
             res.append(ArgsObject(x))
        self._args=res
        self._templateargs=templateargs

    
    # For graphviz generation



    @property
    def graphvizName(self):
        """Name for graph vizualization"""
        return ("%s<BR/>(%s)" % (self.nodeName,self.typeName))

    def inputPortFromID(self,i):
        r = sorted(list(self._inputs.keys()))
        k = r[i]
        return self._inputs[k]
    
    def outputPortFromID(self,i):
        r = sorted(list(self._outputs.keys()))
        k = r[i]
        return self._outputs[k]

    @property
    def inputNames(self):
        return sorted(list(self._inputs.keys()))

    @property
    def outputNames(self):
        return sorted(list(self._outputs.keys()))

    @property
    def hasManyInputs(self):
        return len(self._inputs.keys())>1

    @property
    def hasManyOutputs(self):
        return len(self._outputs.keys())>1

    @property
    def hasManyIOs(self):
        return (self.hasManyInputs or self.hasManyOutputs)

    @property
    def nbInputs(self):
        return(len(self._inputs.keys()))

    @property
    def nbOutputs(self):
        return(len(self._outputs.keys()))

    @property
    def nbEmptyInputs(self):
        return (self.maxNbIOs - len(self._inputs.keys()))
    
    @property
    def nbEmptyOutputs(self):
        return (self.maxNbIOs - len(self._outputs.keys()))

    @property
    def maxNbIOs(self):
        return max(len(self._inputs.keys()),len(self._outputs.keys()))
    
    

class GenericSink(BaseNode):
    """A sink in the dataflow graph""" 

    def __init__(self,name,identified=True):
        BaseNode.__init__(self,name,identified=identified)
        self._isPureNode = False
    
    @property
    def isPureNode(self):
        return self._isPureNode

    @property
    def typeName(self):
        return "void"

    def addInput(self,name,theType,theLength):
        self._inputs[name]=Input(self,name,theType,theLength)


class GenericSource(BaseNode):
    """A source in the dataflow graph""" 

    def __init__(self,name,identified=True):
        BaseNode.__init__(self,name,identified=identified)
        self._isPureNode = False
    
    @property
    def isPureNode(self):
        return self._isPureNode

    @property
    def typeName(self):
        return "void"

    def addOutput(self,name,theType,theLength):
        self._outputs[name]=Output(self,name,theType,theLength)

class GenericNode(BaseNode):
    """A source in the dataflow graph""" 

    def __init__(self,name,identified=True):
        BaseNode.__init__(self,name,identified=identified)
        # Pure node is for instance a 
        # CMSIS-DSP function.
        # It is not packaged into an object
        # and the code is generated directly
        self._isPureNode = False
    
    @property
    def isPureNode(self):
        return self._isPureNode
    
    @property
    def typeName(self):
        return "void"

    def addInput(self,name,theType,theLength):
        self._inputs[name]=Input(self,name,theType,theLength)

    def addOutput(self,name,theType,theLength):
        self._outputs[name]=Output(self,name,theType,theLength)

class GenericToManyNode(BaseNode):
    """A source in the dataflow graph""" 

    def __init__(self,name,identified=True):
        BaseNode.__init__(self,name,identified=identified)
        # Pure node is for instance a 
        # CMSIS-DSP function.
        # It is not packaged into an object
        # and the code is generated directly
        self._isPureNode = False
    
    @property
    def isPureNode(self):
        return self._isPureNode
    
    @property
    def typeName(self):
        return "void"

    # Should be in alphabetical order so that the
    # index used here is the index in the C code
    def outputNameFromIndex(self,i):
        return f"o{chr(ord('a')+i)}"

    def addInput(self,name,theType,theLength):
        self._inputs[name]=Input(self,name,theType,theLength)

    def addManyOutput(self,theType,theLength,nb):
        self._outputLength = theLength
        self._outputType = theType
        for i in range(nb):
            name = self.outputNameFromIndex(i)
            self._outputs[name]=Output(self,name,theType,theLength)

    @property
    def args(self):
        """String of fifo args for object initialization
            with literal argument and variable arguments"""
        allArgs=self.listOfargs
        theInputs = allArgs[:len(self._inputs)]
        theOutputs = allArgs[len(self._inputs):]
        
        ioInputs = [x.reference for x in theInputs]
        ioOutputs = ["{" + "".join(joinit([x.pointer for x in theOutputs],",")) + "}"]
        # Add specific argrs after FIFOs
        sched = []
        if self.schedArgs:
            for lit in self.schedArgs:
                sched.append(lit.arg)
        return "".join(joinit(ioInputs+ioOutputs+sched,","))

    def ioTemplate(self):
        """ioTemplate is different for window
        """
        ios=[] 
        ios += self.getCTemplateArgumentsFor(self.inputNames,self._inputs)
       
        # Use the max in case of cyclo static
        # scheduling on the output
        if isinstance(self._outputLength,int):
            nbOut = self._outputLength 
        else:
            nbOut = np.max(self._outputLength )
        ios.append("%s,%d" % (self._outputType.ctype,nbOut))
        return("".join(joinit(ios,",")))


class GenericFromManyNode(BaseNode):
    """A source in the dataflow graph""" 

    def __init__(self,name,identified=True):
        BaseNode.__init__(self,name,identified=identified)
        # Pure node is for instance a 
        # CMSIS-DSP function.
        # It is not packaged into an object
        # and the code is generated directly
        self._isPureNode = False
    
    @property
    def isPureNode(self):
        return self._isPureNode
    
    @property
    def typeName(self):
        return "void"

    # Should be in alphabetical order so that the
    # index used here is the index in the C code
    def inputNameFromIndex(self,i):
        return f"i{chr(ord('a')+i)}"

    def addOutput(self,name,theType,theLength):
        self._outputs[name]=Output(self,name,theType,theLength)

    def addManyInput(self,theType,theLength,nb):
        self._inputLength = theLength
        self._inputType = theType
        for i in range(nb):
            name = self.inputNameFromIndex(i)
            self._inputs[name]=Input(self,name,theType,theLength)

    @property
    def args(self):
        """String of fifo args for object initialization
            with literal argument and variable arguments"""
        allArgs=self.listOfargs
        theInputs = allArgs[:len(self._inputs)]
        theOutputs = allArgs[len(self._inputs):]
        
        ioInputs = ["{" + "".join(joinit([x.pointer for x in theInputs],",")) + "}"]
        ioOutputs = [x.reference for x in theOutputs]
        # Add specific argrs after FIFOs
        sched = []
        if self.schedArgs:
            for lit in self.schedArgs:
                sched.append(lit.arg)
        return "".join(joinit(ioInputs+ioOutputs+sched,","))

    def ioTemplate(self):
        """ioTemplate is different for window
        """
        ios=[] 
       
        # Use the max in case of cyclo static
        # scheduling on the output
        if isinstance(self._inputLength,int):
            nbOut = self._inputLength 
        else:
            nbOut = np.max(self._inputLength )
        ios.append("%s,%d" % (self._inputType.ctype,nbOut))
        
        ios += self.getCTemplateArgumentsFor(self.outputNames,self._outputs)


        return("".join(joinit(ios,",")))

class GenericManyToManyNode(BaseNode):
    """A source in the dataflow graph""" 

    def __init__(self,name,identified=True):
        BaseNode.__init__(self,name,identified=identified)
        # Pure node is for instance a 
        # CMSIS-DSP function.
        # It is not packaged into an object
        # and the code is generated directly
        self._isPureNode = False
    
    @property
    def isPureNode(self):
        return self._isPureNode
    
    @property
    def typeName(self):
        return "void"

    # Should be in alphabetical order so that the
    # index used here is the index in the C code
    def inputNameFromIndex(self,i):
        return f"i{chr(ord('a')+i)}"

    def outputNameFromIndex(self,i):
        return f"o{chr(ord('a')+i)}"

    def addManyOutput(self,theType,theLength,nb):
        self._outputLength = theLength
        self._outputType = theType
        for i in range(nb):
            name = self.outputNameFromIndex(i)
            self._outputs[name]=Output(self,name,theType,theLength)

    def addManyInput(self,theType,theLength,nb):
        self._inputLength = theLength
        self._inputType = theType
        for i in range(nb):
            name = self.inputNameFromIndex(i)
            self._inputs[name]=Input(self,name,theType,theLength)

    @property
    def args(self):
        """String of fifo args for object initialization
            with literal argument and variable arguments"""
        allArgs=self.listOfargs
        theInputs = allArgs[:len(self._inputs)]
        theOutputs = allArgs[len(self._inputs):]
        
        ioInputs = ["{" + "".join(joinit([x.pointer for x in theInputs],",")) + "}"]
        ioOutputs = ["{" + "".join(joinit([x.pointer for x in theOutputs],",")) + "}"]
        # Add specific argrs after FIFOs
        sched = []
        if self.schedArgs:
            for lit in self.schedArgs:
                sched.append(lit.arg)
        return "".join(joinit(ioInputs+ioOutputs+sched,","))

    def ioTemplate(self):
        """ioTemplate is different for window
        """
        ios=[] 
       
        # Use the max in case of cyclo static
        # scheduling on the output
        if isinstance(self._inputLength,int):
            nbOut = self._inputLength 
        else:
            nbOut = np.max(self._inputLength )
        ios.append("%s,%d" % (self._inputType.ctype,nbOut))
        

        if isinstance(self._outputLength,int):
            nbOut = self._outputLength 
        else:
            nbOut = np.max(self._outputLength )
        ios.append("%s,%d" % (self._outputType.ctype,nbOut))
        

        return("".join(joinit(ios,",")))
       
class SlidingBuffer(GenericNode):

    def __init__(self,name,theType,length,overlap):
        GenericNode.__init__(self,name)
        self._length = length 
        self._overlap = overlap 
        self.addInput("i",theType,length-overlap)
        self.addOutput("o",theType,length)
    
    def ioTemplate(self):
        """ioTemplate is different for window
        """
        theType=self._inputs[self.inputNames[0]].ctype  
        ios="%s,%d,%d" % (theType,self._length,self._overlap)
        return(ios)

    def pythonIoTemplate(self):
        """ioTemplate is different for window
        """
        theType=self._inputs[self.inputNames[0]].ctype  
        ios="%d,%d" % (self._length,self._overlap)
        return(ios)
        

    @property
    def typeName(self):
        return "SlidingBuffer"

class OverlapAdd(GenericNode):

    def __init__(self,name,theType,length,overlap):
        GenericNode.__init__(self,name)
        self._length = length 
        self._overlap = overlap 
        self.addInput("i",theType,length)
        self.addOutput("o",theType,length-overlap)
    
    def ioTemplate(self):
        """ioTemplate is different for window
        """
        theType=self._inputs[self.inputNames[0]].ctype  
        ios="%s,%d,%d" % (theType,self._length,self._overlap)
        return(ios)

    def pythonIoTemplate(self):
        """ioTemplate is different for window
        """
        theType=self._inputs[self.inputNames[0]].ctype  
        ios="%d,%d" % (self._length,self._overlap)
        return(ios)

    
        

    @property
    def typeName(self):
        return "OverlapAdd"




class ArgLength():
    def __init__(self,n,sample_unit=True):
        self.name = n 
        self.sample_unit = sample_unit


# Pure compute functions
# It is supporting unary function (src,dst,blockize)
# and binary functions (sraa,srcb, dst, blocksize)
# For cmsis, the prefix arm and the type suffix are not needed
# if class Dsp is used
class GenericFunction(GenericNode):
    # Number of function node of each category
    # Used to generate unique ID and names when
    # unique names are required
    # like for creating the graph where each call to
    # the same function must be identified as a
    # separate node
    NODEID={}

    ENV = Environment(
       loader=PackageLoader("cmsis_stream.cg.scheduler"),
       autoescape=select_autoescape(),
       lstrip_blocks=True,
       trim_blocks=True
    )
    
    CTEMPLATE = ENV.get_template("cmsis.cpp")
    CCHECKTEMPLATE = ENV.get_template("cmsisCheck.cpp")

    PYTEMPLATE = ENV.get_template("cmsis.py")

    def __init__(self,funcname,argsDesc):
        if not (funcname in GenericFunction.NODEID):
            GenericFunction.NODEID[funcname]=1 

        GenericNode.__init__(self,"%s%d" % (funcname,GenericFunction.NODEID[funcname]),identified=False)

        self._hasState = False
        #self._length = length 
        self._nodeName = funcname
        self._isPureNode = True
        self._argsDesc = argsDesc

        GenericFunction.NODEID[funcname]=GenericFunction.NODEID[funcname]+1

    @BaseNode.identified.setter
    def identified(self, value):
        self._identified = False

    @property
    def realInputs(self):
        return self._realInputs
    
    @property
    def realOutputs(self):
        return self._realOutputs
    
    @property
    def constructorTypes(self):
        return self._constructorTypes

    @property
    def constructorArguments(self):
        return self._constructorArguments
    
    @property
    def templateParameters(self):
        return self._templateParameters

    @property
    def specializedTemplateParameters(self):
        return self._specializedTemplateParameters
    

    @property
    def templateArguments(self):
        return self._templateArguments
    

    @property
    def templateParametersForGeneric(self):
        return self._templateParametersForGeneric

    @property
    def datatypeForConstructor(self):
        return self._datatypeForConstructor

    @property
    def genericConstructorArgs(self):
        return self._genericConstructorArgs
    

    @property
    def hasState(self):
        return self._hasState
    

    @property
    def typeName(self):
        return "Function"

    # Prepare for code generation.
    # All those values are used in the
    # code generation template
    # They are both used for the run part
    # and the prepareForRunning part
    def _prepareForCodeGen(self,ctemplate=True):
       
       theId = 0
       # List of buffer and corresponding fifo to initialize buffers
       inputs=[]
       outputs=[]
       # List of buffers variable to declare
       ptrs=[]

       # Argument names (buffer or constant node)
       inargs=[]
       outargs=[]

       argsStr=""
       inArgsStr=""
       outArgsStr=""
       inputId=1
       outputId=1
       fifoToBuf = {}
       for io in self.inputNames:
            ioObj = self._inputs[io] 
            if ioObj.constantNode:
               # Argument is name of constant Node
               inargs.append(ioObj.constantNode.name)
               fifoToBuf[io] = ioObj.constantNode.name
            else:
               if ctemplate:
                    the_type = ioObj.ctype
               else:
                    the_type = ioObj.nptype
               # Argument is a buffer created from FIFO
               buf = "i%d" % theId
               ptrs.append((the_type,buf))
               inargs.append(buf)
               fifoToBuf[io] = buf
               
               # Buffer and fifo
               nb = ioObj.nbSamples
               inputs.append((buf,self.listOfargs[theId],nb))
               inputId = inputId + 1
            theId = theId + 1
       for io in self.outputNames:
            ioObj = self._outputs[io] 
            if ctemplate:
                the_type = ioObj.ctype
            else:
                the_type = ioObj.nptype
            buf = "o%d" % theId
            ptrs.append((the_type,buf))
            outargs.append(buf)
            fifoToBuf[io] = buf
            nb = ioObj.nbSamples
            outputs.append((buf,self.listOfargs[theId],nb))
            outputId = outputId + 1
            theId = theId + 1

       sched = []
       if self.schedArgs:
            for lit in self.schedArgs:
                sched.append(lit.arg)

       args=[]
       # inargs and outargs are used for Python version
       # of a pure function
       inargs=[] 
       outargs=[]
       for a in self._argsDesc:
           # Name of an IO
           if isinstance(a,str):
              args.append(fifoToBuf[a])
              if a in self._inputs:
                 inargs.append(fifoToBuf[a])
              else:
                 outargs.append(fifoToBuf[a])
           # ID of a special argument (literal or C
           # variable)
           elif isinstance(a,int):
              args.append(sched[a])
              inargs.append(sched[a])
           # FIFO lengths
           else:
              # Length description for an IO
              # For Python, length is derived from
              # the array so it is not passed as
              # argument and those arguments
              # are ignored for the generated Python API
              nb = None 
              the_type = None
              isInput = False
              # Get the type and the number of samples
              # for the IO
              if a.name in self._inputs:
                 isInput = True
                 nb = self._inputs[a.name].nbSamples
                 if ctemplate:
                    the_type = self._inputs[a.name].ctype
                 else:
                    the_type = self._inputs[a.name].nptype
              if a.name in self._outputs:
                 nb = self._outputs[a.name].nbSamples
                 if ctemplate:
                    the_type = self._outputs[a.name].ctype
                 else:
                    the_type = self._outputs[a.name].nptype
              
              if a.sample_unit:
                 args.append(f"{nb}")
              else:
                 args.append(f"sizeof({the_type})*{nb}")
                 

       argsStr="".join(joinit(args,","))
       inArgsStr="".join(joinit(inargs,","))
       outArgsStr="".join(joinit(outargs,","))

       return ({"ptrs" : ptrs,
               "args" : argsStr,
               "inArgsStr" : inArgsStr,
               "outArgsStr" :outArgsStr,
               "inputs":inputs, 
               "outputs":outputs})

    def cCheck(self,asyncDefaultSkip=True):
        params = self._prepareForCodeGen(True)
        result=GenericFunction.CCHECKTEMPLATE.render(func=self._nodeName,
           ptrs = params["ptrs"],
           args = params["args"],
           inputs=params["inputs"], 
           outputs=params["outputs"],
           node=self,
           asyncDefaultSkip = asyncDefaultSkip
           )
        return(result)
        

    # To clean
    def cRun(self,config,ctemplate=True):
       params = self._prepareForCodeGen(ctemplate)

       if ctemplate:
              result=GenericFunction.CTEMPLATE.render(func=self._nodeName,
               ptrs = params["ptrs"],
               args = params["args"],
               inputs=params["inputs"], 
               outputs=params["outputs"],
               node=self
               )
       else:
           result=GenericFunction.PYTEMPLATE.render(func=self._nodeName,
            ptrs = params["ptrs"],
            args = params["args"],
            inArgs= params["inArgsStr"], 
            outArgs= params["outArgsStr"], 
            inputs=params["inputs"], 
            outputs=params["outputs"],
            node=self
            )
       return(result)

    
class Unary(GenericFunction):
    def __init__(self,funcname,theType,length,input_name="i",output_name="o"):
        GenericFunction.__init__(self,funcname,
            [input_name
            ,output_name
            ,ArgLength(output_name)])



        self.addInput(input_name,theType,length)
        self.addOutput(output_name,theType,length)


class Binary(GenericFunction):
    def __init__(self,funcname,theType,length,input_names=["ia","ib"],output_name="o"):
        GenericFunction.__init__(self,funcname,
            [input_names[0]
            ,input_names[1]
            ,output_name
            ,ArgLength(output_name)])

        self.addInput(input_names[0],theType,length)
        self.addInput(input_names[1],theType,length)
        
        self.addOutput(output_name,theType,length)