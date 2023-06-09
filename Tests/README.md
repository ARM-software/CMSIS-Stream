# README

Some tests to validate some parts of the Compute graph. They all rely on the CMSIS build tools and Arm Virtual Hardware.

## List of tests

* `create_sync.py`
  * Create a complex graph containing all classes defined in `GenericNodes.h` (synchronous mode). Used to test that it builds and that there are no errors in the templates
  * `cbuild "syncgraph.CommandLine+VHT-Corstone-300.cprj" `
* `create_async.py`
  * Create a complex graph containing all classes defined in `GenericNodes.h` (asynchronous mode). Used to test that it builds and that there are no errors in the templates
  * `cbuild "asyncgraph.CommandLine+VHT-Corstone-300.cprj" `
* `create_fifobench_sync.py`
  * Create a graph with FFT / IFFT : the graph is decomposing a signal and rebuilding it. It is used to test the performance of different FIFOs implementations (synchronous mode)
  * `cbuild "fifobench_sync.CommandLine+VHT-Corstone-300.cprj"`
  * Run the result in AVH
* `create_fifobench_async.py`
  * Create a graph with FFT / IFFT : the graph is decomposing a signal and rebuilding it. It is used to test the performance of different FIFOs implementations (asynchronous mode)
  * `cbuild "fifobench_async.CommandLine+VHT-Corstone-300.cprj"`
  * Run the result in AVH
* There is a simple FIFO test to check the behavior of the FIFO implementation:
  * `cbuild "fifo.CommandLine+VHT-Corstone-300.cprj" `
  * Run the result in AVH
* `create_duplicate_sync.py`
  * Validate the `Duplicate` node that is a more complex kind of node (one-to-many)
  * Create a `build` folder and inside type `cmake -G "Unix Makefiles" ..`
  * Run the result on host computer