#ifndef REGIONS_V2M_MPS3_SSE_300_FVP_H
#define REGIONS_V2M_MPS3_SSE_300_FVP_H


//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <n> Auto-generated using information from packs
// <i> Device Family Pack (DFP):   ARM::V2M_MPS3_SSE_300_BSP@1.5.0
// <i> Board Support Pack (BSP):   ARM::V2M_MPS3_SSE_300_BSP@1.5.0

// <h> ROM Configuration
// =======================
// <h> __ROM0 (is rxn memory: IROM1 from DFP)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region. Default: 0x10000000
//   <i> Contains Startup and Vector Table
#define __ROM0_BASE 0x10000000
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region. Default: 0x00200000
#define __ROM0_SIZE 0x00200000
// </h>

// <h> __ROM1 (unused)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region.
#define __ROM1_BASE 0x0
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region.
#define __ROM1_SIZE 0x0
// </h>

// <h> __ROM2 (unused)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region.
#define __ROM2_BASE 0x0
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region.
#define __ROM2_SIZE 0x0
// </h>

// <h> __ROM3 (unused)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region.
#define __ROM3_BASE 0x0
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region.
#define __ROM3_SIZE 0x0
// </h>

// </h>

// <h> RAM Configuration
// =======================
// <h> __RAM0 (is rwxn memory: IRAM1 from DFP)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region. Default: 0x30000000
//   <i> Contains uninitialized RAM, Stack, and Heap
#define __RAM0_BASE 0x30000000
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region. Default: 0x00020000
#define __RAM0_SIZE 0x00020000
// </h>

// <h> __RAM1 (is rwxn memory: ITCM_NS from DFP)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region. Default: 0x00000000
#define __RAM1_BASE 0x00000000
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region. Default: 0x00080000
#define __RAM1_SIZE 0x00080000
// </h>

// <h> __RAM2 (is rwxn memory: SRAM_NS from DFP)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region. Default: 0x01000000
#define __RAM2_BASE 0x01000000
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region. Default: 0x00100000
#define __RAM2_SIZE 0x00100000
// </h>

// <h> __RAM3 (is rwxn memory: DTCM0_NS+DTCM1_NS+DTCM2_NS+DTCM3_NS from DFP)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region. Default: 0x20000000
#define __RAM3_BASE 0x20000000
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region. Default: 0x00080000
#define __RAM3_SIZE 0x00080000
// </h>

// </h>

// <h> Stack / Heap Configuration
//   <o0> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
//   <o1> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
#define __STACK_SIZE 0x00000200
#define __HEAP_SIZE 0x00000C00
// </h>

// <n> Resources that are not allocated to linker regions
// <i> rwxn RAM: ISRAM0_NS from DFP:     BASE: 0x21000000  SIZE: 0x00100000
// <i> rwxn RAM: ISRAM1_NS from DFP:     BASE: 0x21100000  SIZE: 0x00100000
// <i> rwxn RAM: QSPI_SRAM_NS from DFP:  BASE: 0x28000000  SIZE: 0x00800000
// <i> rwxs RAM: ITCM_S from DFP:        BASE: 0x10000000  SIZE: 0x00080000
// <i> rwxs RAM: SRAM_S from DFP:        BASE: 0x11000000  SIZE: 0x00100000
// <i> rwxs RAM: DTCM0_S from DFP:       BASE: 0x30000000  SIZE: 0x00020000
// <i> rwxs RAM: DTCM1_S from DFP:       BASE: 0x30020000  SIZE: 0x00020000
// <i> rwxs RAM: DTCM2_S from DFP:       BASE: 0x30040000  SIZE: 0x00020000
// <i> rwxs RAM: DTCM3_S from DFP:       BASE: 0x30060000  SIZE: 0x00020000
// <i> rwxs RAM: ISRAM0_S from DFP:      BASE: 0x31000000  SIZE: 0x00100000
// <i> rwxs RAM: ISRAM1_S from DFP:      BASE: 0x31100000  SIZE: 0x00100000
// <i> rwxs RAM: QSPI_SRAM_S from DFP:   BASE: 0x38000000  SIZE: 0x00800000


#endif /* REGIONS_V2M_MPS3_SSE_300_FVP_H */
