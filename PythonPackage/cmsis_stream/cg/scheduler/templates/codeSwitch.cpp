{% extends "commonc.cpp" %}

{% block schedArray %}
/*

Description of the scheduling. 

*/
static {{schedSwitchDataType}} schedule[{{schedLen}}]=
{ 
{{schedDescription}}
};
{% endblock %}

{% block scheduleLoop %}
    CG_BEFORE_SCHEDULE;
{% if config.callback -%}
    CG_RESTORE_STATE_MACHINE_STATE;
{% endif %}
{% if config.debug %}
    while((cgStaticError==0) && (debugCounter > 0))
{% else %}
    while(cgStaticError==0)
{% endif %}
    {
        /* Run a schedule iteration */
        {% if config.eventRecorder -%}
        EventRecord2 (Evt_Scheduler, nbSchedule, 0);
        {% endif -%}
        CG_BEFORE_ITERATION;
        unsigned long id=0;
{% if config.callback %}
        if (scheduleState->status==PAUSED_SCHEDULER)
        {
            id = scheduleState->scheduleStateID;
        }
{% endif %}
        for(; id < {{schedLen}}; id++)
        {
            {% if config.eventRecorder -%}
            EventRecord2 (Evt_Node, schedule[id], 0);
            {% endif -%}
            CG_BEFORE_NODE_EXECUTION(schedule[id]);

            {% if config.asynchronous or config.fullyAsynchronous -%}
            cgStaticError = 0;
            CG_ASYNC_BEFORE_NODE_CHECK(schedule[id]);
            switch(schedule[id])
            {
                {% for nodeID in range(nbNodes) -%}
                case {{nodeID}}:
                {
                    {% if not nodes[nodeID].isPureNode -%}
                    {%- if not config.heapAllocation -%}
                    cgStaticError = {{nodes[nodeID].nodeName}}.prepareForRunning();
                    {%- else -%}
                    cgStaticError = nodes.{{nodes[nodeID].nodeName}}->prepareForRunning();
                    {%- endif -%}
                    {%- else -%}
                    {{nodes[nodeID].cCheck(config.asyncDefaultSkip)}}
                    {%- endif %}

                }
                break;

                {% endfor -%}

                default:
                break;
            }

            CG_ASYNC_AFTER_NODE_CHECK(schedule[id]);

            if (cgStaticError == CG_SKIP_EXECUTION_ID_CODE)
            { 
              cgStaticError = 0;
              CG_NODE_NOT_EXECUTED(schedule[id]);
              continue;
            }

            {% if config.eventRecorder -%}
            if (cgStaticError<0)
            {
                EventRecord2 (Evt_Error, cgStaticError, 0);
            }
            {% endif -%}

            CHECKERROR;

            {% endif -%}

            switch(schedule[id])
            {
                {% for nodeID in range(nbNodes) -%}
                case {{nodeID}}:
                {
                   {{nodes[nodeID].cRun(config)}}

                   {%- if config.dumpFIFO %}
                   {%- for fifoID in sched.outputFIFOs(nodes[nodeID]) %}
                   
                   std::cout << "{{nodes[nodeID].nodeName}}:{{fifoID[1]}}" << std::endl;
                   fifo{{fifoID[0]}}.dump();
                   {%- endfor %}
                   {%- endif %}

                }
                break;

                {% endfor -%}
                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION(schedule[id]);
            {% if config.eventRecorder -%}
            if (cgStaticError<0)
            {
                EventRecord2 (Evt_Error, cgStaticError, 0);
            }
            {% endif -%}

{% if config.callback -%}
            if (cgStaticError == CG_PAUSE_SCHEDULER)
            {
                CG_SAVE_STATE_MACHINE_STATE;
                scheduleState->status = PAUSED_SCHEDULER;
                scheduleState->nbSched = nbSchedule;
                scheduleState->scheduleStateID = id;
            }
{% endif %}
            CHECKERROR;
        }
{% if config.debug %}
       debugCounter--;
{% endif %}
       CG_AFTER_ITERATION;
       nbSchedule++;
    }

{% endblock %}