# README

This example is **very experimental**.

It is a test of a runtime mode (graph can be changed at runtime without recompiling).

The runtime mode **can** use the schedule pre-computed by the Python script.

The runtime mode **supports** cyclo-static scheduling

Current test is running one iteration of the scheduling.

But the content of the scheduling is defined in `sched_flat.dat` loaded at runtime.



## Temporary limitations:

Following features from static mode are not yet available:

* dup node not yet implemented
* fifo delay not yet implemented
* async mode not implemented
* customization hooks missing
* not yet any way to pass data to the node
* result of python memory optimization not yet used
* Node identification for outside access not yet implemented

## Limitations:

Those limitations are not temporary.

* No way to have different FIFO implementations for different branches
