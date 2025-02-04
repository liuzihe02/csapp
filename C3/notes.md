# Machine-Level Representation of Programs

## Program Encodings

Process of GCC command:

```c
#include <stdio.h>

int main() {
  printf("hello, world\n");
  return 0;
}

/**
 * hello program life:
 *                                                                                          printf.o --|
 *                pre-processor               compiler                      assembler                  |--> linker
 *    hello.c    -------------->   hello.i   ---------->     hello.s      -------------->    hello.o    ------------>   hello
 * (source file)                ( modified              (assembly program)               ( relocatable               ( executable
 *                               source file )                                            object programs )          object programs )
 *                                                                                           (binary)                   (binary)
 * 
 * - Preprocessing phase: modifies the original C program according to directives thar begin with the '#' character, and result is another C program, typically with the .i suffix.
 * 
 * - Compilation phase: translate .i to .s, which is an assembly-language program.
 *                      Assembly language is useful because it provides a common output language for different compilers for different hight-level languages.  
 *
 * - Assembly phase: translate .i to .o, which is mechine-language instructions, but it only contains main function(so hello.o contain gibberish text).
 *
 * - Linking phase: merging other .o files.
 * 
 * */ 

/**
 * GCC instance:
 * 
 * 1. -------------
 * gcc hello.c 
 * ---------------- default ouput an executable file named a.out
 *
 * 2. -------------
 * gcc hello.c -o hello
 * ---------------- output an executable file named hello
 * 
 * 3. -------------
 * gcc -E hello.c -o hello.i
 * ---------------- preprocessing and output a file named hello.i
 * 
 * 4. -------------
 * gcc -S hello.i
 * ---------------- translate hello.i to hello.s
 * 
 * 5. -------------
 * gcc -c hello.s
 * ---------------- translate hello.s to hello.o
 * 
 * 6. -------------
 * gcc hello.o -o hello2
 * ---------------- translate hello.o to an executable file named hello2
 * 
 * 7. -------------
 * gcc -O1 hello.c -o hello3
 * ---------------- open O1 optimizer
 * */
 ```

### Machine Level Code

For machine level programming:

* *Instruction set architecture*, or **ISA** (including IA32 and x86-64) defines the processor state, the format of the instructions, and the effect each of these instructions will have on the state.
* The *memory addresses* used by a machine-level program are virtual addresses, providing a memory model that appears to be a very large byte array.

Parts of the processor state are visible that normally are hidden from the C programmer:

* The *program counter* (commonly referred to as the “PC,” and called %eip in IA32) indicates the address in memory of the next instruction to be executed
* The *integer register* file for x86-64 contains 16 named locations storing 64-bit values. Can hold addresses or data
* The *condition code registers* hold status information about the most recently executed arithmetic or logical instruction.
* A set of *floating-point registers* store floating-point data.

The program memory contains the executable machine code for the program, some information required by the operating system, a run-time stack for managing procedure calls and returns, and blocks of memory allocated by the user (for example, by using the malloc library function).

## Data Formats

Size of C data types in x86-64:

|C declaration|Intel data type|Assembly code suffix|Size(bytes)|
|-|-|-|-|
|char|Byte|b|1|
|short|Word|w|2|
|int|Double word|l|4|
|long int|Double word|l|4|
|long long int|-|-|4|
|char *|Double word|l|4|
|float|Single precision|s|4|
|double|Double precision|l|8|
|long double|Extented precision|t|10/12|

## Accessing Information

An x86-64 CPU contains a set of 16 **general-purpose registers** storing 64-bit values. Theses register are used to store integer data as well as pointers.

