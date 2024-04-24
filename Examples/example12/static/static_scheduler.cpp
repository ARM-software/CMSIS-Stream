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
#include "static_scheduler.h"

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
static uint8_t schedule[8]=
{ 
1,0,3,5,6,2,4,7,
};

/*

Internal ID identification for the nodes

*/
#define BUF1_INTERNAL_ID 0
#define BUF2_INTERNAL_ID 1
#define BUFCOPY_INTERNAL_ID 2
#define DUP0_INTERNAL_ID 3
#define INPLACE_INTERNAL_ID 4
#define PROCESSING_INTERNAL_ID 5
#define SINKA_INTERNAL_ID 6
#define SINKB_INTERNAL_ID 7



CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 1
#define FIFOSIZE1 1
#define FIFOSIZE2 1
#define FIFOSIZE3 1
#define FIFOSIZE4 1
#define FIFOSIZE5 1
#define FIFOSIZE6 1

#define BUFFERSIZE0 1
CG_BEFORE_BUFFER
Shared<buffer,false> static_buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 1
CG_BEFORE_BUFFER
Shared<buffer,false> static_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 1
CG_BEFORE_BUFFER
Shared<buffer,false> static_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 1
CG_BEFORE_BUFFER
Shared<buffer,false> static_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 1
CG_BEFORE_BUFFER
Shared<buffer,false> static_buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 1
CG_BEFORE_BUFFER
Shared<buffer,true> static_buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 1
CG_BEFORE_BUFFER
Shared<buffer,true> static_buf6[BUFFERSIZE6]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t static_scheduler(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=1;


    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<Shared<buffer,false>,FIFOSIZE0,1,0> fifo0(static_buf0);
    FIFO<Shared<buffer,false>,FIFOSIZE1,1,0> fifo1(static_buf1);
    FIFO<Shared<buffer,false>,FIFOSIZE2,1,0> fifo2(static_buf2);
    FIFO<Shared<buffer,false>,FIFOSIZE3,1,0> fifo3(static_buf3);
    FIFO<Shared<buffer,false>,FIFOSIZE4,1,0> fifo4(static_buf4);
    FIFO<Shared<buffer,true>,FIFOSIZE5,1,0> fifo5(static_buf5);
    FIFO<Shared<buffer,true>,FIFOSIZE6,1,0> fifo6(static_buf6);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    BufferSource<Shared<buffer,false>,1> buf1(fifo4); /* Node ID = 0 */
    BufferSource<Shared<buffer,false>,1> buf2(fifo0); /* Node ID = 1 */
    BufferCopy<Shared<buffer,true>,1,Shared<buffer,false>,1> bufCopy(fifo6,fifo1); /* Node ID = 2 */
    Duplicate<Shared<buffer,false>,1,Shared<buffer,true>,1> dup0(fifo4,{&fifo5,&fifo6}); /* Node ID = 3 */
    InPlace<Shared<buffer,false>,1,Shared<buffer,false>,1> inplace(fifo1,fifo3); /* Node ID = 4 */
    Processing<Shared<buffer,false>,1,Shared<buffer,true>,1,Shared<buffer,false>,1> processing(fifo0,fifo5,fifo2); /* Node ID = 5 */
    BufferSink<Shared<buffer,false>,1> sinkA(fifo2); /* Node ID = 6 */
    BufferSink<Shared<buffer,false>,1> sinkB(fifo3); /* Node ID = 7 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 8; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   cgStaticError = buf1.run();
                }
                break;

                case 1:
                {
                    
                   cgStaticError = buf2.run();
                }
                break;

                case 2:
                {
                    
                   cgStaticError = bufCopy.run();
                }
                break;

                case 3:
                {
                    
                   cgStaticError = dup0.run();
                }
                break;

                case 4:
                {
                    
                   cgStaticError = inplace.run();
                }
                break;

                case 5:
                {
                    
                   cgStaticError = processing.run();
                }
                break;

                case 6:
                {
                    
                   cgStaticError = sinkA.run();
                }
                break;

                case 7:
                {
                    
                   cgStaticError = sinkB.run();
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
