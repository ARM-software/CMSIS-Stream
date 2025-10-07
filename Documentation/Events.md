# Graph of events

The inputs and outputs of the CMSIS Stream nodes can be of two kind:

- Data flow
- Events

Data flow is the original CMSIS Stream concept based upon streaming of samples between nodes.

Events are a new feature : you can have both a dataflow graph and an event graph mixed.

You can even have a graph that only generates and consumes events.

An event is transmitted synchronously or asynchronously from one event output to several event inputs.

An event input can receive events from several outputs.

An event is like a function call:

- It has a name
- It has some arguments

The function call is executed by the node when it receives the event and if it has an event handler for this kind of event.

Refer to the `events` example in `Examples` folder to see a full example with:
- Data flow
- Events
- Multi-threading
- Remote procedure call (client / server)
- Custom memory allocators

# Working with events

## Python description

Event inputs and outputs are defined in a similar way to the data flow inputs and outputs. The differences are:

- They are not named but numbered
- They have not a "number of samples" property
- They have no type

```python
self.addEventInput()
```

This functions adds a new event input and increments the event number. The initial event input has number 0.

```python
self.addEventInput(4)
```

This function adds 4 new event inputs starting from the current event number.

Event outputs are defined in a similar way.

```python
self.addEventOutput()
```

```python
self.addEventOutput(4)
```

## C++ definition

For each event output that you have defined in Python, you need to define a member variable of type `EventOutput`. This object tracks the nodes connected to the ouput.

```C++
protected:
   EventOutput ev0;
```

When a node is connected to the output, the function `subscribe` will be called. Your node must provides an implementation.

The function receives:

- The output port from which you can select the right `EventOutput` object
- The destination object
- The destination event input

```C++
void subscribe(int outputPort,StreamNode &dst,int dstPort)
{
    if (outputPort == 0)
    {
        ev0.subscribe(dst,dstPort);
    }
}
```
## Receiving and sending events in C++

### Receiving events 

When an event is received, the function `processEvent` is used. It is defined in the CMSIS-Stream root class `StreamNode` and it must be overriden in your implementation.

```C++
 void processEvent(int dstPort,const Event &evt) final
```

Assume you want have an handler in your class:

```C++
void myHandler(float a,int32_t b);
```

You want to use it to process an event.

The simplest way is to rely on the template functions provided by the `StreamNode.h` file and write something like that in your `processEvent` function:

```C++
if (evt.wellFormed<float,int32_t>())
{
    evt.apply<float,int32_t>(&MyClass::myHandler, *this);
}
```

Note that an `int32_t` argument in those functions will also take into account `int16_t` and `int8_t` and introduce casts to `int32_t`.

Same with `float` that can receive an `int32_t`, `int16_t` or `int8_t` in the event.

The `apply` functions takes a pointer to a member function and the current object `*this`.

Note that the function `apply` is using `std::get<T>` from `std::variant` and will throw (if exceptions are enabled) if the type can't be found in the event. `wellFormed` should always be used before trying to use `apply`.

### Sending events

If you send an event from the `processEvent` function you can send it in a synchronous way. It will be executed immediately by another `processEvent` function in some other nodes.

It you publish the event from anywhere else you need to publish it in an asynchronous way: the events is pushed to an event queue with some priority. You'll have to provide an implementation of this event queue.

For instance, if the events are published from the `run` function of the data flow nodes, then they must be sent in an asynchronous way.

To send an event, you use the API of the `EventOutput` object.

The second argument is the event ID. Event ID are extensible using selectors (see below).

For instance:

```C++
ev0.sendSync(kNormalPriority,kDo);
```

or

```C++
res = ev0.sendAsync(kNormalPriority,kDo);
```

`res` is false is there was a queue overflow when trying to send the event in an asynchronous mode.

You can also use several arguments:

```C++
ev0.sendAsync(kNormalPriority,kValue,uint32_t(1),float(1.0),int32_t(-4));
```

It is better to give the type of constant values to avoid any ambiguity.

It is also possible to send an event to the application. 

```C++
EventOutput::sendSyncToApp(kNormalPriority,kDo)
```

or

```C++
EventOutput::sendAsyncToApp(kNormalPriority,kDo)
```

This is a static function of the class `EventOutput` because it is not related to any specific output. The application is virtually connected to all nodes.

You need to install an application event handler to process those events. This handler must be installed in the `EventQueue`. See the section about the `EventQueue`.

