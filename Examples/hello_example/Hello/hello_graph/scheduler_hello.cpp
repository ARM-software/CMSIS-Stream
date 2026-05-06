/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "app_config.hpp"
#include "stream_platform_config.hpp"
#include "cg_enums.h"
#include "StreamNode.hpp"
#include "cstream_node.h"
#include "IdentifiedNode.hpp"
#include "EventQueue.hpp"
#include "GenericNodes.hpp"
#include "AppNodes_hello.hpp"
#include "scheduler_hello.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif


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





CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[2]=
{ 
1,0,
};

/*

Internal ID identification for the nodes

*/
#define SINK_INTERNAL_ID 0
#define SRC_INTERNAL_ID 1



/***********

Node identification

************/
static CStreamNode identifiedNodes[STREAM_HELLO_NB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 10

#define BUFFERSIZE0 40
CG_BEFORE_BUFFER
uint8_t stream_hello_buf0[BUFFERSIZE0]={0};


typedef struct {
FIFO<float,FIFOSIZE0,1,0> *fifo0;
} fifos_t;

typedef struct {
    NullSink<float,10> *sink;
    EmptySource<float,10> *src;
} nodes_t;


static fifos_t fifos={0};

static nodes_t nodes={0};

CStreamNode* get_scheduler_hello_node(int32_t nodeID)
{
    if (nodeID >= STREAM_HELLO_NB_IDENTIFIED_NODES)
    {
        return(nullptr);
    }
    if (nodeID < 0)
    {
        return(nullptr);
    }
    return(&identifiedNodes[nodeID]);
}

int init_scheduler_hello(void *evtQueue_,helloParams_t *params)
{
    EventQueue *evtQueue = reinterpret_cast<EventQueue *>(evtQueue_);
    (void)evtQueue;

    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new (std::nothrow) FIFO<float,FIFOSIZE0,1,0>(stream_hello_buf0);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    cg_status initError;

    nodes.sink = new (std::nothrow) NullSink<float,10>(*(fifos.fifo0));
    if (nodes.sink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAM_HELLO_SINK_ID]=createStreamNode(*nodes.sink);
    nodes.sink->setID(STREAM_HELLO_SINK_ID);

    nodes.src = new (std::nothrow) EmptySource<float,10>(*(fifos.fifo0),params->src);
    if (nodes.src==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAM_HELLO_SRC_ID]=createStreamNode(*nodes.src);
    nodes.src->setID(STREAM_HELLO_SRC_ID);


/* Subscribe nodes for the event system*/

    initError = CG_SUCCESS;
    initError = nodes.sink->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.src->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
   


    return(CG_SUCCESS);

}

void free_scheduler_hello()
{
    if (fifos.fifo0!=NULL)
    {
       delete fifos.fifo0;
    }

    if (nodes.sink!=NULL)
    {
        delete nodes.sink;
    }
    if (nodes.src!=NULL)
    {
        delete nodes.src;
    }
}

void reset_fifos_scheduler_hello(int all)
{
    if (fifos.fifo0!=NULL)
    {
       fifos.fifo0->reset();
    }
   // Buffers are set to zero too
   if (all)
   {
       std::fill_n(stream_hello_buf0, BUFFERSIZE0, (uint8_t)0);
   }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler_hello(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;






    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 2; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   cgStaticError = nodes.sink->run();
                }
                break;

                case 1:
                {
                    
                   cgStaticError = nodes.src->run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION(schedule[id]);
                        CHECKERROR;
        }
       CG_AFTER_ITERATION;
       nbSchedule++;
    }
errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
    
}
