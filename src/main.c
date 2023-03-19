/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive qemu, but should not have any
   dependencies to any particular implementation.

*/

// RISC-V CSR definitions and access classes
#include "riscv-csr.h"
#include "riscv-interrupts.h"
#include "riscv-abi.h"
#include "timer.h"
#include "vector_table.h"

// NOLINTBEGIN(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
// The _enter() function is referenced to implement a soft reset.
extern void _enter(void);
// NOLINTEND(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
// cppcoreguidelines-avoid-non-const-global-variables: Using global variables here as they are easier to watch in the debugger.

// Global to hold current timestamp, written in MTI handler.
static volatile uint64_t timestamp = 0;
// Expect this to increment one time per second -
// inside exception handler, after each return of MTI handler.
static volatile uint64_t ecall_count = 0;

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/** Dummy call IDs for calling into exception handler
 */
typedef enum {
    ECALL_INCREMENT_COUNT = 1,
    ECALL_DUMMY = 2,
} ecall_function_id_t;

// NOLINTBEGIN (hicpp-no-assembler)
// hicpp-no-assembler: Use of assembler is unavoidable. Wrapping it in helper functions.

/** Simple wrapper for the WFI (Wait for Interrupt) instruction.
 */
static inline void riscv_wfi(void) {
    __asm__ volatile("wfi");
}
// NOLINTEND (hicpp-no-assembler)

/** Wrapper for the ECALL instruction, allow passsing args to the exception handler.
 * @param function_id  Function identifier of the call. Select action to be performed in handler.
 * @param param0       First argument of the call.
 * @retval             Value returned from called function.
 */
static unsigned long int riscv_ecall(ecall_function_id_t function_id, unsigned long int param0);


int main(void) {

    // Global interrupt disable
    csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK);
    csr_write_mie(0);

    // Setup the IRQ handler entry point, set the mode to vectored
    csr_write_mtvec(((uint_xlen_t)riscv_mtvec_table) | ((uint_xlen_t)RISCV_MTVEC_MODE_VECTORED));

    // Enable MIE.MTI
    csr_set_bits_mie(MIE_MTI_BIT_MASK);

    // Global interrupt enable
    csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK);

    // Setup timer for 1 second interval
    timestamp = mtimer_get_raw_time();
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));

    // Keep a local counter of how many times `ecall` has been executed.
    unsigned int local_ecallcount = 0;
    // Busy loop
    do {
        // Wait for timer interrupt
        riscv_wfi();
        // Try a synchronous exception - ask the exception handler to increment our counter.
        local_ecallcount = riscv_ecall(ECALL_INCREMENT_COUNT, local_ecallcount);
    } while (1);

    // Will not reach here
    return 0;
}


// The 'riscv_mtvec_mti' function is added to the vector table by the vector_table.c
void riscv_mtvec_mti(void) {
    // Timer exception, re-program the timer for a one second tick.
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
    timestamp = mtimer_get_raw_time();
}

// The 'riscv_mtvec_exception' function is added to the vector table by the vector_table.c
// This function looks at the cause of the exception,
// if it is an 'ecall' instruction then increment a global counter.
exception_stack_frame_t* riscv_mtvec_exception(exception_stack_frame_t* stack_frame) {
    uint_xlen_t this_cause = csr_read_mcause();
    uint_xlen_t this_pc = csr_read_mepc();
    // uint_xlen_t this_value = csr_read_mtval();
    switch (this_cause) {
    case RISCV_EXCP_ENVIRONMENT_CALL_FROM_M_MODE: {
        // Counter that can be observed in global watch
        ecall_count++;
        // Dummy syscall handling...
        unsigned long int ecall_id = stack_frame->RISCV_REG_LAST_ARG;
        if (ecall_id == ECALL_INCREMENT_COUNT) {
            unsigned long int arg0 = stack_frame->a0;
            stack_frame->a0 = arg0 + 1;
        }
        // Make sure the return address is the instruction AFTER ecall
        csr_write_mepc(this_pc + 4);
        break;
    }
    case RISCV_EXCP_ENVIRONMENT_CALL_FROM_U_MODE:
    case RISCV_EXCP_ENVIRONMENT_CALL_FROM_S_MODE:
        // Make sure the return address is the instruction AFTER ecall
        csr_write_mepc(this_pc + 4);
        break;
    default:
        // All other system calls.
        // Unexpected calls, do a soft reset by returning to the startup function.
        csr_write_mepc((uint_xlen_t)_enter);
        break;
    }
    return stack_frame;
}

// NOLINTBEGIN (hicpp-no-assembler)
// hicpp-no-assembler: Use of assembler is unavoidable. Wrapping it in helper functions.

static unsigned long int riscv_ecall(ecall_function_id_t function_id, unsigned long int param0) {
    // Pass and return value register.
    register unsigned long a0 __asm__("a0") = param0;
    // Use the last argument register as call ID
#ifdef __riscv_32e
    // RV32E only has a0-a3 argument registers
    register unsigned long ecall_id __asm__("a3") = function_id;
#else
    // Non -e variants have has a0-a7 argument registers
    register unsigned long ecall_id __asm__("a7") = function_id;
#endif
    __asm__ volatile("ecall "
                     : "+r"(a0) /* output : register */
                     : "r"(a0), "r"(ecall_id) /* input : register*/
                     : /* clobbers: none */);
    return a0;
}
// NOLINTEND (hicpp-no-assembler)
