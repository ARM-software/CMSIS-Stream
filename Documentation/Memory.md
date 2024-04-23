# Memory optimizations

## Buffers

Sometimes, a FIFO is in fact a buffer. In below graph, the source is writing 5 samples and the sink is reading 5 samples.

![buffer](assets/buffer.png)

The scheduling will obviously be something like:

`Source, Sink, Source, Sink ...`

In this case, the FIFO is used as a simple buffer. The read and the write are always taking place from the start of the buffer.

The schedule generator will detect FIFOs that are used as buffer and the FIFO implementation will be replaced by buffers : the third argument of the template (`isArray`) is set to one:

```C++
FIFO<float32_t,FIFOSIZE0,1,0> fifo0(buf1);
```

## Buffer sharing

When several FIFOs are used as buffers then it may be possible to share the underlying memory for all of those buffers. This optimization is enabled by setting `memoryOptimization` to `true` in the configuration object:

```python
conf.memoryOptimization=True
```

The optimization depends on how the graph has been scheduled.

With the following graph there is a possibility for buffer sharing:

![memory](assets/memory.png)

Without `memoryOptimization`, the FIFO are consuming 60 bytes (4*5 * 3 FIFOs). With `memoryOptimization`, only 40 bytes are needed.

You cannot share memory for the input / output of a node since a node needs both to read and write for its execution. This imposes some constraints on the graph.

The constraints are internally represented by a different graph that represents when buffers are live at the same time : the interference graph. The input / output buffers of a node are live at the same time (except for `Duplicate` node where the constraint is relaxed). Graph coloring is used to identify, from this graph of interferences, when memory for buffers can be shared. 

The interference graph is highly depend on how the compute graph is scheduled : a buffer is live when a write has taken place but no read has yet read the full content.

For the above compute graph and its computed schedule, the interference graph would be:

![inter](assets/inter.png)



Adjacent vertices in the graph should use different colors. A coloring of this graph is equivalent to assigning memory areas. Graph coloring of the previous interference graph is giving the following buffer sharing:

![fifos](assets/fifos.png)

The dimension of the buffer is the maximum for all the edges using this buffers.

In the C++ code it is represented as:

```C++
#define BUFFERSIZE0 20
CG_BEFORE_BUFFER
uint8_t buf0[BUFFERSIZE0]={0};
```

`uint8_t` is used (instead of the `float32_t` of this example) because different edges of the graph may use different datatypes.

It is really important that you use the macro `CG_BEFORE_BUFFER` to align this buffer so that the alignment is coherent with the datatype used on all the FIFOs.

Note that liveliness analysis is currently very simple since it generates only an interval. But the activity of a FIFO should be a list of interval.  With a list of interval more buffer sharing would be possible (but it is not currently implemented).

## Duplicate node optimizations

This optimization is enabled when `memoryOptimization` is enabled. It is experimental and can be disabled with `disableDuplicateOptimization`.

Generally, the input and output FIFOs of a node are active at the same time and thus cannot share a buffer.

For the `Duplicate` node, this constraint is relaxed and in some case the output FIFOs are not considered active at the same time as the input one. It enables the input and outputs to share buffers.

When FIFOs are sharing buffers, they are not generated in the C++ initialization of the `Duplicate` node so that copy will occur only for a subset of the FIFOs in the `Duplicate` node.



## Very big graph example

The example below is obviously unrealistic for an embedded device. It is too big. But it is a good way to illustrate what the memory optimization can achieve.

There are 511 FIFOs in this graph. They use 261632 bytes of memory. But the graph has lot of parallelism. By following the computed schedule, it may be possible to reuse the FIFO buffers between different branches of the graph. 

When the memory optimization feature is enabled, the system is only using 16 buffers for a memory consumption of 8192 bytes.

**We thus go from 261632 bytes to just 8192 bytes for this graph.**

![big_graph](assets/big_graph.png)