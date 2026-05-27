# Zephyr Application Configuration Templates

The files in this folder are examples to copy into a Zephyr application and
customize. They are application-specific glue around the shared Zephyr runtime:

- `app_config.hpp`: generated-scheduler hooks and application logging aliases.
- `cstream_node.h`: C API used to expose identified C++ stream nodes.
- `IdentifiedNode.hpp`: helper that builds `CStreamNode` handles for identified
  C++ nodes.
- `stream_init.hpp` and `stream_init.cpp`: example top-level stream startup and
  shutdown code for one generated graph.
- `prj.conf`: example Zephyr Kconfig fragment for CMSIS-Stream runtime sizing,
  priorities, stacks, pools, and logging.

Unlike the CMSIS-RTOS runtime, Zephyr runtime settings are Kconfig symbols named
`CONFIG_CMSISSTREAM_*`. Put the values from `prj.conf` in the application's
`prj.conf` or an overlay configuration file.

The sample `stream_init.cpp` assumes a generated graph named `hello`, with
`scheduler_hello.h` and `hello_params.h`. Replace those includes, init calls,
parameter assignments, and execution-context entries with the generated files
for the target application.