| 63 - 0 | 31 - 0 | 15 - 0 | 7 - 0|  description   |
| -      |  -     | -      | -    |     -          |
| %rax   | %eax   | %ax    | %al  |  return value  |
| %rbx   | %ebx   | %bx    | %bl  |  callee saved  |
| %rcx   | %ecx   | %cx    | %cl  |  4th argument  |
| %rdx   | %edx   | %dx    | %dl  |  3rd argument  |
| %rsi   | %esi   | %si    | %sil |  2nd argument  |
| %rdi   | %edi   | %di    | %dil |  1st argument  |
| %rbp   | %ebp   | %bp    | %bpl |  callee saved  |
| %rsp   | %esp   | %sp    | %spl |  stack pointer |
| %r8    | %r8d   | %r8w   | %r8b |  5th argument  |
| %r9    | %r9d   | %r9w   | %r9b |  6th argument  |
| %r10   | %r10d  | %r10w  | %r10b|  callee saved  |
| %r11   | %r11d  | %r11w  | %r11b|  callee saved  |
| %r12   | %r12d  | %r12w  | %r12b|  callee saved  |
| %r13   | %r13d  | %r13w  | %r13b|  callee saved  |
| %r14   | %r14d  | %r14w  | %r14b|  callee saved  |
| %r15   | %r15d  | %r15w  | %r15b|  callee saved  |

### Operand Specifiers

Most instructions have one or more operands specifying the source values to use
in performing an operation and the destination location into which to place the result.

* *immediate* is for constant values. written with a `$` followed by an integer e.g.`$0x400`, `$-533` 
* *register* denotes the contents of one of the registers e.g. `%rax` , `%r13`
* *memory* reference accesses some memory location according to a computed address (often called the *effective address*).

### Data Movement Instructions

```
mov Source, Destination
```

Operations such as `movb` (for bytes) or `movw` (for words) move data from a source to a destination. Different variation of `mov` for different data types. The source operand designates a value that is immediate, stored in register, or stored in memory. The destination operand designates a location that is either a register or a memory address. Hence x86 cannot have both source and destination as memory locations. The following combinations are possible:

```x86asm
movl $0x4050,%eax Immediate--Register, 4 bytes
movw %bp,%sp Register--Register, 2 bytes
movb (%rdi,%rcx),%al Memory--Register, 1 byte
movb $-17,(%esp) Immediate--Memory, 1 byte
movq %rax,-12(%rbp) Register--Memory, 8 bytes
```

**Example**
```c
long exchange(long* xp, long y) {
    long x = *xp;
    *xp = y;
    return x;
}
```
```bash
gcc -Og -c exchange.c
objdump -d exchange.o
```
```nasm
;xp and y are stored register %rdi and %rsi
0000000000000000 <exchange>:
   0:   f3 0f 1e fa             endbr64
    ;loads the value that xp points to into %rax.
    ;read from memory, store in register
    ;note that () is the dereferencing operator in asm
   4:   48 8b 07                mov    (%rdi),%rax 
   ;stores y into the location pointed to by xp
   7:   48 89 37                mov    %rsi,(%rdi)
   a:   c3                      ret    
```

* “pointers” in C are simply addresses. Dereferencing a pointer involves
copying that pointer into a register, and then using this register in a memory
reference
* local variables such as x are often kept in registers rather than
stored in memory locations. Register access is much faster than memory access.

## Pushing and Popping Stack

<img src="../images/C3_Stack.png" width =500>

The stack grows downwards here, so the "top" of the stack is at the lowest address. The stack pointer `%rsp` holds the address of the top stack element. The `pushq` instruction provides the ability to push data onto the stack, while the `popq` instruction pops it. 

Pushing a quad word value onto the stack involves first decrementing the stack pointer by 8 and then writing the value at the new top-of-stack address. Equivalent to:
```nasm
subq 8, %rsp ;decrement stack pointer
movq %rax, (%rsp) ;store rax onto the stack
```

Popping a quad word involves reading from the top-of-stack location and then incrementing the stack pointer by 8. Equivalent to:
```nasm
movq (%rsp),%rdx ;move data to %rdx
addq $8,%rsp ;Increment stack pointer
```


## Arithmetic and Logical Operations

