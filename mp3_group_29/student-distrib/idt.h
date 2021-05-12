#ifndef _IDT_H_
#define _IDT_H_

#include "x86_desc.h"
#include "lib.h"
#include "exception_wrap.h"
#include "interrupt_wrap.h"
#include "systemcall_wrap.h"


#define SYSTEM_CALL 0x80

void idt_init(void);

#endif

