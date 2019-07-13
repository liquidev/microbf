/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#ifndef ubf_debug_h
#define ubf_debug_h

#include "ubf_compiler.h"

/// Disassembles a chunk of bytecode and prints it out to stdout.
void ubf_disassemble(ubf_chunk_t *chunk);

/// Returns the name of an opcode.
const char* ubf_get_opcode_name(ubf_opcode code);

#endif