## Selectors

Without selectors, a node can decide what to do based upon:

- The predefined event IDs
- The event input that received the event
- The datatype of the value

It is already flexible but it should also be
possible for a node to define new kind of events.

A selector is a name for an event that is mapped to a unique number used at runtime to receive and send those events.

The Python description of a node lists the events that can be received or sent by the node by giving their names as strings.

The python scheduling is generating unique identification number for those event names.

Those unique numbers are used to initialize a class member variables in the C++ class implementing the node.

The C++ for the node is using those global
numbers to communicate with other nodes.


### Defining selectors in the Python

You can use the argument `selectors` to pass the name of the events you want to use in your node (receiving or sending).

```python
GenericNode.__init__(self,name,selectors=["reset","increment"])
```

The events have a local ID : the position in the list.
`reset` will have local ID 0 and `increment` will have local ID 1.

### Initialization of selectors in C++

In the generated scheduler code, a class member is initialized with the custom global selectors ID.

You should define a public class variable like:

```C++
static std::array<uint16_t,N> selectors;
```

where `N` is the number of selectors you can receive or send.

The class variable is initialized like:

```C++
template<>
std::array<uint16_t,2> ProcessingNode<float,7,float,7>::selectors = {SEL_RESET_ID, SEL_INCREMENT_ID};
```

`SEL_RESET_ID` and `SEL_INCREMENT_ID` are global unique identifiers. They are unique for a given graph and are computed by the Python script that is computing the graph scheduling.

Internally, in your node implementation you can use the local ID : position in the `selectors` array.

It is also a good idea to define an enum in your class to more easily track the events using their local IDs:

```C++
enum selector {selReset=0,
               selIncrement=1};
```

### Use of selectors in C++

To use the selectors, you should always use their global ID.

If you want to test if you have received the message `increment`, you should compare the received global ID with `selectors[selIncrement]`.

If you want to send the event `increment`, you should use the global ID `selectors[selIncrement]`

```C++
ev0.sendAsync(kNormalPriority,selectors[selIncrement]);
```

In below code, an event taking 3 arguments is built and sent.

```C++
ev0.sendAsync(kNormalPriority,selectors[selComplexEvent],1,1.0,-4);
```

If you have 0 or 1 arguments, a normal event will be created.

If you have more than 1 arguments, a list event will be created.

Note that NO CHECK is done on the number of arguments or their types.

If you sent an event with the wrong number of arguments or types it will likely be ignored in the destination node.

The `processEvent` functions should check that the event received if well formed and ignore it if it is not the case.

## Event datatype

The event contains:

- An event ID
- A priority
- The data

```C++
uint32_t event_id;
uint32_t priority;
EventData data;
```

Some events are standardized:

```C++
 enum cg_standard_event
    {
        kNoEvent = 0,
        kDo = 1,
        kPause = 2,
        kResume = 3,
        kGetParam = 5,
        kValue = 6,
        kStopGraph = 7,
        kDebug = 8,
        kStartNodeInitialization = 9,     
        kNodeWasInitialized = 10,    
        kNodeWillBeDestroyed = 11,
        kNodeReadyToBeDestroyed = 12, 
        kError = 13,
        kLongRun = 14,     
        // Node selectors are starting at 100
        kSelector = 100
    };
```

Starting from ID 100, you can use custom events using the selectors (see below).
The first selector ID is 100. Values before 100 are reserved for CMSIS-Stream.

- `kDo` : Do a default action that is node dependent
- `kPause` : Pause the dataflow. The nodes stop their computations but still updates the FIFO and write output data. In case of memory optimization, the output buffer can be shared between different nodes that's why a node must always write something into the FIFO. So, the pause here is a pause of the computation but not of the flow of data
- `kResume` : Resume the computation
- `kGetParam` : Get the value of a parameter. This value will be sent as an output event on the default event output (port 0 that must be present)
- `kValue` : Send a value to an event input
- `kStopGraph` : Stop the data flow graph
- `kDebug` : Any debug action that may be implemented by the node

In generate, nodes are initialized in their C++ constructors.
Some nodes may require more complex initializations that could fail and should do those initializations with `init` method if possible.
When the initializations may take too much time, it is possible to use events to do an asynchronous initialization with some nodes.

Some events have been defined to handle this:

- `kStartNodeInitialization`:

  - Ask a node to start a complex initialization in asynchronous way

