
{% for ptr in inputs %}
       {{ptr[0]}}={{ptr[1].access}}getReadBuffer({{ptr[2]}})
{% endfor %}
{% for ptr in outputs %}
       {{ptr[0]}}={{ptr[1].access}}getWriteBuffer({{ptr[2]}})
{% endfor %}
       {{outArgs}}[:]=dsp.{{func}}({{inArgs}})
       cgStaticError = 0
       