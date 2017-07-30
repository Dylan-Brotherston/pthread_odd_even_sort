#!/usr/bin/env bash
gcc -o oes odd-even-sort.c -lpthread
./oes
sleep 1
rm -rf oes
