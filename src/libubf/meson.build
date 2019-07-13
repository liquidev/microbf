libubf_include = '.'
libubf_src = [
  'ubf_brainfuck.c',
  'ubf_compiler.c',
  'ubf_debug.c'
]
libubf_lib = library('ubf', libubf_src)
libubf_dep = declare_dependency(link_with: libubf_lib,
                                include_directories: libubf_include)
