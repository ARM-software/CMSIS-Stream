/*

Generated with CMSIS-DSP Compute Graph Scripts.
The generated code is not covered by CMSIS-DSP license.

The support classes and code is covered by CMSIS-DSP license.

*/


#include <cstdint>
#include "custom.h"
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
#define CG_BEFORE_NODE_EXECUTION
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION
#endif

CG_AFTER_INCLUDES


/*

Description of the scheduling. 

*/
static unsigned int schedule[13]=
{ 
11,12,6,1,8,0,7,2,3,4,5,9,10,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 2
#define FIFOSIZE1 2
#define FIFOSIZE2 2
#define FIFOSIZE3 2
#define FIFOSIZE4 3
#define FIFOSIZE5 2
#define FIFOSIZE6 2
#define FIFOSIZE7 2
#define FIFOSIZE8 2
#define FIFOSIZE9 2
#define FIFOSIZE10 2
#define FIFOSIZE11 2

#define BUFFERSIZE1 2
CG_BEFORE_BUFFER
int16_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 2
CG_BEFORE_BUFFER
int16_t buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 2
CG_BEFORE_BUFFER
int16_t buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 2
CG_BEFORE_BUFFER
int16_t buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 3
CG_BEFORE_BUFFER
int16_t buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 2
CG_BEFORE_BUFFER
int16_t buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 2
CG_BEFORE_BUFFER
int16_t buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 2
CG_BEFORE_BUFFER
int16_t buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 2
CG_BEFORE_BUFFER
int16_t buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 2
CG_BEFORE_BUFFER
int16_t buf10[BUFFERSIZE10]={0};

#define BUFFERSIZE11 2
CG_BEFORE_BUFFER
int16_t buf11[BUFFERSIZE11]={0};

#define BUFFERSIZE12 2
CG_BEFORE_BUFFER
int16_t buf12[BUFFERSIZE12]={0};


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=10;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<int16_t,FIFOSIZE0,0,1> fifo0(buf1);
    FIFO<int16_t,FIFOSIZE1,0,1> fifo1(buf2);
    FIFO<int16_t,FIFOSIZE2,0,1> fifo2(buf3);
    FIFO<int16_t,FIFOSIZE3,0,1> fifo3(buf4);
    FIFO<int16_t,FIFOSIZE4,0,1> fifo4(buf5);
    FIFO<int16_t,FIFOSIZE5,0,1> fifo5(buf6);
    FIFO<int16_t,FIFOSIZE6,0,1> fifo6(buf7);
    FIFO<int16_t,FIFOSIZE7,0,1> fifo7(buf8);
    FIFO<int16_t,FIFOSIZE8,0,1> fifo8(buf9);
    FIFO<int16_t,FIFOSIZE9,0,1> fifo9(buf10);
    FIFO<int16_t,FIFOSIZE10,0,1> fifo10(buf11);
    FIFO<int16_t,FIFOSIZE11,0,1> fifo11(buf12);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    NullSink<int16_t,1> debug(fifo4);
    NullSink<int16_t,1> debug0(fifo8);
    NullSink<int16_t,1> debug1(fifo9);
    NullSink<int16_t,1> debug2(fifo10);
    NullSink<int16_t,1> debug3(fifo11);
    Duplicate<int16_t,1> dup0(fifo2,{&fifo3,&fifo4});
    Duplicate<int16_t,1> dup1(fifo5,{&fifo6,&fifo7,&fifo8,&fifo9,&fifo10,&fifo11});
    ProcessingOddEven<int16_t,1,int16_t,1,int16_t,1> proc(fifo3,fifo0,fifo1);
    SinkAsync<int16_t,1> sinka(fifo6);
    SinkAsync<int16_t,1> sinkb(fifo7);
    SourceEven<int16_t,1> sourceEven(fifo0);
    SourceOdd<int16_t,1> sourceOdd(fifo2);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 13; id++)
        {
            CG_BEFORE_NODE_EXECUTION;

            cgStaticError = 0;
            switch(schedule[id])
            {
                case 0:
                {
                                        
                    bool canRun=true;
                    canRun &= !fifo1.willUnderflowWith(1);
                    canRun &= !fifo5.willOverflowWith(1);

                    if (!canRun)
                    {
                      cgStaticError = CG_SKIP_EXECUTION_ID_CODE;
                    }
                    else
                    {
                        cgStaticError = 0;
                    }
                }
                break;

                case 1:
                {
                    cgStaticError = debug.prepareForRunning();
                }
                break;

                case 2:
                {
                    cgStaticError = debug0.prepareForRunning();
                }
                break;

                case 3:
                {
                    cgStaticError = debug1.prepareForRunning();
                }
                break;

                case 4:
                {
                    cgStaticError = debug2.prepareForRunning();
                }
                break;

                case 5:
                {
                    cgStaticError = debug3.prepareForRunning();
                }
                break;

                case 6:
                {
                    cgStaticError = dup0.prepareForRunning();
                }
                break;

                case 7:
                {
                    cgStaticError = dup1.prepareForRunning();
                }
                break;

                case 8:
                {
                    cgStaticError = proc.prepareForRunning();
                }
                break;

                case 9:
                {
                    cgStaticError = sinka.prepareForRunning();
                }
                break;

                case 10:
                {
                    cgStaticError = sinkb.prepareForRunning();
                }
                break;

                case 11:
                {
                    cgStaticError = sourceEven.prepareForRunning();
                }
                break;

                case 12:
                {
                    cgStaticError = sourceOdd.prepareForRunning();
                }
                break;

                default:
                break;
            }

            if (cgStaticError == CG_SKIP_EXECUTION_ID_CODE)
            { 
              cgStaticError = 0;
              continue;
            }

            CHECKERROR;

            switch(schedule[id])
            {
                case 0:
                {
                   
                  {

                   int16_t* i0;
                   int16_t* o1;
                   i0=fifo1.getReadBuffer(1);
                   o1=fifo5.getWriteBuffer(1);
                   compute(i0,o1,1);
                   cgStaticError = 0;
                  }
                }
                break;

                case 1:
                {
                   cgStaticError = debug.run();
                }
                break;

                case 2:
                {
                   cgStaticError = debug0.run();
                }
                break;

                case 3:
                {
                   cgStaticError = debug1.run();
                }
                break;

                case 4:
                {
                   cgStaticError = debug2.run();
                }
                break;

                case 5:
                {
                   cgStaticError = debug3.run();
                }
                break;

                case 6:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 7:
                {
                   cgStaticError = dup1.run();
                }
                break;

                case 8:
                {
                   cgStaticError = proc.run();
                }
                break;

                case 9:
                {
                   cgStaticError = sinka.run();
                }
                break;

                case 10:
                {
                   cgStaticError = sinkb.run();
                }
                break;

                case 11:
                {
                   cgStaticError = sourceEven.run();
                }
                break;

                case 12:
                {
                   cgStaticError = sourceOdd.run();
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
