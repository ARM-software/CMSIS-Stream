# README

POSIX implementation of the CMSIS Stream event system.

It is using C++ standard library for most of the features.

Thread priority control is platform dependent and the code is different for Windows, Linux and Mac.

It is handled by `stream_event_queue.cpp` and the difference is hidden for the CMSIS Stream event runtime.

The high-level runtime API is exposed from `stream_runtime_init.hpp` and
`stream_runtime_init.cpp`, matching the CMSIS-RTOS and Zephyr runtimes.

The `config` folder contains template files that should be copied into a client
application for customization.

This implementation uses one thread for the dataflow scheduler and one thread
for managing events.

The part related to inter process communication and buffer sharing
without copy is not implemented in this runtime.

## CMake usage

When the runtime is used from source with `add_subdirectory`, point
`POSIX_RUNTIME_CONFIG_DIR` to the client application's copied config folder
before adding the runtime:

```cmake
set(POSIX_RUNTIME_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/config" CACHE PATH "" FORCE)
add_subdirectory(path/to/CMSIS-Stream/platform/posix_runtime)

target_link_libraries(my_app PRIVATE cmsis_stream::posix_runtime)
```

To install the runtime as a CMake package:

```sh
cmake -S platform/posix_runtime -B build/posix_runtime -DCMAKE_INSTALL_PREFIX=/path/to/install
cmake --build build/posix_runtime --config Release
cmake --install build/posix_runtime --config Release
```

CMake installs the package configuration file under:

```text
<install-prefix>/lib/cmake/cmsis-stream/cmsis-streamConfig.cmake
```

`find_package(cmsis-stream CONFIG REQUIRED)` searches known install prefixes for
that file. On Linux, packages installed to common prefixes such as `/usr` or
`/usr/local` are often found automatically. For a custom prefix, pass the install
prefix when configuring the client application:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/install
```

On Windows, custom install prefixes usually need to be provided explicitly:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\path\to\install
```

Alternatively, point directly to the package directory:

```powershell
cmake -S . -B build -Dcmsis-stream_DIR=C:\path\to\install\lib\cmake\cmsis-stream
```

Then a client project can use:

```cmake
find_package(cmsis-stream CONFIG REQUIRED)

add_executable(my_app main.cpp config/stream_init.cpp generated/scheduler.cpp)
target_include_directories(my_app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/config")
target_link_libraries(my_app PRIVATE cmsis_stream::posix_runtime)
```

The package also defines `CMSIS_STREAM_POSIX_RUNTIME_CONFIG_TEMPLATE_DIR`,
which points to the installed runtime config templates. Copy those files into
the client application and customize the copy; do not include the installed
template directory directly in the build.