- `kNodeWasInitialized`:
  - Event sent by a node when its initialization is finished. The event should contain one value : the node ID as an `uint32_t`. If an error occurred,
  the 'kError' event should be sent with the node ID as `uint32_t` and an error code as an `int32_t`.
- `kNodeWillBeDestroyed`:
  - If a node was initialized with the previous process, the C++ destructor cannot be called before the node has had the chance to undo what has been done during the initialization. This event signals a node that its C++ destructor is going to be used
- `kNodeReadyToBeDestroyed`:
  - When the node is ready to be destroyed and the C++ destructor can be called, it should signal it.

The application can check if a node needs an asynchronous initialization using the API `bool needsAsynchronousInit()`.

- `kLongRun`:
   This event is used by a node when the node needs a thread to run a long (or forever) task. This event should be sent by the node to itself in an asynchronous way. Like that, the event handler will be executed on the thread pool managing the event queue. A long running task should use the event queue method `mustEnd` to know if the tasks must end.

**A node is free to ignore any events.**

### Event values

An event is either an atomic value or a list of value (`ListValue`).

An atomic value is of type `cg_value`.

`cg_value` is basically a variant:

```C++
using cg_value_variant = std::variant<std::monostate,
    int8_t,
    int16_t,
    int32_t,
    int64_t,
    float,
    double,
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t,
    BufferPtr,
    std::string,
    cg_any_tensor,
    cg_any_const_tensor> value;
```

`BufferPtr` is an alias for `ProtectedBuffer<RawBuffer>`

`cg_any_tensor` and `cg_any_const_tensor` are two other variants for tensors.

For instance, `cg_any_tensor` contains `TensorPtr<float>`.

`TensorPtr<T>` is an alias for `ProtectedBuffer<Tensor<T>>`

Read the section about `ProtectedBuffer` to know how to allocate raw buffers and tensors.

### ProtectedBuffer

Buffers (raw buffer or tensor) can be shared between several events because we do not want to have to copy big buffers.

As consequence, those datatype must be accessed through a `std::shared_ptr` that will manage a reference count to know how many events are referencing the buffer.

In some platforms, the events can be managed in an asynchronous way using threads. As consequence, those buffers can be accessed from different thread and the access must be protected with mutexes.

It is also important to protect the ref count.

If you want to be able to do in-place modification on some big buffers you need to know if the buffer is shared or not. It is not possible to know the sharing status if the ref count is also not protected by a mutex.

`ProtectedBuffer` is used to provide a protected access to a buffer or tensor description and its reference count.

`ProtectedBuffer` protects a buffer description : `RawBuffer` or `Tensor<T>`. A buffer descriptor describes the data (length, shape) but does not contain the data. It contains a unique pointer to the data. The unique pointer is using the class `UniquePtr` provided by `StreamNodes.hpp`.

To create a protected buffer, you first must have some data. You should create a `UniquePtr` with a deleter if needed.

```C++
UniquePtr<float> ptr(10);
```

Once the content has been created, you can get a `ProtectedBuffer` describing this content:

```C++
 TensorPtr<float> t = TensorPtr<float>::create_with((uint8_t)1,cg_tensor_dims_t{10},std::move(ptr));
```

The arguments of the `create_with` are the arguments required to create a `Tensor<float>` object.

To access the content of the `TensorPtr` you need to use the `lock` and `lock_shared` APIs.

`lock_shared` is used when you need a read access. It takes a lambda as argument:

```C++
obj.lock_shared([](CG_MUTEX_ERROR_TYPE error, const Tensor<T> &t)
{
    ...
}
```

In the lambda you have access to an error code if an error occured while trying to lock the mutex.

In the lambda, you have access to the object but if an error occured you should not access it.

The error type is defined with the macro `CG_MUTEX_ERROR_TYPE` to provide portability.

You should not raise exceptions in the lambda.

Note that the lambda is called only if there is an object in the `ProtectedBuffer`. If the object pointer is null because of previous errors, the lambda is not called.

If you want a write access. you use:

```C++
t.lock([](CG_MUTEX_ERROR_TYPE error,bool isShared, Tensor<float> &tensor)
{
    ...
}
```

There is an additional argument to the lambda : `bool isShared`.

It is true if the buffer is shared : if there are several references to the buffer.

The events are passed by rvalue reference to the handler (`processEvent` in a node or the application handler).
The reason is related to the ref count : using `const Event&` would introduce an untracked sharing that would not be reflected in the ref count. 

