#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif

{% if config.bufferAllocation %}
#if !defined(CG_MALLOC)
#define CG_MALLOC(A) malloc((A))
#endif 

#if !defined(CG_FREE)
#define CG_FREE(A) free((A))
#endif 
{% endif %}

#if !defined(CG_BEFORE_ITERATION)
#define CG_BEFORE_ITERATION
#endif 

#if !defined(CG_AFTER_ITERATION)
#define CG_AFTER_ITERATION
#endif 

#if !defined(CG_BEFORE_SCHEDULE)
#define CG_BEFORE_SCHEDULE
#endif

#if !defined(CG_AFTER_SCHEDULE)
#define CG_AFTER_SCHEDULE
#endif

#if !defined(CG_BEFORE_BUFFER)
#define CG_BEFORE_BUFFER
#endif

#if !defined(CG_BEFORE_FIFO_BUFFERS)
#define CG_BEFORE_FIFO_BUFFERS
#endif

#if !defined(CG_BEFORE_FIFO_INIT)
#define CG_BEFORE_FIFO_INIT
#endif

#if !defined(CG_BEFORE_NODE_INIT)
#define CG_BEFORE_NODE_INIT
#endif

#if !defined(CG_AFTER_INCLUDES)
#define CG_AFTER_INCLUDES
#endif

#if !defined(CG_BEFORE_SCHEDULER_FUNCTION)
#define CG_BEFORE_SCHEDULER_FUNCTION
#endif

#if !defined(CG_BEFORE_NODE_EXECUTION)
#define CG_BEFORE_NODE_EXECUTION(ID)
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION(ID)
#endif

{% if config.asynchronous or config.fullyAsynchronous -%}

#if !defined(CG_NODE_NOT_EXECUTED)
#define CG_NODE_NOT_EXECUTED(ID)
#endif

#if !defined(CG_ASYNC_BEFORE_NODE_CHECK)
#define CG_ASYNC_BEFORE_NODE_CHECK(ID)
#endif

#if !defined(CG_ASYNC_AFTER_NODE_CHECK)
#define CG_ASYNC_AFTER_NODE_CHECK(ID)
#endif
{% endif %}


{% if config.callback -%}
#if !defined(CG_RESTORE_STATE_MACHINE_STATE)
#define CG_RESTORE_STATE_MACHINE_STATE
#endif

#if !defined(CG_SAVE_STATE_MACHINE_STATE)
#define CG_SAVE_STATE_MACHINE_STATE
#endif
{% endif %}