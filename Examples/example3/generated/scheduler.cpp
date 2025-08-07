/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include "arm_math.h"
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





CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[25]=
{ 
6,2,0,7,3,4,8,1,6,2,0,7,3,4,8,1,5,2,0,7,3,4,8,1,5,
};

/*

Internal ID identification for the nodes

*/
#define AUDIOOVERLAP_INTERNAL_ID 1
#define AUDIOWIN_INTERNAL_ID 2
#define CFFT_INTERNAL_ID 3
#define ICFFT_INTERNAL_ID 4
#define SINK_INTERNAL_ID 5
#define SRC_INTERNAL_ID 6
#define TOCMPLX_INTERNAL_ID 7
#define TOREAL_INTERNAL_ID 8




CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 256
#define FIFOSIZE1 256
#define FIFOSIZE2 256
#define FIFOSIZE3 512
#define FIFOSIZE4 512
#define FIFOSIZE5 512
#define FIFOSIZE6 256
#define FIFOSIZE7 256

#define BUFFERSIZE0 256
CG_BEFORE_BUFFER
float buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 256
CG_BEFORE_BUFFER
float32_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 256
CG_BEFORE_BUFFER
float32_t buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 512
CG_BEFORE_BUFFER
float32_t buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 512
CG_BEFORE_BUFFER
float32_t buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 512
CG_BEFORE_BUFFER
float32_t buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 256
CG_BEFORE_BUFFER
float32_t buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 256
CG_BEFORE_BUFFER
float32_t buf7[BUFFERSIZE7]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=40;


    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float,FIFOSIZE0,0,0> fifo0(buf0);
    FIFO<float32_t,FIFOSIZE1,1,0> fifo1(buf1);
    FIFO<float32_t,FIFOSIZE2,1,0> fifo2(buf2);
    FIFO<float32_t,FIFOSIZE3,1,0> fifo3(buf3);
    FIFO<float32_t,FIFOSIZE4,1,0> fifo4(buf4);
    FIFO<float32_t,FIFOSIZE5,1,0> fifo5(buf5);
    FIFO<float32_t,FIFOSIZE6,1,0> fifo6(buf6);
    FIFO<float32_t,FIFOSIZE7,0,0> fifo7(buf7);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
   cgStaticError = CG_SUCCESS;
    OverlapAdd<float32_t,256,128> audioOverlap(fifo6,fifo7); /* Node ID = 1 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = audioOverlap.init();
    }
    SlidingBuffer<float32_t,256,128> audioWin(fifo0,fifo1); /* Node ID = 2 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = audioWin.init();
    }
    CFFT<float32_t,512,float32_t,512> cfft(fifo3,fifo4); /* Node ID = 3 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = cfft.init();
    }
    ICFFT<float32_t,512,float32_t,512> icfft(fifo4,fifo5); /* Node ID = 4 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = icfft.init();
    }
    FileSink<float,192> sink(fifo7,"output_example3.txt"); /* Node ID = 5 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = sink.init();
    }
    FileSource<float,192> src(fifo0,"input_example3.txt"); /* Node ID = 6 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = src.init();
    }
    ToComplex<float32_t,256,float32_t,512> toCmplx(fifo2,fifo3); /* Node ID = 7 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = toCmplx.init();
    }
    ToReal<float32_t,512,float32_t,256> toReal(fifo5,fifo6); /* Node ID = 8 */
    if (cgStaticError == CG_SUCCESS)
    {
        cgStaticError = toReal.init();
    }

   if (cgStaticError != CG_SUCCESS)
   {
       goto errorHandling;
   }

/* Subscribe nodes for the event system*/





    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 25; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   
                  {

                   float32_t* i0;
                   float32_t* o2;
                   i0=fifo1.getReadBuffer(256);
                   o2=fifo2.getWriteBuffer(256);
                   arm_mult_f32(i0,HANN,o2,256);
                   cgStaticError = 0;
                  }
                }
                break;

                case 1:
                {
                    
                   cgStaticError = audioOverlap.run();
                }
                break;

                case 2:
                {
                    
                   cgStaticError = audioWin.run();
                }
                break;

                case 3:
                {
                    
                   cgStaticError = cfft.run();
                }
                break;

                case 4:
                {
                    
                   cgStaticError = icfft.run();
                }
                break;

                case 5:
                {
                    
                   cgStaticError = sink.run();
                }
                break;

                case 6:
                {
                    
                   cgStaticError = src.run();
                }
                break;

                case 7:
                {
                    
                   cgStaticError = toCmplx.run();
                }
                break;

                case 8:
                {
                    
                   cgStaticError = toReal.run();
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