| Instruction | Arguments | Effect | Description |
|-------------|-----------|---------|-------------|
| `leaq` | S, D | D ← &S | Load effective address |
| `INC` | D | D ← D+1 | Increment |
| `DEC` | D | D ← D-1 | Decrement |
| `NEG` | D | D ← -D | Negate |
| `NOT` | D | D ← ~D | Complement |
| `ADD` | S, D | D ← D + S | Add |
| `SUB` | S, D | D ← D - S | Subtract |
| `IMUL` | S, D | D ← D * S | Multiply |
| `XOR` | S, D | D ← D ^ S | Exclusive-or |
| `OR` | S, D | D ← D \| S | Or |
| `AND` | S, D | D ← D & S | And |
| `SAL` | k, D | D ← D << k | Left shift |
| `SHL` | k, D | D ← D << k | Left shift (same as SAL) |
| `SAR` | k, D | D ← D >>ₐ k | Arithmetic right shift |
| `SHR` | k, D | D ← D >>ₗ k | Logical right shift |

## Control

Machine code provides two basic low-level mechanisms for implementing conditional behavior: it tests data values and then either alters the control flow or the data flow based on the result of these tests.

### Condition Codes

These condition codes are implicitly set by arithmetics operations:

* CF: Carry Flag. The most recent operation generated a carry out of the most significant bit. Used to detect overflow for unsigned operations.
* ZF: Zero Flag. The most recent operation yielded zero.
* SF: Sign Flag. The most recent operation yielded a negative value.
* OF: Overflow Flag. The most recent operation caused a two’s-complement overflow—either negative or positive.

### Explicitly Setting Condition Codes

|Instruction|Based on|Description|
|-|-|-|
|`CMP S2,S1`|`S1-S2`|Compare|
|`TEST S2,S`1|`S1 & S2`|Test|

- `CMP` instructions set the condition codes according to the differences of their two operands. It behave in the same way as the `SUB` instructions, except that they set the condition codes without updating their destinations.
- `TEST` instructions behave in the same manner as the `AND` instructions, except that they set the condition codes without altering their destinations.

### Accessing Condition Codes

Three ways of using the conditions codes:

1. **Set** a single byte to 0 or 1 depending on some combination of the condition codes.

Instructions like `sete D` sets the destination `D` to `1` if the Zero Flag ZF is set

```nasm
cmp eax, ebx    ; Compare two values
sete al         ; al = 1 if eax == ebx, al = 0 if not equal
```
Has either one of the low-order single-byte register elements or a single-byte memory location as its destination, setting this byte to either 0 or 1

2. Conditionally **jump** to some other part of the program

E.g. `je Label` (Jump If Equal) jumps to the specified label if ZF is set

```nasm
cmp eax, ebx    ; Compare two values
je  label       ; Jump to "label" if eax == ebx
; code here is skipped if jump taken
label:
; execution continues here if jump taken
```
3. Conditional **Control**

 Assembly implementation typically adheres to the following form, where we use C syntax to describe the control flow:

 ```
     t = test-expr;
    if (!t) {
        goto false;
    }
    then-statement;
    goto done;
false:
    else-statement;
done:
```

### Conditional Moves

The conventional way to implement conditional operations is through a conditional transfer of control, where the program follows one execution path when a condition holds and another when it does not. It's simple, but it can be very inefficient on modern processors. An alternate strategy is through a conditional transfer of data(only can be used in restricted cases)

Conditional Control:

```cpp
long absdiff(long x, long y) {
    long result;
    if (x < y) {
        result = y - x;
    }
    else {
        result = x - y;
    }
    return result;
}
```

Conditional Moves:

```cpp
long cmovdiff(long x, long y) {
    long rval = y - x;
    long eval = x - y;
    long ntest = x >= y;
    if (ntest) {
        rval = eval;
    } // no else statement
    return rval;
}
```

