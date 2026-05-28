# README

POSIX implementation of the CMSIS Stream event system.

It is using C++ standard library for most of the features.

Thread priority control is platform dependent and the code is different for Windows, Linux and Mac.

It is handled by `stream_event_queue.cpp` and the difference is hidden for the CMSIS Stream event runtime.

The high-level runtime API is exposed from `stream_runtime_init.hpp` and
`stream_runtime_init.cpp`, matching the CMSIS-RTOS and Zephyr runtimes.

The `config` folder contains template files that should be copied into a client
application for customization.

This implementation uses one thread for the dataflow scheduler and one thread
for managing events.

The part related to inter process communication and buffer sharing
without copy is not implemented in this runtime.
