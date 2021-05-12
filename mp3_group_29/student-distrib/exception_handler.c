
#include "exception_handler.h"
#define PAGE_FAULT 14


//name of exception handler 
static char* exception[20] = 
{
"Divide Error",
"RESERVED", 
"NMI interrupt", 
"Breakpoint", 
"Overflow", 
"BOUND Range Exceeded", 
"Invalid Opcode", 
"Device Not Available",
"Double Fault",
"Coprocessor Segment Overrun",
"Invalid TSS",
"Segment Not Present",
"Stack-Segment Fault",
"General Protection",
"Page Fault",
"(Intel reserved. Do not use.)",
"x87 FPU Floating-Point Error",
"Alignment Check",
"Machine Check",
"SIMD Floating-Point Exception"
};



/*
 * exception_handler
 *   DESCRIPTION: handler of all the exceptions in the system , when a exception happens,
 *               print out the errors and go to a dead loop
 *               
 *   INPUTS: (uint32_t vect_n , all_regs all_regs , uint32_t err_code, control_save control_save )
 *            vect_n: the exception vector index 
 * 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: make the system trap into a infinite loop
 */
void exception_handler(uint32_t vect_n , all_regs all_regs , uint32_t err_code, control_save control_save ){

    cli();

    //print error
    printf("SYSTEM ERROR:\n");
    printf("EXCEPTIONS%d: %s\n", vect_n, exception[vect_n]);

    global_flag_exception = 1;
    //page initialize fault
    if(vect_n == PAGE_FAULT){
        printf("page fault");
    }
    //infinite loop
    while(1){}


}

