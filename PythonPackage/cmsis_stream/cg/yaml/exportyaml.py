from ..scheduler import *
from yaml import dump

_yaml_version = "3.0.0"

class _YAMLConstantEdge():
    def __init__(self,src,dst):
        self._src = src
        self._dst = dst

    @property
    def src(self):
        return self._src

    @property
    def dst(self):
        return self._dst

    def yaml(self):
        yaml_desc = {}
        srcNode = self.src
        dstNode = self.dst.owner 

        yaml_desc["src"] = {
          "node": srcNode.name,
        }
        yaml_desc["dst"] = {
          "node": dstNode.nodeID,
          "input":self.dst.name
        }

        return(yaml_desc)

def _mkBufferConstraint(fifoCustomBuffer):
    buf = {}
    if  fifoCustomBuffer.name:
        buf["name"] = fifoCustomBuffer.name
    buf["must-be-array"] = fifoCustomBuffer.mustBeArray
    buf["assigned-by-node"] = fifoCustomBuffer.assignedByNode
    buf["can-be-shared"] = fifoCustomBuffer.canBeShared
    return buf

class _YAMLEdge():
    def __init__(self,src,dst,fifoClass,fifoScale,fifoDelay,fifoAsyncLength,fifoWeak,constantEdge,fifoCustomBuffer):
        self._src = src
        self._dst = dst
        self._fifoClass = fifoClass 
        self._fifoScale = fifoScale
        self._fifoDelay = fifoDelay
        self._fifoAsyncLength = fifoAsyncLength
        self._fifoWeak = fifoWeak
        self._constantEdge = constantEdge
        self._fifoCustomBuffer = fifoCustomBuffer
        self._fifo_desc_for_code = None

    @property
    def src(self):
        return self._src

    @property
    def dst(self):
        return self._dst

    @property
    def fifoClass(self):
        return self._fifoClass

    @property
    def fifoScale(self):
        return self._fifoScale
    

    @property
    def fifoDelay(self):
        return self._fifoDelay

    @property
    def fifoAsyncLength(self):
        return self._fifoAsyncLength
    
    @property
    def fifoWeak(self):
        return self._fifoWeak
    

    @property
    def constantEdge(self):
        return self._constantEdge

    def _mk_fifo_desc(self):
        if self._fifo_desc_for_code is None:
            if isinstance(self.fifoClass,str):
               raise FIFOClassCannotBeString(self.fifoClass)

            self._fifo_desc_for_code = self.fifoClass(self.theType,self.length)

    def yaml(self):
        yaml_desc = {}
        srcNode = self.src.owner 
        dstNode = self.dst.owner 

        yaml_desc["src"] = {
          "node": srcNode.nodeID,
          "output":self.src.name
        }
        yaml_desc["dst"] = {
          "node": dstNode.nodeID,
          "input":self.dst.name
        }

        if self.fifoClass:
            if isinstance(self.fifoClass,str):
                raise FIFOClassCannotBeString(self.fifoClass)
            if self.fifoClass != StreamFIFO:
               yaml_desc["class"] = self.fifoClass.__name__
        if self.fifoScale:
            if self.fifoScale != 1.0:
               yaml_desc["scale"] = self.fifoScale
        if self.fifoDelay:
            yaml_desc["delay"] = self.fifoDelay
        if self.fifoAsyncLength:
            yaml_desc["async-length"] = self.fifoAsyncLength
        if self.fifoWeak:
            yaml_desc["weak-edge"] = self.fifoWeak
        if self._fifoCustomBuffer and not self.src._bufferConstraint and not self.dst._bufferConstraint:
            yaml_desc["buffer-constraint"] = _mkBufferConstraint(self._fifoCustomBuffer)
        return(yaml_desc)
    
def _create_YAML_type(t,structured_datatypes):
    if isinstance(t,CType):
       return(t.ctype)
    else:
       yaml_desc = {}
       yaml_desc["cname"] = t.ctype 
       #yaml_desc["python_name"] = t._python_name
       if isinstance(t,SharedType):
          yaml_desc["cname"] = "Shared"
          yaml_desc["internal"] = t._refType.ctype
          yaml_desc["shared"] = t.shared
          _create_YAML_type(t._refType,structured_datatypes)
       else:
          yaml_desc["bytes"] = t.bytes

      
       if not t.ctype  in structured_datatypes:
             structured_datatypes[t.ctype] = yaml_desc
       return(t.ctype)

    
    

