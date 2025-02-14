# The Memory Hierarchy

In practice, a **memory system** is a hierarchy of storage devices with different capacities, costs, and access times.

- CPU registers hold the most frequently used data.
- Small, fast *cache memories* nearby the CPU act as staging areas for a subset of the data and instructions stored in the relatively slow main memory.
- The main memory stages data stored on large, slow disks, which in turn often serve as staging areas for data stored on the disks or tapes of other machines connected by networks.

This idea center around a fundamental property of computer programs known as **locality**
- Well-written programs tend ot access storage at a particular level more frequently than storage at the next lower level
- Storage at the next level can afford to be slower and larger


## Storage Technologies

### Random Access Memory (RAM)

RAM comes in 2 categories - static (SRAM) and dynamic (DRAM).

| | Transistors per bit | Relative access time | Persistent | Sensitive | Relative cost | Applications |
| - | - | - | - | - | - | - |
| SRAM | 6 | 1x | yes | no | 1000x  | Cache memory |
| DRAM | 1 | 10x | no | yes | 1x | Main memory, frame buffers|

SRAM is faster and significantly more expensive than DRAM. SRAM often used for cache memories, on and off CPU. DRAM used for main memory and frame buffers. Usually few tens of megabytes of SRAM, but thousands of megabytes of DRAM. SRAM is persistent as long as power is applied. Unlike DRAM, no refresh is necessary. SRAM can be accessed faster than DRAM. SRAM is not sensitive to disturbances such as light and electrical noise. The trade-off is that SRAM cells use more transistors than DRAM cells and thus have lower densities, are more expensive, and consume more power.

### Nonvolatile Memory

DRAM and SRAM are volatile in the sense that they lose their information when supply voltage is turned off. Nonvolatile memory retain their information even when powered off, referred to as Read-Only Memories (ROM). ROMs are distinguished by number of times they can be written to, and mechanism for writing to.

- Read-only memory (ROM): programmed during production
- Programmable ROM (PROM): can be programmed once
- Eraseable PROM (EPROM): can be bulk erased (UV, X-Ray)
- Electrically eraseable PROM (EEPROM): electronic erase capability
- Flash memory: EEPROMs. with partial (block-level) erase capability. Wears out after about 100,000 erasings

Programs stored in ROM devices are called firmware. When computer powers up, it runs firmware stored in ROM. Some systems with small set of primitive input/output functions in firmware - like a PC's BIOS (basic input/output system) routines.

### Accessing Main Memory

Data flows back and forth between the CPU and the DRAM main memory over shared electrical conduits called buses. Each transfer called a bus transaction. A read transaction transfers data from main memory to CPU. A write transaction transfers data from CPU to main memory. A *bus* is a collection of parallel wires that carry address, data and control signals.

<img src="images/C6_BusStructure.png" width =500>

- System bus connects CPU to the I/O bridge
- Memory bus connects I/O bridge to the main memory
- I/O bridge translates electrical signals of system bus to signals of memory bus

Consider what happens with a load operation `movq A, %rax` occurs:


<img src="images/C6_LoadBus.png" width =600>

- Circuitry on the CPU called bus interface initiates a read transaction
  - CPU places address A on system bus, I/O bridge passes signal to memory bus
  - Main memory senses address signal on memory bus, fetches data from DRAM, and writes data to memory bus
  - I/O bridge translates memory bus signal to system bus signal and passes it to system bus
  - CPU senses data on system bus, reads data, and copies it to register `%rax`

## Disk Storage

Disks are workhouse storage devices that hold enormous amounts of data, much more than DRAM/SRAM. However, much slower than DRAM/SRAM.

### Connecting I/O Devices

I/O devices (graphics cards, mice etc) are connected to the CPU and main memory via I/O bus. Unlike the system bus and memory buses which are CPU-specific, I/O buses are independent of CPU design.

<img src="images/C6_IOBusStructure.png" width =600>

I/O bus is slower than system and memory buses, but accomodate many third party I/O devices.

### Accessing Disks

