                    
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
                      cgStaticError = CG_SKIP_EXECUTION_ID_CODE;
{% else %}
                      cgStaticError = CG_BUFFER_ERROR_ID_CODE;
{%- endif %}
                    }
                    else
                    {
                        cgStaticError = 0;
                    }