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
static uint8_t schedule[44]=
{ 
4,6,1,6,1,2,7,0,3,5,4,6,1,2,7,0,3,5,4,6,1,6,1,2,7,0,3,5,4,6,1,2,7,0,3,5,4,6,1,2,
7,0,3,5,
};


CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 1
#define FIFOSIZE1 11
#define FIFOSIZE2 11
#define FIFOSIZE3 11
#define FIFOSIZE4 11
#define FIFOSIZE5 11
#define FIFOSIZE6 5
#define FIFOSIZE7 5
#define FIFOSIZE8 5
#define FIFOSIZE9 5
#define FIFOSIZE10 5
#define FIFOSIZE11 5
#define FIFOSIZE12 5
#define FIFOSIZE13 1
#define FIFOSIZE14 1
#define FIFOSIZE15 1
#define FIFOSIZE16 1

#define BUFFERSIZE1 1
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 11
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 11
CG_BEFORE_BUFFER
float buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 11
CG_BEFORE_BUFFER
float buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 11
CG_BEFORE_BUFFER
float buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 11
CG_BEFORE_BUFFER
float buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 5
CG_BEFORE_BUFFER
complex buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 5
CG_BEFORE_BUFFER
complex buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 5
CG_BEFORE_BUFFER
complex buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 5
CG_BEFORE_BUFFER
complex buf10[BUFFERSIZE10]={0};

#define BUFFERSIZE11 5
CG_BEFORE_BUFFER
complex buf11[BUFFERSIZE11]={0};

#define BUFFERSIZE12 5
CG_BEFORE_BUFFER
complex buf12[BUFFERSIZE12]={0};

#define BUFFERSIZE13 5
CG_BEFORE_BUFFER
complex buf13[BUFFERSIZE13]={0};

#define BUFFERSIZE14 1
CG_BEFORE_BUFFER
Shared<buffer,false> buf14[BUFFERSIZE14]={0};

#define BUFFERSIZE15 1
CG_BEFORE_BUFFER
complex buf15[BUFFERSIZE15]={0};

#define BUFFERSIZE16 1
CG_BEFORE_BUFFER
complex buf16[BUFFERSIZE16]={0};

#define BUFFERSIZE17 1
CG_BEFORE_BUFFER
complex buf17[BUFFERSIZE17]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error,int someVariable)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=1;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float,FIFOSIZE0,1,0> fifo0(buf1);
    FIFO<float,FIFOSIZE1,0,0> fifo1(buf2);
    FIFO<float,FIFOSIZE2,0,0> fifo2(buf3);
    FIFO<float,FIFOSIZE3,0,0> fifo3(buf4);
    FIFO<float,FIFOSIZE4,0,0> fifo4(buf5);
    FIFO<float,FIFOSIZE5,0,0> fifo5(buf6);
    FIFO<complex,FIFOSIZE6,1,0> fifo6(buf7);
    FIFO<complex,FIFOSIZE7,1,0> fifo7(buf8);
    FIFO<complex,FIFOSIZE8,1,0> fifo8(buf9);
    FIFO<complex,FIFOSIZE9,1,0> fifo9(buf10);
    FIFO<complex,FIFOSIZE10,1,0> fifo10(buf11);
    FIFO<complex,FIFOSIZE11,1,0> fifo11(buf12);
    FIFO<complex,FIFOSIZE12,1,0> fifo12(buf13);
    FIFO<Shared<buffer,false>,FIFOSIZE13,1,0> fifo13(buf14);
    FIFO<complex,FIFOSIZE14,1,0> fifo14(buf15);
    FIFO<complex,FIFOSIZE15,1,0> fifo15(buf16);
    FIFO<complex,FIFOSIZE16,1,0> fifo16(buf17);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    Duplicate<complex,1,complex,1> dup0(fifo14,{&fifo15,&fifo16}); /* Node ID = 0 */
    FromSource<float,1,float,5> fromSource(fifo0,{&fifo1,&fifo2,&fifo3,&fifo4,&fifo5}); /* Node ID = 1 */
    ProcessingNode<float,7,complex,5> processing({&fifo1,&fifo2,&fifo3,&fifo4,&fifo5},{&fifo6,&fifo7,&fifo8,&fifo9,&fifo10,&fifo11,&fifo12}); /* Node ID = 2 */
    SharedSink<complex,1,Shared<buffer,false>,1> sharedSink(fifo16,fifo13); /* Node ID = 3 */
    Source<Shared<buffer,false>,1> sharedSource(fifo13); /* Node ID = 4 */
    Sink<complex,1> sink(fifo15); /* Node ID = 5 */
    Source<float,1> source(fifo0); /* Node ID = 6 */
    ToSink<complex,5,complex,1> toSink({&fifo6,&fifo7,&fifo8,&fifo9,&fifo10,&fifo11,&fifo12},fifo14); /* Node ID = 7 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 44; id++)
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
                   cgStaticError = fromSource.run();
                }
                break;

                case 2:
                {
                   cgStaticError = processing.run();
                }
                break;

                case 3:
                {
                   cgStaticError = sharedSink.run();
                }
                break;

                case 4:
                {
                   cgStaticError = sharedSource.run();
                }
                break;

                case 5:
                {
                   cgStaticError = sink.run();
                }
                break;

                case 6:
                {
                   cgStaticError = source.run();
                }
                break;

                case 7:
                {
                   cgStaticError = toSink.run();
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
