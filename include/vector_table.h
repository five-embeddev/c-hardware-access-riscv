/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.

   Declarations of interrupt service routine entry points.

   If no implementation is defined then an alias to a default "NOP"
   implementation will be linked instead.

*/

#ifndef VECTOR_TABLE_H
#define VECTOR_TABLE_H

/** Symbol for machine mode vector table - do not call
 */
void riscv_mtvec_table(void) __attribute__((naked));
void riscv_stvec_table(void) __attribute__((naked));
void riscv_utvec_table(void) __attribute__((naked));

/** Machine mode syncronous exception handler.

    @param stack_frame Stack frame of saved registers.
    @retval Stack frame to restore on return.

    This (and the stvec handler) differs to other interrupt handlers as it is called from a
    shim that captures the stack frame. This allows the exception
    handler to inspect the register values when the exception occured,
    and also modify them.

 */
exception_stack_frame_t* riscv_mtvec_exception(exception_stack_frame_t* stack_frame);

/** Supervisor mode syncronous exception handler.

    @param stack_frame Stack frame of saved registers.
    @retval Stack frame to restore on return.

*/
exception_stack_frame_t* riscv_stvec_exception(exception_stack_frame_t* stack_frame);

/** Machine mode software interrupt */
void riscv_mtvec_msi(void) __attribute__((interrupt("machine")));
/** Machine mode timer interrupt */
void riscv_mtvec_mti(void) __attribute__((interrupt("machine")));
/** Machine mode al interrupt */
void riscv_mtvec_mei(void) __attribute__((interrupt("machine")));

/** Supervisor mode software interrupt */
void riscv_mtvec_ssi(void) __attribute__((interrupt("machine")));
/** Supervisor mode timer interrupt */
void riscv_mtvec_sti(void) __attribute__((interrupt("machine")));
/** Supervisor mode al interrupt */
void riscv_mtvec_sei(void) __attribute__((interrupt("machine")));


/** Supervisor mode software interrupt */
void riscv_stvec_ssi(void) __attribute__((interrupt("supervisor")));
/** Supervisor mode timer interrupt */
void riscv_stvec_sti(void) __attribute__((interrupt("supervisor")));
/** Supervisor mode al interrupt */
void riscv_stvec_sei(void) __attribute__((interrupt("supervisor")));

/** User mode software interrupt */
void riscv_utvec_usi(void) __attribute__((interrupt("user")));
/** User mode timer interrupt */
void riscv_utvec_uti(void) __attribute__((interrupt("user")));
/** User mode al interrupt */
void riscv_utvec_uei(void) __attribute__((interrupt("user")));

#ifndef VECTOR_TABLE_MTVEC_PLATFORM_INTS

/* Platform interrupts, bits 16+ of mie, mip etc
 */

/* Platform interrupt 0, bit 16 of mip/mie */
void riscv_mtvec_platform_irq0(void) __attribute__((interrupt("machine")));
/* Platform interrupt 1, bit 17 of mip/mie */
void riscv_mtvec_platform_irq1(void) __attribute__((interrupt("machine")));
/* Platform interrupt 2, bit 18 of mip/mie */
void riscv_mtvec_platform_irq2(void) __attribute__((interrupt("machine")));
/* Platform interrupt 3, bit 19 of mip/mie */
void riscv_mtvec_platform_irq3(void) __attribute__((interrupt("machine")));
/* Platform interrupt 4, bit 20 of mip/mie */
void riscv_mtvec_platform_irq4(void) __attribute__((interrupt("machine")));
/* Platform interrupt 5, bit 21 of mip/mie */
void riscv_mtvec_platform_irq5(void) __attribute__((interrupt("machine")));
/* Platform interrupt 6, bit 22 of mip/mie */
void riscv_mtvec_platform_irq6(void) __attribute__((interrupt("machine")));
/* Platform interrupt 7, bit 23 of mip/mie */
void riscv_mtvec_platform_irq7(void) __attribute__((interrupt("machine")));
/* Platform interrupt 8, bit 24 of mip/mie */
void riscv_mtvec_platform_irq8(void) __attribute__((interrupt("machine")));
/* Platform interrupt 9, bit 25 of mip/mie */
void riscv_mtvec_platform_irq9(void) __attribute__((interrupt("machine")));
/* Platform interrupt 10, bit 26 of mip/mie */
void riscv_mtvec_platform_irq10(void) __attribute__((interrupt("machine")));
/* Platform interrupt 11, bit 27 of mip/mie */
void riscv_mtvec_platform_irq11(void) __attribute__((interrupt("machine")));
/* Platform interrupt 12, bit 28 of mip/mie */
void riscv_mtvec_platform_irq12(void) __attribute__((interrupt("machine")));
/* Platform interrupt 13, bit 29 of mip/mie */
void riscv_mtvec_platform_irq13(void) __attribute__((interrupt("machine")));
/* Platform interrupt 14, bit 30 of mip/mie */
void riscv_mtvec_platform_irq14(void) __attribute__((interrupt("machine")));
/* Platform interrupt 15, bit 31 of mip/mie */
void riscv_mtvec_platform_irq15(void) __attribute__((interrupt("machine")));


#endif// #ifndef VECTOR_TABLE_MTVEC_PLATFORM_INTS


#endif// #ifndef VECTOR_TABLE_H
