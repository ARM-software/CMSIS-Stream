# README

This is a Python package for the Arm open source [CMSIS-Stream library](https://github.com/ARM-software/CMSIS-Stream).

You can install it with:

`pip install cmsis-stream`

Then you can follow the [documentation](https://github.com/ARM-software/CMSIS-Stream) to learn how to:

* Define new compute nodes
* Connect them into a dataflow graph to process streams
* Generate at build time a static C scheduler to run the graph on your target (no need of an RTOS)

# Change history

## Version 1.14.1:
* Bug correction for issue #2

## Version 1.14.0:

* More customization possible for the FIFO class:
  * It is no more described with a string but now uses a Python class to give the name of the C++ class implementing the FIFO and the additional arguments used by the C++ constructor of this FIFO.
  * Having a custom FIFO with additional arguments may require to change the definitions in `GenericNodes.h`

## Version 1.13.0:

* New option to prevent a custom buffer from being shared

## Version 1.12.0:

* Corrected issues with Duplicate node buffer optimizations
* Added a new callback mode
* Added new option for memory optimization (graph coloring algorithm to use)

## Version 1.11.0:

* With memory optimization mode, the constraints on Duplicate nodes are relaxed to avoid copies as much as possible

## Version 1.10.0:

* A specific memory buffer can now be assigned to a FIFO (so buffer external to this framework)
* New option to dynamically allocate the FIFO buffers using custom memory allocator
* And some corrections applied to 1.9.2

## Version 1.9.2:

* A specific memory buffer can now be assigned to a FIFO (so buffer external to this framework)
* New option to dynamically allocate the FIFO buffers using custom memory allocator
* (Version number not correct - the middle number should have been increased)

## Version 1.9.1:

* Correction to the management of sharing buffer states. It makes it easier to use this feature from the Python

## Version 1.9.0:

* Introduced a C++ namespace for the nodes
* Introduced some support for static reference counting (in type system). It is to help with use of buffers in a graph and detect at build time some problems. See `example12` and documentation about [buffers](../Documentation/Buffer.md)

## Version 1.8.0:

* Introduction of a new asynchronous mode : `fullyAsynchronous`
* Previous asynchronous mode is deprecated and will be removed in a future version

## Version 1.7.0:

* Improvements to GenericNodes.h when building with gcc
* `addLiteral` can now also use `bool` values from Python
* Naming of the function to get a node was not consistent between the C and CPP APIs

## Version 1.6.1:

* Correction to 1.6.0 : Missing YAML module 

## Version 1.6.0:

* SDS sensor and recorder nodes to interact with the [Arm SDS Framework](https://github.com/ARM-software/SDS-Framework/tree/main) to record stream of samples and replay them in Arm Virtual Hardware
* YAML import / export of a graph and its code generation settings

## Version 1.5.1:

* Wrong `GenericNodes.h` was included in version 1.5.0
* 1.5.0 is no more available for download and has been replaced by 1.5.1 correction

## Version 1.5.0:

* Corrections to the heap mode
* Added possibility to identify and access nodes from the outside of the scheduler (it implies the heap mode). See documentation for more information

## Version 1.4.0:

* Possibility to customize the style of the graph pictures (colors, fonts ...)
* Graph picture can now also be generated before schedule computation (useful to debug an incorrect graph before trying to compute a schedule)
* Possibility to use more **pure** C functions in the graph. The `GenericFunction` node can now easily be used to plug more kind of C functions without having to write a C++ wrapper
* `Dsp` node has been removed. Instead use the `Unary`, `Binary` or the more flexible `GenericFunction` node

## Version 1.3.0:

* New nodes `GenericToMany`, `GenericFromMany`, `GenericManyToMany`
* `Duplicate` node implementation now using `GenericToMany`
* Possibility to change the name of IOs in `Binary`, `Unary` and `Dsp` nodes

## Version 1.2.1:

* Correct an issue in 1.2.0. Some templates needed
by the new command line tool had not been included in the
package.

## Version 1.2.0:

* The file cg_status.h can now also be created from
the python package

* A new heapAllocate option has been introduced. When true,
FIFO and node objects are allocated on the heap and no more
on the stack. Two new functions are generated in the scheduler to initialize or free those objects

* A new command line tool cmsis-stream is available. It can be used to quickly create a project with : cmsis-stream create TestFolder. The folder will contain makefiles for windows, mac and linux. It will also contain a CMSIS build tool solution file to build for Arm Virtual Hardware CS 300

## Version 1.1.0:

* The file GenericNodes.h can now be created from the
python package. Like that, it is possible to start using
CMSIS-Stream by only installing the Python package.
Of course, if other specifics nodes are required (FFT, MFCC) the CMSIS-Stream repository will have to be used to get the headers

* Duplicate node and duplicate insertion has been improved: one-to-many connections are now fully supported. Before it was automatically supported up to 3 outputs and for more outputs, Duplicate nodes had to be inserted manually.


## Version 1.0.0:

* First version (originally it was the compute graph library part of the CMSIS-DSP library)
