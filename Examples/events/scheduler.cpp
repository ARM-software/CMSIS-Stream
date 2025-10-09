/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "custom.hpp"
#include "cg_enums.h"
#include "StreamNode.hpp"
#include "cstream_node.h"
#include "IdentifiedNode.hpp"
#include "EventQueue.hpp"
#include "GenericNodes.hpp"
#include "AppNodes.hpp"
#include "scheduler.h"

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
static uint8_t schedule[19]=
{ 
2,2,0,1,2,0,1,2,2,0,1,1,2,0,1,2,0,1,1,
};

/*

Internal ID identification for the nodes

*/
#define PROCESSING_INTERNAL_ID 0
#define SINK_INTERNAL_ID 1
#define SOURCE_INTERNAL_ID 2
#define EVTSINK_INTERNAL_ID 3

/* Initialize the selectors global IDs in each class */
template<>
std::array<uint16_t,2> ProcessingNode<float,7,float,7>::selectors = {SEL_RESET_ID, SEL_INCREMENT_ID};
template<>
std::array<uint16_t,1> Sink<float,5>::selectors = {SEL_INCREMENT_ID};
template<>
std::array<uint16_t,1> Source<float,5>::selectors = {SEL_INCREMENT_ID};
std::array<uint16_t,2> EvtSink::selectors = {SEL_INCREMENT_ID, SEL_VALUE_ID};


/***********

Node identification

************/
static CStreamNode identifiedNodes[NB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 11
#define FIFOSIZE1 11

#define BUFFERSIZE0 11
CG_BEFORE_BUFFER
float buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 11
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};


typedef struct {
FIFO<float,FIFOSIZE0,0,0> *fifo0;
FIFO<float,FIFOSIZE1,0,0> *fifo1;
} fifos_t;

typedef struct {
    ProcessingNode<float,7,float,7> *processing;
    Sink<float,5> *sink;
    Source<float,5> *source;
    EvtSink *evtsink;
} nodes_t;


static fifos_t fifos={0};

static nodes_t nodes={0};

CStreamNode* get_scheduler_node(int32_t nodeID)
{
    if (nodeID >= NB_IDENTIFIED_NODES)
    {
        return(nullptr);
    }
    if (nodeID < 0)
    {
        return(nullptr);
    }
    return(&identifiedNodes[nodeID]);
}

int init_scheduler()
{

    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new (std::nothrow) FIFO<float,FIFOSIZE0,0,0>(buf0);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new (std::nothrow) FIFO<float,FIFOSIZE1,0,0>(buf1);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    cg_status initError;

    nodes.processing = new (std::nothrow) ProcessingNode<float,7,float,7>(*(fifos.fifo0),*(fifos.fifo1));
    if (nodes.processing==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[PROCESSING_ID]=createStreamNode(*nodes.processing);
    nodes.processing->setID(PROCESSING_ID);

    nodes.sink = new (std::nothrow) Sink<float,5>(*(fifos.fifo1));
    if (nodes.sink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[SINK_ID]=createStreamNode(*nodes.sink);
    nodes.sink->setID(SINK_ID);

    nodes.source = new (std::nothrow) Source<float,5>(*(fifos.fifo0));
    if (nodes.source==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[SOURCE_ID]=createStreamNode(*nodes.source);
    nodes.source->setID(SOURCE_ID);

    nodes.evtsink = new (std::nothrow) EvtSink;
    if (nodes.evtsink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[EVTSINK_ID]=createStreamNode(*nodes.evtsink);
    nodes.evtsink->setID(EVTSINK_ID);


/* Subscribe nodes for the event system*/
    nodes.source->subscribe(0,*nodes.sink,0);
    nodes.source->subscribe(0,*nodes.evtsink,0);

    initError = CG_SUCCESS;
    initError = nodes.processing->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.sink->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.source->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.evtsink->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
   


    return(CG_SUCCESS);

}

void free_scheduler()
{
    if (fifos.fifo0!=NULL)
    {
       delete fifos.fifo0;
    }
    if (fifos.fifo1!=NULL)
    {
       delete fifos.fifo1;
    }

    if (nodes.processing!=NULL)
    {
        delete nodes.processing;
    }
    if (nodes.sink!=NULL)
    {
        delete nodes.sink;
    }
    if (nodes.source!=NULL)
    {
        delete nodes.source;
    }
    if (nodes.evtsink!=NULL)
    {
        delete nodes.evtsink;
    }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error)
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
        for(; id < 19; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   cgStaticError = nodes.processing->run();
                }
                break;

                case 1:
                {
                    
                   cgStaticError = nodes.sink->run();
                }
                break;

                case 2:
                {
                    
                   cgStaticError = nodes.source->run();
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
