# README

# Files provided by the application

The application must provide some headers (their name can be redefined in the Python script used to generate the scheduler):
- `app_config.hpp` : Application configuration
- `cstream_node.h` : C API to the nodes
- `IdentifiedNode.hpp` : Implementation of the C API
- `stream_runtime_config.hpp` : Runtime configuration

## `app_config.hpp`

This header is generally used to define `CG_BEFORE_BUFFER` and `CG_BEFORE_NODE_EXECUTION` than can be used to modify the CMSIS Stream scheduler.

`CG_BEFORE_BUFFER` __must__ be used to enforce alignment of buffers when memory optimization feature is activated.

## `stream_runtime_config.hpp`

This header can be used to redefine macros `CMSISSTREAM_LOG_ERR` and `CMSISSTREAM_LOG_DBG` for logging.

All configuration macros can also be redefined in this header.

## Other header

Nodes of CMSIS Stream can implement some APIs.
The header `cstream_node.h` and `IdentifiedNode.hpp` provides a way to expose those API to the external world and hide complexity of C++ multiple-inheritance.

The APIs implemented are application dependent and that's why those files must be provided by the application. See CMSIS Stream documentation for more information.

# How to start CMSIS Stream

The `stream_runtime_init.hpp` is a convenience header to start CMSIS Stream.
It is possible to start CMSIS Stream manually but the APIs in this header takes care of creating the memory pools, the event queues and the threads.

They also provide a way to pause / resume a graph and to context switch to another graph.

1. Call `stream_init_memory`
2. Allocate event queues for each of your apps with `stream_new_event_queue`
3. Init the scheduler for each of your app (using functions in the generated scheduler header)
4. Resume the app you want to start first with `resume_scheduler_app`
5. Create the CMSIS Stream runtime threads with `stream_start_threads`
6. Starts this RTOS scheduling with `osKernelStart`

To context switch between two app use:

1. `pause_scheduler_app`
2. `resume_scheduler_app`


# Configuration values
Default configuration values that can be modified when building.
You can redefine any of those values:

- By redefining them on the command line
- By redefining them in the application configuration header used by any CMSIS Stream application for CMSIS RTOS.



## CONFIG_CMSISSTREAM_EVT_THREAD_STACK_SIZE

Stack size of event thread

Default value:
```C
#define CONFIG_CMSISSTREAM_EVT_THREAD_STACK_SIZE 4096
```

## CONFIG_CMSISSTREAM_STREAM_THREAD_STACK_SIZE

Stack size of stream thread

Default value:
```C
#define CONFIG_CMSISSTREAM_STREAM_THREAD_STACK_SIZE 4096
```

## CONFIG_CMSISSTREAM_NB_MAX_EVENTS

Maximum number of events expected in the system at a given time.

This is used to size the memory pools involved in event creation.
Several event queues will use the same pool so this number
may be bigger than the event queue length.

In current `stream_runtime_init` APIs, each application (even paused) has its own queue but they all share the same pool.

Default value:
```C
#define CONFIG_CMSISSTREAM_NB_MAX_EVENTS 16
```

## CONFIG_CMSISSTREAM_NB_MAX_BUFS

Maximum number of buffers (tensors ...) expected in the system at a given time.

This used to size the memory pool involved in atomic
reference counting of buffers (tensors ...).
An event may hold references to several buffers so this number
may be bigger than the number of events.

Default value:
```C
#define CONFIG_CMSISSTREAM_NB_MAX_BUFS 16
```

## CONFIG_CMSISSTREAM_STREAM_THREAD_PRIORITY

Priority of the stream thread. It should be real-time. IT should be higher than any priority used for events.

Default value:
```C
#define CONFIG_CMSISSTREAM_STREAM_THREAD_PRIORITY osPriorityRealTime
```

## CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY

Priority of the event thread when it is processing high priority event.

Default value:
```C
#define CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY osPriorityHigh
```


## CONFIG_CMSISSTREAM_EVT_NORMAL_PRIORITY

Priority of the event thread when it is processing normal priority event.

Default value:
```C
#define CONFIG_CMSISSTREAM_EVT_NORMAL_PRIORITY osPriorityNormal
```

## CONFIG_CMSISSTREAM_EVT_LOW_PRIORITY

Priority of the event thread when it is processing low priority event.

Default value:
```C
#define CONFIG_CMSISSTREAM_EVT_LOW_PRIORITY osPriorityLow
```

## CONFIG_CMSISSTREAM_SHARED_OVERHEAD

Additional memory required by `std::allocate_shared`.

When allocating a object of type `T` with `std::allocate_shared<T>` we need
enough memory for T and some additional memory required by the 
implementation of `std::shared_ptr`.
This parameter defines the additional memory required.
This value is used by the memory pool allocators to allocate enough memory.
The request to the allocator does not have the size `sizeof(T) + sizeof(std::shared_ptr<T>)`
It is slightly bigger.

Default value:
```C
#define CONFIG_CMSISSTREAM_SHARED_OVERHEAD 16
```

## CONFIG_MAX_NUMBER_EVENT_ARGUMENTS

Maximum number of arguments in an event.

It defines the maximum number of arguments that can be passed in an event.
This number impacts the size of memory pools used by the event system.

Default value:
```C
#define CONFIG_MAX_NUMBER_EVENT_ARGUMENTS 8
```

## CMSISSTREAM_TENSOR_MAX_DIMENSIONS

Maximum number of tensor dimensions.

It defines the maximum number of dimensions for tensors used in the CMSIS Stream library.
The default value of 3 is suitable for image oriented applications (height, width, channels).

Default value:
```C
#define CMSISSTREAM_TENSOR_MAX_DIMENSIONS 3
```

## CONFIG_CMSISSTREAM_EVENT_QUEUE_LENGTH

Length of an event queue.

This is the number of events that can be queued in an event queue.
In case of overflow of the queue, new events are dropped.

Default value:
```C
#define CONFIG_CMSISSTREAM_EVENT_QUEUE_LENGTH 20
```

## CONFIG_CMSISSTREAM_POOL_SECTION

Linker section where CMSIS Stream memory pools are placed

Default value:
```C
#define CONFIG_CMSISSTREAM_POOL_SECTION ".bss.evt_pool"
```