                    
                    bool canRun=true;
{% for ptr in inputs %}
                    canRun &= !({{ptr[1].access}}willUnderflowWith({{ptr[2]}}));
{% endfor %}
{% for ptr in outputs %}
                    canRun &= !({{ptr[1].access}}willOverflowWith({{ptr[2]}}));
{% endfor %}

                    if (!canRun)
                    {
{% if asyncDefaultSkip %}
                      cgStaticError = CG_SKIP_EXECUTION;
{% else %}
                      cgStaticError = CG_BUFFER_ERROR;
{%- endif %}
                    }
                    else
                    {
                        cgStaticError = 0;
                    }