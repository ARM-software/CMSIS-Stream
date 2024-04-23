#include <cstdio>
#include <cstdint>
#include "scheduler.h"
#include "cg_status.h"

scheduler_cb_t cb_state;

extern "C" {
    extern int g_frame_available;
}

int g_frame_available = 0;
int state=0;

int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    init_cb_state_scheduler(&cb_state);
    init_scheduler(&state);
    uint32_t nbSched;

    for(int i=0;i<7;i++)
    {
         nbSched=scheduler(&error,&cb_state,&state);
         g_frame_available=1;
         printf("-- Pause\n");
    }

    nbSched=scheduler(&error,&cb_state,&state);
    printf("-- Pause and end\n");

    free_scheduler(&state);

    printf("Nb sched = %d\n",nbSched);
    if ((error != CG_PAUSE_SCHEDULER) && (error != CG_STOP_SCHEDULER))
    {
       printf("Error = %d\n",error);
    }
    return 0;
}