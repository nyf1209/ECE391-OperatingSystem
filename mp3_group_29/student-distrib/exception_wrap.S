

#define ASM     1

.text
/* step 1. push error or dummy code*/
/* step 2. push all registers*/
/* step 3. push exception index*/
/* step 4. call exception handler*/


/*exceptio handler pointer*/
.globl  divide_error_exception
.globl  debug_exception
.globl  NMI_interrupt
.globl  breakpoint_exception
.globl  overflow_exception
.globl  bound_range_exceeded_exception
.globl  invalid_opcode_exception
.globl  divice_not_avaliable_exception
.globl  double_fault_exception
.globl  coprocessor_segment_overrun
.globl  invalid_TSS_exception
.globl  segment_not_present
.globl  stack_fault_exception
.globl  general_protection_exception
.globl  page_fault_exception
.globl  x87_FPU_floating_point_error
.globl  alignment_check_exception
.globl  machine_check_exception
.globl  SIMD_floating_point_exception



divide_error_exception:
        pushl $0                    
        pushal                      
        pushl $0                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


debug_exception:
        pushl $0                    
        pushal                      
        pushl $1                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



NMI_interrupt:
        pushl $0                    
        pushal                      
        pushl $2                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



breakpoint_exception:
        pushl $0                    
        pushal                      
        pushl $3                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



overflow_exception:
        pushl $0                    
        pushal                      
        pushl $4                  
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



bound_range_exceeded_exception:
        pushl $0                    
        pushal                      
        pushl $5                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



invalid_opcode_exception:
        pushl $0                    
        pushal                      
        pushl $6                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



divice_not_avaliable_exception:
        pushl $0                    
        pushal                      
        pushl $7                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


/*ERROR code 0*/
double_fault_exception:                   
        pushal                      
        pushl $8                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



coprocessor_segment_overrun:
        pushl $0                    
        pushal                      
        pushl $9                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


/*ERROR code*/
invalid_TSS_exception:                   
        pushal                      
        pushl $10                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


/*ERROR code*/
segment_not_present:                   
        pushal                      
        pushl $11                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


/*ERROR code*/
stack_fault_exception:                   
        pushal                      
        pushl $12                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


/*ERROR code*/
general_protection_exception:                   
        pushal                      
        pushl $13                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


/*ERROR code*/
page_fault_exception:                    
        pushal                      
        pushl $14                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



x87_FPU_floating_point_error:
        pushl $0                    
        pushal                      
        pushl $16                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret


/*ERROR code 0*/
alignment_check_exception:                   
        pushal                      
        pushl $17                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



machine_check_exception:
        pushl $0                    
        pushal                      
        pushl $18                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret



SIMD_floating_point_exception:
        pushl $0                    
        pushal                      
        pushl $19                   
        call exception_handler 
        addl $4, %esp               
        popal                       
        addl $4, %esp               
        iret

