/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "custom.h"
#include "GenericNodes.h"
#include "cg_status.h"
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
static uint8_t schedule[184]=
{ 
28,8,29,9,28,29,27,14,15,16,5,11,23,24,10,13,26,7,19,2,20,0,3,21,1,4,8,9,11,23,24,17,6,12,25,18,22,22,22,22,
22,22,22,22,28,29,10,13,26,7,19,2,20,0,3,21,1,4,12,25,8,9,11,23,24,28,29,10,13,26,7,19,2,20,0,3,21,1,4,12,
25,8,9,11,23,24,28,29,10,13,26,7,19,2,20,0,3,21,1,4,12,25,8,9,11,23,24,28,29,10,13,26,7,19,2,20,0,3,21,1,
4,12,25,8,9,11,23,24,28,29,10,13,26,7,19,2,20,0,3,21,1,4,12,25,8,9,11,23,24,28,29,10,13,26,7,19,2,20,0,3,
21,1,4,12,25,8,9,11,23,24,10,13,26,7,19,2,20,0,3,21,1,4,12,25,
};


CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 128
#define FIFOSIZE1 128
#define FIFOSIZE2 128
#define FIFOSIZE3 128
#define FIFOSIZE4 16
#define FIFOSIZE5 16
#define FIFOSIZE6 16
#define FIFOSIZE7 16
#define FIFOSIZE8 16
#define FIFOSIZE9 16
#define FIFOSIZE10 32
#define FIFOSIZE11 32
#define FIFOSIZE12 32
#define FIFOSIZE13 32
#define FIFOSIZE14 32
#define FIFOSIZE15 128
#define FIFOSIZE16 128
#define FIFOSIZE17 128
#define FIFOSIZE18 128
#define FIFOSIZE19 128
#define FIFOSIZE20 128
#define FIFOSIZE21 128
#define FIFOSIZE22 16
#define FIFOSIZE23 16
#define FIFOSIZE24 16
#define FIFOSIZE25 16
#define FIFOSIZE26 16
#define FIFOSIZE27 16
#define FIFOSIZE28 16
#define FIFOSIZE29 16
#define FIFOSIZE30 16
#define FIFOSIZE31 16
#define FIFOSIZE32 16

#define BUFFERSIZE1 128
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 128
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 128
CG_BEFORE_BUFFER
float buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 128
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

#define BUFFERSIZE11 32
CG_BEFORE_BUFFER
float buf11[BUFFERSIZE11]={0};

#define BUFFERSIZE12 32
CG_BEFORE_BUFFER
float buf12[BUFFERSIZE12]={0};

#define BUFFERSIZE13 32
CG_BEFORE_BUFFER
float buf13[BUFFERSIZE13]={0};

#define BUFFERSIZE14 32
CG_BEFORE_BUFFER
float buf14[BUFFERSIZE14]={0};

#define BUFFERSIZE15 32
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

#define BUFFERSIZE19 128
CG_BEFORE_BUFFER
float buf19[BUFFERSIZE19]={0};

#define BUFFERSIZE20 128
CG_BEFORE_BUFFER
float buf20[BUFFERSIZE20]={0};

#define BUFFERSIZE21 128
CG_BEFORE_BUFFER
float buf21[BUFFERSIZE21]={0};

#define BUFFERSIZE22 128
CG_BEFORE_BUFFER
float buf22[BUFFERSIZE22]={0};

#define BUFFERSIZE23 16
CG_BEFORE_BUFFER
float buf23[BUFFERSIZE23]={0};

#define BUFFERSIZE24 16
CG_BEFORE_BUFFER
float buf24[BUFFERSIZE24]={0};

#define BUFFERSIZE25 16
CG_BEFORE_BUFFER
float buf25[BUFFERSIZE25]={0};

#define BUFFERSIZE26 16
CG_BEFORE_BUFFER
float buf26[BUFFERSIZE26]={0};

#define BUFFERSIZE27 16
CG_BEFORE_BUFFER
float buf27[BUFFERSIZE27]={0};

