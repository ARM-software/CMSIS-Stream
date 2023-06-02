#include <stdio.h>
#include <stdint.h>
#include "scheduler.h"

int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    uint32_t nbSched=scheduler(&error);
    return 0;
}