def _create_YAML_IO(io,structured_datatypes,is_input=False):
    yaml_desc = {}
    if is_input:
       yaml_desc["input"] = io.name 
    else:
       yaml_desc["output"] = io.name 
    if isinstance(io._nbSamples,list):
       yaml_desc["samples"] = list(io.nbSamples)
    else:
       yaml_desc["samples"] = io.nbSamples 
    yaml_desc["type"] = _create_YAML_type(io.theType,structured_datatypes)
    if io._bufferConstraint:
       yaml_desc["buffer-constraint"] = _mkBufferConstraint(io._bufferConstraint)
    
    return(yaml_desc)

def _create_YAML_OUTPUT_LIST_IO(self):
    yaml_desc={}
    yaml_desc["samples"] = self.node._outputLength
    yaml_desc["type"] = _create_YAML_type(self.node._outputType,self.structured_datatypes)
    out=[]
    for o in self.node._outputs:
        io = self.node._outputs[o]
        if io._bufferConstraint:
           buf = _mkBufferConstraint(io._bufferConstraint)
           d={"output":
             {"name":io.name,
              "buffer-constraint":buf
             }
           }
           out.append(d)
    
        else:
            out.append({"output":io.name})
    yaml_desc["descriptions"]=out
    #outputs={"list":yaml_desc}
    outputs = yaml_desc

    return(outputs)

def _create_YAML_INPUT_LIST_IO(self):
    yaml_desc={}
    yaml_desc["samples"] = self.node._inputLength
    yaml_desc["type"] = _create_YAML_type(self.node._inputType,self.structured_datatypes)
    inp=[]
    for o in self.node._inputs:
        io = self.node._inputs[o]
        if io._bufferConstraint:
           buf = _mkBufferConstraint(io._bufferConstraint)
           d={"input":
             {"name":io.name,
              "buffer-constraint":buf
             }
           }
           inp.append(d)
    
        else:
            inp.append({"input":io.name})
    yaml_desc["descriptions"]=inp
    inputs=yaml_desc

    return(inputs)


class _YAMLConstantNode():
    def __init__(self,node):
        self._node = node

    @property
    def node(self):
        return self._node

    def yaml(self):
        res = {}
        res["node"] = self.node.name
        return(res)

def _mkArgs(args):
    print("mkArgs")
    r = []
    for l in args:
        if isinstance(l,ArgLength):
            d = {}
            d['name'] = l.name 
            d['sample-unit'] = l.sample_unit
            r.append({'io-length':d})
        elif isinstance(l,int):
            r.append({'args':l})
        else:
            r.append({'io':l})
    return(r)

class _YAMLNode():
    def __init__(self,node,structured_datatypes):
        self._node = node
        self._structured_datatypes = structured_datatypes

    @property
    def node(self):
        return self._node

    @property
    def structured_datatypes(self):
        return self._structured_datatypes
    

    def yaml(self):
        res = {}
        res["node"] = self.node.nodeID
        res["identified"] = self.node.identified
        if isinstance(self.node,Unary):
           res["unary"] = self.node.nodeName
        elif isinstance(self.node,Binary):
           res["binary"] = self.node.nodeName
        elif isinstance(self.node,GenericFunction):
           d = {'name':self.node.nodeName,'args':_mkArgs(self.node._argsDesc)}
           res["c-function"] = d
        else:
           res["kind"] = self.node.typeName

        inputs = []
        outputs = []

        if isinstance(self.node,GenericFromManyNode) or isinstance(self.node,GenericManyToManyNode):
            inputs=_create_YAML_INPUT_LIST_IO(self)
        else:
            for i in self.node._inputs:
                io = self.node._inputs[i]
                inputs.append(_create_YAML_IO(io,self.structured_datatypes,is_input=True))

        if inputs:
           res["inputs"] = inputs

        if isinstance(self.node,GenericToManyNode) or isinstance(self.node,GenericManyToManyNode):
           outputs=_create_YAML_OUTPUT_LIST_IO(self)
        else:
            for o in self.node._outputs:
                io = self.node._outputs[o]
                outputs.append(_create_YAML_IO(io,self.structured_datatypes))

        if outputs:
           res["outputs"] = outputs

        if self.node.schedArgs:
           yaml_args = []
           for arg in self.node.schedArgs:
               if isinstance(arg,ArgLiteral):
                  yaml_args.append({"literal" : arg._name})
               else:
                  yaml_args.append({"variable" : arg._name})
           if len(yaml_args)>0:
              res["args"] = yaml_args
           else:
              print(f"Error parsing args for node {self.node.nodeID}")

        return(res)


