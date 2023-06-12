
                  {

{% for ptr in ptrs %}
                   {{ptr[0]}}* {{ptr[1]}};
{% endfor %}
{% for ptr in inputs %}
                   {{ptr[0]}}={{ptr[1].access}}getReadBuffer({{ptr[2]}});
{% endfor %}
{% for ptr in outputs %}
                   {{ptr[0]}}={{ptr[1].access}}getWriteBuffer({{ptr[2]}});
{% endfor %}
                   {{func}}({{args}});
                   cgStaticError = 0;
                  }