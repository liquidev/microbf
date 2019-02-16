# microbf
A small, optimizing Brainfuck interpreter.

## Usage
microbf isn't have a standalone interpreter yet. It can be used directly in C:

```c
#include "brainfuck.h"

int main() {
  ubf_vm_t* vm = ubf_init_vm();
  ubf_interpret(vm, "some brainfuck code +++++[>+++++<-]");
  ubf_free_vm(vm);
  return 0;
}
```
