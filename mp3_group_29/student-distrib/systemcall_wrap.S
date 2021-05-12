
#define ASM     1

.text

.globl  system_call

# system_call
system_call:
        pushfl                  
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        pushl   %ebp
        pushl   %esp
        pushl   %esi
        pushl   %edi                               
        /*call systemcall_handler */  
        cmpl    $1, %eax        /* check if the index in eax is greater than or equal to 1*/ 
        jl      ret_invalid
        cmpl    $10, %eax       /* check if the index in eax is smaller than or equal to 10*/ 
        jg      ret_invalid
        pushl   %edx 
        pushl   %ecx 
        pushl   %ebx
        decl    %eax        /* decrement eax by 1 so the first index (for halt) is 0 instead of one*/
        jmp     *jump_table(, %eax, 4)   /* multiply by 4 to get the offset of the index for jump table*/     

sys_halt:
        call    halt_handler
        jmp     done

sys_execute:
        call    execute_handler
        jmp     done

sys_read:
        call    read
        jmp     done

sys_write:
        call    write
        jmp     done

sys_open:
        call    open
        jmp     done

sys_close:
        call    close
        jmp     done

sys_getargs:
        call    getargs_handler
        jmp     done

sys_vidmap:
        call    vidmap_handler
        jmp     done

sys_set_handler:
        call    set_handler_handler
        jmp     done

sys_sigreturn:
        call    sigreturn_handler
        jmp     done

ret_invalid:
        popl    %edi
        popl    %esi
        popl    %esp
        popl    %ebp
        popl    %edx
        popl    %ecx
        popl    %ebx  
        popfl
        movl    $-1, %eax       /* return -1 to indicate error*/
        iret
done:
        addl    $12, %esp       /* pop edx, ecx, ebx, so move esp up by 12 */
        popl    %edi
        popl    %esi
        popl    %esp
        popl    %ebp
        popl    %edx
        popl    %ecx
        popl    %ebx  
        popfl
        iret


jump_table:
        .long   sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn


