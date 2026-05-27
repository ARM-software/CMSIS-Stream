# CMSIS-Stream Hello on Zephyr

This example is the Zephyr equivalent of `Examples/hello_example` from the
CMSIS-Stream repository. It builds the same generated `hello` graph, but uses
the CMSIS-Stream Zephyr runtime and runs on the Arm Corstone-300 FVP.

The CMSIS solution uses the `west` project mechanism, following the pattern used
by `MyApps/ZephyrAudio/Zephyr.csolution.yml`.

Build with CMSIS Toolbox:

```bat
cbuild Zephyr.csolution.yml --context CMSISStreamHello.Debug+AVH-SSE-300
```

Or build directly with west from this directory:

```bat
west build -p auto -b mps3/corstone300/fvp .
west build -t run
```

The project expects the CMSIS-Stream Zephyr module to be available in the SDK
west workspace at `modules/lib/cmsisstream`. Add it to the west manifest with:

```yaml
- name: cmsisstream
  url: https://github.com/ARM-software/CMSIS-Stream
  revision: main
  path: modules/lib/cmsisstream
```
