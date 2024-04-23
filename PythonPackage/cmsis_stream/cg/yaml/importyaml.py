from ..scheduler import *
from yaml import safe_load

# Include definitions from the Python package
from ..scheduler import GenericNode,GenericSink,GenericSource

def _dataType(s,cstruct):
    if s == 'float64_t':
        return(CType(F64,cmsis_dsp=True))
    if s == 'double':
        return(CType(F64,cmsis_dsp=False))
    if s == 'float32_t':
        return(CType(F32,cmsis_dsp=True))
    if s == 'float':
        return(CType(F32,cmsis_dsp=False))
    if s == 'float16_t':
        return(CType(F16,cmsis_dsp=True))
    if s == 'q31_t':
        return(CType(Q31,cmsis_dsp=True))
    if s == 'q15_t':
        return(CType(Q15,cmsis_dsp=True))
    if s == 'q7_t':
        return(CType(Q7,cmsis_dsp=True))
    if s == 'uint32_t':
        return(CType(UINT32,cmsis_dsp=True))
    if s == 'uint16_t':
        return(CType(UINT16,cmsis_dsp=True))
    if s == 'uint8_t':
        return(CType(UINT8,cmsis_dsp=True))
    if s == 'int32_t':
        return(CType(SINT32,cmsis_dsp=True))
    if s == 'int16_t':
        return(CType(SINT16,cmsis_dsp=True))
    if s == 'int8_t':
        return(CType(SINT8,cmsis_dsp=True))
    if s in cstruct:
        return(cstruct[s])

def _addBufferConstraint(io_yaml,io):
    if 'buffer-constraint' in io_yaml:
        constraint = io_yaml['buffer-constraint']
        io.setBufferConstraint(name=constraint["name"],mustBeArray=constraint["must-be-array"],assignedByNode=constraint["assigned-by-node"])
      

class _YamlSource(GenericSource):
    def __init__(self,yaml,cstruct,identified):
        GenericSource.__init__(self,yaml['node'],identified=identified)
        self._cpp = yaml['kind']
        for o in yaml['outputs']:
            theType = o['type']
            nbSamples = o['samples']
            name = o['output']
            self.addOutput(name,_dataType(theType,cstruct),nbSamples)
            _addBufferConstraint(o,self[name])

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return (self._cpp)

class _YamlSink(GenericSink):
    def __init__(self,yaml,cstruct,identified):
        GenericSink.__init__(self,yaml['node'],identified=identified)
        self._cpp = yaml['kind']
        for i in yaml['inputs']:
            theType = i['type']
            nbSamples = i['samples']
            name = i['input']
            self.addInput(name,_dataType(theType,cstruct),nbSamples)
            _addBufferConstraint(i,self[name])

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return (self._cpp)

def _processInputs(self,yaml,cstruct):
    for i in yaml['inputs']:
        theType = i['type']
        nbSamples = i['samples']
        name = i['input']
        self.addInput(name,_dataType(theType,cstruct),nbSamples)
        _addBufferConstraint(i,self[name])

def _processOutputs(self,yaml,cstruct):
    for o in yaml['outputs']:
        theType = o['type']
        nbSamples = o['samples']
        name = o['output']
        self.addOutput(name,_dataType(theType,cstruct),nbSamples)
        _addBufferConstraint(o,self[name])

def _get_many_io_names(x):
    if 'output' in x:
        if 'name' in x['output']:
            return x['output']['name']
        else:
            return x['output']

    if 'input' in x:
        if 'name' in x['input']:
            return x['input']['name']
        else:
            return x['input']

def _processListOutput(self,yaml,cstruct):
    
    theType = yaml['type']
    nbSamples = yaml['samples']
    self.fromOutputNames = yaml['descriptions']

    self.addManyOutput(_dataType(theType,cstruct),nbSamples,len(self.fromOutputNames))
    for o in range(len(self.fromOutputNames)):
        oyaml = self.fromOutputNames[o]['output']
        name = self.outputNameFromIndex(o)
        _addBufferConstraint(oyaml,self[name])

def _processListInput(self,yaml,cstruct):
    
    theType = yaml['type']
    nbSamples = yaml['samples']
    self.fromInputNames = yaml['descriptions']
   
    self.addManyInput(_dataType(theType,cstruct),nbSamples,len(self.fromInputNames))
    for i in range(len(self.fromInputNames)):
        iyaml = self.fromInputNames[i]['input']
        name = self.inputNameFromIndex(i)
        _addBufferConstraint(iyaml,self[name])

