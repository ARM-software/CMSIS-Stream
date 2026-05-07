#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdlib.h>
#include <stdio.h>

#include "stream_init.hpp"

extern "C" {

#include "cmsis_os2.h" /* CMSIS-RTOS2 API */

}

void app_main (void *argument)
 {
   // Stream is configured here because the resume functions
   // of some nodes may want to use RTOS resources like timer.
   // So the RTOS must already be started.
   stream_configure_and_start();

}

int main() {

    printf("Hello, World!\n");

    osStatus_t err = osKernelInitialize();
    if (err != osOK) {
        CMSISSTREAM_LOG_ERR("Kernel initialization failed\n");
        goto error;
    }

    // Configure stream graphs and start the graph selected by currentNetwork.
    osThreadNew(app_main, NULL, NULL);    // Create application main thread

    printf("Start kernel\n");

    // Hand control to the RTOS scheduler.
    osKernelStart();


error:
    printf("\x04");

    for (;;) {}
}
