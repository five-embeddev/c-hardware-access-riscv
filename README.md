# C Hardware Access for RISC-V

Example for accessing low level RISC-V hardware with C.

The code will initialize the C run-time environment and jump to main,
enable and handle timer interrupts, and initiate and handle ecall
exceptions.

More details at either:

- https://five-embeddev.com/
- https://philmulholland.medium.com/ 

## Files

Source Code

- src/startup.c - Entry/Startup/Runtime
- src/main.c - Main Program, Interrupt Handlers, Exception Handler
- src/timer.c / include/timer.h - Timer Driver
- src/vector_table.c / include/vector_table.h - Interrupt Vector Table
- include/riscv-csr.h / include/riscv-abi.h / include/riscv-interrupts.h - RISC-V Hardware Support

Platform IO:

- platformio.ini - IDE Configuration
- post_build.py - Example post build to create disassembly file

CMake Build Environment:

- build_cmake.sh
- cmake/riscv.cmake
- src/CMakeLists.txt

GitHub/Docker CI

- Dockerfile
- docker_entrypoint.sh
- action.yml
- .github/workflows/main.yml

Source Code Check/Formatting

- check.sh - Run clang-tidy and cppcheck 
- format - Run clang-format


## Building

Platform IO or CMake is used to build the project locally.

Docker is used to build the project on github.

### PlatformIO

Platform IO will download the required toolchain to build and QEMU to run the target locally. This is using q quite an old GCC (8.3.0).

~~~
platformio run --target build
~~~

### CMake

To build with CMake a RISC-V cross compiler must be installed. It will look for one of:

- riscv-none-embed-gcc
- riscv-none-elf-gcc
- riscv32-unknown-elf-gcc

The tools can be found at: https://xpack.github.io/dev-tools/riscv-none-elf-gcc/

~~~
cmake \
        -G "Unix Makefiles" \
        -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv.cmake \
        -B build \
        -S src 
cmake --build build
~~~

### Docker

The included dockerfile installs the [xpack RISC-V GCC
toolchain](https://xpack.github.io/riscv-none-embed-gcc/) and uses
CMake to compile the project. This is using a recent GCC (12.2).

To build locally using the docker file use these commands:

~~~
docker build --tag=c_hw_access_riscv:latest .
docker run \
       -it \
       -v `pwd`:/work \
       c_hw_access_riscv:latest \
       /work \
       ./src

~~~