class _YamlNode(GenericNode):
    def __init__(self,yaml,cstruct,identified):
        GenericNode.__init__(self,yaml['node'],identified=identified)
        self._cpp = yaml['kind']
        _processInputs(self,yaml,cstruct)
        _processOutputs(self,yaml,cstruct)
       
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return (self._cpp)
    
class _YamlToManyNode(GenericToManyNode):
    def __init__(self,yaml,cstruct,identified):
        GenericToManyNode.__init__(self,yaml['node'],identified=identified)
        self._cpp = yaml['kind']
        _processInputs(self,yaml,cstruct)
        _processListOutput(self,yaml["outputs"],cstruct)

    def outputNameFromIndex(self,i):
        return(_get_many_io_names(self.fromOutputNames[i]))
       
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return (self._cpp)
    
class _YamlFromManyNode(GenericFromManyNode):
    def __init__(self,yaml,cstruct,identified):
        GenericFromManyNode.__init__(self,yaml['node'],identified=identified)
        self._cpp = yaml['kind']
        _processListInput(self,yaml["inputs"],cstruct)
        _processOutputs(self,yaml,cstruct)

    def inputNameFromIndex(self,i):
        return(_get_many_io_names(self.fromInputNames[i]))
       
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return (self._cpp)
    
class _YamlManyToManyNode(GenericManyToManyNode):
    def __init__(self,yaml,cstruct,identified):
        GenericManyToManyNode.__init__(self,yaml['node'],identified=identified)
        self._cpp = yaml['kind']
        _processListInput(self,yaml["inputs"],cstruct)
        _processListOutput(self,yaml["outputs"],cstruct)

    def outputNameFromIndex(self,i):
        return(_get_many_io_names(self.fromOutputNames[i]))
    
    def inputNameFromIndex(self,i):
        return(_get_many_io_names(self.fromInputNames[i]))
       
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return (self._cpp)



class _YamlGenericFunction(GenericFunction):
    def __init__(self,name,yaml,cstruct,args):
        GenericFunction.__init__(self,name,args)
        for i in yaml['inputs']:
            theType = i['type']
            nbSamples = i['samples']
            name = i['input']
            self.addInput(name,_dataType(theType,cstruct),nbSamples)
            _addBufferConstraint(i,self[name])
        for o in yaml['outputs']:
            theType = o['type']
            nbSamples = o['samples']
            name = o['output']
            self.addOutput(name,_dataType(theType,cstruct),nbSamples)
            _addBufferConstraint(o,self[name])

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return (self._cpp)

def _mkUnaryNode(yaml,cstruct):
    name = yaml['unary']
    firstInput = yaml['inputs'][0]
    theType = _dataType(firstInput['type'],cstruct)
    nbSamples = firstInput['samples']

    return(Unary(name,theType,nbSamples))

def _mkBinaryNode(yaml,cstruct):
    name = yaml['binary']
    firstInput = yaml['inputs'][0]
    theType = _dataType(firstInput['type'],cstruct)
    nbSamples = firstInput['samples']

    return(Binary(name,theType,nbSamples))

def _mkFunctionNode(yaml,cstruct):
    node = yaml['c-function']
    name = node['name']
    args=[] 
    for a in node['args']:
        if 'io' in a:
            args.append(a['io']) 
        elif 'io-length' in a:
            name = a['io-length']['name']
            sample_unit = a['io-length']['sample-unit']
            args.append(ArgLength(name,sample_unit=sample_unit))
        elif 'args' in a:
            args.append(a['args']) 
    return(_YamlGenericFunction(name,yaml,cstruct,args))

def _processArguments(node,n):
    if 'args' in n:
        for k in n['args']:
            if 'literal' in k:
                node.addLiteralArg(k['literal'])
            if 'variable' in k:
                node.addVariableArg(k['variable'])

NODE = 1 
TO_MANY_NODE = 2 
FROM_MANY_NODE = 3
MANY_TO_MANY_NODE = 4 

def _identifyNodeKind(n):
    hasListInput = True 
    hasListOutput = True
    for i in n['inputs']:
        if 'input' in i:
            hasListInput = False
    for i in n['outputs']:
        if 'output' in i:
            hasListOutput = False
    if hasListInput and hasListOutput:
        return(MANY_TO_MANY_NODE)
    if not hasListInput and hasListOutput:
        return(TO_MANY_NODE)
    if hasListInput and not hasListOutput:
        return(FROM_MANY_NODE)
    return(NODE)

