
#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

#include "types.h"
#include "lib.h"
//struct to save all registers saved before interrupt
typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
 }all_regs;


//struct to store control save register
 typedef struct {
    uint32_t return_IREP;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} control_save;

int global_flag_exception;

//exception handler to handle the exception from the system
void exception_handler(uint32_t vect_n , all_regs all_regs , uint32_t err_code, control_save control_save );



#endif
