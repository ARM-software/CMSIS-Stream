# README

This example is inside the folder `examples/simple` of the CMSIS-Stream folder.

This example describes a very simple synchronous compute graph with 3 nodes:

![simple](docassets/simple.png)

The nodes are:

* A source generating 5 floating point values (0,1,2,3,4) each time it is run
* A processing node adding 1 to those values and working by packet of 7 values
* A sink printing its input values (1,2,3,4,5)

The graph generates an infinite streams of values : 1,2,3,4,5,1,2,3,4,5,1,2,3,4,5 ... For this example, the number of iterations will be limited so that it does not run forever.

Each node is using the floating point data type for the values.

The sink and the sources are working on packets of 5 values.

The processing node is working on packets of 7 values.

## Principle of operation

The graph is described with a Python script `create.py`.

When this Python script is executed, it computes a static schedule and generates a C++ implementation. This implementation is using some C++ wrappers that must have been defined somewhere (`AppNodes.h`). 

To run the script you first must install the CMSIS-Stream Python package:

`pip install cmsis-stream`

Once the CMSIS-Stream python package has been installed, you can run the script with:

`python create.py`

This will generate the following files:

* `generated/scheduler.cpp`
* `generated/scheduler.h`
* `simple.dot` (the [graphviz](https://graphviz.org/) representation of the graph)

A graphical representation of the graph is generated in [graphviz](https://graphviz.org/) dot format. If you have [graphviz](https://graphviz.org/) installed, you can generate a `png` file representing the graph with:

`dot -Tpng -o simple.png simple.dot`

To compile the executable you can use any of the `Makefile` depending on your host OS:

* `make -f Makefile.windows`
* `make -f Makefile.linux`
* `make -f Makefile.mac`

Then you can run the generated executable:

* `simple.exe` on Windows
* `./simple` on Linux or Mac

To get all the details about how to describe the graph in Python and write the C++ wrappers refer to the [main documentation](../../README.md#How-to-write-the-Python-script-and-the-C-wrappers).


### Expected output

The `source` and `sink` are producing 5 samples each time they are executed. The `processing` node is consuming 7 samples each time it is executed.

One iteration of the schedule is executing the `sink` and `source` 7 times and the `processing` node 5 times so that the amount of samples produced / consumed is equal to the amount of samples processed.

Each node is printing its name.

The `Sink` node is printing the received samples on the output. 

```
Start
Source
Source
ProcessingNode
Sink
1
2
3
4
5
Source
ProcessingNode
Sink
1
2
3
4
5
Source
Source
ProcessingNode
Sink
1
2
3
4
5
Sink
1
2
3
4
5
Source
ProcessingNode
Sink
1
2
3
4
5
Source
ProcessingNode
Sink
1
2
3
4
5
Sink
1
2
3
4
5
```







