#!/bin/bash

if ! [ -e ./build/ ]; then
	mkdir ./build/;
fi

clang main.c -o ./build/bz2_exec && echo "Build successful";
