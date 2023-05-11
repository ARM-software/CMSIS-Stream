/*

Generated with CMSIS-DSP Compute Graph Scripts.
The generated code is not covered by CMSIS-DSP license.

The support classes and code is covered by CMSIS-DSP license.

*/


#include <cstdint>
#include "custom.h"
#include "GenericNodes.h"
#include "ComplexAppNodes.h"
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
static unsigned int schedule[1864]=
{ 
16,6,7,8,0,2,3,12,13,5,15,3,12,13,9,1,4,14,10,11,11,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,
4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,
12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,
2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,
3,12,13,9,1,10,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,
12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,
11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,
4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,4,14,2,
5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,
5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,
12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,
12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,
14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,
14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,
5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,
0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,
12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,
5,15,3,12,13,9,1,10,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,
14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,
1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,
5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,4,
14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,
13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,
14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,
14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,4,14,2,5,15,4,14,3,12,13,2,5,
15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,
15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,
13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,
7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,
3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,
2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,
15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,
13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,
13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,
11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,
3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,
15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,
15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,4,14,2,5,15,4,14,3,12,13,
2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,
2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,
3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,
16,6,7,8,0,3,12,13,2,5,15,3,12,13,9,1,10,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,
4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,
12,13,2,5,15,3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,
2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,16,6,7,8,0,3,12,13,2,5,15,
3,12,13,9,1,10,11,11,11,4,14,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,
3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,3,12,13,2,5,15,4,14,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 128
#define FIFOSIZE1 128
#define FIFOSIZE2 128
#define FIFOSIZE3 352
#define FIFOSIZE4 16
#define FIFOSIZE5 16
#define FIFOSIZE6 16
#define FIFOSIZE7 16
#define FIFOSIZE8 16
#define FIFOSIZE9 16
#define FIFOSIZE10 16
#define FIFOSIZE11 128
#define FIFOSIZE12 128
#define FIFOSIZE13 128
#define FIFOSIZE14 128
#define FIFOSIZE15 128
#define FIFOSIZE16 128
#define FIFOSIZE17 128

#define BUFFERSIZE1 128
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 128
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 128
CG_BEFORE_BUFFER
float buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 352
CG_BEFORE_BUFFER
float buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 16
CG_BEFORE_BUFFER
float buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 16
CG_BEFORE_BUFFER
float buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 16
CG_BEFORE_BUFFER
float buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 16
CG_BEFORE_BUFFER
float buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 16
CG_BEFORE_BUFFER
float buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 16
CG_BEFORE_BUFFER
float buf10[BUFFERSIZE10]={0};

#define BUFFERSIZE11 16
CG_BEFORE_BUFFER
float buf11[BUFFERSIZE11]={0};

#define BUFFERSIZE12 128
CG_BEFORE_BUFFER
float buf12[BUFFERSIZE12]={0};

#define BUFFERSIZE13 128
CG_BEFORE_BUFFER
float buf13[BUFFERSIZE13]={0};

#define BUFFERSIZE14 128
CG_BEFORE_BUFFER
float buf14[BUFFERSIZE14]={0};

#define BUFFERSIZE15 128
CG_BEFORE_BUFFER
float buf15[BUFFERSIZE15]={0};

#define BUFFERSIZE16 128
CG_BEFORE_BUFFER
float buf16[BUFFERSIZE16]={0};

#define BUFFERSIZE17 128
CG_BEFORE_BUFFER
float buf17[BUFFERSIZE17]={0};

#define BUFFERSIZE18 128
CG_BEFORE_BUFFER
float buf18[BUFFERSIZE18]={0};


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
    FIFO<float,FIFOSIZE0,1,0> fifo0(buf1);
    FIFO<float,FIFOSIZE1,1,0> fifo1(buf2);
    FIFO<float,FIFOSIZE2,1,0> fifo2(buf3);
    FIFO<float,FIFOSIZE3,0,0> fifo3(buf4);
    FIFO<float,FIFOSIZE4,1,0> fifo4(buf5);
    FIFO<float,FIFOSIZE5,1,0> fifo5(buf6);
    FIFO<float,FIFOSIZE6,1,0> fifo6(buf7);
    FIFO<float,FIFOSIZE7,1,0> fifo7(buf8);
    FIFO<float,FIFOSIZE8,1,0> fifo8(buf9);
    FIFO<float,FIFOSIZE9,1,0> fifo9(buf10);
    FIFO<float,FIFOSIZE10,1,0> fifo10(buf11);
    FIFO<float,FIFOSIZE11,1,0> fifo11(buf12);
    FIFO<float,FIFOSIZE12,1,0> fifo12(buf13);
    FIFO<float,FIFOSIZE13,0,0> fifo13(buf14);
    FIFO<float,FIFOSIZE14,0,0> fifo14(buf15);
    FIFO<float,FIFOSIZE15,1,0> fifo15(buf16);
    FIFO<float,FIFOSIZE16,1,0> fifo16(buf17);
    FIFO<float,FIFOSIZE17,0,0> fifo17(buf18);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    Duplicate3<float,128,float,128,float,128,float,128> dup0(fifo11,fifo12,fifo13,fifo14);
    Duplicate2<float,128,float,128,float,128> dup1(fifo15,fifo16,fifo17);
    ProcessingNode12<float,16,float,16,float,16> proc12(fifo13,fifo4,fifo5);
    ProcessingNode13<float,16,float,16,float,16,float,16> proc13(fifo14,fifo6,fifo7,fifo8);
    ProcessingNode21<float,16,float,16,float,16> proc21A(fifo17,fifo4,fifo9);
    ProcessingNode21<float,16,float,16,float,16> proc21B(fifo5,fifo6,fifo10);
    ProcessingNode<float,128,float,128> procA(fifo0,fifo1);
    ProcessingNode<float,128,float,128> procB(fifo1,fifo2);
    ProcessingNode<float,128,float,128> procC(fifo2,fifo11);
    ProcessingNode<float,128,float,128> procD(fifo12,fifo15);
    ProcessingNode<float,128,float,256> procE(fifo16,fifo3);
    Sink<float,100> sink(fifo3);
    Sink<float,16> sinkB(fifo7);
    Sink<float,16> sinkC(fifo8);
    Sink<float,16> sinkD(fifo9);
    Sink<float,16> sinkE(fifo10);
    Source<float,128> source(fifo0);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 1864; id++)
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
                   cgStaticError = dup1.run();
                }
                break;

                case 2:
                {
                   cgStaticError = proc12.run();
                }
                break;

                case 3:
                {
                   cgStaticError = proc13.run();
                }
                break;

                case 4:
                {
                   cgStaticError = proc21A.run();
                }
                break;

                case 5:
                {
                   cgStaticError = proc21B.run();
                }
                break;

                case 6:
                {
                   cgStaticError = procA.run();
                }
                break;

                case 7:
                {
                   cgStaticError = procB.run();
                }
                break;

                case 8:
                {
                   cgStaticError = procC.run();
                }
                break;

                case 9:
                {
                   cgStaticError = procD.run();
                }
                break;

                case 10:
                {
                   cgStaticError = procE.run();
                }
                break;

                case 11:
                {
                   cgStaticError = sink.run();
                }
                break;

                case 12:
                {
                   cgStaticError = sinkB.run();
                }
                break;

                case 13:
                {
                   cgStaticError = sinkC.run();
                }
                break;

                case 14:
                {
                   cgStaticError = sinkD.run();
                }
                break;

                case 15:
                {
                   cgStaticError = sinkE.run();
                }
                break;

                case 16:
                {
                   cgStaticError = source.run();
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
