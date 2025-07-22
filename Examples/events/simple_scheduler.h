/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef _SIMPLE_SCHEDULER_H_ 
#define _SIMPLE_SCHEDULER_H_

#ifdef   __cplusplus
extern "C"
{
#endif


/* Node identifiers */
#define NB_IDENTIFIED_NODES 2
#define SINK_ID 0
#define SOURCE_ID 1

/* Selectors global identifiers */
#define SEL_RESET_ID 100 
#define SEL_INCREMENT_ID 101 
#define SEL_VALUE_ID 102 

extern CStreamNode* get_simple_node(int32_t nodeID);


extern int init_simple();
extern void free_simple();

extern uint32_t simple(int *error);

#ifdef   __cplusplus
}
#endif

#endif

