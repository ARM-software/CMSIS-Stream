# Configuration {#cmsis_stream_configuration}

Default configuration values can be modified when building.
You can redefine any of those values:

- By redefining them on the command line
- By redefining them in the copied `stream_runtime_config.hpp` file

## CMSISSTREAM_EVT_THREAD_STACK_SIZE

Stack size of event thread.

Default value:

```C
#define CMSISSTREAM_EVT_THREAD_STACK_SIZE 4096
```

## CMSISSTREAM_STREAM_THREAD_STACK_SIZE

Stack size of stream thread.

Default value:

```C
#define CMSISSTREAM_STREAM_THREAD_STACK_SIZE 4096
```

## CMSISSTREAM_NB_MAX_EVENTS

Maximum number of events expected in the system at a given time.

This is used to size the memory pools involved in event creation. Several event
queues will use the same pool so this number may be bigger than the event queue
length.

In current `stream_runtime_init` APIs, each application, even paused, has its
own queue but they all share the same pool.

Default value:

```C
#define CMSISSTREAM_NB_MAX_EVENTS 16
```

## CMSISSTREAM_NB_MAX_BUFS

Maximum number of buffers, tensors, and similar objects expected in the system
at a given time.

This is used to size the memory pool involved in atomic reference counting of
buffers. An event may hold references to several buffers so this number may be
bigger than the number of events.

Default value:

```C
#define CMSISSTREAM_NB_MAX_BUFS 16
```

## CMSISSTREAM_STREAM_THREAD_PRIORITY

Priority of the stream thread. It should be real-time and higher than any
priority used for events.

Default value:

```C
#define CMSISSTREAM_STREAM_THREAD_PRIORITY osPriorityRealtime
```

## CMSISSTREAM_EVT_HIGH_PRIORITY

Priority of the event thread when it is processing high priority event.

Default value:

```C
#define CMSISSTREAM_EVT_HIGH_PRIORITY osPriorityHigh
```

## CMSISSTREAM_EVT_NORMAL_PRIORITY

Priority of the event thread when it is processing normal priority event.

Default value:

```C
#define CMSISSTREAM_EVT_NORMAL_PRIORITY osPriorityNormal
```

## CMSISSTREAM_EVT_LOW_PRIORITY

Priority of the event thread when it is processing low priority event.

Default value:

```C
#define CMSISSTREAM_EVT_LOW_PRIORITY osPriorityLow
```

## CMSISSTREAM_SHARED_OVERHEAD

Additional memory required by `std::allocate_shared`.

When allocating an object of type `T` with `std::allocate_shared<T>`, enough
memory is needed for `T` and some additional memory required by the
implementation of `std::shared_ptr`. This parameter defines the additional
memory required.

Default value:

```C
#define CMSISSTREAM_SHARED_OVERHEAD 16
```

## CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS

Maximum number of arguments in an event.

It defines the maximum number of arguments that can be passed in an event. This
number impacts the size of memory pools used by the event system.

Default value:

```C
#define CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS 8
```

## CMSISSTREAM_TENSOR_MAX_DIMENSIONS

Maximum number of tensor dimensions.

It defines the maximum number of dimensions for tensors used in the
CMSIS-Stream library. The default value of 3 is suitable for image oriented
applications: height, width, channels.

Default value:

```C
#define CMSISSTREAM_TENSOR_MAX_DIMENSIONS 3
```

## CMSISSTREAM_EVENT_QUEUE_LENGTH

Length of an event queue.

This is the number of events that can be queued in an event queue. In case of
overflow of the queue, new events are dropped.

Default value:

```C
#define CMSISSTREAM_EVENT_QUEUE_LENGTH 20
```

## CMSISSTREAM_POOL_SECTION

Linker section where CMSIS-Stream memory pools are placed.

Default value:

```C
#define CMSISSTREAM_POOL_SECTION ".bss.evt_pool"
```
