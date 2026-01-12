#include <cstdio>
#include <cstdint>
#include "scheduler.h"

int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    // NULL queue since only dataflow nodes are used in this example
    uint32_t nbSched=scheduler(&error,NULL);
    return 0;
}