/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef _CV_SCHEDULER1_H_ 
#define _CV_SCHEDULER1_H_

#ifdef   __cplusplus
extern "C"
{
#endif



extern int init_buffer_scheduler(uint8_t *myBuffer,
                              uint8_t *myBufferB);
extern void free_buffer_scheduler(uint8_t *myBuffer,
                              uint8_t *myBufferB);


extern uint32_t scheduler(int *error,uint8_t *myBuffer,
                              uint8_t *myBufferB);

#ifdef   __cplusplus
}
#endif

#endif

