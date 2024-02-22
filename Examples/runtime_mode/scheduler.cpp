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
static uint8_t schedule[37]=
{ 
19,20,0,21,22,14,23,24,2,25,26,3,4,27,28,7,8,29,30,9,31,32,10,11,12,33,34,13,1,5,6,16,15,16,15,17,18,
};

/***********

Node identification

************/
static void * identifiedNodes[NB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 8
#define FIFOSIZE1 8
#define FIFOSIZE2 8
#define FIFOSIZE3 8
#define FIFOSIZE4 8
#define FIFOSIZE5 8
#define FIFOSIZE6 8
#define FIFOSIZE7 8
#define FIFOSIZE8 8
#define FIFOSIZE9 8
#define FIFOSIZE10 8
#define FIFOSIZE11 8
#define FIFOSIZE12 8
#define FIFOSIZE13 8
#define FIFOSIZE14 8
#define FIFOSIZE15 8
#define FIFOSIZE16 8
#define FIFOSIZE17 8
#define FIFOSIZE18 8
#define FIFOSIZE19 8
#define FIFOSIZE20 8
#define FIFOSIZE21 8
#define FIFOSIZE22 8
#define FIFOSIZE23 8
#define FIFOSIZE24 8
#define FIFOSIZE25 8
#define FIFOSIZE26 8
#define FIFOSIZE27 8
#define FIFOSIZE28 8
#define FIFOSIZE29 8
#define FIFOSIZE30 8
#define FIFOSIZE31 4
#define FIFOSIZE32 8
#define FIFOSIZE33 8

#define BUFFERSIZE0 32
CG_BEFORE_BUFFER
uint8_t buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 32
CG_BEFORE_BUFFER
uint8_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 32
CG_BEFORE_BUFFER
uint8_t buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 32
CG_BEFORE_BUFFER
uint8_t buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 32
CG_BEFORE_BUFFER
uint8_t buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 32
CG_BEFORE_BUFFER
uint8_t buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 32
CG_BEFORE_BUFFER
uint8_t buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 8
CG_BEFORE_BUFFER
float buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 8
CG_BEFORE_BUFFER
float buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 8
CG_BEFORE_BUFFER
float buf9[BUFFERSIZE9]={0};


typedef struct {
FIFO<float,FIFOSIZE0,1,0> *fifo0;
FIFO<float,FIFOSIZE1,1,0> *fifo1;
FIFO<float,FIFOSIZE2,1,0> *fifo2;
FIFO<float,FIFOSIZE3,1,0> *fifo3;
FIFO<float,FIFOSIZE4,1,0> *fifo4;
FIFO<float,FIFOSIZE5,1,0> *fifo5;
FIFO<float,FIFOSIZE6,1,0> *fifo6;
FIFO<float,FIFOSIZE7,1,0> *fifo7;
FIFO<float,FIFOSIZE8,1,0> *fifo8;
FIFO<float,FIFOSIZE9,1,0> *fifo9;
FIFO<float,FIFOSIZE10,1,0> *fifo10;
FIFO<float,FIFOSIZE11,1,0> *fifo11;
FIFO<float,FIFOSIZE12,1,0> *fifo12;
FIFO<float,FIFOSIZE13,1,0> *fifo13;
FIFO<float,FIFOSIZE14,1,0> *fifo14;
FIFO<float,FIFOSIZE15,1,0> *fifo15;
FIFO<float,FIFOSIZE16,1,0> *fifo16;
FIFO<float,FIFOSIZE17,1,0> *fifo17;
FIFO<float,FIFOSIZE18,1,0> *fifo18;
FIFO<float,FIFOSIZE19,1,0> *fifo19;
FIFO<float,FIFOSIZE20,1,0> *fifo20;
FIFO<float,FIFOSIZE21,1,0> *fifo21;
FIFO<float,FIFOSIZE22,1,0> *fifo22;
FIFO<float,FIFOSIZE23,1,0> *fifo23;
FIFO<float,FIFOSIZE24,1,0> *fifo24;
FIFO<float,FIFOSIZE25,1,0> *fifo25;
FIFO<float,FIFOSIZE26,1,0> *fifo26;
FIFO<float,FIFOSIZE27,1,0> *fifo27;
FIFO<float,FIFOSIZE28,1,0> *fifo28;
FIFO<float,FIFOSIZE29,1,0> *fifo29;
FIFO<float,FIFOSIZE30,0,0> *fifo30;
FIFO<float,FIFOSIZE31,1,0> *fifo31;
FIFO<float,FIFOSIZE32,0,0> *fifo32;
FIFO<float,FIFOSIZE33,0,0> *fifo33;
} fifos_t;

typedef struct {
    Adder<float,8,float,8,float,8> *adder1;
    Adder<float,8,float,8,float,8> *adder10;
    Adder<float,8,float,8,float,8> *adder11;
    Adder<float,8,float,8,float,8> *adder12;
    Adder<float,8,float,8,float,8> *adder13;
    Adder<float,8,float,8,float,8> *adder14;
    Adder<float,8,float,8,float,8> *adder15;
    Adder<float,8,float,8,float,8> *adder2;
    Adder<float,8,float,8,float,8> *adder3;
    Adder<float,8,float,8,float,8> *adder4;
    Adder<float,8,float,8,float,8> *adder5;
    Adder<float,8,float,8,float,8> *adder6;
    Adder<float,8,float,8,float,8> *adder7;
    Adder<float,8,float,8,float,8> *adder8;
    Adder<float,8,float,8,float,8> *adder9;
    Duplicate<float,4,float,4> *dup0;
    ProcessingNode<float,4,float,4> *processing;
    Sink<float,8> *sinka;
    Sink<float,8> *sinkb;
    Source<float,8> *source0;
    Source<float,8> *source1;
    Source<float,8> *source10;
    Source<float,8> *source11;
    Source<float,8> *source12;
    Source<float,8> *source13;
    Source<float,8> *source14;
    Source<float,8> *source15;
    Source<float,8> *source2;
    Source<float,8> *source3;
    Source<float,8> *source4;
    Source<float,8> *source5;
    Source<float,8> *source6;
    Source<float,8> *source7;
    Source<float,8> *source8;
    Source<float,8> *source9;
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
    fifos.fifo0 = new FIFO<float,FIFOSIZE0,1,0>(buf0);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new FIFO<float,FIFOSIZE1,1,0>(buf1);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo2 = new FIFO<float,FIFOSIZE2,1,0>(buf3);
    if (fifos.fifo2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo3 = new FIFO<float,FIFOSIZE3,1,0>(buf5);
    if (fifos.fifo3==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo4 = new FIFO<float,FIFOSIZE4,1,0>(buf2);
    if (fifos.fifo4==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo5 = new FIFO<float,FIFOSIZE5,1,0>(buf4);
    if (fifos.fifo5==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo6 = new FIFO<float,FIFOSIZE6,1,0>(buf4);
    if (fifos.fifo6==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo7 = new FIFO<float,FIFOSIZE7,1,0>(buf5);
    if (fifos.fifo7==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo8 = new FIFO<float,FIFOSIZE8,1,0>(buf5);
    if (fifos.fifo8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo9 = new FIFO<float,FIFOSIZE9,1,0>(buf6);
    if (fifos.fifo9==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo10 = new FIFO<float,FIFOSIZE10,1,0>(buf2);
    if (fifos.fifo10==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo11 = new FIFO<float,FIFOSIZE11,1,0>(buf4);
    if (fifos.fifo11==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo12 = new FIFO<float,FIFOSIZE12,1,0>(buf3);
    if (fifos.fifo12==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo13 = new FIFO<float,FIFOSIZE13,1,0>(buf5);
    if (fifos.fifo13==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo14 = new FIFO<float,FIFOSIZE14,1,0>(buf4);
    if (fifos.fifo14==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo15 = new FIFO<float,FIFOSIZE15,1,0>(buf5);
    if (fifos.fifo15==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo16 = new FIFO<float,FIFOSIZE16,1,0>(buf1);
    if (fifos.fifo16==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo17 = new FIFO<float,FIFOSIZE17,1,0>(buf3);
    if (fifos.fifo17==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo18 = new FIFO<float,FIFOSIZE18,1,0>(buf3);
    if (fifos.fifo18==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo19 = new FIFO<float,FIFOSIZE19,1,0>(buf0);
    if (fifos.fifo19==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo20 = new FIFO<float,FIFOSIZE20,1,0>(buf1);
    if (fifos.fifo20==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo21 = new FIFO<float,FIFOSIZE21,1,0>(buf4);
    if (fifos.fifo21==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo22 = new FIFO<float,FIFOSIZE22,1,0>(buf1);
    if (fifos.fifo22==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo23 = new FIFO<float,FIFOSIZE23,1,0>(buf5);
    if (fifos.fifo23==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo24 = new FIFO<float,FIFOSIZE24,1,0>(buf3);
    if (fifos.fifo24==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo25 = new FIFO<float,FIFOSIZE25,1,0>(buf4);
    if (fifos.fifo25==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo26 = new FIFO<float,FIFOSIZE26,1,0>(buf4);
    if (fifos.fifo26==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo27 = new FIFO<float,FIFOSIZE27,1,0>(buf1);
    if (fifos.fifo27==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo28 = new FIFO<float,FIFOSIZE28,1,0>(buf2);
    if (fifos.fifo28==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo29 = new FIFO<float,FIFOSIZE29,1,0>(buf0);
    if (fifos.fifo29==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo30 = new FIFO<float,FIFOSIZE30,0,0>(buf7);
    if (fifos.fifo30==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo31 = new FIFO<float,FIFOSIZE31,1,0>(buf0);
    if (fifos.fifo31==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo32 = new FIFO<float,FIFOSIZE32,0,0>(buf8);
    if (fifos.fifo32==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo33 = new FIFO<float,FIFOSIZE33,0,0>(buf9);
    if (fifos.fifo33==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    nodes.adder1 = new Adder<float,8,float,8,float,8>(fifo0,fifo1,fifo4);
    if (nodes.adder1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER1_ID]=(void*)nodes.adder1;
    nodes.adder1->setID(ADDER1_ID);
    nodes.adder10 = new Adder<float,8,float,8,float,8>(fifo18,fifo19,fifo26);
    if (nodes.adder10==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER10_ID]=(void*)nodes.adder10;
    nodes.adder10->setID(ADDER10_ID);
    nodes.adder11 = new Adder<float,8,float,8,float,8>(fifo20,fifo21,fifo24);
    if (nodes.adder11==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER11_ID]=(void*)nodes.adder11;
    nodes.adder11->setID(ADDER11_ID);
    nodes.adder12 = new Adder<float,8,float,8,float,8>(fifo22,fifo23,fifo25);
    if (nodes.adder12==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER12_ID]=(void*)nodes.adder12;
    nodes.adder12->setID(ADDER12_ID);
    nodes.adder13 = new Adder<float,8,float,8,float,8>(fifo24,fifo25,fifo27);
    if (nodes.adder13==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER13_ID]=(void*)nodes.adder13;
    nodes.adder13->setID(ADDER13_ID);
    nodes.adder14 = new Adder<float,8,float,8,float,8>(fifo26,fifo27,fifo29);
    if (nodes.adder14==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER14_ID]=(void*)nodes.adder14;
    nodes.adder14->setID(ADDER14_ID);
    nodes.adder15 = new Adder<float,8,float,8,float,8>(fifo28,fifo29,fifo30);
    if (nodes.adder15==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER15_ID]=(void*)nodes.adder15;
    nodes.adder15->setID(ADDER15_ID);
    nodes.adder2 = new Adder<float,8,float,8,float,8>(fifo2,fifo3,fifo5);
    if (nodes.adder2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER2_ID]=(void*)nodes.adder2;
    nodes.adder2->setID(ADDER2_ID);
    nodes.adder3 = new Adder<float,8,float,8,float,8>(fifo4,fifo5,fifo12);
    if (nodes.adder3==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER3_ID]=(void*)nodes.adder3;
    nodes.adder3->setID(ADDER3_ID);
    nodes.adder4 = new Adder<float,8,float,8,float,8>(fifo6,fifo7,fifo10);
    if (nodes.adder4==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER4_ID]=(void*)nodes.adder4;
    nodes.adder4->setID(ADDER4_ID);
    nodes.adder5 = new Adder<float,8,float,8,float,8>(fifo8,fifo9,fifo11);
    if (nodes.adder5==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER5_ID]=(void*)nodes.adder5;
    nodes.adder5->setID(ADDER5_ID);
    nodes.adder6 = new Adder<float,8,float,8,float,8>(fifo10,fifo11,fifo13);
    if (nodes.adder6==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER6_ID]=(void*)nodes.adder6;
    nodes.adder6->setID(ADDER6_ID);
    nodes.adder7 = new Adder<float,8,float,8,float,8>(fifo12,fifo13,fifo28);
    if (nodes.adder7==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER7_ID]=(void*)nodes.adder7;
    nodes.adder7->setID(ADDER7_ID);
    nodes.adder8 = new Adder<float,8,float,8,float,8>(fifo14,fifo15,fifo18);
    if (nodes.adder8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER8_ID]=(void*)nodes.adder8;
    nodes.adder8->setID(ADDER8_ID);
    nodes.adder9 = new Adder<float,8,float,8,float,8>(fifo16,fifo17,fifo19);
    if (nodes.adder9==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ADDER9_ID]=(void*)nodes.adder9;
    nodes.adder9->setID(ADDER9_ID);
    nodes.dup0 = new Duplicate<float,4,float,4>(fifo31,{&fifo32,&fifo33});
    if (nodes.dup0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.processing = new ProcessingNode<float,4,float,4>(fifo30,fifo31);
    if (nodes.processing==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[PROCESSING_ID]=(void*)nodes.processing;
    nodes.processing->setID(PROCESSING_ID);
    nodes.sinka = new Sink<float,8>(fifo32);
    if (nodes.sinka==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.sinkb = new Sink<float,8>(fifo33);
    if (nodes.sinkb==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source0 = new Source<float,8>(fifo0);
    if (nodes.source0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source1 = new Source<float,8>(fifo1);
    if (nodes.source1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source10 = new Source<float,8>(fifo16);
    if (nodes.source10==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source11 = new Source<float,8>(fifo17);
    if (nodes.source11==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source12 = new Source<float,8>(fifo20);
    if (nodes.source12==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source13 = new Source<float,8>(fifo21);
    if (nodes.source13==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source14 = new Source<float,8>(fifo22);
    if (nodes.source14==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source15 = new Source<float,8>(fifo23);
    if (nodes.source15==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source2 = new Source<float,8>(fifo2);
    if (nodes.source2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source3 = new Source<float,8>(fifo3);
    if (nodes.source3==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source4 = new Source<float,8>(fifo6);
    if (nodes.source4==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source5 = new Source<float,8>(fifo7);
    if (nodes.source5==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source6 = new Source<float,8>(fifo8);
    if (nodes.source6==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source7 = new Source<float,8>(fifo9);
    if (nodes.source7==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source8 = new Source<float,8>(fifo14);
    if (nodes.source8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.source9 = new Source<float,8>(fifo15);
    if (nodes.source9==NULL)
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
    if (fifos.fifo14!=NULL)
    {
       delete fifos.fifo14;
    }
    if (fifos.fifo15!=NULL)
    {
       delete fifos.fifo15;
    }
    if (fifos.fifo16!=NULL)
    {
       delete fifos.fifo16;
    }
    if (fifos.fifo17!=NULL)
    {
       delete fifos.fifo17;
    }
    if (fifos.fifo18!=NULL)
    {
       delete fifos.fifo18;
    }
    if (fifos.fifo19!=NULL)
    {
       delete fifos.fifo19;
    }
    if (fifos.fifo20!=NULL)
    {
       delete fifos.fifo20;
    }
    if (fifos.fifo21!=NULL)
    {
       delete fifos.fifo21;
    }
    if (fifos.fifo22!=NULL)
    {
       delete fifos.fifo22;
    }
    if (fifos.fifo23!=NULL)
    {
       delete fifos.fifo23;
    }
    if (fifos.fifo24!=NULL)
    {
       delete fifos.fifo24;
    }
    if (fifos.fifo25!=NULL)
    {
       delete fifos.fifo25;
    }
    if (fifos.fifo26!=NULL)
    {
       delete fifos.fifo26;
    }
    if (fifos.fifo27!=NULL)
    {
       delete fifos.fifo27;
    }
    if (fifos.fifo28!=NULL)
    {
       delete fifos.fifo28;
    }
    if (fifos.fifo29!=NULL)
    {
       delete fifos.fifo29;
    }
    if (fifos.fifo30!=NULL)
    {
       delete fifos.fifo30;
    }
    if (fifos.fifo31!=NULL)
    {
       delete fifos.fifo31;
    }
    if (fifos.fifo32!=NULL)
    {
       delete fifos.fifo32;
    }
    if (fifos.fifo33!=NULL)
    {
       delete fifos.fifo33;
    }

    if (nodes.adder1!=NULL)
    {
        delete nodes.adder1;
    }
    if (nodes.adder10!=NULL)
    {
        delete nodes.adder10;
    }
    if (nodes.adder11!=NULL)
    {
        delete nodes.adder11;
    }
    if (nodes.adder12!=NULL)
    {
        delete nodes.adder12;
    }
    if (nodes.adder13!=NULL)
    {
        delete nodes.adder13;
    }
    if (nodes.adder14!=NULL)
    {
        delete nodes.adder14;
    }
    if (nodes.adder15!=NULL)
    {
        delete nodes.adder15;
    }
    if (nodes.adder2!=NULL)
    {
        delete nodes.adder2;
    }
    if (nodes.adder3!=NULL)
    {
        delete nodes.adder3;
    }
    if (nodes.adder4!=NULL)
    {
        delete nodes.adder4;
    }
    if (nodes.adder5!=NULL)
    {
        delete nodes.adder5;
    }
    if (nodes.adder6!=NULL)
    {
        delete nodes.adder6;
    }
    if (nodes.adder7!=NULL)
    {
        delete nodes.adder7;
    }
    if (nodes.adder8!=NULL)
    {
        delete nodes.adder8;
    }
    if (nodes.adder9!=NULL)
    {
        delete nodes.adder9;
    }
    if (nodes.dup0!=NULL)
    {
        delete nodes.dup0;
    }
    if (nodes.processing!=NULL)
    {
        delete nodes.processing;
    }
    if (nodes.sinka!=NULL)
    {
        delete nodes.sinka;
    }
    if (nodes.sinkb!=NULL)
    {
        delete nodes.sinkb;
    }
    if (nodes.source0!=NULL)
    {
        delete nodes.source0;
    }
    if (nodes.source1!=NULL)
    {
        delete nodes.source1;
    }
    if (nodes.source10!=NULL)
    {
        delete nodes.source10;
    }
    if (nodes.source11!=NULL)
    {
        delete nodes.source11;
    }
    if (nodes.source12!=NULL)
    {
        delete nodes.source12;
    }
    if (nodes.source13!=NULL)
    {
        delete nodes.source13;
    }
    if (nodes.source14!=NULL)
    {
        delete nodes.source14;
    }
    if (nodes.source15!=NULL)
    {
        delete nodes.source15;
    }
    if (nodes.source2!=NULL)
    {
        delete nodes.source2;
    }
    if (nodes.source3!=NULL)
    {
        delete nodes.source3;
    }
    if (nodes.source4!=NULL)
    {
        delete nodes.source4;
    }
    if (nodes.source5!=NULL)
    {
        delete nodes.source5;
    }
    if (nodes.source6!=NULL)
    {
        delete nodes.source6;
    }
    if (nodes.source7!=NULL)
    {
        delete nodes.source7;
    }
    if (nodes.source8!=NULL)
    {
        delete nodes.source8;
    }
    if (nodes.source9!=NULL)
    {
        delete nodes.source9;
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
        for(unsigned long id=0 ; id < 37; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = adder1.run();
                }
                break;

                case 1:
                {
                   cgStaticError = adder10.run();
                }
                break;

                case 2:
                {
                   cgStaticError = adder11.run();
                }
                break;

                case 3:
                {
                   cgStaticError = adder12.run();
                }
                break;

                case 4:
                {
                   cgStaticError = adder13.run();
                }
                break;

                case 5:
                {
                   cgStaticError = adder14.run();
                }
                break;

                case 6:
                {
                   cgStaticError = adder15.run();
                }
                break;

                case 7:
                {
                   cgStaticError = adder2.run();
                }
                break;

                case 8:
                {
                   cgStaticError = adder3.run();
                }
                break;

                case 9:
                {
                   cgStaticError = adder4.run();
                }
                break;

                case 10:
                {
                   cgStaticError = adder5.run();
                }
                break;

                case 11:
                {
                   cgStaticError = adder6.run();
                }
                break;

                case 12:
                {
                   cgStaticError = adder7.run();
                }
                break;

                case 13:
                {
                   cgStaticError = adder8.run();
                }
                break;

                case 14:
                {
                   cgStaticError = adder9.run();
                }
                break;

                case 15:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 16:
                {
                   cgStaticError = processing.run();
                }
                break;

                case 17:
                {
                   cgStaticError = sinka.run();
                }
                break;

                case 18:
                {
                   cgStaticError = sinkb.run();
                }
                break;

                case 19:
                {
                   cgStaticError = source0.run();
                }
                break;

                case 20:
                {
                   cgStaticError = source1.run();
                }
                break;

                case 21:
                {
                   cgStaticError = source10.run();
                }
                break;

                case 22:
                {
                   cgStaticError = source11.run();
                }
                break;

                case 23:
                {
                   cgStaticError = source12.run();
                }
                break;

                case 24:
                {
                   cgStaticError = source13.run();
                }
                break;

                case 25:
                {
                   cgStaticError = source14.run();
                }
                break;

                case 26:
                {
                   cgStaticError = source15.run();
                }
                break;

                case 27:
                {
                   cgStaticError = source2.run();
                }
                break;

                case 28:
                {
                   cgStaticError = source3.run();
                }
                break;

                case 29:
                {
                   cgStaticError = source4.run();
                }
                break;

                case 30:
                {
                   cgStaticError = source5.run();
                }
                break;

                case 31:
                {
                   cgStaticError = source6.run();
                }
                break;

                case 32:
                {
                   cgStaticError = source7.run();
                }
                break;

                case 33:
                {
                   cgStaticError = source8.run();
                }
                break;

                case 34:
                {
                   cgStaticError = source9.run();
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
