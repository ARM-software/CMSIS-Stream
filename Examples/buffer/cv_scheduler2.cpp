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
#include "cv_scheduler2.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif

#if !defined(CG_MALLOC)
#define CG_MALLOC(A) malloc((A))
#endif 

#if !defined(CG_FREE)
#define CG_FREE(A) free((A))
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
static uint8_t schedule[5]=
{ 
4,1,0,2,3,
};

/*

Internal ID identification for the nodes

*/
#define DUP0_INTERNAL_ID 0
#define PROCESSING1_INTERNAL_ID 1
#define SINK1_INTERNAL_ID 2
#define SINK2_INTERNAL_ID 3
#define SOURCE_INTERNAL_ID 4



CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 5
#define FIFOSIZE1 5
#define FIFOSIZE2 5
#define FIFOSIZE3 5

typedef struct {
uint8_t  *buf0;
} buffers_t;

CG_BEFORE_BUFFER
static buffers_t buffers={0};

int init_buffer_scheduler(uint8_t *myBuffer,
                              uint8_t *myBufferB)
{
    buffers.buf0 = (uint8_t *)CG_MALLOC(20 * sizeof(uint8_t));
    if (buffers.buf0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    return(CG_SUCCESS);
}

void free_buffer_scheduler(uint8_t *myBuffer,
                              uint8_t *myBufferB)
{
    if (buffers.buf0!=NULL)
    {
        CG_FREE(buffers.buf0);
    }
}



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error,uint8_t *myBuffer,
                              uint8_t *myBufferB)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=1;


    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float,FIFOSIZE0,1,0> fifo0(nullptr);
    FIFO<float,FIFOSIZE1,1,0> fifo1(buffers.buf0);
    FIFO<float,FIFOSIZE2,1,0> fifo2(buffers.buf0);
    FIFO<float,FIFOSIZE3,1,0> fifo3(buffers.buf0);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    Duplicate<float,5,float,5> dup0(fifo1,{}); /* Node ID = 0 */
    ProcessingNode<float,5,float,5> processing1(fifo0,fifo1); /* Node ID = 1 */
    Sink<float,5> sink1(fifo2,"sink1"); /* Node ID = 2 */
    Sink<float,5> sink2(fifo3,"sink2"); /* Node ID = 3 */
    SourceC2<float,5> source(fifo0); /* Node ID = 4 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 5; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   cgStaticError = dup0.run();
                }
                break;

                case 1:
                {
                    
                   cgStaticError = processing1.run();
                }
                break;

                case 2:
                {
                    
                   cgStaticError = sink1.run();
                }
                break;

                case 3:
                {
                    
                   cgStaticError = sink2.run();
                }
                break;

                case 4:
                {
                    
                   cgStaticError = source.run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION(schedule[id]);
                        CHECKERROR;
        }
       debugCounter--;
       CG_AFTER_ITERATION;
       nbSchedule++;
    }

errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
}
