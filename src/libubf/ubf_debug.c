/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#ifndef ubf_debug_c
#define ubf_debug_c

#include <stdio.h>

#include "ubf_debug.h"

void ubf_disassemble(ubf_chunk_t* chunk) {
  #define VAL(offset) chunk->bytecode[idx + offset]
  #define WRITE(fmt, arg) \
    printf(fmt, arg); \
    idx += 2; \
    break; \

  for (size_t i = 0; i < chunk->length; i++) {
    printf("%02x ", (int) chunk->bytecode[i]);
  }
  printf("\n");

  size_t idx = 0;
  while (idx < chunk->length) {
    printf("%08x  ", (int) idx);
    switch (VAL(0)) {
      case UBF_INC: WRITE("INC %d\n", VAL(1));
      case UBF_DEC: WRITE("DEC %d\n", VAL(1));
      case UBF_LT:  WRITE("LT  %d\n", VAL(1));
      case UBF_RT:  WRITE("RT  %d\n", VAL(1));
      case UBF_PUT: WRITE("PUT %d\n", VAL(1));
      case UBF_GET: WRITE("GET %d\n", VAL(1));
      case UBF_JZ:  WRITE("JZ  @%08x\n", (int) chunk->offset_table[VAL(1)]);
      case UBF_JNZ: WRITE("JNZ @%08x\n", (int) chunk->offset_table[VAL(1)]);
      case UBF_FIN: printf("FIN\n"); return;
    }
  }

  #undef VAL
}

const char* ubf_get_opcode_name(ubf_opcode opcode) {
  switch (opcode) {
    case UBF_INC: return "INC";
    case UBF_DEC: return "DEC";
    case UBF_LT:  return "LT";
    case UBF_RT:  return "RT";
    case UBF_JZ:  return "JZ";
    case UBF_JNZ: return "JNZ";
    case UBF_PUT: return "PUT";
    case UBF_GET: return "GET";
    case UBF_FIN: return "FIN";
    default:      return "<unknown>";
  }
}

#endif
