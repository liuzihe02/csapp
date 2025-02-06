# Optimizing Program Performance

Modern compilers are optimizing compilers that generate efficient machine code. However programmers can write bad code creating optimizing blockers that hinder efficient optimization.

**Summary of Performance Improvement Techniques**
- Basic coding principles:
  - Eliminate excessive function calls.
  - Eliminate unnecessary memory references.
- Low-level optimizations:
  - Unroll loops to reduce overhead and to enable further optimizations.
  - Find ways to increase instruction-level parallelism.
  - Rewrite condition operations in a functional style to enable compilation via conditional data transfers.

## Vector Example

We implement the vector ADT to illustrate some optimizations:

```c
/* Create abstract data type for vector */
typedef struct {
    long len;
    data_t *data;
} vec_rec, *vec_ptr; 
//declares two types, the structure type itself and a pointer type pointing to the struct
```

Generic operation involving vectors (inefficient):
```c
//identity value for the operation, 0 for add and 1 for divide
#define IDENT 0;
#define OP +;

//dest is a pointer to where final result is stored
void combine1(vec_ptr v, data_t* dest) {
    long i;
    *dest = IDENT;
    for (i = 0; i < vec_length(v); i++) {
        //temp variable use to store each element
        data_t val;
        get_vec_element(v, i, &val);
        *dest = *dest OP val;
    }
}
```

### Eliminating Loop Inefficiencies

Observe that `combine1` calls function `vec_length` as the test condition of the `for` loop, called every loop! The length of the vector does not change as the loop proceeds. We could therefore compute the vector length only **once**  and use this value in our test condition:

```c
void combine2(vec_ptr v, data_t* dest) {
    long i;
    //taken out of loop and computed onlu once
    long length = vec_length(v);
    *dest = IDENT;
    for (i = 0; i < length; i++) {
        data_t val;
        get_vec_element(v, i, &val);
        *dest = *dest OP val;
    }
}
```

*Code motion* is a type of optimization which identifies computation that is performed multiple times, but has the same result. We therefore move this computation out of the loop.

### Reducing Procedure Calls

In the code for `combine2`, the `get_vec_element` is called on every loop iteration to retrieve the next vector element. In `get_vec_element`, the function checks the vector index `i` against the loop bounds in every loop, a clear source of inefficiency. (Bounds checking might be a useful feature when dealing with arbitrary array accesses, but a simple analysis of the code for `combine2` shows that all references will be valid).

We remove function calls in the inner loop:
```c
data_t *get_vec_start(vec_ptr v) {
    return v->data;
}

void combine3(vec_ptr v, data_t *dest) {
    long i;
    long length = vec_length(v);
    data_t *data = get_vec_start(v);

    *dest = IDENT;
    for (i = 0; i < length; i++) {
        //access array directly instead of unncessary function calls
        *dest = *dest OP data[i];
    }
}
```

### Eliminating Unneeded Memory References

For `data_t` as `double` and with multiplication `*` as the `OP` , the x86-64 code of inner loop of `combine3`:

```nasm
; dest in %rbx, data+i in %rdx, data+length in %rax
.L17
  vmovsd (%rdx), %xmm0            ; read product from dest
  vmulsd (%rdx), %xmm0, %xmm0     ; multiply product by data[i]
  vmovsd %xmm0, (%rbx)            ; store result at dest
  addq   $8, %rdx ; increment data to read next elem
  cmpq   %rax, %rdx ;compare to data+length
  jne    .L17 ;if !=, then go to loop
```

The accumulated value is read from and written to memory on each iteration. This reading and writing is wasteful. We eliminate unneccessary reading and writing of memory in `combine4`:

```c
void combine4(vec_ptr v, data_t *dest) {
    long i;
    long length = vec_length(v);
    data_t *data = gec_vec_start(v);
    data_t acc = IDENT;

    for (i = 0; i < length; i++) {
        //introduce a temp local variable that accumulates
        acc = acc OP data[i];
    }
    //result is stored in dest only after the loop completes
    *dest = acc;
}
```

The assembly code that follows show:

```nasm
.L25
  vmulsd (%rdx), %xmm0, %xmm0 ;multiply acc by data[i]
  addq   $8, %rdx ;increment data
  cmpq   %rax, %rdx ;compare
  jne    .L25 ;if !=, goto loop
```

Significant overhead in reading and writing!
