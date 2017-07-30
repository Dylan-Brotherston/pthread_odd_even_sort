#!/usr/bin/env bash
gcc -o test test.c -lpthread
./test
rm -rf test
rm -rf t
