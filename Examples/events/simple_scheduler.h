/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef SIMPLE_SCHEDULER_H_ 
#define SIMPLE_SCHEDULER_H_


#include <stdint.h>

#ifdef   __cplusplus
extern "C"
{
#endif

#include "cstream_node.h"


/* Node identifiers */
#define NB_IDENTIFIED_NODES 2
#define SINK_ID 0
#define SOURCE_ID 1

/* Selectors global identifiers */
#define SEL_RESET_ID 100 
#define SEL_INCREMENT_ID 101 
#define SEL_VALUE_ID 102 

extern CStreamNode* get_simple_node(int32_t nodeID);

extern int init_simple(void *evtQueue_);
extern void free_simple();
extern uint32_t simple(int *error);
extern void reset_fifos_simple(int all);

#ifdef   __cplusplus
}
#endif

#endif

