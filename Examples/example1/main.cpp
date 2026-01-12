#include <cstdio>
#include <cstdint>
#include "scheduler.h"

int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    uint32_t nbSched=scheduler(&error,NULL,1);
    return 0;
}