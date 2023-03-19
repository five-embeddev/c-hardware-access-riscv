#!/bin/bash

ALL_SRC=`find src/ include/ -name '*.h' -or -name '*.c' -type f `
echo "FORMAT: ${ALL_SRC}"
clang-format -i ${ALL_SRC}
