cmake -DHOST=YES ^
   -DDOT="path to dot.EXE" ^
   -DCMSISDSP="path to cmsis dsp" ^
   -DFLAT="path to flatbuffer includes for runtime mode of CMSIS stream" ^
   -G "Unix Makefiles" ..