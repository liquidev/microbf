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

#include "brainfuck.h"
#include "compiler.h"
#include "debug.h"

#define NEW(T) (T*) malloc(sizeof(T))

ubf_cell_t* init_cell(void) {
  ubf_cell_t* cell = NEW(ubf_cell_t);
  cell->value = 0;
  cell->left = NULL;
  cell->right = NULL;
  return cell;
}

void free_cell(ubf_cell_t* cell) {
  ubf_cell_t* left = cell->left;
  ubf_cell_t* right = cell->right;
  free(cell);
  if (left != NULL) {
    left->right = NULL;
    free_cell(left);
  }
  if (right != NULL) {
    right->left = NULL;
    free_cell(right);
  }
}

ubf_vm_t* ubf_init_vm(void) {
  ubf_vm_t* vm = NEW(ubf_vm_t);
  vm->pc = 0;
  vm->pos = 0;
  vm->ptr = init_cell();
}

void ubf_free_vm(ubf_vm_t* vm) {
  free_cell(vm->ptr);
  free(vm);
}

char getch(void) {
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

ubf_interpret_result interpret(ubf_vm_t* vm, ubf_chunk_t* chunk) {
  // microbf uses computed gotos for code execution.
  // You can read more on that here:
  // https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
  // (damn it, the link didn't fit in 80 columns)
  static void* dispatch_table[] = {
    &&opc_inc, &&opc_dec,
    &&opc_lt,  &&opc_rt,
    &&opc_jz,  &&opc_jnz,
    &&opc_put, &&opc_get,
    &&opc_fin
  };
  #define READ() chunk->bytecode[vm->pc++]
  #define DISPATCH() goto *dispatch_table[READ()]

  DISPATCH();
  while (true) {
    opc_inc: {
      vm->ptr->value += READ();
      DISPATCH();
    }
    opc_dec: {
      vm->ptr->value -= READ();
      DISPATCH();
    }
    opc_lt: {
      uint8_t amt = READ();
      for (uint8_t i = 0; i < amt; i++) {
        if (vm->ptr->left == NULL) {
          vm->ptr->left = init_cell();
          vm->ptr->left->value = 0;
          vm->ptr->left->right = vm->ptr;
        }
        vm->ptr = vm->ptr->left;
      }
      vm->pos -= amt;
      DISPATCH();
    }
    opc_rt: {
      uint8_t amt = READ();
      for (uint8_t i = 0; i < amt; i++) {
        if (vm->ptr->right == NULL) {
          vm->ptr->right = init_cell();
          vm->ptr->right->value = 0;
          vm->ptr->right->left = vm->ptr;
        }
        vm->ptr = vm->ptr->right;
      }
      vm->pos += amt;
      DISPATCH();
    }
    opc_jz: {
      uint8_t addr = chunk->offset_table[READ()];
      if (vm->ptr->value == 0) {
        vm->pc = addr;
      }
      DISPATCH();
    }
    opc_jnz: {
      uint8_t addr = chunk->offset_table[READ()];
      if (vm->ptr->value != 0) {
        vm->pc = addr;
      }
      DISPATCH();
    }
    opc_put: {
      uint8_t amt = READ();
      for (uint8_t i = 0; i < amt; i++) {
        printf("%c", vm->ptr->value);
      }
      DISPATCH();
    }
    opc_get: {
      uint8_t amt = READ();
      for (uint8_t i = 0; i < amt; i++) {
        vm->ptr->value = getch();
      }
      DISPATCH();
    }
    opc_fin: {
      return UBF_OK;
    }
  }

  #undef DISPATCH
  #undef READ
}

ubf_interpret_result ubf_interpret(ubf_vm_t* vm, const char* code) {
  ubf_chunk_t* chunk = ubf_alloc_chunk(0);
  ubf_compile(code, strlen(code), chunk);

  interpret(vm, chunk);

  ubf_free_chunk(chunk);
}

#undef NEW

#endif