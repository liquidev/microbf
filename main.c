/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#include <stdio.h>

#include "brainfuck.h"

int main(int argc, const char* argv[]) {
  ubf_vm_t* vm = ubf_init_vm();
  ubf_interpret(vm, ">++++++++[-<+++++++++>]<.>>+>-[+]++>++>+++[>[->+++<<+++>]<<]>-----.>->\n"
  "+++..+++.>-.<<+[>[+>+]>>]<--------------.>>.+++.------.--------.>+.>+.");
  ubf_free_vm(vm);
}
