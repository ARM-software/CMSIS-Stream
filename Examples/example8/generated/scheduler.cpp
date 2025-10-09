/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
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
static uint8_t schedule[7]=
{ 
6,1,5,0,2,3,4,
};

/*

Internal ID identification for the nodes

*/
#define DUP0_INTERNAL_ID 0
#define FILTER_INTERNAL_ID 1
#define SA_INTERNAL_ID 2
#define SB_INTERNAL_ID 3
#define SC_INTERNAL_ID 4
#define SD_INTERNAL_ID 5
#define SOURCE_INTERNAL_ID 6




CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 5
#define FIFOSIZE1 5
#define FIFOSIZE2 5
#define FIFOSIZE3 5
#define FIFOSIZE4 5
#define FIFOSIZE5 5

#define BUFFERSIZE0 40
CG_BEFORE_BUFFER
uint8_t buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 40
CG_BEFORE_BUFFER
uint8_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 40
CG_BEFORE_BUFFER
uint8_t buf2[BUFFERSIZE2]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error,int someVariable)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=2;


    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<complex,FIFOSIZE0,1,0> fifo0(buf0);
    FIFO<complex,FIFOSIZE1,1,0> fifo1(buf1);
    FIFO<complex,FIFOSIZE2,1,0> fifo2(buf2);
    FIFO<complex,FIFOSIZE3,1,0> fifo3(buf0);
    FIFO<complex,FIFOSIZE4,1,0> fifo4(buf0);
    FIFO<complex,FIFOSIZE5,1,0> fifo5(buf0);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */


    Duplicate<complex,5,complex,5> dup0(fifo2,{&fifo3}); /* Node ID = 0 */
    ProcessingNode<complex,5,complex,5,complex,5> filter(fifo0,fifo2,fifo1,4,"Test",someVariable); /* Node ID = 1 */
    Sink<complex,5> sa(fifo3); /* Node ID = 2 */
    Sink<complex,5> sb(fifo4); /* Node ID = 3 */
    Sink<complex,5> sc(fifo5); /* Node ID = 4 */
    Sink<complex,5> sd(fifo1); /* Node ID = 5 */
    Source<complex,5> source(fifo0); /* Node ID = 6 */


/* Subscribe nodes for the event system*/

    cgStaticError = CG_SUCCESS;
    cgStaticError = dup0.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = filter.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = sa.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = sb.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = sc.init();
    if (cgStaticError != CG_SUCCESS)
    {
        *error=cgStaticError;
        return(0);
    }
    cgStaticError = sd.init();
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
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 7; id++)
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
                    
                   cgStaticError = filter.run();
                }
                break;

                case 2:
                {
                    
                   cgStaticError = sa.run();
                }
                break;

                case 3:
                {
                    
                   cgStaticError = sb.run();
                }
                break;

                case 4:
                {
                    
                   cgStaticError = sc.run();
                }
                break;

                case 5:
                {
                    
                   cgStaticError = sd.run();
                }
                break;

                case 6:
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
       debugCounter--;
       CG_AFTER_ITERATION;
       nbSchedule++;
    }
errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
    
}
