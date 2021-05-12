#define ASM 1

.text

.globl  keyboard_irq
.globl  rtc_irq
.globl  pit_irq

# interrupt of keyboard
keyboard_irq:
        pushfl                    
        pushal                                        
        call keyboard_handler               
        popal
        popfl                                  
        iret



# interrupt of RTC
rtc_irq:
        pushfl                    
        pushal                                        
        call RTC_handler               
        popal
        popfl                                  
        iret

# interrupt of PIT
pit_irq:
        pushfl                    
        pushal                                        
        call PIT_handler              
        popal
        popfl                                  
        iret