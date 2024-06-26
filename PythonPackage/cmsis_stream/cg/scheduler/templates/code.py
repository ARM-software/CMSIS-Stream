#
# Generated with CMSIS-Stream python scripts.
# The generated code is not covered by CMSIS-Stream license.
# 
# The support classes and code are covered by CMSIS-Stream license.
#

import sys


import numpy as np
import cmsisdsp as dsp
from cmsis_stream.cg.nodes.simu import *
from {{config.appNodesPythonName}} import * 
from {{config.customPythonName}} import *

{% macro optionalargs() -%}
{% if config.pyOptionalArgs %}{{config.pyOptionalArgs}}{% endif %}
{% endmacro -%}

{% if config.dumpFIFO %}
DEBUGSCHED=True
{% else %}
DEBUGSCHED=False
{% endif %}

# 
# FIFO buffers
# 


{% for id in range(nbFifos) %}
FIFOSIZE{{id}}={{fifos[id].length}}

{{config.prefix}}buf{{id}}=np.empty(FIFOSIZE{{id}},dtype={{fifos[id].theType.nptype}})
for i in range(FIFOSIZE{{id}}):
    {{config.prefix}}buf{{id}}[i] = {{fifos[id].theType.fillValue}}

{% endfor %}

def {{config.schedName}}({{optionalargs()}}):
    cgStaticError=0
    nbSchedule=0
{% if config.debug %}
    debugCounter={{config.debugLimit}}
{% endif %}

    #
    #  Create FIFOs objects
    #
{% for id in range(nbFifos) %}
{% if fifos[id].hasDelay  or fifos[id].hasAdditionalArgs %}
    fifo{{id}}={{fifos[id].fifo_class_str}}(FIFOSIZE{{id}},{{config.prefix}}buf{{id}},delay={{fifos[id].delay}}{{fifos[id].fifo_additional_args}})
{% else %}
    fifo{{id}}={{fifos[id].fifo_class_str}}(FIFOSIZE{{id}},{{config.prefix}}buf{{id}})
{% endif %}
{% endfor %}

    # 
    #  Create node objects
    #
{% for node in nodes %}
{% if node.hasState %}
    {{node.nodeName}} = {{node.typeName}}({{node.pythonIoTemplate()}},{{node.args}})
{% endif %}
{% endfor %}

{% if config.debug %}
    while((cgStaticError==0) and (debugCounter > 0)):
{% else %}
    while(cgStaticError==0):
{% endif %}
       nbSchedule = nbSchedule + 1

{% for s in schedule %}
       {{nodes[s].cRun(config,False)}}
       if cgStaticError < 0:
          break
{% if config.dumpFIFO %}
{% for fifoID in sched.outputFIFOs(nodes[s]) %}
       print("{{nodes[s].nodeName}}:{{fifoID[1]}}")
       fifo{{fifoID[0]}}.dump()
{% endfor %}
{% endif %}
{% endfor %}

{% if config.debug %}
       debugCounter = debugCounter - 1 
{% endif %}
    return(nbSchedule,cgStaticError)