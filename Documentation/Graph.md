# Adding nodes to the graph

## Creating a connection

Those methods must be applied to a graph object created with `Graph()`. The `Graph` class is defined inside `cmsis_stream.cg.scheduler` from the cmsis-stream Python package.

```python
def connect(self,input_io,output_io,fifoClass=None,fifoScale = 1.0,fifoAsyncLength=0,buffer=None,customBufferMustBeArray=True):
```

Typically this method is used as:

```python
the_graph = Graph()

# Connect the source output to the processing node input and add this directed
# edge to the object the_graph
the_graph.connect(src.o,processing.i)
```

There are few optional arguments for the `connect` function:

* `fifoClass` : To use a different C++ class for implementing the connection between the two IOs. (it is also possible to change the FIFO class globally by setting an option on the graph. See below). The `FIFO` class is provided by default. Any new implementation must inherit from `FIFObase<T>`.  This must be a Python class name. This class is used to specify the name of the FIFO in the C++ and the argument to pass to the constructor (see below). Standard nodes may have to be modified to use a custom FIFO if the FIFOBase API must be changed.
* `fifoScale` : In asynchronous mode (deprecated), it is a scaling factor to increase the length of the FIFO compared to what has been computed by the synchronous approximation. This setting can also be set globally using the scheduler options. `fifoScale` is overriding the global setting. It must be a `float` (not an `int`).
* `fifoAsyncLength` : In fully asynchronous mode. It is the size to use for the FIFO
* `buffer`: Custom memory buffer to use for this FIFO. When a custom buffer is specified, the FIFO is no more participating to the memory optimization algorithm.
* `customBufferMustBeArray`: True if the custom buffer must be used only as an array

```python
def connectWithDelay(self,input_io,output_io,delay,fifoClass=None,fifoScale=1.0,fifoAsyncLength=0,buffer=None,customBufferMustBeArray=True):  
```

The only difference with the previous function is the `delay` argument. It could be used like:

```python
the_graph.connect(src.o,processing.i, 10)
```

The `delay` is the number of samples contained in the FIFO at start (initialized to zero). The FIFO length (computed by the scheduling) is generally bigger by this amount of sample. The result is that it is delaying the output by `delay` samples.

It is generally useful when the graph has some loops to make it schedulable.

## Options for the graph

Those options needs to be used on the graph object created with `Graph()`.

For instance :

```python
g = Graph()
g.defaultFIFOClass = StreamFIFO
```

### defaultFIFOClass (default = "StreamFIFO")

Class used for FIFO by default. Can also be customized for each connection (`connect` of `connectWithDelay` call). This must be a Python class name. This class is used to specify the name of the FIFO in the C++ and the argument to pass to the constructor (see below).

Standard nodes may have to be modified to use a custom FIFO if the FIFOBase API must be changed.

### duplicateNodeClassName(default="Duplicate")

Prefix used to generate the duplicate node classes like `Duplicate2`, `Duplicate3` ...

Those nodes are inserted automatically to implement one-to-many connections.

If you need to connect an output to more than 3 nodes, you'll have to create the `Duplicate` nodes.

## The FIFO class description

Option for custom FIFOs require use of a Python class with following constructor:

```Python
def __init__(self,the_type,length):
```

The following API must be provided in the class

```python
@property
def cname(self):
    
@property
def args(self):
```

`cname` is the C++ class name

`args` are the additional arguments for the constructor (following the buffer argument). By default, this is an empty list. Otherwise, it is a list of C arguments.

If the API provided by the FIFO must be different from the one provided by `FIFOBase<T>` then standard nodes will have to be modified. In general, the easiest way is to just modify `GenericNodes.h` in your project and extend the `FIFOBase<T>` API.

