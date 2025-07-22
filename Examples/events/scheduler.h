/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef _SCHEDULER_H_ 
#define _SCHEDULER_H_

#ifdef   __cplusplus
extern "C"
{
#endif


/* Node identifiers */
#define NB_IDENTIFIED_NODES 4
#define PROCESSING_ID 0
#define SINK_ID 1
#define SOURCE_ID 2
#define EVTSINK_ID 3

/* Selectors global identifiers */
#define SEL_RESET_ID 100 
#define SEL_INCREMENT_ID 101 
#define SEL_VALUE_ID 102 

extern CStreamNode* get_scheduler_node(int32_t nodeID);


extern int init_scheduler();
extern void free_scheduler();

extern uint32_t scheduler(int *error);

#ifdef   __cplusplus
}
#endif

#endif

