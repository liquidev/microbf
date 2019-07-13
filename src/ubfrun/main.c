/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <ubf_brainfuck.h>

// #define BENCHMARK

#ifdef BENCHMARK
# include "cycle.h"
#endif

typedef struct {
  char* string;
  size_t length;
} string_t;

void read_stdin(string_t* result) {
  #define BUF_SIZE 4
  #define NEW_STR(cap) (char*) malloc(cap * sizeof(char))

  result->length = 0;
  result->string = NULL;

  int readlen;
  char readbuf[BUF_SIZE];
  while ((readlen = read(STDIN_FILENO, readbuf, BUF_SIZE)) > 0) {
    if (result->string == NULL) {
      result->string = NEW_STR(result->length + readlen + 1);
      memcpy(&result->string[result->length], readbuf, readlen);
    } else {
      char* old_string = result->string;
      result->string = NEW_STR(result->length + readlen + 1);
      memcpy(result->string, old_string, result->length);
      free(old_string);
      memcpy(&result->string[result->length], readbuf, readlen);
    }
    result->length += readlen;
  }

  // C strings are always null-terminated, if we don't add a null byte we get
  // an invalid read (and possibly a segmentation fault).
  result->string[result->length] = '\0';

  #undef NEW_STR
  #undef BUF_SIZE
}

void free_string(string_t* string) {
  free(string->string);
}

int main(void) {
  string_t code;
  read_stdin(&code);

  #ifdef BENCHMARK
  ticks t0 = getticks();
  #endif

  ubf_vm_t* vm = ubf_init_vm();
  ubf_interpret(vm, code.string);

  #ifdef BENCHMARK
  ticks t1 = getticks();
  double time = elapsed(t1, t0);
  printf("\nfinished in %f ticks\n", time);
  #endif

  ubf_free_vm(vm);

  free_string(&code);

  return 0;
}
