/*

Generated with CMSIS-DSP Compute Graph Scripts.
The generated code is not covered by CMSIS-DSP license.

The support classes and code is covered by CMSIS-DSP license.

*/


#include <cstdint>
#include "custom.h"
#include "GenericNodes.h"
#include "cg_status.h"
#include "DuplicateAppNodes.h"
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
#define CG_BEFORE_NODE_EXECUTION
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION
#endif

CG_AFTER_INCLUDES


/*

Description of the scheduling. 

*/
static unsigned int schedule[7]=
{ 
6,0,1,2,3,4,5,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 192
#define FIFOSIZE1 192
#define FIFOSIZE2 192
#define FIFOSIZE3 192
#define FIFOSIZE4 192
#define FIFOSIZE5 192

#define BUFFERSIZE1 192
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 192
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 192
CG_BEFORE_BUFFER
float buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 192
CG_BEFORE_BUFFER
float buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 192
CG_BEFORE_BUFFER
float buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 192
CG_BEFORE_BUFFER
float buf6[BUFFERSIZE6]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error,float* inputArray,
                              float* outputArray)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=1;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float,FIFOSIZE0,1,0> fifo0(buf1);
    FIFO<float,FIFOSIZE1,1,0> fifo1(buf2);
    FIFO<float,FIFOSIZE2,1,0> fifo2(buf3);
    FIFO<float,FIFOSIZE3,1,0> fifo3(buf4);
    FIFO<float,FIFOSIZE4,1,0> fifo4(buf5);
    FIFO<float,FIFOSIZE5,1,0> fifo5(buf6);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    Duplicate<float,192> dup0(fifo0,{&fifo1,&fifo2,&fifo3,&fifo4,&fifo5});
    ArraySink<float,192> sink0(fifo1,&outputArray[0]);
    ArraySink<float,192> sink1(fifo2,&outputArray[192]);
    ArraySink<float,192> sink2(fifo3,&outputArray[384]);
    ArraySink<float,192> sink3(fifo4,&outputArray[576]);
    ArraySink<float,192> sink4(fifo5,&outputArray[768]);
    ArraySource<float,192> src(fifo0,inputArray);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 7; id++)
        {
            CG_BEFORE_NODE_EXECUTION;

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 1:
                {
                   cgStaticError = sink0.run();
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
                   cgStaticError = sink3.run();
                }
                break;

                case 5:
                {
                   cgStaticError = sink4.run();
                }
                break;

                case 6:
                {
                   cgStaticError = src.run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION;
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
