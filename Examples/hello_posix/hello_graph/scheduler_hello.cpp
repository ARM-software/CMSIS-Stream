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
static uint8_t schedule[3]=
{ 
2,0,1,
};

/*

Internal ID identification for the nodes

*/
#define PROCESS_INTERNAL_ID 0
#define SINK_INTERNAL_ID 1
#define SRC_INTERNAL_ID 2
#define EVTSINK_INTERNAL_ID 3

/* Initialize the selectors global IDs in each class */
template<>
std::array<uint16_t,1> DebugSink<float,1>::selectors = {SEL_MESSAGE_ID};
std::array<uint16_t,1> DebugEvtSink::selectors = {SEL_MESSAGE_ID};


/***********

Node identification

************/
static CStreamNode identifiedNodes[STREAM_HELLO_NB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 10
#define FIFOSIZE1 1

#define BUFFERSIZE0 40
CG_BEFORE_BUFFER
uint8_t stream_hello_buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 4
CG_BEFORE_BUFFER
uint8_t stream_hello_buf1[BUFFERSIZE1]={0};


typedef struct {
FIFO<float,FIFOSIZE0,1,0> *fifo0;
FIFO<float,FIFOSIZE1,1,0> *fifo1;
} fifos_t;

typedef struct {
    DebugProcess<float,10,float,1> *process;
    DebugSink<float,1> *sink;
    DebugSource<float,10> *src;
    DebugEvtSink *evtSink;
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
    fifos.fifo1 = new (std::nothrow) FIFO<float,FIFOSIZE1,1,0>(stream_hello_buf1);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    cg_status initError;

    nodes.process = new (std::nothrow) DebugProcess<float,10,float,1>(*(fifos.fifo0),*(fifos.fifo1));
    if (nodes.process==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAM_HELLO_PROCESS_ID]=createStreamNode(*nodes.process);
    nodes.process->setID(STREAM_HELLO_PROCESS_ID);

    nodes.sink = new (std::nothrow) DebugSink<float,1>(*(fifos.fifo1),evtQueue);
    if (nodes.sink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAM_HELLO_SINK_ID]=createStreamNode(*nodes.sink);
    nodes.sink->setID(STREAM_HELLO_SINK_ID);

    nodes.src = new (std::nothrow) DebugSource<float,10>(*(fifos.fifo0),params->src);
    if (nodes.src==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAM_HELLO_SRC_ID]=createStreamNode(*nodes.src);
    nodes.src->setID(STREAM_HELLO_SRC_ID);

    nodes.evtSink = new (std::nothrow) DebugEvtSink;
    if (nodes.evtSink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAM_HELLO_EVTSINK_ID]=createStreamNode(*nodes.evtSink);
    nodes.evtSink->setID(STREAM_HELLO_EVTSINK_ID);


/* Subscribe nodes for the event system*/
    nodes.sink->subscribe(0,*nodes.evtSink,0);

    initError = CG_SUCCESS;
    initError = nodes.process->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.sink->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.src->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.evtSink->init();
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
    if (fifos.fifo1!=NULL)
    {
       delete fifos.fifo1;
    }

    if (nodes.process!=NULL)
    {
        delete nodes.process;
    }
    if (nodes.sink!=NULL)
    {
        delete nodes.sink;
    }
    if (nodes.src!=NULL)
    {
        delete nodes.src;
    }
    if (nodes.evtSink!=NULL)
    {
        delete nodes.evtSink;
    }
}

void reset_fifos_scheduler_hello(int all)
{
    if (fifos.fifo0!=NULL)
    {
       fifos.fifo0->reset();
    }
    if (fifos.fifo1!=NULL)
    {
       fifos.fifo1->reset();
    }
   // Buffers are set to zero too
   if (all)
   {
       std::fill_n(stream_hello_buf0, BUFFERSIZE0, (uint8_t)0);
       std::fill_n(stream_hello_buf1, BUFFERSIZE1, (uint8_t)0);
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
        for(; id < 3; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   cgStaticError = nodes.process->run();
                }
                break;

                case 1:
                {
                    
                   cgStaticError = nodes.sink->run();
                }
                break;

                case 2:
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
