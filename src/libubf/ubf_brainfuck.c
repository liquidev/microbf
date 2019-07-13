/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#ifndef ubf_c
#define ubf_c

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "ubf_brainfuck.h"
#include "ubf_compiler.h"
#include "ubf_debug.h"

ubf_cell_t *ubf__init_cell(void) {
  ubf_cell_t *cell = (ubf_cell_t *)malloc(sizeof(ubf_cell_t));
  cell->value = 0;
  cell->left = NULL;
  cell->right = NULL;
  return cell;
}

void ubf__free_cell(ubf_cell_t *cell) {
  ubf_cell_t *left = cell->left;
  ubf_cell_t *right = cell->right;
  free(cell);
  if (left != NULL) {
    left->right = NULL;
    ubf__free_cell(left);
  }
  if (right != NULL) {
    right->left = NULL;
    ubf__free_cell(right);
  }
}

ubf_vm_t *ubf_init_vm(void) {
  ubf_vm_t *vm = (ubf_vm_t *)malloc(sizeof(ubf_vm_t));
  vm->pc = 0;
  vm->pos = 0;
  vm->ptr = ubf__init_cell();
  return vm;
}

void ubf_free_vm(ubf_vm_t *vm) {
  ubf__free_cell(vm->ptr);
  free(vm);
}

char ubf__getch(void) {
  struct termios old, new;
  int ch;

  tcgetattr(STDIN_FILENO, &old);
  new = old;
  new.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &new);
  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &old);

  return ch;
}

ubf_interpret_result ubf__interpret_impl(ubf_vm_t *vm, ubf_chunk_t *chunk) {
  // microbf uses computed gotos for code execution.
  // You can read more on that here:
  // https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
  #define READ() chunk->bytecode[vm->pc++]
  #ifdef UBF_VM_USE_COMPUTED_GOTO
  # define DISPATCH() goto *dispatch_table[READ()]
  static void *dispatch_table[] = {
    &&_UBF_INC, &&_UBF_DEC,
    &&_UBF_LT,  &&_UBF_RT,
    &&_UBF_JZ,  &&_UBF_JNZ,
    &&_UBF_PUT, &&_UBF_GET,
    &&_UBF_FIN
  };
  # define CASE(e) _##e:
  #else
  # define DISPATCH() break
  # define CASE(e) case e:
  #endif

  vm->pc = 0;

  #ifdef UBF_VM_USE_COMPUTED_GOTO
  DISPATCH();
  while (true) {
  #else
  while (true) {
    switch (READ()) {
  #endif
      CASE(UBF_INC) {
        vm->ptr->value += READ();
        DISPATCH();
      }
      CASE(UBF_DEC) {
        vm->ptr->value -= READ();
        DISPATCH();
      }
      CASE(UBF_LT) {
        uint8_t amt = READ();
        for (uint8_t i = 0; i < amt; i++) {
          if (vm->ptr->left == NULL) {
            vm->ptr->left = ubf__init_cell();
            vm->ptr->left->value = 0;
            vm->ptr->left->right = vm->ptr;
          }
          vm->ptr = vm->ptr->left;
        }
        vm->pos -= amt;
        DISPATCH();
      }
      CASE(UBF_RT) {
        uint8_t amt = READ();
        for (uint8_t i = 0; i < amt; i++) {
          if (vm->ptr->right == NULL) {
            vm->ptr->right = ubf__init_cell();
            vm->ptr->right->value = 0;
            vm->ptr->right->left = vm->ptr;
          }
          vm->ptr = vm->ptr->right;
        }
        vm->pos += amt;
        DISPATCH();
      }
      CASE(UBF_JZ) {
        uint8_t addr = chunk->offset_table[READ()];
        if (vm->ptr->value == 0) {
          vm->pc = addr;
        }
        DISPATCH();
      }
      CASE(UBF_JNZ) {
        uint8_t addr = chunk->offset_table[READ()];
        if (vm->ptr->value != 0) {
          vm->pc = addr;
        }
        DISPATCH();
      }
      CASE(UBF_PUT) {
        uint8_t amt = READ();
        for (uint8_t i = 0; i < amt; i++) {
          printf("%c", vm->ptr->value);
        }
        DISPATCH();
      }
      CASE(UBF_GET) {
        uint8_t amt = READ();
        for (uint8_t i = 0; i < amt; i++) {
          vm->ptr->value = ubf__getch();
        }
        DISPATCH();
      }
      CASE(UBF_FIN) {
        return UBF_OK;
      }
    }
  #ifndef UBF_VM_USE_COMPUTED_GOTO
  }
  #endif

  #undef READ
  #undef DISPATCH
  #undef CASE
}

ubf_interpret_result ubf_interpret(ubf_vm_t *vm, const char *code) {
  ubf_chunk_t *chunk = ubf__alloc_chunk(0);
  ubf_compile(code, strlen(code), chunk);

  ubf_interpret_result result = ubf__interpret_impl(vm, chunk);

  ubf__free_chunk(chunk);
  return result;
}

#endif
