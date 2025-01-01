# A Tour of Computer Systems

## Information is Bits + Context

```c
/* $begin hello */
#include <stdio.h>

int main() 
{
    printf("hello, world\n");
    return 0;
}
/* $end hello */
```

Our hello program begins life as a *source program* that the programmer creates with an editor and saves in a text file called *hello.c*. The source program is a sequence of *bits*, each with a value of 0 or 1, organized in 8-bits chunk called *bytes*. Each byte represents some text character in the program.

## Programs Are Translated by Other Programs into Different Forms

The *GCC* compiler driver reads the source file *hello.c* and translates it into an executable object file *hello*, via 4 phases in the compilation system:

<img src="../images/C1_Fig1.3.png" width=600>

* **Preprocessing phase**. The pre-processor (cpp) modifies the original C program according to directives that begin with the # character. The result is another C program, typically with the .i suffix.
* **Compilation phase**. The compiler (cc1) translates the text file *hello.i* into the text file *hello.s*, which contains an *assembly-language program*. Each statement in an assembly-language program exactly describes one low-level machine-language instruction in a standard text form. Assembly language is useful because it provides a common output language for different compilers for different high-level languages.
* **Assembly phase**. The assemble (as) translate *hello.s* into machine-language instructions, packages them in a form known as a *relocatable object program*, and stores te result in the object file *hello.o*. The *hello.o* file is a binary file whose bytes encode machine-language instructions rather than characters.
* **Linking phase**. Notice that our *hello* program calls the *printf* function, which is part of the *standard C library* provided by every C compiler. The *printf* function resides in a seperate precompiled object file called *printf.o*, which must somehow be merged with our *hello.o* program. The linker (ld) handles this merging. The result is the *hello* file, which is an *executable object file* (or simply *executable*) that is ready to be loaded into memory and executed by the system.

