/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef SCHEDULER_H_ 
#define SCHEDULER_H_


#include <stdint.h>

#ifdef   __cplusplus
extern "C"
{
#endif




extern int init_scheduler(void *evtQueue_);
extern void free_scheduler();
extern uint32_t scheduler(int *error);
extern void reset_fifos_scheduler(int all);

#ifdef   __cplusplus
}
#endif

#endif

