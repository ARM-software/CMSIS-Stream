/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "custom.h"
#include "cg_status.h"
#include "GenericNodes.h"
#include "AppNodes.h"
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
static uint8_t schedule[19]=
{ 
2,2,0,1,2,0,1,2,2,0,1,1,2,0,1,2,0,1,1,
};

#define PAUSED_SCHEDULER 1 
#define SCHEDULER_NOT_STARTED 2 


void init_cb_state_scheduler(scheduler_cb_t* state)
{
    if (state)
    {
      state->status = SCHEDULER_NOT_STARTED;
      state->nbSched = 0;
      state->scheduleStateID = 0;
    }
}


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
} nodes_t;

CG_BEFORE_BUFFER
static fifos_t fifos={0};

CG_BEFORE_BUFFER
static nodes_t nodes={0};


int init_scheduler(int *someVariable)
{
    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new FIFO<float,FIFOSIZE0,0,0>(buf0);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new FIFO<float,FIFOSIZE1,0,0>(buf1);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    nodes.processing = new ProcessingNode<float,7,float,7>(*(fifos.fifo0),*(fifos.fifo1));
    if (nodes.processing==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.sink = new Sink<float,5>(*(fifos.fifo1));
    if (nodes.sink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source = new Source<float,5>(*(fifos.fifo0));
    if (nodes.source==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

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
uint32_t scheduler(int *error,scheduler_cb_t *scheduleState,int *someVariable)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

if (scheduleState->status==PAUSED_SCHEDULER)
   {
      nbSchedule = scheduleState->nbSched;

   }


    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
CG_RESTORE_STATE_MACHINE_STATE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        if (scheduleState->status==PAUSED_SCHEDULER)
        {
            id = scheduleState->scheduleStateID;
        }
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
            if (cgStaticError == CG_PAUSE_SCHEDULER)
            {
                CG_SAVE_STATE_MACHINE_STATE;
                scheduleState->status = PAUSED_SCHEDULER;
                scheduleState->nbSched = nbSchedule;
                scheduleState->scheduleStateID = id;
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
