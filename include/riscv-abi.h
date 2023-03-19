/*
   RISC-V ABI.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

*/

#ifndef RISCV_ABI_H
#define RISCV_ABI_H

// Define a sized type for integer registers
#if __riscv_xlen == 32
typedef uint32_t uint_reg_t;
#elif __riscv_xlen == 64
typedef uint64_t uint_reg_t;
#else
#error "Unknown XLEN for ABI register size."
#endif


/** Define the stack frame saved in ecall handlers
 */
typedef struct {
    // Global registers - saved if
    uint_reg_t ra;// return address (global)
#if 0
    // SP Can be restored via pop increment
    uint_reg_t sp;   // stack pointer  (global)    
    // These do not need to saved - will no change as we are a single binary.
    // (Would need to change for a separate binary monitor)
    uint_reg_t gp;   // global pointer (global)
    uint_reg_t tp;   // thread pointer (global)
#endif
    // Saved as they will not be saved by callee
    uint_reg_t t0;// temporary register 0 (not saved)
    uint_reg_t t1;// temporary register 1 (not saved)
#if !defined(__riscv_32e)
    uint_reg_t t2;// temporary register 2 (not saved)
#endif
#if 0
    // These do not need to be saved as any function called will save them
    uint_reg_t s0;   // saved register 0 (callee saved)
    uint_reg_t s1;   // saved register 1 (callee saved)
#endif
    // Arguments are saved for reference by 'ecall' handler
    // and as any function called expects them to be saved.
    uint_reg_t a0;// function argument/return value 0 (caller saved)
    uint_reg_t a1;// function argument/return value 1 (caller saved)
    uint_reg_t a2;// function argument 2 (caller saved)
    uint_reg_t a3;// function argument 3 (caller saved)
#if !defined(__riscv_32e)
    uint_reg_t a4;// function argument 4 (caller saved)
    uint_reg_t a5;// function argument 5 (caller saved)
    uint_reg_t a6;// function argument 6 (caller saved)
    uint_reg_t a7;// function argument 7 (caller saved)
#endif
#if 0
    // These do not need to be saved as any function called will save them
    uint_reg_t s2;   // saved register 2  (callee saved)
    uint_reg_t s3;   // saved register 3  (callee saved)
    uint_reg_t s4;   // saved register 4  (callee saved)
    uint_reg_t s5;   // saved register 5  (callee saved)
    uint_reg_t s6;   // saved register 6  (callee saved)
    uint_reg_t s7;   // saved register 7  (callee saved)
    uint_reg_t s8;   // saved register 8  (callee saved)
    uint_reg_t s9;   // saved register 9  (callee saved)
    uint_reg_t s10;  // saved register 10 (callee saved)
    uint_reg_t s11;  // saved register 11 (callee saved)
#endif
#if !defined(__riscv_32e)
    // Saved as they will not be saved by callee
    uint_reg_t t3;// temporary register 3 (not saved)
    uint_reg_t t4;// temporary register 4 (not saved)
    uint_reg_t t5;// temporary register 5 (not saved)
    uint_reg_t t6;// temporary register 6 (not saved)
#endif
} exception_stack_frame_t;

#if defined(__riscv_32e)
#define RISCV_REG_LAST_ARG a3
#else
#define RISCV_REG_LAST_ARG a7
#endif

#endif /* RISCV_ABI_H */
