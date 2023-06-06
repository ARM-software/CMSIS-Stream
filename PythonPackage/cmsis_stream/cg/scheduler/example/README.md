# README

## Generate the C++ scheduler for the graph.

`python graph.py`

## Host

### Compile

`make -f Makefile.windows` for instance

### Run

On unix like system, `./simple`

On windows, `simple.exe`

## Embedded target

### Compile

Using CMSIS build tools:

`csolution convert -s simple_ac6.csolution.yml`

`cbuild simple.CommandLine+VHT-Corstone-300.cprj`

### Run

On windows `run_vht.bat`

On other platform, you can look at the content of the script and adapt it.

The Arm Virtual Hardware for CS 300 must be installed.
