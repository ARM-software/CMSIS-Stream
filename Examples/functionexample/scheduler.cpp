/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include "arm_math.h"
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
static uint8_t schedule[4]=
{ 
2,3,0,1,
};


CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 7
#define FIFOSIZE1 7
#define FIFOSIZE2 5

#define BUFFERSIZE0 7
CG_BEFORE_BUFFER
float32_t buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 7
CG_BEFORE_BUFFER
float32_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 5
CG_BEFORE_BUFFER
q15_t buf2[BUFFERSIZE2]={0};



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
    FIFO<float32_t,FIFOSIZE0,1,0> fifo0(buf0);
    FIFO<float32_t,FIFOSIZE1,1,0> fifo1(buf1);
    FIFO<q15_t,FIFOSIZE2,1,0> fifo2(buf2);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    Sink<q15_t,5> sink(fifo2); /* Node ID = 1 */
    Source<float32_t,7> sourceA(fifo0); /* Node ID = 2 */
    Source<float32_t,7> sourceB(fifo1); /* Node ID = 3 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 4; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);

            switch(schedule[id])
            {
                case 0:
                {
                   
                  {

                   float* i0;
                   float* i1;
                   q15_t* o2;
                   i0=fifo0.getReadBuffer(7);
                   i1=fifo1.getReadBuffer(7);
                   o2=fifo2.getWriteBuffer(5);
                   myfunc(i0,7,testVar,i1,sizeof(float)*7,o2,5,4,"5");
                   cgStaticError = 0;
                  }
                }
                break;

                case 1:
                {
                   cgStaticError = sink.run();
                }
                break;

                case 2:
                {
                   cgStaticError = sourceA.run();
                }
                break;

                case 3:
                {
                   cgStaticError = sourceB.run();
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
