#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdlib.h>
#include <stdio.h>

#include "retarget_stdout.h"

extern int stdout_init (void);

int main() {
    stdout_init();

    printf("Hello, World!\n");
    exit(0);
}
