/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#ifndef ubf_compiler_h
#define ubf_compiler_h

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/// Result of compilation.
typedef enum {
  UBF_COMPILE_OK,
  UBF_COMPILE_UNBALANCED_LOOP
} ubf_compile_result;

/// A microbf opcode.
typedef enum {
  UBF_INC, UBF_DEC, // + and -
  UBF_LT,  UBF_RT,  // < and >
  UBF_JZ,  UBF_JNZ, // jump if zero, jump if not zero
  UBF_PUT, UBF_GET, // . and ,
  UBF_FIN
} ubf_opcode;

/// A chunk of UBF bytecode.
typedef struct {
  uint8_t *bytecode;
  size_t length;
  size_t capacity;

  size_t offset_table[256];
  uint8_t offset_length;
} ubf_chunk_t;

/// Allocates a new chunk of bytecode.
ubf_chunk_t *ubf__alloc_chunk(size_t initial_length);

/// Frees a previously allocated chunk of bytecode.
void ubf__free_chunk(ubf_chunk_t *chunk);

/// Compiles brainfuck code into a chunk of bytecode.
void ubf_compile(const char *code, size_t length, ubf_chunk_t *chunk);

#endif