Events only need to be copied when they are sent to many nodes. It is done by the `EventOutput` object. In the application code, events don't have generally a reason to be copied.

You may need to __copy__ the event if you need to pass it to a function for read only processing (like displaying on stdout with iostream).

__Never pass the event by reference or pointer.__

## Sharing buffers between processes 

When some nodes in the graph are communicating with different processes, it is possible (when supported by the platform) to avoid copying buffers and use shared memory instead.

The `TensorPtr` and `RawBuffer` should use a `SharedBuffer` instead of a `UniquePtr`.

The `SharedBuffer` is a unique reference to a shared buffer.

It contains a platform dependent descriptor, a `global_id` that is used to identify the buffer, the size of the buffer in bytes and a pointer to the data.

The pointer to the data may be a `nullptr` if the buffer has not been mapped in the CMSIS-Stream process. As consequence, a CMSIS Stream node do not always have to map the buffer if it is just controlling an hardware accelerator.

For more details about how to integrate this feature in your platform, see the section IPC.


# Integrate the events in your application

## Event queue processing

### Event queue

The events should not disrupt the data flow of CMSIS Stream.

So they should be executed asynchronously from the data flow.

Events are put onto a queue and the queue is processed separately from the data flow.

When events are created, you can use a priority. Depending on the queue implementation, those priorities may be used or ignored.

```C
enum cg_event_priority
    {
        kLowPriority = 0,
        kNormalPriority = 1,
        kHighPriority = 2
    };
```

This priority has nothing to do with thread priorities. The queue implementation that you select may use those priorities so that events with highest priorities are handled first. But events of higher priority will not preempt the events of lower priority unless they are executed on threads of different priorities.

With an OS, the queue can be processed with a thread or a thread pool.

The event queue interface is declared in `StreamNode.h`. You should define an implementation.

If you use the python `generateEventSystemExample(".",posix=True)` you will get a default implementation of the queue using C++ standard libraries.
With `posix=False` you'll get an example `CMSIS RTOS2 API` implementation.

```C++
class EventQueue
{
public:
    using AppHandler = bool (*)(void *data, const Event &evt);

    inline static EventQueue *cg_eventQueue = nullptr;

    EventQueue() {};
    virtual ~EventQueue() {};

    virtual bool push(Message &&message) = 0;
    virtual bool isEmpty() = 0;
    virtual void clear() = 0;

    virtual void execute() = 0;

    bool callHandler(const arm_cmsis_stream::Event &evt);

    void setHandler(void *data, AppHandler handler);

};
```

The queue implementation should be thread safe.

The `push` may be called from `execute` so the implementation should be careful not to create any deadlock.

The event queue is a static variable of the class `EventQueue` and is the same for all graphs that you may use in the application.

You can also install an event handler in the `EventQueue` for application events.

### Event queue with threads

The queue execution can rely on threads and thread pool.

If you don't use heap allocation for the data flow scheduler, the nodes are destroyed on exit from the scheduler function.

In this case, you should flush the queue before returning from the scheduler function:

```C++
#define CG_AFTER_SCHEDULE       \
  if (::cg_eventQueue) {        \
      ::cg_eventQueue->clear(); \
  }
```

If the events are not flushed, they may act on a node that has been destroyed since they are processed on a different thread from the data flow thread.

**The queue API should be thread safe.**

Using the python `generateExamplePosixMain(".")` you can generate an example implementation.

#### Macros

For portability between different platforms, the thread and mutex implementation is provided with macros that can be defined in your `custom.hpp` file.

With C++ thread and mutexes, you could define:

```C++
#define CG_MUTEX std::shared_mutex
#define CG_INIT_MUTEX(MUTEX)
#define CG_MUTEX_DELETE(MUTEX, ERROR)
#define CG_MUTEX_ERROR_TYPE int
#define CG_MUTEX_HAS_ERROR(ERROR) (ERROR != 0)
#define CG_ENTER_CRITICAL_SECTION(MUTEX, ERROR) \
  {                                             \
    std::unique_lock lock((MUTEX));             \
    ERROR = 0;

#define CG_EXIT_CRITICAL_SECTION(MUTEX, ERROR) \
  ERROR = 0;                                   \
  }

#define CG_ENTER_READ_CRITICAL_SECTION(MUTEX, ERROR) \
  {                                                  \
    std::shared_lock lock((MUTEX));                  \
    ERROR = 0;

#define CG_EXIT_READ_CRITICAL_SECTION(MUTEX, ERROR) \
  ERROR = 0;                                        \
  }
```

