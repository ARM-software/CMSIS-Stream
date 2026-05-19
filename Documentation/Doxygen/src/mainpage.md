@mainpage Introduction

CMSIS-Stream is a low overhead framework for building AI and multimedia
applications with connected dataflow and event graphs.

## Full CMSIS-Stream Documentation

This Doxygen reference is only one part of the CMSIS-Stream documentation. It
documents the CMSIS-RTOS runtime API that is needed for the event graph.

The full CMSIS-Stream documentation, including the Python and C/C++ framework
guides, lives in the official GitHub repository:
https://github.com/ARM-software/CMSIS-Stream/

To use the CMSIS-Stream runtime, a CMSIS Stream graph must be generated. Python is used to describe a graph of processing components and generate the state machine that schedules it.

## API Scope

This reference documents the platform headers and the CMSIS-RTOS runtime
implementation provided by this repository.

The generated pages are intentionally scoped to:

- headers in the `platform` folder
- files in `platform/cmsis_rtos_runtime`
