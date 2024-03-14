#!/bin/sh

# Build assembly code generated from bfi

nasm -felf64 -o "$1.o" "$1" && ld "$1.o" -lc && rm -rf "$1.o"
