/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.

*/

#include <stdint.h>
#include <stddef.h>

#include "riscv-abi.h"

enum {
    VECTOR_TABLE_ALIGNMENT = 256,
};

// Makes use of GCC interrupt and weak reference/alias attributes
// https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes
// https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Function-Attributes.html#RISC-V-Function-Attributes

// Vector table - not to be called.
void riscv_mtvec_table() __attribute__((naked, section(".text.mtvec_table"), aligned(VECTOR_TABLE_ALIGNMENT)));
void riscv_stvec_table() __attribute__((naked, section(".text.stvec_table"), aligned(VECTOR_TABLE_ALIGNMENT)));
void riscv_utvec_table() __attribute__((naked, section(".text.utvec_table"), aligned(VECTOR_TABLE_ALIGNMENT)));

// Default "NOP" implementations
static exception_stack_frame_t* riscv_nop_exception(exception_stack_frame_t* stack_frame);
static void riscv_nop_machine(void) __attribute__((interrupt("machine")));
static void riscv_nop_supervisor(void) __attribute__((interrupt("supervisor")));
static void riscv_nop_user(void) __attribute__((interrupt("user")));

// Weak alias to the "NOP" implementations. If another function
exception_stack_frame_t* riscv_mtvec_exception(exception_stack_frame_t* stack_frame)
    __attribute__((weak, alias("riscv_nop_exception")));
void riscv_mtvec_msi(void) __attribute__((interrupt("machine"),
                                          weak,
                                          alias("riscv_nop_machine")));
void riscv_mtvec_mti(void) __attribute__((interrupt("machine"),
                                          weak,
                                          alias("riscv_nop_machine")));
void riscv_mtvec_mei(void) __attribute__((interrupt("machine"),
                                          weak,
                                          alias("riscv_nop_machine")));
void riscv_mtvec_ssi(void) __attribute__((interrupt("supervisor"),
                                          weak,
                                          alias("riscv_nop_machine")));
void riscv_mtvec_sti(void) __attribute__((interrupt("supervisor"),
                                          weak,
                                          alias("riscv_nop_machine")));
void riscv_mtvec_sei(void) __attribute__((interrupt("supervisor"),
                                          weak,
                                          alias("riscv_nop_machine")));

exception_stack_frame_t* riscv_stvec_exception(exception_stack_frame_t* stack_frame)
    __attribute__((weak, alias("riscv_nop_exception")));
void riscv_stvec_ssi(void) __attribute__((interrupt("supervisor"),
                                          weak,
                                          alias("riscv_nop_supervisor")));
void riscv_stvec_sti(void) __attribute__((interrupt("supervisor"),
                                          weak,
                                          alias("riscv_nop_supervisor")));
void riscv_stvec_sei(void) __attribute__((interrupt("supervisor"),
                                          weak,
                                          alias("riscv_nop_supervisor")));

void riscv_utvec_usi(void) __attribute__((interrupt("user"),
                                          weak,
                                          alias("riscv_nop_user")));
void riscv_utvec_uti(void) __attribute__((interrupt("user"),
                                          weak,
                                          alias("riscv_nop_user")));
void riscv_utvec_uei(void) __attribute__((interrupt("user"),
                                          weak,
                                          alias("riscv_nop_user")));

#ifndef VECTOR_TABLE_MTVEC_PLATFORM_INTS

