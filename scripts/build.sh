#!/bin/bash

echo " * cleaning"
rm *.o
rm microbf

echo " * compiling"
gcc -g -O0 -c *.c

echo " * linking"
gcc *.o -o microbf
