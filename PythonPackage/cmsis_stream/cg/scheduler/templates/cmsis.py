
{% for ptr in inputs %}
       {{ptr[0]}}={{ptr[1].access}}getReadBuffer({{nb}})
{% endfor %}
{% for ptr in outputs %}
       {{ptr[0]}}={{ptr[1].access}}getWriteBuffer({{nb}})
{% endfor %}
       {{outArgs}}[:]=dsp.{{func}}({{inArgs}})
       cgStaticError = 0
       