void riscv_mtvec_platform_irq0(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq1(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq2(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq3(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq4(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq5(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq6(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq7(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq8(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq9(void) __attribute__((interrupt("machine"),
                                                    weak,
                                                    alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq10(void) __attribute__((interrupt("machine"),
                                                     weak,
                                                     alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq11(void) __attribute__((interrupt("machine"),
                                                     weak,
                                                     alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq12(void) __attribute__((interrupt("machine"),
                                                     weak,
                                                     alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq13(void) __attribute__((interrupt("machine"),
                                                     weak,
                                                     alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq14(void) __attribute__((interrupt("machine"),
                                                     weak,
                                                     alias("riscv_nop_machine")));
void riscv_mtvec_platform_irq15(void) __attribute__((interrupt("machine"),
                                                     weak,
                                                     alias("riscv_nop_machine")));

#endif// #ifndef VECTOR_TABLE_MTVEC_PLATFORM_INTS

// Helper macros to load/save

/** @def SAVE_REG
    @brief     Macro wrapper around assembler to save register to the stack frame struct.
    @param REG Name of the abi register
*/
#if __riscv_xlen == 64
#define SAVE_REG(REG)                                                       \
    __asm__ volatile(                                                       \
        "sd	" #REG " , %0(sp); "                                            \
        : /* no output */                                                   \
        : /* immediate input */ "i"(offsetof(exception_stack_frame_t, REG)) \
        : /* no sclobber */)
#else
#define SAVE_REG(REG)                                                       \
    __asm__ volatile(                                                       \
        "sw	" #REG " , %0(sp); "                                            \
        : /* no output */                                                   \
        : /* immediate input */ "i"(offsetof(exception_stack_frame_t, REG)) \
        : /* no clobber */)
#endif

/** @def LOAD_REG
    @brief     Macro wrapper around assembler to restore a register from the stack frame struct.
    @param REG Name of the abi register
*/
#if __riscv_xlen == 64
#define LOAD_REG(N)                                                         \
    __asm__ volatile(                                                       \
        "ld	" #REG " , %0(sp); "                                            \
        : /* no output */                                                   \
        : /* immediate input */ "i"(offsetof(exception_stack_frame_t, REG)) \
        : /* no clobber */)
#else
#define LOAD_REG(REG)                                                       \
    __asm__ volatile(                                                       \
        "lw	" #REG " , %0(sp); "                                            \
        : /* no output */                                                   \
        : /* immediate input */ "i"(offsetof(exception_stack_frame_t, REG)) \
        : /* no clobber */)
#endif

#if defined(__riscv_32e)
// Ignore registers not implemented in rv32e
#define SAVE_REG_NOT_E(REG)
#define LOAD_REG_NOT_E(REG)
#else
// Handle all registers
#define SAVE_REG_NOT_E SAVE_REG
#define LOAD_REG_NOT_E LOAD_REG
#endif

/** @def EXCEPTION_SAVE_STACK
 *  @brief Save registers before calling into C
 */
#define EXCEPTION_SAVE_STACK                                         \
    /* Move stack frame */                                           \
    __asm__ volatile(                                                \
        "addi sp, sp, -%0;"                                          \
        : /* no output */                                            \
        : /* immediate input */ "i"(sizeof(exception_stack_frame_t)) \
        : /* no clobber */);                                         \
    /* Ignore 0: zero */                                             \
    SAVE_REG(ra);                                                    \
    /* Ignore 2,3,4: Stack, global, thread pointers */               \
    SAVE_REG(t0);                                                    \
    SAVE_REG(t1);                                                    \
    SAVE_REG_NOT_E(t2);                                              \
    SAVE_REG(a0);                                                    \
    SAVE_REG(a1);                                                    \
    SAVE_REG(a2);                                                    \
    SAVE_REG(a3);                                                    \
    SAVE_REG_NOT_E(a4);                                              \
    SAVE_REG_NOT_E(a5);                                              \
    SAVE_REG_NOT_E(a6);                                              \
    SAVE_REG_NOT_E(a7);                                              \
    SAVE_REG_NOT_E(t3);                                              \
    SAVE_REG_NOT_E(t4);                                              \
    SAVE_REG_NOT_E(t5);                                              \
    SAVE_REG_NOT_E(t6)

/** @def EXCEPTION_RESTORE_STACK
 *  @brief Restore registers after returning from C
 */
#define EXCEPTION_RESTORE_STACK                                      \
    /* Ignore 0: zero */                                             \
    LOAD_REG(ra);                                                    \
    /* Ignore 2,3,4: Stack, global, thread pointers */               \
    LOAD_REG(t0);                                                    \
    LOAD_REG(t1);                                                    \
    LOAD_REG_NOT_E(t2);                                              \
    LOAD_REG(a0);                                                    \
    LOAD_REG(a1);                                                    \
    LOAD_REG(a2);                                                    \
    LOAD_REG(a3);                                                    \
    LOAD_REG_NOT_E(a4);                                              \
    LOAD_REG_NOT_E(a5);                                              \
    LOAD_REG_NOT_E(a6);                                              \
    LOAD_REG_NOT_E(a7);                                              \
    LOAD_REG_NOT_E(t3);                                              \
    LOAD_REG_NOT_E(t4);                                              \
    LOAD_REG_NOT_E(t5);                                              \
    LOAD_REG_NOT_E(t6);                                              \
    /* Restore stack frame */                                        \
    __asm__ volatile(                                                \
        "addi sp, sp, %0;"                                           \
        : /* no output */                                            \
        : /* immediate input */ "i"(sizeof(exception_stack_frame_t)) \
        : /* no clobber */)


#pragma GCC push_options

// Ensure all ISR tables are aligned.
#pragma GCC optimize("align-functions=4")

// NOLINTBEGIN (hicpp-no-assembler)
// hicpp-no-assembler: This cannot be implemented without assembler.

// Ensure the vector table is aligned.
// The bottom 4 bits of MTVEC are ignored - so align to 16 bytes

// Vector table. Do not call!
// Possible entries defined by mcause table
// http://five-embeddev.com/riscv-isa-manual/latest/machine.html#sec:mcause
//
// When vectored interrupts are enabled, interrupt cause 0, which
// corresponds to user-mode software interrupts, are vectored to the
// same location as synchronous exceptions. This ambiguity does not
// arise in practice, since user-mode software interrupts are either
// disabled or delegated to user mode.
void riscv_mtvec_table() {
    __asm__ volatile(
        ".org  riscv_mtvec_table + 0*4;"
        "jal   zero,.handle_mtvec_exception;" /* 0  */
        ".org  riscv_mtvec_table + 1*4;"
        "jal   zero,riscv_mtvec_ssi;" /* 1  */
        ".org  riscv_mtvec_table + 3*4;"
        "jal   zero,riscv_mtvec_msi;" /* 3  */
        ".org  riscv_mtvec_table + 5*4;"
        "jal   zero,riscv_mtvec_sti;" /* 5  */
        ".org  riscv_mtvec_table + 7*4;"
        "jal   zero,riscv_mtvec_mti;" /* 7  */
        ".org  riscv_mtvec_table + 9*4;"
        "jal   zero,riscv_mtvec_sei;" /* 9  */
        ".org  riscv_mtvec_table + 11*4;"
        "jal   zero,riscv_mtvec_mei;" /* 11 */
#ifndef VECTOR_TABLE_MTVEC_PLATFORM_INTS
        ".org  riscv_mtvec_table + 16*4;"
        "jal   riscv_mtvec_platform_irq0;"
        "jal   riscv_mtvec_platform_irq1;"
        "jal   riscv_mtvec_platform_irq2;"
        "jal   riscv_mtvec_platform_irq3;"
        "jal   riscv_mtvec_platform_irq4;"
        "jal   riscv_mtvec_platform_irq5;"
        "jal   riscv_mtvec_platform_irq6;"
        "jal   riscv_mtvec_platform_irq7;"
        "jal   riscv_mtvec_platform_irq8;"
        "jal   riscv_mtvec_platform_irq9;"
        "jal   riscv_mtvec_platform_irq10;"
        "jal   riscv_mtvec_platform_irq11;"
        "jal   riscv_mtvec_platform_irq12;"
        "jal   riscv_mtvec_platform_irq13;"
        "jal   riscv_mtvec_platform_irq14;"
        "jal   riscv_mtvec_platform_irq15;"
#endif
        ".handle_mtvec_exception:");

    EXCEPTION_SAVE_STACK;

    __asm__ volatile(
        // Current stack pointer
        // Save to a0
        "mv a0, sp;"

        // Jump to exception hander
        // Pass
        "jal   ra,riscv_mtvec_exception;" /* 0  */

        // Restore stack pointer from return value (a0)
        "mv sp, a0;");

    EXCEPTION_RESTORE_STACK;

    // Return
    __asm__ volatile("mret;");
}
// Vector table. Do not call!
// See scause table for possible entries.
// http://five-embeddev.com/riscv-isa-manual/latest/supervisor.html#sec:scause
void riscv_stvec_table() {
    __asm__ volatile(
        ".org  riscv_stvec_table + 0*4;"
        "jal   zero,.handle_stvec_exception;" /* 0  */
        ".org  riscv_stvec_table + 1*4;"
        "jal   zero,riscv_stvec_ssi;" /* 1  */
        ".org  riscv_stvec_table + 5*4;"
        "jal   zero,riscv_stvec_sti;" /* 5  */
        ".org  riscv_stvec_table + 9*4;"
        "jal   zero,riscv_stvec_sei;" /* 9  */
        ".handle_stvec_exception:");

    EXCEPTION_SAVE_STACK;

    __asm__ volatile(
        // Current stack pointer
        // Save to a0
        "mv a0, sp;"

        // Jump to exception hander
        // Pass
        "jal   ra,riscv_stvec_exception;" /* 0  */

        // Restore stack pointer from return value (a0)
        "mv sp, a0;");

    EXCEPTION_RESTORE_STACK;

    // Return
    __asm__ volatile("sret;");
}
// Vector table. Do not call!
void riscv_utvec_table() {
    __asm__ volatile(
        ".org  riscv_utvec_table + 0*4;"
        "jal   zero,riscv_utvec_usi;" /* 0  */
        ".org  riscv_utvec_table + 4*4;"
        "jal   zero,riscv_utvec_uti;" /* 4  */
        ".org  riscv_utvec_table + 8*4;"
        "jal   zero,riscv_utvec_uei;" /* 8  */
    );
}

// NOLINTEND (hicpp-no-assembler)

static exception_stack_frame_t* riscv_nop_exception(exception_stack_frame_t* stack_frame) {
    // Nop exception handler
    return stack_frame;
}

static void riscv_nop_machine(void) {
    // Nop machine mode interrupt.
}
static void riscv_nop_supervisor(void) {
    // Nop supervisor mode interrupt.
}
static void riscv_nop_user(void) {
    // Nop user mode interrupt.
}

#pragma GCC pop_options