def import_graph(filename):
    with open(filename,"r") as f:
        r = safe_load(f)
        if 'graph' in r:
            g = r['graph']
            nodes = {}
            the_graph = Graph()
            cstruct={} 
            if 'options' in g:
                if 'FIFO' in g['options']:
                    the_graph.defaultFIFOClass = g['options']['FIFO']
                if 'Duplicate' in g['options']:
                    the_graph.duplicateNodeClassName = g['options']['Duplicate']
            if 'custom-types' in g:
                for c in g['custom-types']:
                    name = g['custom-types'][c]['cname']
                    if name == "Shared":
                        internal = g['custom-types'][c]['internal']
                        shared = g['custom-types'][c]['shared']
                        s = SharedType(cstruct[internal])
                        s._shared = shared
                        cstruct[c] = s
                    else:
                       size = g['custom-types'][c]['bytes']
                       cstruct[c]=CStructType(name,size)
            if 'nodes' in g:
                for n in g['nodes']:
                    name = n['node']
                    identified = n['identified']
                    if 'inputs' in n and 'outputs' in n:
                       if 'kind' in n:
                           nodeKind = _identifyNodeKind(n)
                           if nodeKind == NODE:
                              nodes[name] = _YamlNode(n,cstruct,identified)
                           if nodeKind == TO_MANY_NODE:
                              nodes[name] = _YamlToManyNode(n,cstruct,identified)
                           if nodeKind == FROM_MANY_NODE:
                              nodes[name] = _YamlFromManyNode(n,cstruct,identified)
                           if nodeKind == MANY_TO_MANY_NODE:
                              nodes[name] = _YamlManyToManyNode(n,cstruct,identified)
                       if 'unary' in n: 
                           nodes[name] = _mkUnaryNode(n,cstruct)
                       if 'binary' in n: 
                           nodes[name] = _mkBinaryNode(n,cstruct)
                       if 'c-function' in n: 
                           nodes[name] = _mkFunctionNode(n,cstruct)
                       _processArguments(nodes[name],n)
                    elif 'inputs' in n:
                       nodes[name] = _YamlSink(n,cstruct,identified)
                       _processArguments(nodes[name],n)
                    elif 'outputs' in n: 
                       nodes[name] = _YamlSource(n,cstruct,identified)
                       _processArguments(nodes[name],n)
                    else:
                       nodes[name] = Constant(name)
                for e in g['edges']:
                    src = nodes[e['src']['node']]
                    o = None
                    if 'output' in e['src']:
                        o = e['src']['output']
    
                    dst = nodes[e['dst']['node']]
                    i = e['dst']['input']
    
                    delay = None 
                    if 'delay' in e:
                        delay = e['delay']
    
                    fifoClass = None 
                    fifoScale = 1.0 
                    fifoAsyncLength = None 
                    fifoWeak = False
                    buffer = None 
                    customBufferMustBeArray = True
    
                    if 'class' in e: 
                        fifoClass = e['class']
    
                    if 'scale' in e:
                        fifoScale = e['scale']

                    if 'async-length' in e:
                        fifoAsyncLength = e['async-length']

                    if 'weak-edge' in e:
                        fifoWeak = e['weak-edge']

                    if 'buffer-constraint' in e:
                        fifoCustomBuffer = e['buffer-constraint']
                        buffer = fifoCustomBuffer['name']
                        customBufferMustBeArray = fifoCustomBuffer['must-be-array']

    
                    if o is None:
                       if delay is not None:
                          the_graph.connectWithDelay(src,dst[i],
                            delay,
                            fifoScale=fifoScale,
                            fifoClass=fifoClass,
                            fifoAsyncLength=fifoAsyncLength,
                            weak=fifoWeak,
                            buffer=buffer,
                            customBufferMustBeArray=customBufferMustBeArray)
                       else:
                          the_graph.connect(src,dst[i],
                            fifoScale=fifoScale,
                            fifoClass=fifoClass,
                            fifoAsyncLength=fifoAsyncLength,
                            weak=fifoWeak,
                            buffer=buffer,
                            customBufferMustBeArray=customBufferMustBeArray)
                    else:
                       if delay is not None:
                          the_graph.connectWithDelay(src[o],dst[i],
                            delay,
                            fifoScale=fifoScale,
                            fifoClass=fifoClass,
                            fifoAsyncLength=fifoAsyncLength,
                            weak=fifoWeak,
                            buffer=buffer,
                            customBufferMustBeArray=customBufferMustBeArray)
                       else:
                          the_graph.connect(src[o],dst[i],
                            fifoScale=fifoScale,
                            fifoClass=fifoClass,
                            fifoAsyncLength=fifoAsyncLength,
                            weak=fifoWeak,
                            buffer=buffer,
                            customBufferMustBeArray=customBufferMustBeArray)
        return(the_graph)

