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
static uint8_t schedule[81]=
{ 
14,14,11,0,12,1,3,4,5,6,7,8,9,10,2,13,14,11,0,12,1,3,4,5,6,7,8,9,10,2,13,14,14,11,0,12,12,1,3,4,
5,6,7,8,9,10,2,13,13,14,11,0,12,1,3,4,5,6,7,8,9,10,2,13,14,11,0,12,12,1,3,4,5,6,7,8,9,10,2,13,
13,
};

/***********

Node identification

************/
static void * identifiedNodes[NB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 11
#define FIFOSIZE1 7
#define FIFOSIZE2 7
#define FIFOSIZE3 7
#define FIFOSIZE4 7
#define FIFOSIZE5 7
#define FIFOSIZE6 7
#define FIFOSIZE7 7
#define FIFOSIZE8 7
#define FIFOSIZE9 7
#define FIFOSIZE10 11
#define FIFOSIZE11 7
#define FIFOSIZE12 11
#define FIFOSIZE13 7

#define BUFFERSIZE1 11
CG_BEFORE_BUFFER
float buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 7
CG_BEFORE_BUFFER
float buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 7
CG_BEFORE_BUFFER
float buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 7
CG_BEFORE_BUFFER
float buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 7
CG_BEFORE_BUFFER
float buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 7
CG_BEFORE_BUFFER
float buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 7
CG_BEFORE_BUFFER
float buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 7
CG_BEFORE_BUFFER
float buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 7
CG_BEFORE_BUFFER
float buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 7
CG_BEFORE_BUFFER
float buf10[BUFFERSIZE10]={0};

#define BUFFERSIZE11 11
CG_BEFORE_BUFFER
float buf11[BUFFERSIZE11]={0};

#define BUFFERSIZE12 7
CG_BEFORE_BUFFER
float buf12[BUFFERSIZE12]={0};

#define BUFFERSIZE13 11
CG_BEFORE_BUFFER
float buf13[BUFFERSIZE13]={0};

#define BUFFERSIZE14 7
CG_BEFORE_BUFFER
float buf14[BUFFERSIZE14]={0};


typedef struct {
FIFO<float,FIFOSIZE0,0,0> *fifo0;
FIFO<float,FIFOSIZE1,1,0> *fifo1;
FIFO<float,FIFOSIZE2,1,0> *fifo2;
FIFO<float,FIFOSIZE3,1,0> *fifo3;
FIFO<float,FIFOSIZE4,1,0> *fifo4;
FIFO<float,FIFOSIZE5,1,0> *fifo5;
FIFO<float,FIFOSIZE6,1,0> *fifo6;
FIFO<float,FIFOSIZE7,1,0> *fifo7;
FIFO<float,FIFOSIZE8,1,0> *fifo8;
FIFO<float,FIFOSIZE9,1,0> *fifo9;
FIFO<float,FIFOSIZE10,0,0> *fifo10;
FIFO<float,FIFOSIZE11,1,0> *fifo11;
FIFO<float,FIFOSIZE12,0,0> *fifo12;
FIFO<float,FIFOSIZE13,1,0> *fifo13;
} fifos_t;

typedef struct {
    Duplicate<float,7,float,7> *dup0;
    ProcessingNode<float,7,float,7> *processing1;
    ProcessingNode<float,7,float,7> *processing10;
    ProcessingNode<float,7,float,7> *processing2;
    ProcessingNode<float,7,float,7> *processing3;
    ProcessingNode<float,7,float,7> *processing4;
    ProcessingNode<float,7,float,7> *processing5;
    ProcessingNode<float,7,float,7> *processing6;
    ProcessingNode<float,7,float,7> *processing7;
    ProcessingNode<float,7,float,7> *processing8;
    ProcessingNode<float,7,float,7> *processing9;
    ProcessingNode<float,7,float,7> *processinga;
    Sink<float,5> *sinka;
    Sink<float,5> *sinkb;
    Source<float,5> *source;
} nodes_t;

CG_BEFORE_BUFFER
static fifos_t fifos={0};

CG_BEFORE_BUFFER
static nodes_t nodes={0};

void *get_scheduler_node(int32_t nodeID)
{
    if (nodeID >= NB_IDENTIFIED_NODES)
    {
        return(NULL);
    }
    if (nodeID < 0)
    {
        return(NULL);
    }
    return(identifiedNodes[nodeID]);
}

int init_scheduler()
{
    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new FIFO<float,FIFOSIZE0,0,0>(buf1);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new FIFO<float,FIFOSIZE1,1,0>(buf2);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo2 = new FIFO<float,FIFOSIZE2,1,0>(buf3);
    if (fifos.fifo2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo3 = new FIFO<float,FIFOSIZE3,1,0>(buf4);
    if (fifos.fifo3==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo4 = new FIFO<float,FIFOSIZE4,1,0>(buf5);
    if (fifos.fifo4==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo5 = new FIFO<float,FIFOSIZE5,1,0>(buf6);
    if (fifos.fifo5==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo6 = new FIFO<float,FIFOSIZE6,1,0>(buf7);
    if (fifos.fifo6==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo7 = new FIFO<float,FIFOSIZE7,1,0>(buf8);
    if (fifos.fifo7==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo8 = new FIFO<float,FIFOSIZE8,1,0>(buf9);
    if (fifos.fifo8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo9 = new FIFO<float,FIFOSIZE9,1,0>(buf10);
    if (fifos.fifo9==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo10 = new FIFO<float,FIFOSIZE10,0,0>(buf11);
    if (fifos.fifo10==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo11 = new FIFO<float,FIFOSIZE11,1,0>(buf12);
    if (fifos.fifo11==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo12 = new FIFO<float,FIFOSIZE12,0,0>(buf13);
    if (fifos.fifo12==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo13 = new FIFO<float,FIFOSIZE13,1,0>(buf14);
    if (fifos.fifo13==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    nodes.dup0 = new Duplicate<float,7,float,7>(fifo11,{&fifo12,&fifo13});
    if (nodes.dup0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing1 = new ProcessingNode<float,7,float,7>(fifo13,fifo1);
    if (nodes.processing1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing10 = new ProcessingNode<float,7,float,7>(fifo9,fifo10);
    if (nodes.processing10==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[PROCESSING10_ID]=(void*)nodes.processing10;
    nodes.processing10->setID(PROCESSING10_ID);
    nodes.processing2 = new ProcessingNode<float,7,float,7>(fifo1,fifo2);
    if (nodes.processing2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing3 = new ProcessingNode<float,7,float,7>(fifo2,fifo3);
    if (nodes.processing3==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing4 = new ProcessingNode<float,7,float,7>(fifo3,fifo4);
    if (nodes.processing4==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing5 = new ProcessingNode<float,7,float,7>(fifo4,fifo5);
    if (nodes.processing5==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing6 = new ProcessingNode<float,7,float,7>(fifo5,fifo6);
    if (nodes.processing6==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing7 = new ProcessingNode<float,7,float,7>(fifo6,fifo7);
    if (nodes.processing7==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing8 = new ProcessingNode<float,7,float,7>(fifo7,fifo8);
    if (nodes.processing8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing9 = new ProcessingNode<float,7,float,7>(fifo8,fifo9);
    if (nodes.processing9==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processinga = new ProcessingNode<float,7,float,7>(fifo0,fifo11);
    if (nodes.processinga==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[PROCESSINGA_ID]=(void*)nodes.processinga;
    nodes.processinga->setID(PROCESSINGA_ID);
    nodes.sinka = new Sink<float,5>(fifo12);
    if (nodes.sinka==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.sinkb = new Sink<float,5>(fifo10);
    if (nodes.sinkb==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source = new Source<float,5>(fifo0);
    if (nodes.source==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    return(CG_SUCCESS);

}

void free_scheduler()
{
    if (fifos.fifo0!=NULL)
    {
       delete fifos.fifo0;
    }
    if (fifos.fifo1!=NULL)
    {
       delete fifos.fifo1;
    }
    if (fifos.fifo2!=NULL)
    {
       delete fifos.fifo2;
    }
    if (fifos.fifo3!=NULL)
    {
       delete fifos.fifo3;
    }
    if (fifos.fifo4!=NULL)
    {
       delete fifos.fifo4;
    }
    if (fifos.fifo5!=NULL)
    {
       delete fifos.fifo5;
    }
    if (fifos.fifo6!=NULL)
    {
       delete fifos.fifo6;
    }
    if (fifos.fifo7!=NULL)
    {
       delete fifos.fifo7;
    }
    if (fifos.fifo8!=NULL)
    {
       delete fifos.fifo8;
    }
    if (fifos.fifo9!=NULL)
    {
       delete fifos.fifo9;
    }
    if (fifos.fifo10!=NULL)
    {
       delete fifos.fifo10;
    }
    if (fifos.fifo11!=NULL)
    {
       delete fifos.fifo11;
    }
    if (fifos.fifo12!=NULL)
    {
       delete fifos.fifo12;
    }
    if (fifos.fifo13!=NULL)
    {
       delete fifos.fifo13;
    }

    if (nodes.dup0!=NULL)
    {
        delete nodes.dup0;
    }
    if (nodes.processing1!=NULL)
    {
        delete nodes.processing1;
    }
    if (nodes.processing10!=NULL)
    {
        delete nodes.processing10;
    }
    if (nodes.processing2!=NULL)
    {
        delete nodes.processing2;
    }
    if (nodes.processing3!=NULL)
    {
        delete nodes.processing3;
    }
    if (nodes.processing4!=NULL)
    {
        delete nodes.processing4;
    }
    if (nodes.processing5!=NULL)
    {
        delete nodes.processing5;
    }
    if (nodes.processing6!=NULL)
    {
        delete nodes.processing6;
    }
    if (nodes.processing7!=NULL)
    {
        delete nodes.processing7;
    }
    if (nodes.processing8!=NULL)
    {
        delete nodes.processing8;
    }
    if (nodes.processing9!=NULL)
    {
        delete nodes.processing9;
    }
    if (nodes.processinga!=NULL)
    {
        delete nodes.processinga;
    }
    if (nodes.sinka!=NULL)
    {
        delete nodes.sinka;
    }
    if (nodes.sinkb!=NULL)
    {
        delete nodes.sinkb;
    }
    if (nodes.source!=NULL)
    {
        delete nodes.source;
    }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;
    int32_t debugCounter=1;


    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while((cgStaticError==0) && (debugCounter > 0))
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 81; id++)
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
                   cgStaticError = processing1.run();
                }
                break;

                case 2:
                {
                   cgStaticError = processing10.run();
                }
                break;

                case 3:
                {
                   cgStaticError = processing2.run();
                }
                break;

                case 4:
                {
                   cgStaticError = processing3.run();
                }
                break;

                case 5:
                {
                   cgStaticError = processing4.run();
                }
                break;

                case 6:
                {
                   cgStaticError = processing5.run();
                }
                break;

                case 7:
                {
                   cgStaticError = processing6.run();
                }
                break;

                case 8:
                {
                   cgStaticError = processing7.run();
                }
                break;

                case 9:
                {
                   cgStaticError = processing8.run();
                }
                break;

                case 10:
                {
                   cgStaticError = processing9.run();
                }
                break;

                case 11:
                {
                   cgStaticError = processinga.run();
                }
                break;

                case 12:
                {
                   cgStaticError = sinka.run();
                }
                break;

                case 13:
                {
                   cgStaticError = sinkb.run();
                }
                break;

                case 14:
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