The previous implementation assume that an error is fatal : it will throw. So the ERROR variable is just used to have a macro API similar the one for RTOS systems.

With CMSIS RTOS2 you could define:

```C++
class CMSISMutex
{
public:
    CMSISMutex()
    {
        mutex_id = osMutexNew(NULL);
    }

    ~CMSISMutex()
    {
        if (mutex_id != nullptr)
        {
            // Ensure the mutex is deleted only if it was created successfully
            osMutexDelete(mutex_id);
        }
    }

    osMutexId_t id() const
    {
        return mutex_id;
    }

protected:
    osMutexId_t mutex_id;
};

class CMSISLock
{
public:
    CMSISLock(CMSISMutex &mutex) : mutex(mutex)
    {
    }

    osStatus_t acquire()
    {
        error = osMutexAcquire(mutex.id(), 0);
        return error;
    }

    ~CMSISLock()
    {
        if (error == osOK)
        {
            error = osMutexRelease(mutex.id());
        }
    }

    osStatus_t getError() const
    {
        return error;
    }

protected:
    CMSISMutex &mutex;
    osStatus_t error;
};

#define CG_MUTEX CMSISMutex
#define CG_MUTEX_ERROR_TYPE osStatus_t

#define CG_MUTEX_HAS_ERROR(ERROR) (ERROR != osOK)

#define CG_ENTER_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                           \
        CMSISLock lock((MUTEX));                \
        ERROR = lock.acquire();

#define CG_EXIT_CRITICAL_SECTION(MUTEX, ERROR) \
    }

#define CG_ENTER_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                                \
        CMSISLock lock((MUTEX));                     \
        ERROR = lock.acquire();

#define CG_EXIT_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    }

```

The mutex must be a C++ class because it is created and initialized through `std::shared_ptr::allocate_shared`.


### Event queue with bare metal

In case of a bare metal systems, there are other possibilities without threads.

You could use CMSIS Stream macro `CG_BEFORE_NODE_EXECUTION` to interleave event execution with the data flow execution.

```C++
#define CG_BEFORE_NODE_EXECUTION(id) \
  ::cg_eventQueue->execute();
```

But if the event processing takes too long it may disrupt the data flow.

Another possibility would be to use the CMSIS-Stream scheduling in callback mode. The callback could be called from an ISR and the main application would just execute events from the queue or sleep when no events are available.

## Memory allocators

It is possible to customize memory allocators to ensure that no uncontrolled memory allocation is occuring.

Memory allocations are occuring in 4 cases:

- Allocation of a list of values (when several arguments are used for an event)
- Description for raw buffers and tensors
- Mutexes
- Content of raw buffer, tensor or std::string

### List of values

The maximum number of elements in a list is defined by the macro `CG_MAX_VALUES` that can be redefined. Its default value is 8. 

A `ListValue` always contains `CG_MAX_VALUES` `cg_value`. Its size if always the same even if less than `CG_MAX_VALUES` values are used.

Memory allocation of list is controlled with `CG_MK_LIST_EVENT_ALLOCATOR`.

A memory pool could be used since the size of `ListValue` is fixed.


Possible definitions:

- Default : `std::allocator<T> {}`
- Memory pool with C++ : `#define CG_MK_LIST_EVENT_ALLOCATOR(T) (std::pmr::polymorphic_allocator<T>(&pool))
`

where `pool` must be defined in one of the `.cpp` files and could be : 

`std::pmr::synchronized_pool_resource pool;`

- CMSIS-RTOS2 : `#define CG_MK_LIST_EVENT_ALLOCATOR(T) (CMSISEventPoolAllocator<T>{})`

where `CMSISEventPoolAllocator` is a __stateless__ memory allocator using a CMSIS RTOS memory pool.

See the implementation of `CMSISEventPoolAllocator` below in this document.

With CMSIS RTOS2, the memory pool can be created with:

```C++
cg_eventPool = osMemoryPoolNew(NB_MAX_EVENTS, sizeof(ListValue) + 16, NULL);
```

It is bigger than `ListValue` because the allocation is using a `std::shared_ptr` and the allocator is also allocating the control block for the `std::shared_ptr`.

