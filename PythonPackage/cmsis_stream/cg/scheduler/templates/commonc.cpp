/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

{% if config.dumpFIFO %}
#define DEBUGSCHED 1
{% endif %}

{% if config.CMSISDSP -%}
#include "arm_math.h"
{% else %}
#include <cstdint>
{% endif %}
#include "{{config.customCName}}"
#include "{{config.cgStatusCName}}"
#include "{{config.genericNodeCName}}"
#include "{{config.appNodesCName}}"
#include "{{config.schedulerCFileName}}.h"
{% if config.postCustomCName -%}
#include "{{config.postCustomCName}}"
{% endif %}

{% include "defineConfig.h" %}


CG_AFTER_INCLUDES

{% macro optionalargs(first) -%}
{% if config.cOptionalArgs %}{% if not first %},{% endif %}{{config.cOptionalArgs}}{% endif %}
{% endmacro -%}

{% macro async() -%}
{% if config.asynchronous or config.fullyAsynchronous%}1{% else %}0{% endif %}
{% endmacro %}

using namespace arm_cmsis_stream;

{% block schedArray %}
{% endblock %}

/*

Internal ID identification for the nodes

*/
{% for nodeID in range(nbNodes) -%}
{% if nodes[nodeID].hasState %}
#define {{nodes[nodeID].nodeName | upper}}_INTERNAL_ID {{nodeID}}
{% endif %}
{% endfor %}

{% if config.callback -%}
/* For callback management */

#define CG_PAUSED_SCHEDULER_ID 1
#define CG_SCHEDULER_NOT_STARTED_ID 2
#define CG_SCHEDULER_RUNNING_ID 3

struct cb_state_t
{
    unsigned long scheduleStateID;
    unsigned long nbSched;
    int status;
    kCBStatus running;
};

static cb_state_t cb_state;


static void init_cb_state()
{
    cb_state.status = CG_SCHEDULER_NOT_STARTED_ID;
    cb_state.nbSched = 0;
    cb_state.scheduleStateID = 0;
    cb_state.running = kNewExecution;
}
{% endif %}

{% if config.nodeIdentification %}
/***********

Node identification

************/
{% if config.CAPI -%}
static void * identifiedNodes[{{config.prefix | upper}}NB_IDENTIFIED_NODES]={0};
{% else %}
static NodeBase * identifiedNodes[{{config.prefix | upper}}NB_IDENTIFIED_NODES]={0};
{% endif %}
{% endif %}

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
{% for fifo in fifos %}
#define FIFOSIZE{{fifo.fifoID}} {{fifo.length}}
{% endfor %}

{% if config.bufferAllocation %}
{% if sched._graph._allBuffers|length >0 %}
typedef struct {
{% for buf in sched._graph._allBuffers %}
{{buf._theType.ctype}}  *buf{{buf._bufferID}};
{% endfor %}
} buffers_t;
{% endif %}

CG_BEFORE_BUFFER
{% if sched._graph._allBuffers|length >0 %}
static buffers_t buffers={0};
{% endif %}

