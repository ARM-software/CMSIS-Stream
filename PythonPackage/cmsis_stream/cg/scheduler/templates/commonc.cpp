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
#include "{{config.appConfigCName}}"
#include "stream_platform_config.hpp"
#include "cg_enums.h"
#include "StreamNode.hpp"
{% if config.nodeIdentification -%}
#include "{{config.cnodeAPI}}"
#include "{{config.cnodeTemplate}}"
{% endif %}
#include "EventQueue.hpp"
#include "GenericNodes.hpp"
#include "{{config.appNodesCName}}"
#include "{{config.schedulerCFileName}}.h"
{% if config.postCustomCName -%}
#include "{{config.postCustomCName}}"
{% endif %}

{% include "defineConfig.h" %}


CG_AFTER_INCLUDES

{% macro initOptionalargs(first) -%}
{% if config.cOptionalInitArgs %}{% if not first %},{% endif %}{{config.cOptionalInitArgs}}{% endif %}
{% endmacro -%}

{% macro executionOptionalargs(first) -%}
{% if config.cOptionalExecutionArgs %}{% if not first %},{% endif %}{{config.cOptionalExecutionArgs}}{% endif %}
{% endmacro -%}

{% macro freeOptionalargs(first) -%}
{% if config.cOptionalFreeArgs %}{% if not first %},{% endif %}{{config.cOptionalFreeArgs}}{% endif %}
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
{% for nodeID in range(nbAllNodes) -%}
{% if allNodes[nodeID].hasState %}
#define {{allNodes[nodeID].nodeName | upper}}_INTERNAL_ID {{nodeID}}
{% endif %}
{% endfor %}

{% if selector_inits %}
/* Initialize the selectors global IDs in each class */
{{selector_inits}}
{% endif %}

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
{% if identifiedNodes %}
/***********

Node identification

************/
static {{config.cNodeStruct}} identifiedNodes[{{config.prefix | upper}}NB_IDENTIFIED_NODES]={0};
{% else %}
/* No identified nodes found so array is forced to length 1*/
static {{config.cNodeStruct}} identifiedNodes[1]={0};
{% endif %}
{% endif %}

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
{% if fifos|length > 0 %}
{% for fifo in fifos %}
#define FIFOSIZE{{fifo.fifoID}} {{fifo.length}}
{% endfor %}
{% endif %}

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

int init_buffer_{{config.schedName}}({{initOptionalargs(True)}})
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

