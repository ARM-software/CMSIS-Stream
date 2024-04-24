# Integration in a system

CMSIS-Stream graph scheduling is a sequence of function calls organized thanks to the dataflow dependencies. There is no concept of time inside the graph.

But when the graph is integrated into a system, it must be synchronized with peripherals (sensors, audio ...)

There are two cases to consider.

## RTOS / Interrupt

In this case, there is possibility to sleep the CMSIS-Stream scheduler and wake-it up in reaction to an event.

With an RTOS, CMSIS-Stream scheduler would be in a thread and  would be waiting on a semaphore, a queue etc ... Generally the synchronization is responsibility of the sink and source nodes.

With an interrupt based system with no RTOS, the CMSIS-Stream loop would sleep and wait for an interrupt.

## Callback

Some systems are designed with callbacks : when an event occurs the callback is executed. The event could be a new audio of video frame.

In this case, CMSIS-Stream must work in `switchCase` mode so that there is a state machine. With the `callback` mode, here is a mechanism to pause and restore the CMSIS-Stream state machine.

`callback` mode will also enable the `heapAllocation` mode since the FIFO and Nodes must be allocated outside of the scheduler loop because this loop is now a callback and will be called several time.

See the example `callback`.

In a node, the execution can be paused by returning `CG_PAUSE_SCHEDULER`.

Note that function like `getReadBuffer`, `getWriteBuffer` must not be called several times for a paused execution of the node. **So when the node is executed you must be able to distinguish if this is a normal execution or if the node is being resumed.** 

If you define additional state using macro like `CG_BEFORE_SCHEDULE` then this state can be saved / restored using macros `CG_RESTORE_STATE_MACHINE_STATE` and `CG_SAVE_STATE_MACHINE_STATE`.

You may need to define additional arguments of the scheduler function to save this additional state.

In asynchronous mode, the function `prepareForRunning` can also pause the execution. 

When a node execution is restarted, both `prepareForRunning` and `run` functions are executed in asynchronous mode.

It is possible to test if the node is being resume or starting a new execution with the function  `executionStatus`.

The status is of type `kCBStatus`:

```C
enum kCBStatus {
   kNewExecution = 1,
   kResumedExecution = 2
};
```

