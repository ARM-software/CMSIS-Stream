# CMSIS-Stream

CMSIS-Stream is a Python package and small set of C++ headers that can be used on embedded devices to process streams of samples with :

* low memory usage
* minimal overhead
* deterministic scheduling
* modular design
* graphical representation

CMSIS-Stream makes it easier to build streaming solutions by connecting components into a graph and computing a scheduling of this graph at **build time** with several memory optimizations.

Stream of samples are processed by this graph as illustrated on the following animation:

![SDF_doc](Documentation/assets/SDF_doc.gif)

Python is used to:

* Describe the graph
* Generate a static scheduling of this graph that is computed at build time with several memory optimizations. 
* Generate the code for this scheduler as a simple C++ file (with a C API).
  * The scheduler can be run on bare metal devices. There is no dependencies to any RTOS. The scheduler is a sequence of function calls

* Generate a graphical representation of the graph

C++ is only used for strong types and static typing (template). The only part of the C++ library that is used is the memory allocator to create the objects.

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

   3. ### [Python default nodes for Python wrappers](Documentation/PythonNodes.md)

5. ### [Memory optimizations](Documentation/Memory.md)

6. ### Extensions

   1. #### [Cyclo-static scheduling](Documentation/CycloStatic.md)

   2. #### [Dynamic / Asynchronous mode](Documentation/Async.md)

7. ### [Maths principles](Documentation/MATHS.md)

8. ### [FAQs](Documentation/FAQ.md)



