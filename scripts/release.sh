#!/bin/bash

echo " * cleaning"
rm -rf *.o dist

echo " * compiling"
gcc -O3 -c *.c

echo " * linking"
mkdir dist
gcc *.o -o dist/microbf

