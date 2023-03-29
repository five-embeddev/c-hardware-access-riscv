/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/)

*/

#include <stdint.h>

// Generic C function pointer.
typedef void (*function_t)(void);

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cppbugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
// We are accessing global regions defined by the linker that are written only once at startup.

// These symbols are defined by the linker script.
// See linker.lds
extern uint8_t bss_target_start;
extern uint8_t bss_target_end;
extern const uint8_t data_source_start;
extern uint8_t data_target_start;
extern uint8_t data_target_end;
extern const uint8_t itim_source_start;
extern uint8_t itim_target_start;
extern uint8_t itim_target_end;
extern const uint8_t lim_source_start;
extern uint8_t lim_target_start;
extern uint8_t lim_target_end;

extern const function_t __preinit_array_start;
extern const function_t __preinit_array_end;
extern const function_t __init_array_start;
extern const function_t __init_array_end;
extern const function_t __fini_array_start;
extern const function_t __fini_array_end;

// This function will be placed by the linker script according to the section
// Raw function 'called' by the CPU with no runtime.
void _enter(void) __attribute__((naked, section(".text.init.enter")));

// Entry and exit points as C functions.

void _start(void) __attribute__((noreturn));
void _Exit(int exit_code) __attribute__((noreturn, noinline));

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cppbugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

// Standard entry point, no arguments.
extern int main(void);

// NOLINTBEGIN (hicpp-no-assembler)

// The linker script will place this in the reset entry point.
// It will be 'called' with no stack or C runtime configuration.
// NOTE - this only supports a single hart.
// tp will not be initialized
void _enter(void) {

    // Setup SP and GP
    // This needs to be done in assembler as there is no stack.
    // The locations are defined in the linker script
    __asm__ volatile(
        ".option push;"
        // The 'norelax' option is critical here.
        // Without 'norelax' the global pointer will
        // be loaded relative to the global pointer!
        ".option norelax;"
        "la    gp, __global_pointer$;"
        ".option pop;"
        "la    sp, _sp;"
        "jal   zero, _start;");
    // This point will not be executed, _start() will be called with no return.
}

// NOLINTEND (hicpp-no-assembler)

// At this point we have a stack and global pointer, but no access to global variables.
void _start(void) {

    // Init memory regions
    // Clear the .bss section (global variables with no initial values)
    // Includes thread local .tbss
    __builtin_memset((void*)&bss_target_start,
                     0,
                     (&bss_target_end - &bss_target_start));

    // Initialize the .data section (global variables with initial values)
    // Includes thread local .tdata
    __builtin_memcpy((void*)&data_target_start,
           (const void*)&data_source_start,
           (&data_target_end - &data_target_start));
    // Initialize the .itim section (code moved from flash to SRAM to improve performance)
    __builtin_memcpy((void*)&itim_target_start,
           (const void*)&itim_source_start,
           (&itim_target_end - &itim_target_start));
    // Initialize the .lim section (code moved from flash to L2 Cache to improve performance)
    __builtin_memcpy((void*)&lim_target_start,
           (const void*)&lim_source_start,
           (&lim_target_end - &lim_target_start));

    // Call constructors
    for (const function_t* entry = &__preinit_array_start;
         entry < &__preinit_array_end;
         ++entry) {
        (*entry)();
    }
    for (const function_t* entry = &__init_array_start;
         entry < &__init_array_end;
         ++entry) {
        (*entry)();
    }

    int main_return_code = main();

    // Call destructors
    for (const function_t* entry = &__fini_array_start;
         entry < &__fini_array_end;
         ++entry) {
        (*entry)();
    }


    _Exit(main_return_code);
}


// This should never be called. Busy loop with the CPU in idle state.
void _Exit(int exit_code) {
    (void)exit_code;
    // Halt
    while (1) {
        __asm__ volatile("wfi");// NOLINT (hicpp-no-assembler)
    }
}
