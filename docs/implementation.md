# The implementation details behind microbf

This document covers the internal architecture of the compiler and the VM.
Some features are implemented differently from common implementations, and this
document is supposed to clear out any confusion one might have regarding the
interpreter.

## Bytecode

microbf is a bytecode interpreter – that means, before it executes any code, it
compiles it to an intermediate representation: bytecode.

### Chunks

microbf's bytecode is constructed out of chunks. This is the chunk declaration:
```c
typedef struct {
  uint8_t* bytecode;
  size_t length;
  size_t capacity;

  size_t offset_table[256];
  uint8_t offset_length;
} ubf_chunk_t;
```
Basically, a chunk is just a glorified dynamic array. However, it has an extra
feature of storing the jump offset table.
This table is what stores all `JZ` and `JNZ` offsets. Those opcodes then
reference them by their index on the offset table.

The current implementation of the offset table only allows for 256 offsets,
which isn't much, but future improvements could be made to increase this number
to 65536, or even higher. This isn't done however, because it would require
more memory allocation. Future versions of the interpreter will replace this
statically-sized array with a dynamic array.

### Opcodes

microbf bytecode consists of 9 different opcodes:

| Opcode | Description |
| --- | --- |
| `INC` | Increments the pointer's value. |
| `DEC` | Decrements the pointer's value. |
| `LT` | Moves the tape to the left. |
| `RT` | Moves the tape to the right. |
| `JZ` | Jumps to a location in bytecode, if the pointer's value is zero. |
| `JNZ` | Jumps to a location in bytecode, if the pointer's value is not zero. |
| `PUT` | Prints the ASCII character at the pointer to the screen. |
| `GET` | Reads a single ASCII character from standard input. |
| `FIN` | Finishes the main VM loop. |

Each of these opcodes occupies a single byte. All opcodes, except `FIN`, accept
an operand:
 - for `INC`, `DEC`, `LT`, `RT`, `PUT`, and `GET` it's the amount of times an
   instruction should be executed;
 - for `JZ` and `JNZ` it's the index of an offset in a bytecode chunk's offset
   table.

## Compiler

microbf has an optimizing compiler in place, which not only compiles brainfuck
code into bytecode, but also optimizes it.

### Optimizing the bytecode

The current implementation only includes one optimization: sequences of
instructions are compiled as single opcodes. More optimizations will be
implemented in future versions of the interpreter.

## Virtual machine

microbf uses a bytecode VM, for increased performance. Chunks of bytecode are
interpreted by the VM using a `while` loop with computed `goto`s.

### Structure

This is the C declaration of the VM:

```c
typedef struct {
  // bytecode
  size_t pc;
  // tape
  int pos;
  ubf_cell_t* ptr;
} ubf_vm_t;
```

The `pc` field is responsible for the VM's while loop. When interpretation
starts, `pc` is set to 0, pointing to the program's beginning. The VM has
no clue about the length of the program – it just executes instructions,
until `FIN` is hit.

The `pos` and `ptr` fields are tape fields, which is described below.

### The tape

Brainfuck operates on an infinite tape, composed of memory cells. As we know,
computer memory is not infinite, but we can imitate this behavior using a
doubly linked list of memory cells. Here's the C declaration of a memory cell:

```c
typedef struct cell_ {
  int8_t value;
  struct cell_* left;
  struct cell_* right;
} ubf_cell_t;
```

A cell stores:
 - its value, as a signed 8-bit integer;
 - pointers to neighbouring cells, `left` and `right`.

The VM doesn't know its position on the tape. This isn't needed, because all
brainfuck has is two instructions (`<` and `>`) for navigating the memory.
That's why a doubly linked list was chosen: it's very easy to navigate, by just
changine the VM's `pointer` to either `pointer->left`, or `pointer->right`.
However, a `pos` field exists, to provide some information for potential
debuggers.

The tape is allocated as needed: when a VM is initialized, a single memory cell
is initialized in its `pointer` field. When a memory cell is initialized, its
`left` and `right` fields are set to `NULL`.

As the VM executes instructions for navigating the memory, it checks if the
`pointer`'s neighbouring cells are `NULL`. If the desired cell *is* `NULL`,
it's allocated and initialized, and its `left` or `right` is set to the
`pointer`. Otherwise, the `pointer` is just set to `pointer->left` or
`pointer->right`, as described previously.

### The execution loop

For extra performance, microbf uses [computed gotos][cptgoto].

  [cptgoto]: https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables

Computed gotos consist of a dispatch table, containing pointers to labels, and
the actual labels. Computed gotos are more efficient than a simple case
statement, even up to 25%. Performance wasn't measured for microbf yet, but
when it eventually does get measured, the results will be put up here.