It is difficult to know in a portable way what is the size of this control block. The easiest is to add a print in the custom memory allocator to measure the size of an allocation and use this size for the blocks of the memory pool.

On a Cortex-M55, it was measured to be `16` bytes so the memory pool block size is `sizeof(ListValue) + 16`.

### Description for buffers and tensors

Buffer and tensors are using a `ProtectedBuffer`. This protected buffer is allocating a `std::shared_ptr` to the underlying object descriptor.

The underlying object is describing the dimensions of the buffer and tensors and contains a pointer to the data. It does not contain the data. As consequence, the size of this descriptor is known and fixed.

`Tensor` is bigger than `Buffer` and should be used to dimension the blocks of the memory pool if a memory pool is used.

The macro used is `CG_MK_PROTECTED_BUF_ALLOCATOR` and it can be used like in the previous example.

In case of a stateless memory allocator, a different class must be used. `CMSISEventPoolAllocator` cannot be reused since a different memory pool with a different block size must be used.

### Mutexes

`ProtectedBuffer` are allocating a `std::shared_ptr` to a mutex. It is used to ensure that the `std::shared_ptr` to the data is protected (including the ref count).

Those memory blocks are much smaller and use a different memory allocator : `CG_MK_PROTECTED_MUTEX_ALLOCATOR`.

### Content of buffer, tensor and strings

Contrary to the prevous datastructure where the size of the data is always the same, the content of buffers is variable.

The content of data can be allocated on a object per object basis.

`std::string` can be created using an allocator and different `std::string` may use a different allocator.

Buffer and tensors use a `UniquePtr` that has unique ownership of the pointed data. The `UniquePtr` can be created with a stateless deleter and so can use any allocation strategy that make sense for the content.

By default, a `UniquePtr` created with a number of bytes or a `T*` will use the normal `std::free`.

If a `const T*` is used, no deleter is used by default.

### Memory allocation conclusion

With the use of the three custom memory allocators, it is possible to only use memory pools for the event system : so no fragmentation and deterministic allocations.

For the buffer / tensor content it depends on your application and how those objects are created.

## Use of several graphs


### Reuse of selector IDs

There are some difficulties if you want to build several graphs using the same kind of nodes in your application.

First, you may want to have the same selector IDs for the selectors they have in common.

You can reuse the selectors ID of a previous graph to compute a new one.

Just set the old selectors IDs in the configuration before computing the new scheduling:

```python
newconf = Configuration()
# Reuse selectors ID definitions from previous graph
newconf.selectorsID = scheduling.selectorsID
```

Then you compute the scheduling for the new graph using those IDs:

```python
scheduling = the_graph.computeSchedule(config=newconf)
```

### Avoid redundant initializations

Selectors are initialized at class level.

If you reuse the same kind of node in different graphs in the application, you want the initialization to occur only once.

The `.cpp` generated file contains class initializations for the selectors. If several graphs use `ProcessingNode<float,7,float,7>` and you don't make anything special then the initialization of the selectors field will occur for each graph and you'll get a linker error.

To avoid this problem, you need to export the list of initializations that occured in one graph and use this to generate the scheduling for another graph.

To export the initialization that occured in your graph, you can do:

```python
scheduling.genJsonSelectorsInit(".",conf)
```

Don't confuse wtih `genJsonSelectors` that is another json files to be used to send events to the graph from an outside tool.

Then, when you compute the schedule for another graph, you can use several json files:

```python
scheduling = the_graph.computeSchedule(config=newconf,oldSelectorsInit=["scheduler_selectors_inits.json"])
```

The initializations that occured in previous graph constructions will not occur again.

**NOTE**: It is very important in this case to use the same selectors IDs for the same selector in both graphs.

Because the initialization made in one graph must also be valid for the other ones. So, you must ensure that the selectors ID of the previous graph are reused for the current graph.

## JSON identification

You may want to address the nodes by name instead of ID.

```python
scheduling.genJsonIdentification(".",conf)
```

This exports a json file mapping the name of a node to its ID in a given graph. It can be used by external tools.

If you are in the C/C++ world you can just use the `#define` generated in the header of the scheduler.

You may also want to use a name for a selector rather than the global ID:

```python
scheduling.genJsonSelectors(".",conf)
```

This generates a JSON file mapping json selector names to their ID for a given graph.

## IPC 

Sharing buffers without copy between processes relies on several APIs that must be implemented for your platform. 

### MemServer 

Shared buffers are managed by a server. Communication with this server is done through the `MemServer` API. You must provide an implementation of this API (and of the corresponding server).

The server is tracking access to the shared buffers : reference counting and locking.

All CMSIS Stream nodes must use the `MemServer` to access shared buffers. Having a global view of all the shared buffers used by all the processes involved in a CMSIS Stream graph is required for the nodes to know if in-place modification of a buffer is possible or not.

`MemServer` APIs return a `Descriptor`. This class is used to identify a shared buffer on client side. It is the only way to use a shared buffer in CMSIS Stream nodes : you first get a `Descriptor` from the `MemServer` and then you create a `SharedBuffer` object from the `Descriptor` object.

### Descriptor

You must provide an implementation of `Descriptor`. 
`Descriptor` is used to map / unmap a buffer or close the corresponding `NativeHandle`.

`Descriptor` must not be used directly but only though a `SharedBuffer` that is the unique owner of a `Descriptor`.

`Descriptor` is using the `NativeHandle` datatype.

`NativeHandle` is the datatype used to represent a shared buffer on your platform. It is `int` on Unix like systems (file descriptor) and it is `void *` on Windows.

### IPC

`IPC` is the API that must be implemented to exchange events with another process.
The `SharedBuffer` are represented by a `global_id` and only the `global_id` is transmitted by the `IPC`.
The receiving process uses the `global_id` and the `MemServer` to get a `Descriptor` for this memory buffer.

`IPC` implementation is not transmitting any file descriptor.

If you're not using `SharedBuffer` but `UniquePtr`, then the buffer will be serialized before being transmitted.

Serialization / Unserialization of events is done with the class in `cg_pack.hpp`.

Not that those classes can also be used to send the event on the network to a machine using the same endianism.
In case of transmission to the network, `SharedBuffers` are ignored and you must specify to the `pack` function that you are packing for network by setting the network argument to `true`.


# Details

## CMSISEventPoolAllocator

Here is an example of a C++ __stateless__ memory allocator using a CMSIS RTOS memory pool.

In case of a memory pool, we cannot allocate block of varying sizes.

As consequence, the `allocator` method has a test:
```C++
if ((n == 1) && (sizeof(T) <= osMemoryPoolGetBlockSize(cg_eventPool)))
```

`n` is the number of samples of type `T`. In our case, we always allocate either one `ListValue`, or one `TensorPtr<T>` or one mutex. So `n` should always be equal to 1.

Then, we should not request more than what can be contained in a block of the memory pool.

In practice, the condition should always be true because this memory allocator is not used to make more general memory allocations.


```C++
template <typename T>
class CMSISEventPoolAllocator
{
public:
    using value_type = T;
    using is_always_equal = std::true_type;

    CMSISEventPoolAllocator() noexcept = default;

    template <typename U>
    CMSISEventPoolAllocator(const CMSISEventPoolAllocator<U> &) noexcept {}

    template <typename U>
    struct rebind
    {
        using other = CMSISEventPoolAllocator<U>;
    };

    T *allocate(std::size_t n)
    {
        if ((n == 1) && (sizeof(T) <= osMemoryPoolGetBlockSize(cg_eventPool)))
        {
            //printf("Alloc event %d\n",sizeof(T));
            return static_cast<T *>(osMemoryPoolAlloc(cg_eventPool,0));
        }
        else
        {
           return nullptr;
        }
    };

    void deallocate(T *p, std::size_t n) noexcept
    {
        osMemoryPoolFree(cg_eventPool,static_cast<void*>(p));
    };

};

template <typename T, typename U>
bool operator==(const CMSISEventPoolAllocator<T> &, const CMSISEventPoolAllocator<U> &) { return true; }

template <typename T, typename U>
bool operator!=(const CMSISEventPoolAllocator<T> &, const CMSISEventPoolAllocator<U> &) { return false; }

```

## Serialization / Unserialization

The `cg_pack.hpp` file, generated with other default files, can be used to serialize / unserialize events and transmit them on a network.

To serialize an event you can do:

```C++
Pack packer;
    
packer.pack(nodeid, evt);

std::vector<uint8_t> d = packer.vector();
```

You need a destination nodeid for this event.

To unpack an event you can do:
```C++
void handleMessage(const std::vector<uint8_t> &message)
{
  uint32_t node_id;

  Unpack unpack(message.data(), message.size());

  Event evt = unpack.unpack(node_id);
...
```