#define BUFFERSIZE28 16
CG_BEFORE_BUFFER
float buf28[BUFFERSIZE28]={0};

#define BUFFERSIZE29 16
CG_BEFORE_BUFFER
float buf29[BUFFERSIZE29]={0};

#define BUFFERSIZE30 16
CG_BEFORE_BUFFER
float buf30[BUFFERSIZE30]={0};

#define BUFFERSIZE31 16
CG_BEFORE_BUFFER
float buf31[BUFFERSIZE31]={0};

#define BUFFERSIZE32 16
CG_BEFORE_BUFFER
float buf32[BUFFERSIZE32]={0};

#define BUFFERSIZE33 16
CG_BEFORE_BUFFER
float buf33[BUFFERSIZE33]={0};



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
    FIFO<float,FIFOSIZE13,1,0> fifo13(buf14);
    FIFO<float,FIFOSIZE14,1,0> fifo14(buf15);
    FIFO<float,FIFOSIZE15,1,0> fifo15(buf16);
    FIFO<float,FIFOSIZE16,1,0> fifo16(buf17);
    FIFO<float,FIFOSIZE17,0,0> fifo17(buf18);
    FIFO<float,FIFOSIZE18,0,0> fifo18(buf19);
    FIFO<float,FIFOSIZE19,1,0> fifo19(buf20);
    FIFO<float,FIFOSIZE20,1,0> fifo20(buf21);
    FIFO<float,FIFOSIZE21,0,0> fifo21(buf22);
    FIFO<float,FIFOSIZE22,1,0> fifo22(buf23);
    FIFO<float,FIFOSIZE23,1,0> fifo23(buf24);
    FIFO<float,FIFOSIZE24,1,0> fifo24(buf25);
    FIFO<float,FIFOSIZE25,1,0> fifo25(buf26);
    FIFO<float,FIFOSIZE26,1,0> fifo26(buf27);
    FIFO<float,FIFOSIZE27,1,0> fifo27(buf28);
    FIFO<float,FIFOSIZE28,1,0> fifo28(buf29);
    FIFO<float,FIFOSIZE29,1,0> fifo29(buf30);
    FIFO<float,FIFOSIZE30,1,0> fifo30(buf31);
    FIFO<float,FIFOSIZE31,1,0> fifo31(buf32);
    FIFO<float,FIFOSIZE32,1,0> fifo32(buf33);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    Sink<float,32> dstaA(fifo10); /* Node ID = 0 */
    Sink<float,32> dstaB(fifo12); /* Node ID = 1 */
    Sink<float,32> dstaC(fifo14); /* Node ID = 2 */
    Sink<float,32> dstbA(fifo11); /* Node ID = 3 */
    Sink<float,32> dstbB(fifo13); /* Node ID = 4 */
    Duplicate<float,128,float,128> dup0(fifo15,{&fifo16,&fifo17,&fifo18}); /* Node ID = 5 */
    Duplicate<float,128,float,128> dup1(fifo19,{&fifo20,&fifo21}); /* Node ID = 6 */
    Duplicate<float,16,float,16> dup2(fifo22,{&fifo23,&fifo24,&fifo25,&fifo26}); /* Node ID = 7 */
    Duplicate<float,16,float,16> dup3(fifo27,{&fifo28,&fifo29}); /* Node ID = 8 */
    Duplicate<float,16,float,16> dup4(fifo30,{&fifo31,&fifo32}); /* Node ID = 9 */
    ProcessingNode12<float,16,float,16,float,16> proc12(fifo17,fifo22,fifo4); /* Node ID = 10 */
    ProcessingNode13<float,16,float,16,float,16,float,16> proc13(fifo18,fifo5,fifo6,fifo7); /* Node ID = 11 */
    ProcessingNode21<float,16,float,16,float,16> proc21A(fifo21,fifo23,fifo8); /* Node ID = 12 */
    ProcessingNode21<float,16,float,16,float,16> proc21B(fifo4,fifo5,fifo9); /* Node ID = 13 */
    ProcessingNode<float,128,float,128> procA(fifo0,fifo1); /* Node ID = 14 */
    ProcessingNode<float,128,float,128> procB(fifo1,fifo2); /* Node ID = 15 */
    ProcessingNode<float,128,float,128> procC(fifo2,fifo15); /* Node ID = 16 */
    ProcessingNode<float,128,float,128> procD(fifo16,fifo19); /* Node ID = 17 */
    ProcessingNode<float,128,float,128> procE(fifo20,fifo3); /* Node ID = 18 */
    ProcessingNodeC<float,16,float,32> proc_f_m({&fifo25,&fifo29,&fifo32},fifo14); /* Node ID = 19 */
    ProcessingNodeA<float,16,float,32> proc_m_to_m({&fifo24,&fifo28,&fifo31},{&fifo10,&fifo11}); /* Node ID = 20 */
    ProcessingNodeB<float,16,float,32> proc_to_m(fifo26,{&fifo12,&fifo13}); /* Node ID = 21 */
    Sink<float,16> sink(fifo3); /* Node ID = 22 */
    Sink<float,16> sinkB(fifo6); /* Node ID = 23 */
    Sink<float,16> sinkC(fifo7); /* Node ID = 24 */
    Sink<float,16> sinkD(fifo8); /* Node ID = 25 */
    Sink<float,16> sinkE(fifo9); /* Node ID = 26 */
    Source<float,128> source(fifo0); /* Node ID = 27 */
    Source<float,16> src0(fifo27); /* Node ID = 28 */
    Source<float,16> src1(fifo30); /* Node ID = 29 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 184; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = dstaA.run();
                }
                break;

                case 1:
                {
                   cgStaticError = dstaB.run();
                }
                break;

                case 2:
                {
                   cgStaticError = dstaC.run();
                }
                break;

                case 3:
                {
                   cgStaticError = dstbA.run();
                }
                break;

                case 4:
                {
                   cgStaticError = dstbB.run();
                }
                break;

                case 5:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 6:
                {
                   cgStaticError = dup1.run();
                }
                break;

                case 7:
                {
                   cgStaticError = dup2.run();
                }
                break;

                case 8:
                {
                   cgStaticError = dup3.run();
                }
                break;

                case 9:
                {
                   cgStaticError = dup4.run();
                }
                break;

                case 10:
                {
                   cgStaticError = proc12.run();
                }
                break;

                case 11:
                {
                   cgStaticError = proc13.run();
                }
                break;

                case 12:
                {
                   cgStaticError = proc21A.run();
                }
                break;

                case 13:
                {
                   cgStaticError = proc21B.run();
                }
                break;

                case 14:
                {
                   cgStaticError = procA.run();
                }
                break;

                case 15:
                {
                   cgStaticError = procB.run();
                }
                break;

                case 16:
                {
                   cgStaticError = procC.run();
                }
                break;

                case 17:
                {
                   cgStaticError = procD.run();
                }
                break;

                case 18:
                {
                   cgStaticError = procE.run();
                }
                break;

                case 19:
                {
                   cgStaticError = proc_f_m.run();
                }
                break;

                case 20:
                {
                   cgStaticError = proc_m_to_m.run();
                }
                break;

                case 21:
                {
                   cgStaticError = proc_to_m.run();
                }
                break;

                case 22:
                {
                   cgStaticError = sink.run();
                }
                break;

                case 23:
                {
                   cgStaticError = sinkB.run();
                }
                break;

                case 24:
                {
                   cgStaticError = sinkC.run();
                }
                break;

                case 25:
                {
                   cgStaticError = sinkD.run();
                }
                break;

                case 26:
                {
                   cgStaticError = sinkE.run();
                }
                break;

                case 27:
                {
                   cgStaticError = source.run();
                }
                break;

                case 28:
                {
                   cgStaticError = src0.run();
                }
                break;

                case 29:
                {
                   cgStaticError = src1.run();
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
