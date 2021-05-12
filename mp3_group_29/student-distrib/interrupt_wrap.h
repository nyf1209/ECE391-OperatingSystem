#include "keyboard.h"
#include "rtc.h"
#include "pit.h"

#ifndef _INTERRUPT_WRAP_H
#define _INTERRUPT_WRAP_H

#ifndef ASM

// interrupt for keyboard to x86
extern void keyboard_irq();
// interrupt for rtc to x86
extern void rtc_irq();
// interrupt for pit
extern void pit_irq();

#endif
#endif
