/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "custom.hpp"
#include "cg_enums.h"
#include "StreamNode.hpp"
#include "cstream_node.h"
#include "IdentifiedNode.hpp"
#include "EventQueue.hpp"
#include "GenericNodes.hpp"
#include "AppNodes.hpp"
#include "simple_scheduler.h"

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

/*

Internal ID identification for the nodes

*/
#define SINK_INTERNAL_ID 0
#define SOURCE_INTERNAL_ID 1

/* Initialize the selectors global IDs in each class */
std::array<uint16_t,1> EvtSource::selectors = {SEL_INCREMENT_ID};


/***********

Node identification

************/
static CStreamNode identifiedNodes[NB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/



typedef struct {
    EvtSink *sink;
    EvtSource *source;
} nodes_t;



static nodes_t nodes={0};

CStreamNode* get_simple_node(int32_t nodeID)
{
    if (nodeID >= NB_IDENTIFIED_NODES)
    {
        return(nullptr);
    }
    if (nodeID < 0)
    {
        return(nullptr);
    }
    return(&identifiedNodes[nodeID]);
}

int init_simple()
{

    CG_BEFORE_FIFO_INIT;

    CG_BEFORE_NODE_INIT;

    nodes.sink = new (std::nothrow) EvtSink;
    if (nodes.sink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
identifiedNodes[SINK_ID]=createStreamNode(*nodes.sink);
    nodes.sink->setID(SINK_ID);
    nodes.source = new (std::nothrow) EvtSource;
    if (nodes.source==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
identifiedNodes[SOURCE_ID]=createStreamNode(*nodes.source);
    nodes.source->setID(SOURCE_ID);

/* Subscribe nodes for the event system*/
    nodes.source->subscribe(0,*nodes.sink,0);


    return(CG_SUCCESS);

}

void free_simple()
{

    if (nodes.sink!=NULL)
    {
        delete nodes.sink;
    }
    if (nodes.source!=NULL)
    {
        delete nodes.source;
    }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t simple(int *error)
{
    *error=CG_SUCCESS;
    while(1){
        // To have possibility to process the event queue
        CG_BEFORE_ITERATION;
    };
    return(0);
}
