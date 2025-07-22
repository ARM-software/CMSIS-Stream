# CMSIS-Stream

CMSIS-Stream is a Python package for source code generation and a set of C++ headers that can be used on embedded devices to process streams of samples and events.

With CMSIS-Stream you can describe a graph of processing elements exchanging data streams and events.

The processing of the data streams in this graph has:

* low memory usage
* minimal overhead
* deterministic scheduling
* modular design
* graphical representation

CMSIS-Stream computes a scheduling of this streaming graph at **build time** with several memory optimizations. The scheduling is a state machine : it is deterministic.

Stream of samples are processed by this graph as illustrated on the following animation:

![SDF_doc](Documentation/assets/SDF_doc.gif)

Processing nodes in the graph can also communicate with events. An event is like a function call : it has a name and arguments. Processing elements can send and receive events.

Python is used to:

* Describe the graph (streaming graph and event graph)
* Generate a **static scheduling** of the streaming graph that is computed **at build time** with several memory optimizations. 
* Generate the code for this scheduler as a simple C++ file (with a C API).
  * The scheduler can be run on bare metal devices. There is no dependencies to any RTOS. The scheduler is a sequence of function calls
* Connect all the nodes for events propagation in the graph of events

* Generate a graphical representation of the graph

For the streaming part, C++ is only used for strong and static typing (template). The only part of the C++ library that is used is the memory allocator to create the objects before the graph is started.

For the event graph (which is optional), more C++ is used. See the section of the documentation about the event graph.



## License Terms

CMSIS-Stream is licensed under [Apache License 2.0](LICENSE).

## Table of contents

1. ### How to get started

   1. [Simple graph creation example](Examples/simple/README.md)

2. ### How to write the Python script and the C++ wrappers

   1. [How to describe the graph in Python](Documentation/WritePython.md)
   2. [How to write the C++ wrappers to use your functions in the graph](Documentation/WriteCPP.md)
   3. [DSP Nodes for working with CMSIS-DSP](Examples/simpledsp/README.md)
   4. [Details about the generated C++ scheduler](Examples/example1/README.md)

3. ### [Examples](Examples/README.md)

4. ### API Details

   1. ### [Python API for creating a graph and its scheduling](Documentation/PythonAPI.md)

   2. ### [C++ default nodes for C++ wrappers](Documentation/CPPNodes.md)


5. ### [Memory optimizations](Documentation/Memory.md)

6. ### [Integration in a system](Documentation/Integration.md)

   1. #### [Node identification](Documentation/NodeIdent.md)

7. ### [Graph of events](Documentation/Events.md)

8. ### Extensions

   1. #### [Cyclo-static scheduling](Documentation/CycloStatic.md)

   2. #### [Dynamic / Asynchronous mode](Documentation/Async.md)

9. ### [Maths principles](Documentation/MATHS.md)

10. ### [FAQs](Documentation/FAQ.md)



