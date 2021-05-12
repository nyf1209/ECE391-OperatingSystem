#include "exception_handler.h"

#ifndef _EXCEPTION_WRAP_H
#define _EXCEPTION_WRAP_H

#ifndef ASM

// excepton handlers to x86

extern void divide_error_exception();
extern void debug_exception();
extern void NMI_interrupt();
extern void breakpoint_exception();
extern void overflow_exception();
extern void bound_range_exceeded_exception();
extern void invalid_opcode_exception();
extern void divice_not_avaliable_exception();
extern void double_fault_exception();
extern void coprocessor_segment_overrun();
extern void invalid_TSS_exception();
extern void segment_not_present();
extern void stack_fault_exception();
extern void general_protection_exception();
extern void page_fault_exception();
extern void x87_FPU_floating_point_error();
extern void alignment_check_exception();
extern void machine_check_exception();
extern void SIMD_floating_point_exception();

#endif
#endif

