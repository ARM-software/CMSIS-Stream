/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "custom.hpp"
#include "cg_enums.h"
#include "StreamNode.hpp"
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



#if !defined(CG_RESTORE_STATE_MACHINE_STATE)
#define CG_RESTORE_STATE_MACHINE_STATE
#endif

#if !defined(CG_SAVE_STATE_MACHINE_STATE)
#define CG_SAVE_STATE_MACHINE_STATE
#endif


CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[24]=
{ 
3,3,1,0,2,3,1,0,2,3,3,1,0,2,2,3,1,0,2,3,1,0,2,2,
};

/*

Internal ID identification for the nodes

*/
#define PROCESSING_INTERNAL_ID 1
#define SINK_INTERNAL_ID 2
#define SOURCE_INTERNAL_ID 3


/* For callback management */

#define CG_PAUSED_SCHEDULER_ID 1
#define CG_SCHEDULER_NOT_STARTED_ID 2
#define CG_SCHEDULER_RUNNING_ID 3

struct cb_state_t
{
    unsigned long scheduleStateID;
    unsigned long nbSched;
    int status;
    kCBStatus running;
};

static cb_state_t cb_state;


static void init_cb_state()
{
    cb_state.status = CG_SCHEDULER_NOT_STARTED_ID;
    cb_state.nbSched = 0;
    cb_state.scheduleStateID = 0;
    cb_state.running = kNewExecution;
}


CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 11
#define FIFOSIZE1 7
#define FIFOSIZE2 11

#define BUFFERSIZE0 11
CG_BEFORE_BUFFER
float buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 7
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 11
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};


typedef struct {
FIFO<float,FIFOSIZE0,0,0> *fifo0;
FIFO<float,FIFOSIZE1,1,0> *fifo1;
FIFO<float,FIFOSIZE2,0,0> *fifo2;
} fifos_t;

typedef struct {
    ProcessingNode<float,7,float,7> *processing;
    Sink<float,5> *sink;
    Source<float,5> *source;
} nodes_t;


static fifos_t fifos={0};

static nodes_t nodes={0};


int init_scheduler(int *someVariable)
{
init_cb_state();

    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new (std::nothrow) FIFO<float,FIFOSIZE0,0,0>(buf0);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new (std::nothrow) FIFO<float,FIFOSIZE1,1,0>(buf1);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo2 = new (std::nothrow) FIFO<float,FIFOSIZE2,0,0>(buf2);
    if (fifos.fifo2==NULL)
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
    initError = nodes.processing->init();
    if (initError != CG_SUCCESS)
    {
        return(initError);
    }

    nodes.sink = new (std::nothrow) Sink<float,5>(*(fifos.fifo2));
    if (nodes.sink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    initError = nodes.sink->init();
    if (initError != CG_SUCCESS)
    {
        return(initError);
    }

    nodes.source = new (std::nothrow) Source<float,5>(*(fifos.fifo0));
    if (nodes.source==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    initError = nodes.source->init();
    if (initError != CG_SUCCESS)
    {
        return(initError);
    }


/* Subscribe nodes for the event system*/


    return(CG_SUCCESS);

}

void free_scheduler(int *someVariable)
{
    if (fifos.fifo0!=NULL)
    {
       delete fifos.fifo0;
    }
    if (fifos.fifo1!=NULL)
    {
       delete fifos.fifo1;
    }
    if (fifos.fifo2!=NULL)
    {
       delete fifos.fifo2;
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
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error,int *someVariable)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

if (cb_state.status==CG_PAUSED_SCHEDULER_ID)
   {
      nbSchedule = cb_state.nbSched;

   }




    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
CG_RESTORE_STATE_MACHINE_STATE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        if (cb_state.status==CG_PAUSED_SCHEDULER_ID)
        {
            id = cb_state.scheduleStateID;
            cb_state.status = CG_SCHEDULER_RUNNING_ID;
        }
        for(; id < 24; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   
                  {

                   float* i0;
                   float* o1;
                   i0=fifos.fifo1->getReadBuffer(7);
                   o1=fifos.fifo2->getWriteBuffer(7);
                   myfunc(i0,o1,7);
                   cgStaticError = 0;
                  }
                }
                break;

                case 1:
                {
                    nodes.processing->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.processing->run();
                }
                break;

                case 2:
                {
                    nodes.sink->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.sink->run();
                }
                break;

                case 3:
                {
                    nodes.source->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.source->run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION(schedule[id]);
            cb_state.running = kNewExecution;
            if (cgStaticError == CG_PAUSED_SCHEDULER)
            {
                CG_SAVE_STATE_MACHINE_STATE;
                cb_state.status = CG_PAUSED_SCHEDULER_ID;
                cb_state.nbSched = nbSchedule;
                cb_state.scheduleStateID = id;
                cb_state.running = kResumedExecution;
            }
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
