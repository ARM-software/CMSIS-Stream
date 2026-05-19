# Files copied to the application {#cmsis_stream_files_copied}

The CMSIS Pack copies the configuration files into the client project so the
application can customize them. Generated schedulers include `app_config.hpp`
and then `stream_platform_config.hpp`; client application source files
do not include the files in folder `config` directly.

The copied configuration files are:

- `app_config.hpp`: Application configuration
- `cstream_node.h`: C API to the nodes
- `IdentifiedNode.hpp`: Implementation of the C API
- `stream_runtime_config.hpp`: Runtime configuration
- `stream_init.hpp`: Application startup interface
- `stream_init.cpp`: Application startup implementation

## `app_config.hpp`

This header is generally used to define `CG_BEFORE_BUFFER` and
`CG_BEFORE_NODE_EXECUTION`, which customize the generated CMSIS-Stream
scheduler.

`CG_BEFORE_BUFFER` **must** be used to enforce alignment of buffers when memory
optimization feature is activated.

This header is only included by the generated schedulers. The runtime does not include it.

## `stream_runtime_config.hpp`

This header can be used to redefine macros `CMSISSTREAM_LOG_ERR` and
`CMSISSTREAM_LOG_DBG` for logging.

All CMSIS-Stream runtime configuration macros can also be redefined in this
header. `stream_platform_config.hpp` includes it before defining platform
defaults, so values defined here override the Pack defaults.

The runtime uses this header.

## Other header

Nodes of CMSIS-Stream can implement some APIs exposed to the software outside of the graph. 

The headers `cstream_node.h` and
`IdentifiedNode.hpp` provides a way to expose those API to the external world
and hide complexity of C++ multiple-inheritance.

The APIs implemented are application dependent and that's why those files must
be provided by the application. See CMSIS-Stream documentation for more
information.

## `stream_init.hpp` and `stream_init.cpp`

`stream_init.hpp` and `stream_init.cpp` are CMSIS-RTOS application templates.
They connect one or more generated schedulers to the shared CMSIS-Stream
runtime. They are copied into the client project because they depend on the
application graph names, generated scheduler headers, hardware initialization,
node parameters, and runtime startup policy.

The template is intentionally more complex than the other copied files. It is
the place where application code binds generated graph code to the CMSIS-RTOS
runtime.

## Customizing `stream_init.hpp`

Update `NB_APPS` to match the number of generated stream applications that can
run in the project. A simple project generally keeps `NB_APPS` set to 1. A
project that context switches between several generated graphs should set it to
the number of available graphs.

Keep `contexts` sized with `NB_APPS`. Each entry describes one generated graph
to the runtime: its scheduler function, FIFO reset function, pause and resume
helpers, node lookup wrapper, event queue, number of identified nodes, and
scheduler length.

Keep `currentNetwork` as the index of the graph that must be started first, or
that is currently selected for execution. Application code may update it before
startup or when selecting another graph.

The public functions `stream_configure_and_start()` and `stream_free_all()` are
the application-level lifecycle API. They can be renamed or wrapped by the
client project if a different startup convention is required.

## Customizing `stream_init.cpp`

Include the generated scheduler header for each graph. The template contains a
single example include such as `scheduler_hello.h`; replace it with the header
generated from the Python graph description. For several graphs, include one
generated scheduler header per graph.

Create one event queue per graph. The template stores them in `queue_app`, and
`stream_configure_and_start()` calls `stream_new_event_queue()` for each entry.
This model allows paused graphs to keep their own event queue while sharing the
runtime memory pools.

Add application hardware initialization before scheduler initialization. This
is where timers, sensors, displays, accelerators, audio interfaces, or other
drivers should be created and configured. Any handles or hardware parameters
needed by generated nodes should be stored in the application parameter
structures before the scheduler `init_scheduler_*` function is called.

Initialize every generated scheduler with the arguments expected by that graph.
The exact `init_scheduler_*` function name and argument list come from the
Python graph description. Some graphs use one parameter block, some use several
node-specific parameter blocks, and some use no parameter block.

For each graph, fill one `stream_execution_context_t` entry. The function
pointers must match the generated scheduler names:

- `scheduler_*`: dataflow scheduler function
- `reset_fifos_scheduler_*`: FIFO reset function
- `get_*_node`: local wrapper around the generated node lookup function
- `pause_scheduler_app` and `resume_scheduler_app`: helpers that visit
  identified nodes implementing `ContextSwitchInterface`

The `get_*_node` wrapper hides the application-specific `CStreamNode` type from
the shared runtime by returning `void *`. Add one wrapper per generated graph
when several graphs are present.

Adapt `pause_scheduler_app()` and `resume_scheduler_app()` when the application
extends `CStreamNode` with additional C-callable interfaces. The default
implementation only checks `ContextSwitchInterface` and calls `pause()` or
`resume()` on identified nodes that implement it.

Select the first graph to run with `currentNetwork`, call
`resume_scheduler_app(&contexts[currentNetwork])`, then start the runtime
threads with `stream_start_threads(&contexts[currentNetwork])`. This ordering
lets nodes publish initial events after their event queue exists.

Update `stream_free_all()` to free every resource allocated during startup.
The usual order is:

1. Wait for the runtime threads with `stream_wait_for_threads_end()`.
2. Call every generated `free_scheduler_*` function.
3. Delete the event queues.
4. Release shared runtime memory with `stream_free_memory()`.

If startup can fail midway, keep the error path consistent with the resources
already allocated by the application. Projects with more hardware resources may
need additional cleanup in the error path.
