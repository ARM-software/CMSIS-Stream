# CMSIS-Stream

CMSIS-Stream is a Python package and small set of C++ headers that can be used to implement streaming applications on your embedded device with a low overhead solution.

Python is used to:

* Describe the dataflow graph
* Generate a static scheduling of this graph that is computed at build time with several memory optimizations. 
* Generate the code for this scheduler into a simple C++ file (with a C API).

C++ is only used for strong types and static typing (template). The only part of the C++ library that is used is the memory allocator to create the objects.

The scheduler can be run on bare metal devices. There is no dependencies to any RTOS. The scheduler is a sequence of function calls.

## Table of contents

1. ### [Introduction](Documentation/Introduction.md)

2. ### How to get started

   1. [Simple graph creation example](Examples/simple/README.md)

   2. [Simple graph creation example with CMSIS-DSP](Examples/simpledsp/README.md)

3. ### [Examples](Examples/README.md)

4. ### [Python API for creating graphs and schedulers](Documentation/PythonAPI.md)

5. ### [C++ default nodes for C++ schedulers](Documentation/CPPNodes.md)

6. ### [Python default nodes for Python schedulers](Documentation/PythonNodes.md)

7. ### [Memory optimizations](Documentation/Memory.md)

8. ### Extensions

   1. #### [Cyclo-static scheduling](Documentation/CycloStatic.md)

   2. #### [Dynamic / Asynchronous mode](Documentation/Async.md)

9. ### [Maths principles](Documentation/MATHS.md)

10. ### [FAQ](Documentation/FAQ.md)



