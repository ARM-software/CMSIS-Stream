                    
                    bool canRun=true;
{% for ptr in inputs %}
                    canRun &= !({{ptr[1].access}}willUnderflowWith({{nb}}));
{% endfor %}
{% for ptr in outputs %}
                    canRun &= !({{ptr[1].access}}willOverflowWith({{nb}}));
{% endfor %}

                    if (!canRun)
                    {
{% if asyncDefaultSkip %}
                      cgStaticError = CG_SKIP_EXECUTION_ID_CODE;
{% else %}
                      cgStaticError = CG_BUFFER_ERROR_ID_CODE;
{%- endif %}
                    }
                    else
                    {
                        cgStaticError = 0;
                    }