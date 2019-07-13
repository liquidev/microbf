# microbf
A small, optimizing Brainfuck interpreter.

## Usage
To use the microbf interpreter, simply execute it:
```
./ubf
```
It will read from the standard input until it hits an EOF (`^D` on Linux,
<kbd>Ctrl</kbd> + <kbd>Z</kbd> on Windows), and interpret the input.

## Compiling
To compile microbf, you'll need a C compiler and Meson.
```bash
meson build
ninja -C build
```
libubf and ubfrun will be built to `build/meson-out`.

## Embedding
microbf can be embedded to create a custom REPL, debugger, or something, but it
doesn't have any modularity or application embedding capabilities.
Not yet, at least.
```c
#include <ubf_brainfuck.h>

int main(void) {
  // To create a VM, use:
  ubf_vm_t *vm = ubf_init_vm();
  // To interpret code, use:
  ubf_interpret(vm, "+++++[>+++++<-]");
  // After you're done with the VM, don't forget to release its memory:
  ubf_free_vm(vm);

  return 0;
}
```

## Contributing
Feel free to contribute to the microbf project. If you find any missing
features or bugs, submit an issue or open a pull request.

## Implementation details
If you want some background on microbf's internal architecture, read
[this document](docs/implementation.md).
