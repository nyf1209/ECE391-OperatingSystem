/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
/* this part is contributed by Bill, the overnighter
 * mostly refer from https://wiki.osdev.org/8259_PIC
 * and lecture 10
 */

#include "i8259.h"
#include "lib.h"

// define the necessary constant
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define IRQ_PER_PIC 0x08
#define IRQ2        0x02

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */


/* Initialize the 8259 PIC */
/* i8259_init
 * DESCRIPTION: Initialize the PIC by sending ICWs 
 * one master PIC and one slave PIC are initialized
 * IRQ2 is occupied by slave PIC
 * INPUTS: none
 * OUTPUTS: none
 * SIDEEFFECTS: initialize 2 PICs one master and one slave
 * IRQ 0-7 master PIC
 * IRQ 8-15 slave PIC
 * IRQ 2 is occupied by slave PIC
 */ 
void i8259_init(void) {
    // master PIC 0x20 - 0x27
    // slave PIC 0x28 - 0x2F

    // for test use 
    // unsigned char cached_21;
    cli();

    //save masks

    // mask all bits of PICs
    // when no command, data port allow us to mask
    outb(MASK_ALL, PIC1_DATA);
    outb(MASK_ALL, PIC2_DATA);

    outb(ICW1, PIC1);                    //ICW1 select master pic init
    outb(ICW2_MASTER, PIC1_DATA);        //ICW2 IR0-7 mapped to 0x20-0x27
    outb(ICW3_MASTER, PIC1_DATA);        //The master has a slave on IR2 (IRQ2 cascade to slave)
    outb(ICW4, PIC1_DATA);               //always expect normal EOI

    outb(ICW1, PIC2);                    //ICW1 select slave init
    outb(ICW2_SLAVE, PIC2_DATA);         //ICW2 IR0-7 mapped to 0x28-0x2f
    outb(ICW3_SLAVE, PIC2_DATA);         //a slave on master's IR2
    outb(ICW4, PIC2_DATA);               //always expect normal EOI

    // reassure maks all port
    outb(MASK_ALL, PIC1_DATA);
    outb(MASK_ALL, PIC2_DATA);

    enable_irq(IRQ2);                   //enable the slave PIC

    // for test use 
    // cached_21 = inb(PIC1_DATA);


}

/* disable_irq
 * DESCRIPTION: mask the specified IRQ
 * INPUTS: irq_num
 * OUTPUTS: none
 * SIDEEFFECTS: mask the IRQ specified by irq_num
 */
void disable_irq(uint32_t irq_num) {

    uint16_t port;
    uint8_t  value, pt_value;



    if(irq_num < IRQ_PER_PIC){
        port = PIC1_DATA;
    }
    else{
        port = PIC2_DATA;
        irq_num -= IRQ_PER_PIC;
    }
    /* set the mask to 1 */
    //      inside port OR 1
    pt_value = inb(port);
    value = pt_value | (1 << irq_num);
    outb(value, port);
    // restore_flags(flags);

}

/* enable_irq 
 * DESCRIPTION: unmask the specified IRQ
 * INPUTS: irq_num
 * OUTPUTS: none
 * SIDEEFFECTS: unmask the IRQ specified by irq_num
 */
void enable_irq(uint32_t irq_num) {
    uint8_t value, pt_value;
    uint16_t port;
    cli();

    if(irq_num < IRQ_PER_PIC){
        port = PIC1_DATA;
    }
    else{
        port = PIC2_DATA;
        irq_num -= IRQ_PER_PIC;
    }
    /* set the mask to 0  */
    //     inside port AND 0
    pt_value = inb(port);
    value = inb(port) & ~(1 << irq_num);
    outb(value,port);
    // restore_flags(flags);

}

/* 
 * send_eoi
 * DESCRIPTION: send end of interruption 
 * INPUTS: none
 * OUTPUTS: none
 * SIDEEFFECTS: send the EOI command to the PIC port
 */ 
void send_eoi(uint32_t irq_num) {
    if(irq_num >= IRQ_PER_PIC){ // from slave
        outb(EOI, PIC2_COMMAND);
    }
    // from master
    outb(EOI, PIC1_COMMAND);
}
