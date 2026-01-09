# README

Posix implementation of the CMSIS Stream event system.

It is using C++ standard library for most of the features.

Thread priority control is platform dependent and the code is different for Windows, Linux and Mac.

It is handled in `posix_thread.cpp` and the difference is hidden for the CMSIS Stream event runtime.

This implementation only use one thread for managing events (a better implementation would use a thread pool).

The part related to inter process communication and buffer sharing
without copy is not implemented in this tutorial.
