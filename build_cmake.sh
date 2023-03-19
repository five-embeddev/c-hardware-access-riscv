#!/bin/bash
cmake \
        -G "Unix Makefiles" \
        -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv.cmake \
        -B build \
        -S src 
cmake --build build

