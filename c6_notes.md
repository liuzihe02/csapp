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

<img src="images/C6_ReadingDisk.png" width =600>

1. Memory Mapped I/O
  - A block of addresses in address space reserved for communicating for I/O devices, called I/O port
  - Disk read will execute three store instructions
    - Command to initiate disk read
    - Logical block number in disk to read
    - Main memory address where contents of disk address is to be stored

2. Direct Memory Access (DMA)
  - Disk controller (without involvement of CPU) reads content of disk sector and transfers contents directly to main memory (DMA transfer)

3. Interrupt Signal
  - Disk controller notifies CPU by sending interrupt signal and returns control to CPU

### Solid State Disks


<img src="images/C6_SSD.png" width =600>

SSD is a storage technology based on flash memory, alternative to a conventional rotating disk. A flash translation layer is a hardware/firmware interface that translates requests for logical blocks to accesses of the physical storage.

SSDs are much faster reading than writing. Thye are built of semiconductor memory, with no moving parts, have much faster random access times than rotating disks, and use less power. However the flask blocks wear out after repeated writes, but in practice the lifespan is many years.

### Storage Trends

Gap between DRAM, disk and CPU performance is widening - CPU improves much faster. Before, this performance gap was a function of latency, with DRAM and disk access times decreasing more slowly than cycle time of a single processor. However, with the introduction of multiple cores, the performance gap is increasingly a function of throughput, with multiple processor cores issuing requests to DRAM/disk in parallel. Modern computers use SRAM caches to bridge memory-processor gap.

## Locality

*Principle of Locality*:  Programs tend to use data and instructions with addresses near or equal to those they have used recently.
- *Temporal Locality*: A memory location that is referenced once is likely to be referenced again multiple times in the near future
- *Spatial Locality* When a memory locaiton is referenced once, program likely to reference nearby memory locations in the near future

Hardware - cache memories hold blocks of most recently referenced instructions and data. OS - system uses main memory as cache of most recently referenced chunks in virtual address space.

### Locality of References to Program Data

Note that sequential reference patterns visits each element of a vector sequentially. A *stride-k reference pattern* visits every *kth* element of a contiguous vector. As the stride increases, the spatial locality decreases.

Stride is important for multipledimensional arrays:

```c
int sumarrayrows(int a[M][N])
{
    int i, j, sum = 0;
    //reads in row major order, good stride-1 reference pattern
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            sum += a[i][j];
    return sum;
}
```

| Address | 0 | 4 | 8 | 12 | 16 | 20 |
|---------|---|---|---|----|----|----| 
| Contents | a₀₀ | a₀₁ | a₀₂ | a₁₀ | a₁₁ | a₁₂ |
| Access order | 1 | 2 | 3 | 4 | 5 | 6 |


```c
int sumarraycols(int a[M][N])
{
    int i, j, sum = 0;
    //poor spatial locality, as it scans memory with a stride-N reference pattern
    for (j = 0; j < N; j++)
        for (i = 0; i < M; i++)
            sum += a[i][j];
    return sum;
}
```

| Address | 0 | 4 | 8 | 12 | 16 | 20 |
|---------|---|---|---|----|----|----| 
| Contents | a₀₀ | a₀₁ | a₀₂ | a₁₀ | a₁₁ | a₁₂ |
| Access order | 1 | 3 | 5 | 2 | 4 | 6 |

## The Memory Hierarchy

<img src="images/C6_MemHier.png" width =500>

Storage devices get slower, larger and cheaper down the memory hierarchy.

<img src="images/C6_CachingDetails.png" width =600>

### Caching in Memory Hierarchy

<img src="images/C6_CachingPrinciple.png" width =500>

A cache is a staging area for data stored in larger, slower devices. Faster/smaller storage at level *k* serves as cache for slower/larger storage one level down *k+1*. Storage at lvl *k+1* are partitioned into blocks, and a subset of blocks is copied to level *k*. Devices lower down the hierarchy have longer access times, hence needs to use larger block sizes to amortize long access times

**Cache Hits**

When program needs object *d* from level *k+1*, it will first look one level up at level *k*. If object is cached at level *k*, called cache hit. This is faster than reading from level *k+1*.

**Cache Misses**

If object *d* is not cached at level *k*, this a cache miss. The cache at level *k* will fetch block containing *d* from level *k+1*, possibly overwriting a existing block in level *k* if cache is full. THis is called replacing a block, set by the replacement policy.
- Random replacement policy chooses a random victim block
- Least Recently Used (LRU) cache chooses the block that was accessed the furthest in the past

After cache at level *k* has fetched the relevant block from level *k+1*, program can now read *d* from level *k*.

**Cold cache**: empty cache so always miss
**Conflict miss**: Cache large enough to hold referenced objects, but multiple lower level blocks map to the same upper level block, so access to these objects will cause constant misses (block keeps getting overwritten)
**Capacity miss**: The working set is a set of blocks that is currently accessed. If working set size exceeds cache size, cannot hold the entire working set, may result in capacity misses.

**Cache Management** Logic to partition cache into blocks, transfer blocks between different levels, deal with misses and hits.
- Compiler manages register files
- L1/L2/L3 managed by hardware logic
- DRAM main memory is cache for disk, managed by OS and CPU instructions
- For machine with distributed file systems like Andrew File System (AFS), local disk is a cache managed by AFS client process

Exploiting temporal locality: Cache recent objects
Exploiting spatial locality: cache blocks instead of single data objects, so other objects within block which may be requested often are easily accessible