#!/bin/bash

# exit when any command fails
set -e

cd $(dirname $0)

echo "*** Running CPP Check ***"

# run static analysis tools to check code
cppcheck \
    --std=c99 \
    --enable=all  \
    -I include/ \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --suppress=comparePointers \
    src/*.c include/*.h

echo "*** Building for CPP Tidy ***"

cmake \
        -G "Unix Makefiles" \
        -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv.cmake \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -B build_tidy \
        -S src
cmake --build build_tidy

# clang-tidy does not know our target
perl -pi -e 's/-march=rv32imac_zicsr/-D__riscv_xlen=64/' build_tidy/compile_commands.json

echo "*** Running CPP Tidy ***"

clang-tidy \
    -p build_tidy  \
    -header-filter=.* \
    src/*.c | grep -v "x86-64 'interrupt'"  | grep -v "__attribute__((interrupt("
