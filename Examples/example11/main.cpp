#include <cstdio>
#include <cstdint>
#include "scheduler.h"

int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    //int err = init_scheduler();
    uint32_t nbSched=scheduler(&error,NULL);
    //free_scheduler();
    return 0;
}