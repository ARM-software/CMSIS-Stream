/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "custom.h"
#include "GenericNodes.h"
#include "cg_status.h"
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



CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[26]=
{ 
2,2,2,0,1,2,2,2,0,1,2,2,2,0,1,1,2,2,0,1,2,2,2,0,1,1,
};


CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 11
#define FIFOSIZE1 11

#define BUFFERSIZE1 11
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 11
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=1;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float,FIFOSIZE0,0,0> fifo0(buf1);
    FIFO<float,FIFOSIZE1,0,0> fifo1(buf2);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    ProcessingNode<float,7,float,7> processing(fifo0,fifo1); /* Node ID = 0 */
    Sink<float,5> sink(fifo1); /* Node ID = 1 */
    Source<float,3> source(fifo0); /* Node ID = 2 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 26; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = processing.run();
                }
                break;

                case 1:
                {
                   cgStaticError = sink.run();
                }
                break;

                case 2:
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
