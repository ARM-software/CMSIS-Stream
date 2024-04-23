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





CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[17]=
{ 
4,0,1,2,3,3,4,0,1,2,3,3,0,1,2,3,3,
};



CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 256
#define FIFOSIZE1 256
#define FIFOSIZE2 13
#define FIFOSIZE3 26

#define BUFFERSIZE0 256
CG_BEFORE_BUFFER
float buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 256
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 13
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 26
CG_BEFORE_BUFFER
float buf3[BUFFERSIZE3]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error,arm_mfcc_instance_f32 *mfccConfig)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=1;


    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float,FIFOSIZE0,0,0> fifo0(buf0);
    FIFO<float,FIFOSIZE1,1,0> fifo1(buf1);
    FIFO<float,FIFOSIZE2,1,0> fifo2(buf2);
    FIFO<float,FIFOSIZE3,0,0> fifo3(buf3);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    SlidingBuffer<float,256,128> audioWin(fifo0,fifo1); /* Node ID = 0 */
    MFCC<float,256,float,13> mfcc(fifo1,fifo2,mfccConfig); /* Node ID = 1 */
    SlidingBuffer<float,26,13> mfccWin(fifo2,fifo3); /* Node ID = 2 */
    FileSink<float,13> sink(fifo3,"output_example6.txt"); /* Node ID = 3 */
    FileSource<float,192> src(fifo0,"input_example6.txt"); /* Node ID = 4 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 17; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = audioWin.run();
                }
                break;

                case 1:
                {
                   cgStaticError = mfcc.run();
                }
                break;

                case 2:
                {
                   cgStaticError = mfccWin.run();
                }
                break;

                case 3:
                {
                   cgStaticError = sink.run();
                }
                break;

                case 4:
                {
                   cgStaticError = src.run();
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