def export_graph(graph,filename):
    allNodes = {}
    allEdges = {}
    structured_datatypes = {}
    yaml = {}
    # Parse edges having a constant node as source first
    for edge in graph._constantEdges:
        (src,dst) = edge
        dstNode = dst.owner 
        srcNode = src

        if not srcNode in allNodes:
            allNodes[srcNode] = _YAMLConstantNode(srcNode)
        if not dst.owner in allNodes:
            allNodes[dstNode] = _YAMLNode(dstNode,structured_datatypes)

        if not edge in allEdges:
           allEdges[edge] = _YAMLConstantEdge(src,dst)


    for edge in graph._edges:
        (src,dst) = edge
        fifoClass = None
        fifoScale = None 
        fifoDelay = None
        fifoAsyncLength = None 
        fifoWeak = False
        constantEdge = False
        fifoCustomBuffer = None

        srcNode = src.owner 
        dstNode = dst.owner 


        if edge in graph._FIFOClasses:
            fifoClass = graph._FIFOClasses[edge]
        if edge in graph._FIFOScale:
            fifoScale = graph._FIFOScale[edge]
        if edge in graph._delays:
            fifoDelay = graph._delays[edge]

        if edge in graph._FIFOCustomBuffer:
            fifoCustomBuffer = graph._FIFOCustomBuffer[edge]


        if edge in graph._FIFOAsyncLength:
            fifoAsyncLength = graph._FIFOAsyncLength[edge]

        if edge in graph._FIFOWeak:
            fifoWeak = graph._FIFOWeak[edge]

        if edge in graph._constantEdges:
            constantEdge = graph._constantEdges[edge]

        if not edge in allEdges:
            allEdges[edge] = _YAMLEdge(src,dst,fifoClass,fifoScale,fifoDelay,fifoAsyncLength,fifoWeak,constantEdge,fifoCustomBuffer)

        if not srcNode in allNodes:
            allNodes[srcNode] = _YAMLNode(srcNode,structured_datatypes)
        if not dst.owner in allNodes:
            allNodes[dstNode] = _YAMLNode(dstNode,structured_datatypes)
        
    yaml["version"] = _yaml_version

    # Scanning nodes is required to extract the structured datatypes
    nodes = [allNodes[x].yaml() for x in allNodes] 
    edges = [allEdges[x].yaml() for x in allEdges]

    options = None
    if graph.defaultFIFOClass != StreamFIFO or graph.duplicateNodeClassName != "Duplicate":
       options = {}
       if graph.defaultFIFOClass != StreamFIFO:
          options['FIFO'] = graph.defaultFIFOClass.__name__
       if graph.duplicateNodeClassName != 'Duplicate':
          options['Duplicate'] = graph.duplicateNodeClassName


    if len(structured_datatypes)>0:
        if options:
            yaml["graph"] = {
             "options" : options,
             "custom-types" : structured_datatypes,
             "nodes":nodes,
             "edges":edges,
           }
        else:
           yaml["graph"] = {
             "custom-types" : structured_datatypes,
             "nodes":nodes,
             "edges":edges,
           }
    else:
        if options:
            yaml["graph"] = {
              "options" : options,
              "nodes":nodes,
              "edges":edges,
            }
        else:
            yaml["graph"] = {
              "nodes":nodes,
              "edges":edges,
            }
   

    with open(filename,"w") as f:
         print(dump(yaml, default_flow_style=False, sort_keys=False),file=f)