void free_buffer_{{config.schedName}}({{freeOptionalargs(True)}})
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
{% if nbFifos > 0 %}
typedef struct {
{% for id in range(nbFifos) %}
{{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}> *fifo{{id}};
{% endfor %}
} fifos_t;
{% endif %}

typedef struct {
{% for node in allNodes %}
{% if node.hasState %}
    {{node.typeName}}{{node.ioTemplate()}} *{{node.nodeName}};
{% endif %}
{% endfor %}
} nodes_t;


{% if nbFifos > 0 %}
static fifos_t fifos={0};
{% endif %}

static nodes_t nodes={0};

{% if config.nodeIdentification %}
{{config.cNodeStruct}}* get_{{config.schedName}}_node(int32_t nodeID)
{
    if (nodeID >= {{config.prefix | upper}}NB_IDENTIFIED_NODES)
    {
        return(nullptr);
    }
    if (nodeID < 0)
    {
        return(nullptr);
    }
    return(&identifiedNodes[nodeID]);
}
{% endif %}

{% if config.CAPI -%}
int init_{{config.schedName}}(void *evtQueue_{{initOptionalargs(False)}})
{% else %}
int init_{{config.schedName}}(EventQueue *evtQueue{{initOptionalargs(False)}})
{% endif %}
{
{% if config.CAPI %}
    EventQueue *evtQueue = reinterpret_cast<EventQueue *>(evtQueue_);
{% endif %}
{% if config.callback -%}
    init_cb_state();
{% endif %}

    CG_BEFORE_FIFO_INIT;
{% for id in range(nbFifos) %}
{% if fifos[id].hasDelay or fifos[id].hasAdditionalArgs %}
    fifos.fifo{{id}} = new (std::nothrow) {{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}>({{fifos[id].bufName(config)}},{{fifos[id].delay}}{{fifos[id].fifo_additional_args}});
    if (fifos.fifo{{id}}==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
{% else %}
    fifos.fifo{{id}} = new (std::nothrow) {{fifos[id].fifo_class_str}}<{{fifos[id].theType.ctype}},FIFOSIZE{{id}},{{fifos[id].isArrayAsInt}},{{async()}}>({{fifos[id].bufName(config)}}{{fifos[id].fifo_additional_args}});
    if (fifos.fifo{{id}}==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
{% endif %}
{% endfor %}

    CG_BEFORE_NODE_INIT;
    cg_status initError;

{% for node in allNodes %}
{% if node.hasState %}
    nodes.{{node.nodeName}} = new (std::nothrow) {{node.typeName}}{{node.ioTemplate()}}{{init_args(sched,node)}};
    if (nodes.{{node.nodeName}}==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
{% if config.nodeIdentification -%}
{% if node.identified %}
    identifiedNodes[{{node.identificationName}}]={{config.cNodeStructCreation}}(*nodes.{{node.nodeName}});
    nodes.{{node.nodeName}}->setID({{node.identificationName}});
{% endif %}
{% endif %}
{% endif %}

{% endfor %}

/* Subscribe nodes for the event system*/
{% for event_edge in eventConnections %}
    nodes.{{event_edge[0].owner.nodeName}}->subscribe({{event_edge[0].name}},*nodes.{{event_edge[1].owner.nodeName}},{{event_edge[1].name}});
{% endfor %}

    initError = CG_SUCCESS;
{% for node in allNodes %}
{% if node.hasState %}
    initError = nodes.{{node.nodeName}}->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
{% endif %}
{% endfor %}
   


    return(CG_SUCCESS);

}

void free_{{config.schedName}}({{freeOptionalargs(True)}})
{
{% for id in range(nbFifos) %}
    if (fifos.fifo{{id}}!=NULL)
    {
       delete fifos.fifo{{id}};
    }
{% endfor %}

{% for node in allNodes %}
{% if node.hasState %}
    if (nodes.{{node.nodeName}}!=NULL)
    {
        delete nodes.{{node.nodeName}};
    }
{% endif %}
{% endfor %}
}

void reset_fifos_{{config.schedName}}(int all)
{
{% for id in range(nbFifos) %}
    if (fifos.fifo{{id}}!=NULL)
    {
       fifos.fifo{{id}}->reset();
    }
{% endfor %}
   // Buffers are set to zero too
   if (all)
   {
{% if config.bufferAllocation %}
{% for buf in sched._graph._allBuffers %}
       if (buffers.buf{{buf._bufferID}}!=NULL)
       {
           std::fill_n(buffers.buf{{buf._bufferID}}, {{buf._length}}, ({{buf._theType.ctype}})0);
       }
{% endfor %}
{% else %}
{% for buf in sched._graph._allBuffers %}
       std::fill_n({{config.prefix}}buf{{buf._bufferID}}, BUFFERSIZE{{buf._bufferID}}, ({{buf._theType.ctype}})0);
{% endfor %}
{% endif %}
   }
}

{% endif %}

CG_BEFORE_SCHEDULER_FUNCTION
{% if not config.heapAllocation %}
{% if config.CAPI -%}
uint32_t {{config.schedName}}(int *error,void *evtQueue_{{executionOptionalargs(False)}})
{% else %}
uint32_t {{config.schedName}}(int *error,EventQueue *evtQueue{{executionOptionalargs(False)}})
{% endif %}
{% else %}
uint32_t {{config.schedName}}(int *error{{executionOptionalargs(False)}})
{% endif %}
{
    {% if not config.heapAllocation -%}
    {% if config.CAPI -%}
    EventQueue *evtQueue = reinterpret_cast<EventQueue *>(evtQueue_);
    {% endif -%}
    {% endif -%}
{% if schedLen > 0 -%}
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


{% for node in allNodes %}
{% if node.hasState %}
    {{node.typeName}}{{node.ioTemplate()}} {{node.nodeName}}{{init_args(sched,node)}}; /* Node ID = {{node.codeID}} */
{% endif %}
{% endfor %}


/* Subscribe nodes for the event system*/
{% for event_edge in eventConnections %}
    {{event_edge[0].owner.nodeName}}.subscribe({{event_edge[0].name}},{{event_edge[1].owner.nodeName}},{{event_edge[1].name}});
{% endfor %}

    cgStaticError = CG_SUCCESS;
{% for node in allNodes %}
{% if node.hasState %}
    cgStaticError = {{node.nodeName}}.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
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
    
{% else %}
{% if not config.heapAllocation %}
    /* 
    Create node objects
    */
{% for node in allNodes %}
{% if node.hasState %}
    {{node.typeName}}{{node.ioTemplate()}} {{node.nodeName}}{{init_args(sched,node)}}; /* Node ID = {{node.codeID}} */
{% endif %}
{% endfor %}
/* Subscribe nodes for the event system*/
{% for event_edge in eventConnections %}
    {{event_edge[0].owner.nodeName}}.subscribe({{event_edge[0].name}},{{event_edge[1].owner.nodeName}},{{event_edge[1].name}});
{% endfor %}
{% endif %}
{% endif %}
{% if schedLen == 0 %}
    *error=CG_STOP_SCHEDULER;
#if !defined(CG_EVENTS_MULTI_THREAD)
    while(1){
        // To have possibility to process the event queue
        CG_BEFORE_ITERATION;
    };
#endif
    return(0);
{% endif %}
}