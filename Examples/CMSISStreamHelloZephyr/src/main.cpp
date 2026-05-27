#include <zephyr/kernel.h>

#include <cstdio>

#include "app_config.hpp"
#include "stream_init.hpp"

int main(void)
{
    std::printf("CMSIS-Stream Zephyr hello on Corstone-300 FVP\n");

    stream_configure_and_start();

    /*
     * The hello graph intentionally raises an error after a few iterations to
     * exercise runtime error handling. The application handler in stream_init.cpp
     * emits EOT for the FVP and then stops the main thread.
     */
    k_sleep(K_FOREVER);
    return 0;
}
