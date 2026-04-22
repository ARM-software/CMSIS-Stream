/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "app_config.hpp"
#include "stream_platform_config.hpp"
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
static uint8_t schedule[12]=
{ 
11,7,8,0,1,4,9,2,3,5,6,10,
};

/*

Internal ID identification for the nodes

*/
#define P0_0_INTERNAL_ID 0
#define P1_0_INTERNAL_ID 1
#define P2_0_INTERNAL_ID 2
#define P3_0_INTERNAL_ID 3
#define P_END_0_INTERNAL_ID 4
#define P_END_1_INTERNAL_ID 5
#define P_END_2_INTERNAL_ID 6
#define P_START_0_INTERNAL_ID 7
#define P_START_1_INTERNAL_ID 8
#define P_START_2_INTERNAL_ID 9
#define SINK_INTERNAL_ID 10
#define SOURCE_INTERNAL_ID 11




CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 100
#define FIFOSIZE1 100
#define FIFOSIZE2 100
#define FIFOSIZE3 100
#define FIFOSIZE4 100
#define FIFOSIZE5 100
#define FIFOSIZE6 100
#define FIFOSIZE7 100
#define FIFOSIZE8 100
#define FIFOSIZE9 100
#define FIFOSIZE10 100
#define FIFOSIZE11 100
#define FIFOSIZE12 100
#define FIFOSIZE13 100

#define BUFFERSIZE0 400
CG_BEFORE_BUFFER
uint8_t buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 400
CG_BEFORE_BUFFER
uint8_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 400
CG_BEFORE_BUFFER
uint8_t buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 400
CG_BEFORE_BUFFER
uint8_t buf3[BUFFERSIZE3]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error,void *evtQueue_)
{
    EventQueue *evtQueue = reinterpret_cast<EventQueue *>(evtQueue_);
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    (void)evtQueue;


    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float,FIFOSIZE0,1,0> fifo0(buf1);
    FIFO<float,FIFOSIZE1,1,0> fifo1(buf2);
    FIFO<float,FIFOSIZE2,1,0> fifo2(buf0);
    FIFO<float,FIFOSIZE3,1,0> fifo3(buf3);
    FIFO<float,FIFOSIZE4,1,0> fifo4(buf1);
    FIFO<float,FIFOSIZE5,1,0> fifo5(buf3);
    FIFO<float,FIFOSIZE6,1,0> fifo6(buf2);
    FIFO<float,FIFOSIZE7,1,0> fifo7(buf2);
    FIFO<float,FIFOSIZE8,1,0> fifo8(buf3);
    FIFO<float,FIFOSIZE9,1,0> fifo9(buf0);
    FIFO<float,FIFOSIZE10,1,0> fifo10(buf3);
    FIFO<float,FIFOSIZE11,1,0> fifo11(buf1);
    FIFO<float,FIFOSIZE12,1,0> fifo12(buf2);
    FIFO<float,FIFOSIZE13,1,0> fifo13(buf0);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */


    ProcessingNode<float,100,float,100> p0_0(fifo3,fifo7); /* Node ID = 0 */
    ProcessingNode<float,100,float,100> p1_0(fifo4,fifo8); /* Node ID = 1 */
    ProcessingNode<float,100,float,100> p2_0(fifo5,fifo9); /* Node ID = 2 */
    ProcessingNode<float,100,float,100> p3_0(fifo6,fifo10); /* Node ID = 3 */
    ProcessingNode21<float,100,float,100,float,100> p_end_0(fifo7,fifo8,fifo11); /* Node ID = 4 */
    ProcessingNode21<float,100,float,100,float,100> p_end_1(fifo9,fifo10,fifo12); /* Node ID = 5 */
    ProcessingNode21<float,100,float,100,float,100> p_end_2(fifo11,fifo12,fifo13); /* Node ID = 6 */
    ProcessingNode12<float,100,float,100,float,100> p_start_0(fifo0,fifo1,fifo2); /* Node ID = 7 */
    ProcessingNode12<float,100,float,100,float,100> p_start_1(fifo1,fifo3,fifo4); /* Node ID = 8 */
    ProcessingNode12<float,100,float,100,float,100> p_start_2(fifo2,fifo5,fifo6); /* Node ID = 9 */
    Sink<float,100> sink(fifo13); /* Node ID = 10 */
    Source<float,100> source(fifo0); /* Node ID = 11 */


/* Subscribe nodes for the event system*/

    cgStaticError = CG_SUCCESS;
    cgStaticError = p0_0.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p1_0.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p2_0.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p3_0.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p_end_0.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p_end_1.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p_end_2.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p_start_0.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p_start_1.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = p_start_2.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = sink.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = source.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }




    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 12; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   cgStaticError = p0_0.run();
                }
                break;

                case 1:
                {
                    
                   cgStaticError = p1_0.run();
                }
                break;

                case 2:
                {
                    
                   cgStaticError = p2_0.run();
                }
                break;

                case 3:
                {
                    
                   cgStaticError = p3_0.run();
                }
                break;

                case 4:
                {
                    
                   cgStaticError = p_end_0.run();
                }
                break;

                case 5:
                {
                    
                   cgStaticError = p_end_1.run();
                }
                break;

                case 6:
                {
                    
                   cgStaticError = p_end_2.run();
                }
                break;

                case 7:
                {
                    
                   cgStaticError = p_start_0.run();
                }
                break;

                case 8:
                {
                    
                   cgStaticError = p_start_1.run();
                }
                break;

                case 9:
                {
                    
                   cgStaticError = p_start_2.run();
                }
                break;

                case 10:
                {
                    
                   cgStaticError = sink.run();
                }
                break;

                case 11:
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
       CG_AFTER_ITERATION;
       nbSchedule++;
    }
errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
    
}