int init_buffer_{{config.schedName}}({{optionalargs(True)}})
{
{% for buf in sched._graph._allBuffers %}
    buffers.buf{{buf._bufferID}} = ({{buf._theType.ctype}} *)CG_MALLOC({{buf._length}} * sizeof({{buf._theType.ctype}}));
    if (buffers.buf{{buf._bufferID}}==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
{% endfor %}
    return(CG_SUCCESS);
}

void free_buffer_{{config.schedName}}({{optionalargs(True)}})
{
{% for buf in sched._graph._allBuffers %}
    if (buffers.buf{{buf._bufferID}}!=NULL)
    {
        CG_FREE(buffers.buf{{buf._bufferID}});
    }
{% endfor %}
}

{% else %}
{% for buf in sched._graph._allBuffers %}
#define BUFFERSIZE{{buf._bufferID}} {{buf._length}}
CG_BEFORE_BUFFER
{{buf._theType.ctype}} {{config.prefix}}buf{{buf._bufferID}}[BUFFERSIZE{{buf._bufferID}}]={0};

{% endfor %}
{% endif %}

{% if config.heapAllocation %}
typedef struct {
{% for id in range(nbFifos) %}
{{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}> *fifo{{id}};
{% endfor %}
} fifos_t;

typedef struct {
{% for node in nodes %}
{% if node.hasState %}
    {{node.typeName}}<{{node.ioTemplate()}}> *{{node.nodeName}};
{% endif %}
{% endfor %}
} nodes_t;

CG_BEFORE_BUFFER
static fifos_t fifos={0};

CG_BEFORE_BUFFER
static nodes_t nodes={0};

{% if config.nodeIdentification %}
{% if config.CAPI -%}
void *get_{{config.schedName}}_node(int32_t nodeID)
{% else %}
NodeBase *get_{{config.schedName}}_node(int32_t nodeID)
{% endif %}
{
    if (nodeID >= {{config.prefix | upper}}NB_IDENTIFIED_NODES)
    {
        return(NULL);
    }
    if (nodeID < 0)
    {
        return(NULL);
    }
    return(identifiedNodes[nodeID]);
}
{% endif %}

int init_{{config.schedName}}({{optionalargs(True)}})
{
{% if config.callback -%}
    init_cb_state();
{% endif %}

    CG_BEFORE_FIFO_INIT;
{% for id in range(nbFifos) %}
{% if fifos[id].hasDelay or fifos[id].hasAdditionalArgs %}
    fifos.fifo{{id}} = new {{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}>({{fifos[id].bufName(config)}},{{fifos[id].delay}}{{fifos[id].fifo_additional_args}});
    if (fifos.fifo{{id}}==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
{% else %}
    fifos.fifo{{id}} = new {{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}>({{fifos[id].bufName(config)}}{{fifos[id].fifo_additional_args}});
    if (fifos.fifo{{id}}==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
{% endif %}
{% endfor %}

    CG_BEFORE_NODE_INIT;
{% for node in nodes %}
{% if node.hasState %}
    nodes.{{node.nodeName}} = new {{node.typeName}}<{{node.ioTemplate()}}>({{node.args}});
    if (nodes.{{node.nodeName}}==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
{% if config.nodeIdentification -%}
{% if node.identified -%}
{% if config.CAPI %}
    identifiedNodes[{{node.identificationName}}]=(void*)nodes.{{node.nodeName}};
{% else %}
    identifiedNodes[{{node.identificationName}}]=nodes.{{node.nodeName}};
{% endif %}
    nodes.{{node.nodeName}}->setID({{node.identificationName}});
{% endif %}
{% endif %}
{% endif %}
{% endfor %}

    return(CG_SUCCESS);

}

void free_{{config.schedName}}({{optionalargs(True)}})
{
{% for id in range(nbFifos) %}
    if (fifos.fifo{{id}}!=NULL)
    {
       delete fifos.fifo{{id}};
    }
{% endfor %}

{% for node in nodes %}
{% if node.hasState %}
    if (nodes.{{node.nodeName}}!=NULL)
    {
        delete nodes.{{node.nodeName}};
    }
{% endif %}
{% endfor %}
}

{% endif %}

CG_BEFORE_SCHEDULER_FUNCTION
uint32_t {{config.schedName}}(int *error{{optionalargs(False)}})
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
{% if config.debug %}
    int32_t debugCounter={{config.debugLimit}};
{% endif %}

{% if config.callback -%}
   if (cb_state.status==CG_PAUSED_SCHEDULER_ID)
   {
      nbSchedule = cb_state.nbSched;

   }
{% endif %}

{% if not config.heapAllocation %}
    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
{% for id in range(nbFifos) %}
{% if fifos[id].hasDelay or fifos[id].hasAdditionalArgs %}
    {{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}> fifo{{id}}({{fifos[id].bufName(config)}},{{fifos[id].delay}}{{fifos[id].fifo_additional_args}});
{% else %}
    {{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}> fifo{{id}}({{fifos[id].bufName(config)}}{{fifos[id].fifo_additional_args}});
{% endif %}
{% endfor %}

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
{% for node in nodes %}
{% if node.hasState %}
    {{node.typeName}}<{{node.ioTemplate()}}> {{node.nodeName}}({{node.args}}); /* Node ID = {{node.codeID}} */
{% endif %}
{% endfor %}
{% endif %}

    /* Run several schedule iterations */
{% block scheduleLoop %}
{% endblock %}
errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
}