#include <cstdio>
#include <cstdint>
#include "scheduler.h"
#include "cg_enums.h"

extern "C" {
    extern int g_frame_available;
    extern void myfunc(const float *i,float *o,int l);
}

int g_frame_available = 0;
int state=0;

#define CHECK                                                     \
if ((error != CG_PAUSED_SCHEDULER))                               \
    {                                                             \
       printf("Error = %d\n",error);                              \
       return 0;                                                  \
    }

void myfunc(const float *src,float *o,int l)
{
   for(int i=0;i<l;i++)
   {
     o[i] = src[i] + 1.0f;
   }
}


int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    init_scheduler(&state);
    uint32_t nbSched;

    for(int i=0;i<7;i++)
    {
         nbSched=scheduler(&error,&state);
         CHECK
         g_frame_available=1;
         printf("-- Pause\n");
    }

    nbSched=scheduler(&error,&state);
    CHECK
    printf("-- Pause and end\n");

    free_scheduler(&state);

    printf("Nb sched = %d\n",nbSched);
    return 0;
}