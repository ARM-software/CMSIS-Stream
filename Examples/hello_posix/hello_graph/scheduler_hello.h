/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef SCHEDULER_HELLO_H_ 
#define SCHEDULER_HELLO_H_


#include <stdint.h>

#ifdef   __cplusplus
extern "C"
{
#endif

#include "cstream_node.h"


/* Node identifiers */
#define STREAM_HELLO_NB_IDENTIFIED_NODES 4
#define STREAM_HELLO_PROCESS_ID 0
#define STREAM_HELLO_SINK_ID 1
#define STREAM_HELLO_SRC_ID 2
#define STREAM_HELLO_EVTSINK_ID 3

#define STREAM_HELLO_SCHED_LEN 3

/* Selectors global identifiers */
#define SEL_MESSAGE_ID 100 

extern CStreamNode* get_scheduler_hello_node(int32_t nodeID);

extern int init_scheduler_hello(void *evtQueue_,helloParams_t *params);
extern void free_scheduler_hello();
extern uint32_t scheduler_hello(int *error);
extern void reset_fifos_scheduler_hello(int all);

#ifdef   __cplusplus
}
#endif

#endif

