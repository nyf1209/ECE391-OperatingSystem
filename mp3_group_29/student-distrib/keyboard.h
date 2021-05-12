/*
 * This portion of code is contributed by Bill, correct the code if I was wrong
 * 
 * for reference check: https://wiki.osdev.org/Keyboard
 * and https://wiki.osdev.org/%228042%22_PS/2_Controller
 * 
 * Assuming we are using "8042" PS/2 controller 
 * 
 */ 
#ifndef _KEYBOARD_H
#define _KEYBOARD_H


#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "cursor.h"
#include "terminal.h"
#include "pit.h"
#include "term_switch.h"
#include "paging.h"

extern int8_t line_buf1[128];    /* this 128 here represents the maximum length of the line buffer*/
extern int8_t line_buf2[128];
extern int8_t line_buf3[128];  
// extern int8_t key_buf[128];
extern int curr_idx1;
extern int curr_idx2;
extern int curr_idx3;
extern int CAPS_state;
extern int read_flag1;
extern int read_flag2;
extern int read_flag3;


/* Initialize the keyboard */
void keyboard_init(void);

/* The keyboard interrupt handler, called by do_irq */
void keyboard_handler(void);


/* a helper function used to transform scan code to ascii char */
uint8_t convert_to_ascii(uint8_t scan);

/* helper function to print the correct characters to the screen */



#endif

