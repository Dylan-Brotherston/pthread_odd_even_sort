#!/usr/bin/env bash
gcc -o oes odd-even-sort.c -lpthread
./oes
rm -rf oes
