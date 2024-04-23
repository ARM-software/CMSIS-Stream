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

typedef struct scheduler_cb
{
    unsigned long scheduleStateID;
    unsigned long nbSched;
    int status;
} scheduler_cb_t;

extern void init_cb_state_scheduler(scheduler_cb_t*);




extern int init_scheduler(int *someVariable);
extern void free_scheduler(int *someVariable);

extern uint32_t scheduler(int *error,scheduler_cb_t *scheduleState,int *someVariable);

#ifdef   __cplusplus
}
#endif

#endif