Processors achieve high performance through **pipelining**, where an instruction is processed via a sequence of stages, each performing one small portion of the required operations. This approach achieves high performance by overlapping the stages of the successive instructions. (Each step does not occur sequentially) To do this requires being able to determine the sequence of instructions to be executed well ahead of time in order to keep the pipeline full of instructions to be executed. When the machine encounter a conditional jump, it can't determine which way the branch will go until it has evaluated the branch condition. Processors employ sophisticated branch prediction logic to guess whether or not each jump instruction will be followed. As long as it can guess reliably(modern microprocessor designs try to achieve success rates on the order of 90%), the instruction pipeline will kept full of instructions.

Mispredicting a jump, on the other hand, requires that the processor discard much of the work it has already done on future instruction - can incur a serious penalty, say, 15-30 clock cycles of wasted effort, causing a serious degradation of program performance. Therefore conditional moves avoid branch mispredictions.



### Loops

The general form of a do-while statement is as follows:

```s
do
    body-statement
    while (test-expr);
```

This general form can be translated into conditionals and goto statements as follows:

```s
loop:
    body-statement
    t = test-expr;
    if (t)
        goto loop;
```

A key to understanding how the generated assembly code relates to the original source code is to find a mapping between program values and registers.

The general form of a while statement is as follows:

```s
while (test-expr)
    body-statement
```

One common approach to transform the code into a do-while loop by using a conditional branch to skip the first execution of the body if needed:

```s
    t = test-expr;
    if (!t)
        goto done;
loop:
    body-statement
    t = test-expr;
    if (t)
        goto loop;
done:
```

The general form of a for loop is as follows:

```s
for (init-expr; test-expr; update-expr)
    body-statement
```

This, in turn, can be transformed into goto code as:

```s
    init-expr;
    t = test-expr;
    if (!t)
        goto done;
loop:
    body-statement
    update-expr;
    t = test-expr;
    if (t)
        goto loop;
done:
```

### Conditional Move Instructions

|Instruction|Synonym|Move condition|Description|
|-|-|-|-|
|cmove S,R|cmovz|ZF|Equal/zero|
|cmovne S,R|cmovnz|~ZF|Not equal/not zero|
|cmovs S,R||SF|Negative|
|cmovns S,R||~SF|Nonnegative|
|cmovg S,R|cmovnle|~(SF^OF)&~ZF|Greater (signed >)|
|cmovge S,R|cmovnl|~(SF^OF)|Greater or equal (signed >=)|
|cmovl S,R|cmovnge|SF^OF|Less (signed <)|
|cmovle S,R|cmovng|(SF^OF)\|ZF|Less or equal (signed <=)|
|cmova S,R|cmovnbe|~CF&~ZF|Above (unsigned >)|
|cmovae S,R|cmovnb|~CF|Above or equal (unsigned >=)|
|cmovb S,R|cmovnae|CF|Below (unsigned <)|
|cmovbe S,R|cmovna|CF\|ZF|Below or equal (unsigned <=)|

Processors employ sophisticated branch prediction logic to try to guess whether or not each jump instruction will be followed. As long as it can guess reliably (modern microprocessor designs try to achieve success rates on the order of 90%), the instruction pipeline will be kept full of instructions. Mispredicting a jump, on the other hand, requires that the processor discard much of the work it has already done on future instructions and then begin filling the pipeline with instructions starting at the correct location. As we will see, such a misprediction can incur a serious penalty, say, 20–40 clock cycles of wasted effort, causing a serious degradation of program performance.

To understand how conditional operations can be implemented via condi- tional data transfers, consider the following general form of conditional expression and assignment:

```s
v = test-expr ? then-expr : else-expr;
```

With traditional IA32, the compiler generates code having a form shown by the
following abstract code:

```s
    if (!test-expr)
        goto false;
    v = true-expr;
    goto done;
false:
    v = else-expr;
done:
```

If one of those two expressions then-expr and else-expr could possibly generate an error condition or a side effect, this could lead to invalid behavior.

### Switch Statements

A jump table is an array where entry i is the address of a code segment implementing the action the program should take when the switch index equals i. The code performs an array reference into the jump table using the switch index to determine the target for a jump instruction.