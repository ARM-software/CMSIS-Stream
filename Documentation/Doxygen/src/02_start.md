# How to start CMSIS Stream {#cmsis_stream_start}

The `stream_runtime_init.hpp` is a convenience header to start CMSIS-Stream.
It is possible to start CMSIS-Stream manually but the APIs in this header take
care of creating the memory pools, the event queues and the threads.

They also provide a way to pause or resume a graph and to context switch to
another graph.

1. Call `stream_init_memory`
2. Allocate event queues for each of your apps with `stream_new_event_queue`
3. Init the nodes for each of your app using functions in the generated
   scheduler header
4. Resume the app you want to start first with `resume_scheduler_app`
5. Create the CMSIS-Stream runtime threads with `stream_start_threads`
6. Start RTOS scheduling with `osKernelStart`

To context switch between two apps use:

1. `pause_scheduler_app`
2. `resume_scheduler_app`
