# README

This is a Python package for the Arm open source [CMSIS-Stream library](https://github.com/ARM-software/CMSIS-Stream).

You can install it with:

`pip install cmsis-stream`

Then you can follow the [documentation](https://github.com/ARM-software/CMSIS-Stream) to learn how to:

* Define new compute nodes
* Connect them into a dataflow graph to process streams
* Generate a C scheduler to run the graph on your target

# Change history

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