def import_config(filename):
    with open(filename,"r") as f:
        r = safe_load(f)
        conf = Configuration()
        if 'schedule-options' in r:
            so = r['schedule-options']
    
            if 'memory-optimization' in so:
                conf.memoryOptimization = so['memory-optimization']
    
            if 'sink-priority' in so:
                conf.sinkPriority = so['sink-priority']
    
            if 'display-fifo-sizes' in so:
                conf.displayFIFOSizes = so['display-fifo-sizes']
    
            if 'dump-schedule' in so:
                conf.dumpSchedule = so['dump-schedule']

            if 'mem-strategy' in so:
                conf.memStrategy = so['mem-strategy']

            if 'buffer-allocation' in so:
                conf.bufferAllocation = so['buffer-allocation']

            if 'callback' in so:
                conf.callback = so['callback']

            if 'heap-allocation' in so:
                conf.heapAllocation = so['heap-allocation']

            if 'disable-duplicate-optimization' in so:
                conf.disableDuplicateOptimization = so['disable-duplicate-optimization']
    
        if 'code-generation-options' in r:
            co = r['code-generation-options']
    
            if 'debug-limit' in co:
                conf.debugLimit = co['debug-limit']
    
            if 'dump-fifo' in co:
                conf.dumpFIFO = co['dump-fifo']
    
            if 'scheduler-name' in co:
                conf.schedName = co['scheduler-name']
    
            if 'fifo-prefix' in co:
                conf.prefix = co['fifo-prefix']
    
        if 'c-code-generation-options' in r:
            cco = r['c-code-generation-options']
    
            if 'c-optional-args' in cco:
                conf.cOptionalArgs = cco['c-optional-args']
    
            if 'code-array' in cco:
                conf.codeArray = cco['code-array']
    
            if 'switch-case' in cco:
                conf.switchCase = cco['switch-case']

            if 'callback' in cco:
                conf.callback = cco['callback']
    
            if 'event-recorder' in cco:
                conf.eventRecorder = cco['event-recorder']
    
            if 'custom-c-name' in cco:
                conf.customCName = cco['custom-c-name']
    
            if 'post-custom-c-name' in cco:
                conf.postCustomCName = cco['post-custom-c-name']
    
            if 'generic-node-c-name' in cco:
                conf.genericNodeCName = cco['generic-node-c-name']
    
            if 'app-nodes-c-name' in cco:
                conf.appNodesCName = cco['app-nodes-c-name']
    
            if 'scheduler-c-file-name' in cco:
                conf.schedulerCFileName = cco['scheduler-c-file-name']
    
            if 'c-api' in cco:
                conf.CAPI = cco['c-api']
    
            if 'cmsis-dsp' in cco:
                conf.CMSISDSP = cco['cmsis-dsp']
    
            if 'asynchronous' in cco:
                conf.asynchronous = cco['asynchronous']
    
            if 'fifo-increase' in cco:
                conf.FIFOIncrease = cco['fifo-increase']
    
            if 'async-default-skip' in cco:
                conf.asyncDefaultSkip = cco['async-default-skip']

            if 'node-identification' in cco:
                conf.nodeIdentification = cco['node-identification']

            if 'fully-asynchronous' in cco:
                conf.fullyAsynchronous = cco['fully-asynchronous']

            if 'asynchronous' in cco:
                conf.asynchronous = cco['asynchronous']
    
        if 'python-code-generation-options' in r:
            pco = r['python-code-generation-options']
    
            if 'py-optional-args' in pco:
                conf.pyOptionalArgs = pco['py-optional-args']
    
            if 'custom-python-name' in pco:
                conf.customPythonName = pco['custom-python-name']
    
            if 'app-nodes-python-name' in pco:
                conf.appNodesPythonName = pco['app-nodes-python-name']
    
            if 'scheduler-python-file-name' in pco:
                conf.schedulerPythonFileName = pco['scheduler-python-file-name']
    
            if 'scheduler-python-file-name' in pco:
                conf.schedulerPythonFileName = pco['scheduler-python-file-name']
    
        if 'graphviz-code-generation-options' in r:
            gco = r['graphviz-code-generation-options']
    
            if 'horizontal' in gco:
                conf.horizontal = gco['horizontal']
    
            if 'display-fifo-buf' in gco:
                conf.displayFIFOBuf = gco['display-fifo-buf']
    
    
        return(conf)

