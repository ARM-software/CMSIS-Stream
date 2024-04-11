#include <cstdio>
#include <cstdint>
#include "scheduler.h"

static uint8_t myBuffer[20];

int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    error = init_buffer_scheduler(myBuffer);
    if (error != 0)
    {
        printf("Error allocating buffers\r\n");
    }
    uint32_t nbSched=scheduler(&error,myBuffer);
    free_buffer_scheduler(myBuffer);
    return 0;
}