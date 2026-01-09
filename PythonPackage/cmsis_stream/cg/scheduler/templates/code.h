/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef {{config.schedulerCFileName |replace(".h","")|upper()}}_H_ 
#define {{config.schedulerCFileName |replace(".h","")|upper()}}_H_


{% macro initOptionalargs(first) -%}
{% if config.cOptionalInitArgs %}{% if not first %},{% endif %}{{config.cOptionalInitArgs}}{% endif %}
{% endmacro -%}

{% macro executionOptionalargs(first) -%}
{% if config.cOptionalExecutionArgs %}{% if not first %},{% endif %}{{config.cOptionalExecutionArgs}}{% endif %}
{% endmacro -%}

{% macro freeOptionalargs(first) -%}
{% if config.cOptionalFreeArgs %}{% if not first %},{% endif %}{{config.cOptionalFreeArgs}}{% endif %}
{% endmacro -%}

{% if config.CAPI -%}

#include <stdint.h>

#ifdef   __cplusplus
extern "C"
{
#endif

{% if config.nodeIdentification -%}
#include "{{config.cnodeAPI}}"
{% endif %}
{% else %}
#include <cstdint>
#include "EventQueue.hpp"
{% if config.nodeIdentification -%}
#include "{{config.cnodeAPI}}"
{% endif %}
{% endif %}

{% if config.eventRecorder %}
#include "EventRecorder.h"

#define EvtSched 0x01 

#define Evt_Scheduler   EventID (EventLevelAPI,   EvtSched, 0x00)
#define Evt_Node        EventID (EventLevelAPI,   EvtSched, 0x01)
#define Evt_Error       EventID (EventLevelError,   EvtSched, 0x02)

{% endif %}

{% if config.nodeIdentification %}
/* Node identifiers */
#define {{config.prefix | upper}}NB_IDENTIFIED_NODES {{identifiedNodes|length}}
{% for node in identifiedNodes %}
#define {{node[0]}} {{node[1]}}
{% endfor %}

{% if selector_defines %}
/* Selectors global identifiers */
{{ selector_defines}}
{% endif %}

{% if config.CAPI -%}
extern {{config.cNodeStruct}}* get_{{config.schedName}}_node(int32_t nodeID);
{% else %}
extern CStreamNode *get_{{config.schedName}}_node(int32_t nodeID);
{% endif %}
{% endif %}

{% if config.bufferAllocation %}
extern int init_buffer_{{config.schedName}}({{initOptionalargs(True)}});
extern void free_buffer_{{config.schedName}}({{initOptionalargs(True)}});
{% endif %}

{% if config.heapAllocation %}
{% if config.CAPI -%}
extern int init_{{config.schedName}}(void *evtQueue_{{initOptionalargs(False)}});
{% else %}
extern int init_{{config.schedName}}(arm_cmsis_stream::EventQueue *evtQueue{{initOptionalargs(False)}});
{% endif %}
extern void free_{{config.schedName}}({{freeOptionalargs(True)}});
{% endif %}

extern uint32_t {{config.schedName}}(int *error{{executionOptionalargs(False)}});

{% if config.CAPI -%}
#ifdef   __cplusplus
}
#endif
{% endif %}

#endif