The *GNU* (short for *GNU's Not Unix) environment includes the *EMACS* editor, *GCC* compiler, *GDB* debugger, assembler, linker, utilities for manipulating binaries, and other components.

## It Pays to Understand How Compilation Systems Work

There are some important reasons why programmers need to understand how compilation systems work:

* *Optimizing program performance*.
* *Understanding link-time errors*.
* *Avoiding security holes*.

## Processors Read and Interpret Instructions Stored in Memory

The *shell* is a command-line interpreter that prints a prompt, waits for you to type a command line, and then performs the command. If the first word of the command line does not correspond to a built-in shell command, then the shell assumes that it is the name of an executable file that it should load and run. So in this case, the shell loads and runs the *hello* program and then waits for it to terminate.

### Hardware Organization of a System

<img src="../images/C1_Fig1.4.png" width=700>

* **Buses**. Buses are typically designed to transfer fixed-sized chunks of bytes known as *words*.
* **Input/output** (I/O) devices are the system's connection to the external world. Each I/O device is connected to the I/O bus by either a *controller* or an *adapter*. The distinction between the two is mainly one of packaging. *Controllers* are chip sets in the device itself or on the system's main printed circuit board (often called *motherboard*). An *adapter* is a card that plugs into a slot on the motherboard.
* **Main Memory**. The *main memory* is a temporary storage device that holds both a program and the data it manipulates while the processor is executing the program. Physically, main memory consists of a collection of *Dynamic Random Access Memory (DRAM)* chips. Logically, memory is organized as a linear array of bytes, each with its own unique address (array index) starting at zero.
* **Processor**. The *central proccess unit* (CPU), or simply *processor*, is the engine that interprets (or executes) instructions stored in main memory. At its core is a word-sized storage device (or register) called *program counter* (PC). At any point in time, the PC points at (contains the address of) some machine-language instruction in main memory. The processor performs the same basic task over and over again: It reads the instruction from memory pointed at by the program counter (PC), interprets the bits in the instruction, performs some simple operation dictated by the instruction, and then updates the PC to point to the next instruction. The *register file* is a small storage device that consists of a collection of word-sized registers, each with its own unique name. The *arithmetic/logic unit* (ALU) computes new data and address values.
  * *Load*: Copy a byte or a word from main memory into a register, overwriting the previous contents of the register.
  * *Store*: Copy a byte or a word from a register to a location in main memory, overwriting the previous contents of that location.
  * *Update*: Copy the contents of two registers to the ALU, which adds the two words together and stores the results in a register, overwriting the previous contents of that register.
  * *I/O Read*: Copy a byte or a word from an I/O device into a register.
  * *I/O Write*: Copy a byte or a word from a register to an I/O device.
  * *Jump*: Extact a word from the instruction itself and copy that word into the program counter (PC), overwriting the previous value of the PC.

## Caches Matter

To deal with the processor-memory gap, system designers include smaller faster storage devices called cache memories (or simply caches) that serve as temporary staging areas for information that the processor is likely to need in the near future.

## Storage Devices Form a Hierarchy

<img src="../images/C1_Fig1.9.png" width=700>

The main idea of a memory hierarchy is that storage at one level serves as a cache for storage at the next lower level. Thus, the register file is a cache for the L1 cache. Caches L1 and L2 are caches for L2 and L3, respectively. The L3 cache is a cache for the main memory, which is a cache for the disk. On some networked systems with distributed file systems, the local disk serves as a cache for data stored on the disks of other systems.

## The Operating System Manages the Hardware

We can think of the operating system as a layer of software interposed between the application program and the hardware.

The operating system has two primary purposes: (1) to protect the hardware from misuse by runaway applications, and (2) to provide applications with simple and uniform mechanisms for manipulating complicated and often wildly different low-level hardware devices.

<img src="../images/C1_Fig1.18.png" width=700>

As this figure suggests, files are abstractions for I/O devices, virtual memory is an abstraction for both the main memory and disk I/O devices, and processes are abstractions for the processor, main memory, and I/O devices.

### Processes

A *process* is the operating system’s abstraction for a running program. Multiple processes can run concurrently on the same system, and each process appears to have exclusive use of the hardware. By oncurrently, we mean that the instructions of one process are interleaved with the instructions of another process.

### Threads

In modern systems a process can actually consist of multiple execution units, called *threads*, each running in the context of the process and sharing the same code and global data (within a process). Threads are an increasingly important programming model because of the requirement for concurrency in network servers, because it is easier to share data between multiple threads than between multiple processes, and because threads are typically more efficient than processes.

### Virtual Memory

*Virtual memory* is an abstraction that provides each process with the illusion that it has exclusive use of the main memory. Each process has the same uniform view of memory, which is known as its *virtual address space*. The virtual address space for Linux processes is shown:

<img src="../images/C1_Fig1.13.png" width=600>

* *Program code and data*. Code begins at the same fixed address for all processes, followed by data locations that correspond to global C variables. The code and data areas are initialized directly from the contents of an executable object file, in our case the *hello* executable.
* *Heap*. The code and data areas are followed immediately by the run-time heap. Unlike the code and data areas, which are fixed in size once the process begins running, the heap expands and contracts dynamically at run time as a result of calls to C standard library routines such as *malloc* and *free*.
* *Shared libraries*. Near the middle of the address space is an area that holds the code and data for *shared libraries* such as the C standard library and the math library.
* *Stack*. At the top of the user’s virtual address space is the *user stack* that the compiler uses to implement function calls. Like the heap, the user stack expands and contracts dynamically during the execution of the program. In particular, each time we call a function, the stack grows. Each time we return from a function, it contracts.
* *Kernel virtual memory*. The kernel is the part of the operating system that is always resident in memory. The top region of the address space is reserved for the kernel. Application programs are not allowed to read or write the contents of this area or to directly call functions defined in the kernel code.

### Files

A *file* is a sequence of bytes. Every I/O device, including disks, keyboards, displays, and even networks, is modeled as a file. All input and output in the system is performed by reading and writing files, using a small set of system calls known as *Unix I/O*.