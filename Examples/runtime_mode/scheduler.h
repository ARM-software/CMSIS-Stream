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


#define NB_IDENTIFIED_NODES 16
#define ADDER1_ID 0
#define ADDER10_ID 1
#define ADDER11_ID 2
#define ADDER12_ID 3
#define ADDER13_ID 4
#define ADDER14_ID 5
#define ADDER15_ID 6
#define ADDER2_ID 7
#define ADDER3_ID 8
#define ADDER4_ID 9
#define ADDER5_ID 10
#define ADDER6_ID 11
#define ADDER7_ID 12
#define ADDER8_ID 13
#define ADDER9_ID 14
#define PROCESSING_ID 15

extern void *get_scheduler_node(int32_t nodeID);

extern int init_scheduler();
extern void free_scheduler();
extern uint32_t scheduler(int *error);

#ifdef   __cplusplus
}
#endif

#endif