def export_config(config,filename):
    yaml = {}
    yaml["version"] = _yaml_version
    default = Configuration()

    schedule_options = {}
    if config.memoryOptimization != default.memoryOptimization:
        schedule_options["memory-optimization"] = config.memoryOptimization

    if config.sinkPriority  != default.sinkPriority :
        schedule_options["sink-priority"] = config.sinkPriority

    if config.displayFIFOSizes   != default.displayFIFOSizes  :
        schedule_options["display-fifo-sizes"] = config.displayFIFOSizes

    if config.dumpSchedule   != default.dumpSchedule  :
        schedule_options["dump-schedule"] = config.dumpSchedule 

    if config.memStrategy != default.memStrategy:
        schedule_options["mem-strategy"] = config.memStrategy

    if config.bufferAllocation != default.bufferAllocation:
        schedule_options["buffer-allocation"] = config.bufferAllocation

    if config.disableDuplicateOptimization != default.disableDuplicateOptimization:
        schedule_options["disable-duplicate-optimization"] = config.disableDuplicateOptimization

    if config.callback != default.callback:
        schedule_options["callback"] = config.callback

    if config.heapAllocation   != default.heapAllocation        :
        schedule_options["heap-allocation"] = config.heapAllocation 

    if schedule_options:
        yaml["schedule-options"] = schedule_options 

    
    code_gen = {}

    if config.debugLimit != default.debugLimit:
        code_gen["debug-limit"] = config.debugLimit  

    if config.dumpFIFO  != default.dumpFIFO :
        code_gen["dump-fifo"] = config.dumpFIFO  

    if config.schedName   != default.schedName  :
        code_gen["scheduler-name"] = config.schedName  


    if config.prefix    != default.prefix   :
        code_gen["fifo-prefix"] = config.prefix   


    if code_gen:
        yaml["code-generation-options"] = code_gen 

    c_code_gen = {}

    if config.cOptionalInitArgs  != default.cOptionalInitArgs:
        c_code_gen["c-optional-init-args"] = config.cOptionalInitArgs 

    if config.cOptionalExecutionArgs  != default.cOptionalExecutionArgs:
        c_code_gen["c-optional-execution-args"] = config.cOptionalExecutionArgs

    if config.cOptionalFreeArgs  != default.cOptionalFreeArgs:
        c_code_gen["c-optional-free-args"] = config.cOptionalFreeArgs

    if config.switchCase  != default.switchCase:
        c_code_gen["switch-case"] = config.switchCase 

    if config.eventRecorder   != default.eventRecorder :
        c_code_gen["event-recorder"] = config.eventRecorder 

    if config.appConfigCName    != default.appConfigCName  :
        c_code_gen["app-config-c-name"] = config.appConfigCName 

    if config.postCustomCName     != default.postCustomCName   :
        c_code_gen["post-custom-c-name"] = config.postCustomCName      

    if config.genericNodeCName      != default.genericNodeCName    :
        c_code_gen["generic-node-c-name"] = config.genericNodeCName       

    if config.appNodesCName      != default.appNodesCName    :
        c_code_gen["app-nodes-c-name"] = config.appNodesCName       

    if config.schedulerCFileName != default.schedulerCFileName    :
        c_code_gen["scheduler-c-file-name"] = config.schedulerCFileName

    if config.CAPI  != default.CAPI     :
        c_code_gen["c-api"] = config.CAPI 

    if config.CMSISDSP   != default.CMSISDSP      :
        c_code_gen["cmsis-dsp"] = config.CMSISDSP 

    if config.asynchronous != default.asynchronous      :
        c_code_gen["asynchronous"] = config.asynchronous

    if config.FIFOIncrease  != default.FIFOIncrease       :
        c_code_gen["fifo-increase"] = config.FIFOIncrease 

    if config.asyncDefaultSkip   != default.asyncDefaultSkip        :
        c_code_gen["async-default-skip"] = config.asyncDefaultSkip  

    if config.nodeIdentification   != default.nodeIdentification        :
        c_code_gen["node-identification"] = config.nodeIdentification  

    if config.fullyAsynchronous   != default.fullyAsynchronous        :
        c_code_gen["fully-asynchronous"] = config.fullyAsynchronous  

    if config.asynchronous   != default.asynchronous        :
        c_code_gen["asynchronous"] = config.asynchronous 

    if c_code_gen:
        yaml["c-code-generation-options"] = c_code_gen 

    python_code_gen = {}
    if config.pyOptionalArgs    != default.pyOptionalArgs         :
        python_code_gen["py-optional-args"] = config.pyOptionalArgs    

    if config.appConfigPythonName     != default.appConfigPythonName          :
        python_code_gen["app-config-python-name"] = config.appConfigPythonName     

    if config.appNodesPythonName != default.appNodesPythonName          :
        python_code_gen["app-nodes-python-name"] = config.appNodesPythonName     

    if config.schedulerPythonFileName != default.schedulerPythonFileName           :
        python_code_gen["scheduler-python-file-name"] = config.schedulerPythonFileName      

    if python_code_gen:
        yaml["python-code-generation-options"] = python_code_gen 

    graphviz_code_gen = {}
    if config.horizontal  != default.horizontal            :
        graphviz_code_gen["horizontal"] = config.horizontal       

    if config.displayFIFOBuf   != default.displayFIFOBuf             :
        graphviz_code_gen["display-fifo-buf"] = config.displayFIFOBuf        

    if graphviz_code_gen:
        yaml["graphviz-code-generation-options"] = graphviz_code_gen 

    with open(filename,"w") as f:
         print(dump(yaml, default_flow_style=False, sort_keys=False),file=f)


