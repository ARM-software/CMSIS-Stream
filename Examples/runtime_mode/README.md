# README

This example is **very experimental**.

It is a test of a runtime mode (graph can be changed at runtime without recompiling).

The runtime mode **can** use the schedule pre-computed by the Python script.

The runtime mode **supports** cyclo-static scheduling

Current test is running one iteration of the scheduling.

But the content of the scheduling is defined in `sched_flat.dat` loaded at runtime.



## Temporary limitations:

Following features from build mode are not yet available:

* fifo delay not yet implemented
* async mode not implemented
* customization hooks missing
* not yet any way to pass data to the node
* result of python memory optimization not yet used
* Node identification for outside access not yet implemented

## Limitations:

Those limitations are not temporary.

* No way to have different FIFO implementations for different branches
* No direct support of pure C function. They have to be packaged into a C++ wrapper (original  build mode can call directly a pure function with no state)
