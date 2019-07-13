/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#ifndef ubf_compiler_c
#define ubf_compiler_c

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ubf_compiler.h"

void ubf__realloc_chunk(ubf_chunk_t *chunk, size_t capacity) {
  if (chunk->bytecode == NULL) {
    chunk->bytecode = (uint8_t *)malloc(capacity * sizeof(uint8_t));
  } else {
    uint8_t *new_bytecode = (uint8_t *)malloc(capacity * sizeof(uint8_t));
    memcpy((void *)new_bytecode, (void *)chunk->bytecode, chunk->capacity);
    free(chunk->bytecode);
    chunk->bytecode = new_bytecode;
  }
  chunk->capacity = capacity;
}

ubf_chunk_t *ubf__alloc_chunk(size_t initial_capacity) {
  ubf_chunk_t *chunk = (ubf_chunk_t *)malloc(sizeof(ubf_chunk_t));

  chunk->bytecode = NULL;
  chunk->length = 0;
  chunk->capacity = 0;
  chunk->offset_length = 0;

  ubf__realloc_chunk(chunk, initial_capacity);

  return chunk;
}

void ubf__free_chunk(ubf_chunk_t *chunk) {
  free(chunk->bytecode);
  free(chunk);
}

void ubf__chunk_print(ubf_chunk_t *chunk) {
  for (size_t i = 0; i < chunk->length; i++) {
    printf("%02x ", (int) chunk->bytecode[i]);
  }
  printf("\n");
}

void ubf__chunk_write(ubf_chunk_t *chunk, uint8_t byte) {
  if (chunk->capacity < chunk->length + 1) {
    ubf__realloc_chunk(chunk, (chunk->capacity < 8) ? 8 : chunk->capacity * 2);
  }
  chunk->bytecode[chunk->length] = byte;
  chunk->length++;
}

uint8_t ubf__chunk_offset(ubf_chunk_t *chunk, size_t offset) {
  for (size_t i = 0; i < chunk->offset_length; i++) {
    if (chunk->offset_table[i] == offset) {
      return (uint8_t) i;
    }
  }
  uint8_t id = (uint8_t) chunk->offset_length;
  chunk->offset_table[chunk->offset_length] = offset;
  chunk->offset_length++;
  return id;
}

int ubf__compile_char(const char *code, size_t length,
                      ubf_chunk_t *chunk,
                      size_t pos) {
  #define AT_END (index >= length)
  #define PEEK() code[index]
  #define NEXT() index++
  #define COLLECT(ch, opcode) \
    do { \
      int amt = 0; \
      while (amt < 256 && PEEK() == ch) { \
        amt++; \
        NEXT(); \
      } \
      ubf__chunk_write(chunk, opcode); \
      ubf__chunk_write(chunk, (uint8_t) amt); \
    } while (false);

  size_t index = pos;

  switch (PEEK()) {
    case '+':
      COLLECT('+', UBF_INC); break;
    case '-':
      COLLECT('-', UBF_DEC); break;
    case '<':
      COLLECT('<', UBF_LT); break;
    case '>':
      COLLECT('>', UBF_RT); break;
    case '.':
      COLLECT('.', UBF_PUT); break;
    case ',':
      COLLECT(',', UBF_GET); break;
    case '[':
      NEXT();
      size_t jz_pos = chunk->length;
      uint8_t jz_offset = ubf__chunk_offset(chunk, jz_pos);
      ubf__chunk_write(chunk, UBF_JZ);
      ubf__chunk_write(chunk, 0);

      while (PEEK() != ']' && !AT_END) {
        index = ubf__compile_char(code, length, chunk, index);
      }
      NEXT();

      ubf__chunk_write(chunk, UBF_JNZ);
      ubf__chunk_write(chunk, jz_offset);
      uint8_t jnz_offset = ubf__chunk_offset(chunk, chunk->length);

      chunk->bytecode[jz_pos + 1] = jnz_offset;
      break;
    default: // comments
      NEXT();
      break;
  }

  return index;

  #undef AT_END
  #undef PEEK
  #undef NEXT
  #undef COLLECT
}

void ubf_compile(const char *code, size_t length, ubf_chunk_t *chunk) {
  size_t index = 0;
  while (index < length) {
    index = ubf__compile_char(code, length, chunk, index);
  }
  ubf__chunk_write(chunk, UBF_FIN);
}

#endif
