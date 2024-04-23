/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef _{{config.schedulerCFileName |replace(".h","")|upper()}}_H_ 
#define _{{config.schedulerCFileName |replace(".h","")|upper()}}_H_

{% macro optionalargs(first) -%}
{% if config.cOptionalArgs %}{% if not first %},{% endif %}{{config.cOptionalArgs}}{% endif %}
{% endmacro -%}

{% if config.CAPI -%}
#ifdef   __cplusplus
extern "C"
{
#endif
{% endif %}

{% if config.callback -%}
typedef struct {{config.schedName}}_cb
{
    unsigned long scheduleStateID;
    unsigned long nbSched;
    int status;
} {{config.schedName}}_cb_t;

extern void init_cb_state_{{config.schedName}}({{config.schedName}}_cb_t*);
{% endif %}

{% if config.eventRecorder %}
#include "EventRecorder.h"

#define EvtSched 0x01 

#define Evt_Scheduler   EventID (EventLevelAPI,   EvtSched, 0x00)
#define Evt_Node        EventID (EventLevelAPI,   EvtSched, 0x01)
#define Evt_Error       EventID (EventLevelError,   EvtSched, 0x02)

{% endif %}

{% if config.nodeIdentification %}
#define {{config.prefix | upper}}NB_IDENTIFIED_NODES {{identifiedNodes|length}}
{% for node in identifiedNodes %}
#define {{node[0]}} {{node[1]}}
{% endfor %}

{% if config.CAPI -%}
extern void *get_{{config.schedName}}_node(int32_t nodeID);
{% else %}
extern NodeBase *get_{{config.schedName}}_node(int32_t nodeID);
{% endif %}
{% endif %}

{% if config.bufferAllocation %}
extern int init_buffer_{{config.schedName}}({{optionalargs(True)}});
extern void free_buffer_{{config.schedName}}({{optionalargs(True)}});
{% endif %}

{% if config.heapAllocation %}
extern int init_{{config.schedName}}({{optionalargs(True)}});
extern void free_{{config.schedName}}({{optionalargs(True)}});
{% endif %}

{% if config.callback %}
extern uint32_t {{config.schedName}}(int *error,{{config.schedName}}_cb_t *scheduleState{{optionalargs(False)}});
{% else %}
extern uint32_t {{config.schedName}}(int *error{{optionalargs(False)}});
{% endif %}

{% if config.CAPI -%}
#ifdef   __cplusplus
}
#endif
{% endif %}

#endif

