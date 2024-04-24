#ifndef _CUSTOM_H_

/* Put anything you need in this file */
// Some additional state for the state machine
#define CG_BEFORE_SCHEDULE \
  int someState=0;

// State is preserved and restored between suspension and restoration
// of state machine
#define CG_RESTORE_STATE_MACHINE_STATE \
   *someVariable = someState;

#define CG_SAVE_STATE_MACHINE_STATE \
    someState = *someVariable;


#ifdef   __cplusplus
extern "C"
{
#endif


extern void myfunc(const float *i,float *o,int l);

#ifdef   __cplusplus
}
#endif

#endif 