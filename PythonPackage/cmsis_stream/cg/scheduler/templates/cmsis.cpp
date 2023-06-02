
                  {

{% for ptr in ptrs %}
                   {{theType}}* {{ptr}};
{% endfor %}
{% for ptr in inputs %}
                   {{ptr[0]}}={{ptr[1].access}}getReadBuffer({{nb}});
{% endfor %}
{% for ptr in outputs %}
                   {{ptr[0]}}={{ptr[1].access}}getWriteBuffer({{nb}});
{% endfor %}
                   {{func}}({{args}},{{nb}});
                   cgStaticError = 0;
                  }