/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#ifndef ubf_h
#define ubf_h

#include <stdint.h>
#include <stdlib.h>

/// A single memory cell.
/// microbf implements memory as a linked list, so theoretically an infinite
/// amount of cells is possible (in the real world, it's limited by
/// the host's memory).
typedef struct cell_ {
  int8_t value;
  struct cell_* left;
  struct cell_* right;
} ubf_cell_t;

/// The microbf virtual machine.
/// This is the state machine for code execution.
typedef struct {
  // bytecode
  size_t pc;
  // memory
  int pos;
  ubf_cell_t* ptr;
} ubf_vm_t;

/// The result of an interpreter session.
typedef enum {
  UBF_OK
} ubf_interpret_result;

/// Initializes and returns a new VM.
ubf_vm_t* ubf_init_vm();

/// Frees a VM.
void ubf_free_vm();

/// Interprets brainfuck code in a VM.
ubf_interpret_result ubf_interpret(ubf_vm_t* vm, const char* code);

#endif
