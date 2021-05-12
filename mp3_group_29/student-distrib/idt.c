#include "idt.h"

/* void init_idt();
 * Inputs: void
 * Return Value: none
 * Function: initialize the Interrupt descriptor table*/
void idt_init()
{
    int i;
    for (i = 0; i < NUM_VEC; i++)
    {
        idt[i].seg_selector = KERNEL_CS;

        //follow idt descriptor table
        idt[i].reserved4 = 0;
        
        idt[i].reserved3 = 0;
        
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1; // for 32 bits
        idt[i].reserved0 = 0;
        //if it is system call
        if (i == SYSTEM_CALL)
        {
            idt[i].dpl = 3;
        }
        else
        {
            idt[i].dpl = 0;
        }
        //enable the idt
        idt[i].present = 1;
    }
    //exceptions 0x00-0x1F
    SET_IDT_ENTRY(idt[0x00], divide_error_exception);
    SET_IDT_ENTRY(idt[0x01], debug_exception);
    SET_IDT_ENTRY(idt[0x02], NMI_interrupt);
    SET_IDT_ENTRY(idt[0x03], breakpoint_exception);
    SET_IDT_ENTRY(idt[0x04], overflow_exception);
    SET_IDT_ENTRY(idt[0x05], bound_range_exceeded_exception);
    SET_IDT_ENTRY(idt[0x06], invalid_opcode_exception);
    SET_IDT_ENTRY(idt[0x07], divice_not_avaliable_exception);
    SET_IDT_ENTRY(idt[0x08], double_fault_exception);
    SET_IDT_ENTRY(idt[0x09], coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[0x0A], invalid_TSS_exception);
    SET_IDT_ENTRY(idt[0x0B], segment_not_present);
    SET_IDT_ENTRY(idt[0x0C], stack_fault_exception);
    SET_IDT_ENTRY(idt[0x0D], general_protection_exception);
    SET_IDT_ENTRY(idt[0x0E], page_fault_exception);
    //interrupt 15 reserved
    SET_IDT_ENTRY(idt[0x10], x87_FPU_floating_point_error);
    SET_IDT_ENTRY(idt[0x11], alignment_check_exception);
    SET_IDT_ENTRY(idt[0x12], machine_check_exception);
    SET_IDT_ENTRY(idt[0x13], SIMD_floating_point_exception);
    //interrupt 20-31 reserved

    SET_IDT_ENTRY(idt[0x20], pit_irq);
    SET_IDT_ENTRY(idt[0x21], keyboard_irq);
    SET_IDT_ENTRY(idt[0x28], rtc_irq);
    SET_IDT_ENTRY(idt[0x80], system_call);

    lidt(idt_desc_ptr); // load the idt
}

