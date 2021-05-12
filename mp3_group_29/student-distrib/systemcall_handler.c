
#include "systemcall_handler.h"
#include "pit.h"
#include "paging.h"
#include "term_switch.h"

#define MAXIMUM_FILENAME    255
#define FOUR_MEGABYTES      0x400000
#define EIGHT_MEGABYTES     0x800000
#define BUFFER_SIZE         28
#define ASC_DEL             0x7F
#define ASC_E               0x45
#define ASC_L               0x4C
#define ASC_F               0x46
#define OFFSET              0x48000
#define START_ADDR          0x8000000
#define SHIFT_8             8
#define SHIFT_16            16
#define SHIFT_24            24
#define BYTE_24             24
#define BYTE_25             25
#define BYTE_26             26
#define BYTE_27             27
#define ARRAY_SIZE          8
#define FOUR_KILOBYTES      4096
#define EIGHT_KILOBYTES     8192
#define FOUR                4

#define ONE_TWO_EIGHT_MB    0x08000000
#define ONE_THREE_TWO_MB    0x08400000
#define ONE_THREE_SIX_MB    0x08800000


int id_array[] = {0, 0, 0, 0, 0, 0};
int pid_vidmap_flag[6] = {0,0,0,0,0,0};
int current_id = -1;
pcb_t* current_pcb_ptr;
pcb_t* pcb_arr[MAXIMUM_PROCESS];


/* operations table for dir, file, stdin, stdout, rtc*/
static file_op_t dir_table =  {.open =dir_open, 
                                .read =dir_read, 
                                .write =dir_write, 
                               .close =dir_close};

static file_op_t file_table =  {.open = file_open, 
                               .read = file_read, 
                               .write =file_write, 
                               .close = file_close};

static file_op_t stdin_table =  {.open =badcall_open, 
                               .read =  badcall_read, 
                               .write = terminal_write, 
                               .close = badcall_close};

static file_op_t stdout_table = {.open =badcall_open, 
                               .read  = terminal_read, 
                               .write = badcall_write, 
                               .close = badcall_close};

static file_op_t rtc_table =  {.open = RTC_open, 
                               .read = RTC_read, 
                               .write = RTC_write, 
                               .close = RTC_close};

// execute current
// execute current->parent child->current
/*
 *execute_handler
 * DESCRIPTION: handling the execute system call, following the instructions described
 * in the documents 
 * INPUTS: command
 * OUTPUTS: none
 * SIDEEFFECTS: execute the command and enter the user stack.
 */
int32_t execute_handler (const uint8_t* command){
    uint8_t program[MAXIMUM_FILENAME], argument[MAXIMUM_FILENAME]; 
    int process_id; // used to finding the mapped page location and so on
    int i, j, k, l;
    uint8_t curr_char;
    uint8_t file_data[BUFFER_SIZE];
    int32_t file_descriptor;
    int32_t entry;
    file_d_t fd_arr[ARRAY_SIZE];
    pcb_t    child_pcb;
    pcb_t*   child_pcb_ptr;
    pcb_t*   parent_pcb_ptr;
    uint32_t new_user_bottom;


    if (command == 0) {
        return -1;      // if command is empty, return invalid
    }

    // check if current program is base shell
    if(current_id != -1){
        parent_pcb_ptr = (pcb_t*)(pcb_arr[current_id]);
    }
    else{
        parent_pcb_ptr = 0;
    }

    /* set process id */
    for(i = 0; i < MAXIMUM_PROCESS; i++){
        if(id_array[i] == 0){
            process_id = i;
            id_array[i] = 1;
            break;
        }
        if(i == MAXIMUM_PROCESS -1){
            // no more process allowed, return error
            return -1;
        }
    }

    /* parse argument(command) */
    i = 0;
    j = 0;
    curr_char = *command;
    while(curr_char != ' '){
        //printf("while 1 %d; current character %c \n",i , curr_char);
        if(curr_char == '\n' || curr_char == '\0'){
            break;
        }
        program[i] = curr_char;
        i += 1;
        curr_char = *(command + i);
    }
    program[i] = '\0';
    // finished reading program (filename)
    while(curr_char == ' '){
        //printf("while 2 %d\n",i);
        if(curr_char == '\n' || curr_char == '\0'){
            break;
        }
        i += 1;
        curr_char = *(command + i);
    }
    while(curr_char != 0){
        //printf("while 3 %d\n",i);
        if(curr_char == '\n' || curr_char == '\0'){
            break;
        }
        argument[j] = curr_char;
        j += 1;
        i += 1;
        curr_char = *(command + i);
    }
    argument[j]='\0';

    // handle third ? 


    // finished reading command 
    // a check
    //printf("pragram %s\n", program);
    //printf("argument %s\n", argument);


    /* check validity */
    // check existence
    if(read_dentry_by_name(program) == -1){
        id_array[process_id] = 0;
        return -1;
    }


    // check EXE
    execute_read(file_descriptor, file_data, BUFFER_SIZE);

    if(file_data[0] != ASC_DEL || file_data[1] != ASC_E || file_data[2] != ASC_L || file_data[3] != ASC_F){
        id_array[process_id] = 0;
        return -1;
    }

    // get the entry if the program
    entry = ((int32_t)file_data[BYTE_24]) + (((int32_t)file_data[BYTE_25])<<SHIFT_8)
     + (((int32_t)file_data[BYTE_26])<<SHIFT_16) + (((int32_t)file_data[BYTE_27]<<SHIFT_24));
    //printf("%x %x %x %x", (int32_t)file_data[24], (int32_t)file_data[25],(int32_t)file_data[26],(int32_t)file_data[27]);
    /* set paging */
    set_page(process_id);

    /* laoding */
    //copy to the desired location
    execute_read(file_descriptor, (uint8_t*)(START_ADDR + OFFSET), FOUR_MEGABYTES);
    // 4mb|             |
    //    |  8kb        |
    //    |  8kb        |
    //    | pcb_t       |
    // 8mb|  8kb        | 0th process kernel stack 8mb - 4; 1th process kernel stack 8mb - 8kb - 4 ...
    // 


    // 128mb |              |
    //       |              |
    // 132mb | 132mb - 4    |   
    /* set up TSS */
    tss.esp0 = EIGHT_MEGABYTES - EIGHT_KILOBYTES*(process_id) - FOUR;
    tss.ss0 = KERNEL_DS;
    new_user_bottom = START_ADDR + FOUR_MEGABYTES - FOUR;
    /* create child pcb */

    child_pcb.parent_ptr = parent_pcb_ptr;
    for(k = 0; k < ARRAY_SIZE; k++){
        fd_arr[k].flags = 0;
    }
    fd_arr[1].file_op_table = stdin_table;      /* entry 1 in fd is for stdin */
    fd_arr[1].inode = 0;
    fd_arr[1].file_position = 0;              /* flags 1 represents exist*/
    fd_arr[1].flags = 1; 
    fd_arr[0].file_op_table = stdout_table;     /* entry 0 in fd is for stdout */
    fd_arr[0].inode = 0;
    fd_arr[0].file_position = 0;
    fd_arr[0].flags = 1;
    for(l = 0; l < ARRAY_SIZE; l++){
        child_pcb.fd_array[l] = fd_arr[l];
    }
    child_pcb.parent_process_id = current_id;

    
    // added for vidmap
    child_pcb.vidmap_flag = 0;
    // added for getargs, to store arguments in pcb
    for (i = 0; i < MAXIMUM_FILENAME; i++){
        if (i <= j){
            child_pcb.execute_arg[i] = argument[i];
        } else {
            child_pcb.execute_arg[i] = '\0';        // fill the rest blank arg with null byte
        }
    }

    // copy pcb to the 8MB - pid*8kb chunk
    uint32_t addr;
    addr = EIGHT_MEGABYTES - EIGHT_KILOBYTES*(process_id+1);
    child_pcb_ptr = (pcb_t*)(memcpy((void*)addr, &child_pcb, sizeof(child_pcb)));
    // adding child pcb to array
    
    pcb_arr[process_id] = child_pcb_ptr;
    // upadate current_id
    current_id = process_id;

    // update current_pcb_ptr
    current_pcb_ptr = child_pcb_ptr;
     /* save current esp and ebp */
    if(parent_pcb_ptr != 0){
        asm("                   \n\
        movl %%ebp, %0          \n\
        movl %%esp, %1          \n\
        "
        :"=r"(parent_pcb_ptr->ebp_when_execute), "=r"(parent_pcb_ptr->esp_when_execute)
        :/* no inputs */
        :"cc"
        );
    }

    /* do fake iret */
    asm("                                   \n\
        cli                                 \n\
        # ss (USER_DS)                      \n\
        movl    %0, %%eax                   \n\
        pushl   %%eax                       \n\
        # esp   bottom of page allocated    \n\
        movl    %1, %%eax                   \n\
        pushl   %%eax                       \n\
        # EFLAGS                            \n\
        pushfl                              \n\
        popl    %%eax                       \n\
        orl     $0x200, %%eax               \n\
        pushl   %%eax                       \n\
        #cs (USER_CS)                       \n\
        movl    %2, %%eax                   \n\
        pushl   %%eax                       \n\
        #return addr (entry)                \n\
        movl    %3, %%eax                   \n\
        pushl   %%eax                       \n\
        iret                                \n\
        HALT_RETURN:                        \n\
        leave                               \n\
        ret                                 \n\
        "
        :/* no outputs */
        :"r"(USER_DS), "r"(new_user_bottom), "r"(USER_CS), "r"(entry)
        :"eax","cc"
    );
    
    
    return 0;

}


/*
 * init_shell
 * DESCRIPTION: the helper function used to init 3 shell at the after the system boot
 * INPUTS:none 
 * OUTPUTS:none
 * SIDEEFFECTS:none
 */
int32_t init_shell (){
    uint8_t program[] = "shell";
    int process_id; // used to finding the mapped page location and so on
    int i, k, l;
    uint8_t file_data[BUFFER_SIZE];
    int32_t file_descriptor;
    int32_t entry;
    file_d_t fd_arr[ARRAY_SIZE];
    pcb_t    child_pcb;
    pcb_t*   child_pcb_ptr;
    pcb_t*   parent_pcb_ptr;
    uint32_t new_user_bottom;

    // set parent_pcb_ptr to be 0
    parent_pcb_ptr = 0;

    /* set process id */
    for(i = 0; i < MAXIMUM_PROCESS; i++){
        if(id_array[i] == 0){
            process_id = i;
            id_array[i] = 1;
            break;
        }
        if(i == MAXIMUM_PROCESS -1){
            // no more process allowed, return error
            return -1;
        }
    }
    /* check validity */
    // check existence
    if(read_dentry_by_name(program) == -1){
        id_array[process_id] = 0;
        return -1;
    }


    // check EXE
    execute_read(file_descriptor, file_data, BUFFER_SIZE);

    if(file_data[0] != ASC_DEL || file_data[1] != ASC_E || file_data[2] != ASC_L || file_data[3] != ASC_F){
        id_array[process_id] = 0;
        return -1;
    }

    // get the entry if the program
    entry = ((int32_t)file_data[BYTE_24]) + (((int32_t)file_data[BYTE_25])<<SHIFT_8)
     + (((int32_t)file_data[BYTE_26])<<SHIFT_16) + (((int32_t)file_data[BYTE_27]<<SHIFT_24));
    //printf("%x %x %x %x", (int32_t)file_data[24], (int32_t)file_data[25],(int32_t)file_data[26],(int32_t)file_data[27]);
    /* set paging */
    set_page(process_id);

    /* laoding */
    //copy to the desired location
    execute_read(file_descriptor, (uint8_t*)(START_ADDR + OFFSET), FOUR_MEGABYTES);
    // 4mb|             |
    //    |  8kb        |
    //    |  8kb        |
    //    | pcb_t       |
    // 8mb|  8kb        | 0th process kernel stack 8mb - 4; 1th process kernel stack 8mb - 8kb - 4 ...
    // 


    // 128mb |              |
    //       |              |
    // 132mb | 132mb - 4    |   
    /* set up TSS */
    tss.esp0 = EIGHT_MEGABYTES - EIGHT_KILOBYTES*(process_id) - FOUR;
    tss.ss0 = KERNEL_DS;
    new_user_bottom = START_ADDR + FOUR_MEGABYTES - FOUR;
    /* create child pcb */

    child_pcb.parent_ptr = parent_pcb_ptr;
    for(k = 0; k < ARRAY_SIZE; k++){
        fd_arr[k].flags = 0;
    }
    fd_arr[1].file_op_table = stdin_table;      /* entry 1 in fd is for stdin */
    fd_arr[1].inode = 0;
    fd_arr[1].file_position = 0;              /* flags 1 represents exist*/
    fd_arr[1].flags = 1; 
    fd_arr[0].file_op_table = stdout_table;     /* entry 0 in fd is for stdout */
    fd_arr[0].inode = 0;
    fd_arr[0].file_position = 0;
    fd_arr[0].flags = 1;
    for(l = 0; l < ARRAY_SIZE; l++){
        child_pcb.fd_array[l] = fd_arr[l];
    }
    child_pcb.parent_process_id = -1;

    
    // added for vidmap
    child_pcb.vidmap_flag = 0;
    // added for getargs, to store arguments in pcb

    // copy pcb to the 8MB - pid*8kb chunk
    uint32_t addr;
    addr = EIGHT_MEGABYTES - EIGHT_KILOBYTES*(process_id+1);
    child_pcb_ptr = (pcb_t*)(memcpy((void*)addr, &child_pcb, sizeof(child_pcb)));
    // adding child pcb to array
    
    pcb_arr[process_id] = child_pcb_ptr;
    // upadate current_id
    current_id = process_id;

    // update current_pcb_ptr
    current_pcb_ptr = child_pcb_ptr;
     /* save current esp and ebp */
    if(parent_pcb_ptr != 0){
        asm("                   \n\
        movl %%ebp, %0          \n\
        movl %%esp, %1          \n\
        "
        :"=r"(parent_pcb_ptr->ebp_when_execute), "=r"(parent_pcb_ptr->esp_when_execute)
        :/* no inputs */
        :"cc"
        );
    }

    // /* do fake iret */
    asm("                                   \n\
        cli                                 \n\
        # ss (USER_DS)                      \n\
        movl    %0, %%eax                   \n\
        pushl   %%eax                       \n\
        # esp   bottom of page allocated    \n\
        movl    %1, %%eax           \n\
        pushl   %%eax                       \n\
        # EFLAGS                            \n\
        pushfl                              \n\
        popl    %%eax                       \n\
        orl     $0x200, %%eax               \n\
        pushl   %%eax                       \n\
        #cs (USER_CS)                       \n\
        movl    %2, %%eax                \n\
        pushl   %%eax                       \n\
        #return addr (entry)                \n\
        movl    %3, %%eax           \n\
        pushl   %%eax                       \n\
        iret                                \n\
        HALT_RETURN_1:                        \n\
        leave                               \n\
        ret                                 \n\
        "
        :/* no outputs */
        :"r"(USER_DS), "r"(new_user_bottom), "r"(USER_CS), "r"(entry)
        :"eax","cc"
    );
    
    
    return 0;

}

#define SEVEN_SYSCALL   7
#define TWO_SYSCALL     2
#define EIGHT_SYSCALL   8


/*
 *open
 * DESCRIPTION: handling the open system call, following the instructions described
 * in the documents 
 * INPUTS: filename -- indicate the file to open
 * OUTPUTS: 0 for success, -1 for error
 * SIDEEFFECTS: open the file and modify fd_array
 */
int32_t open (const uint8_t* filename){
    if (filename == NULL) { //if filenmae invalid
        return -1;
    }
    int index;
    

    //index from 2 to 7 to open slot
    //find which block is available in array
    for( index = TWO_SYSCALL ; index < EIGHT_SYSCALL ; index++  ){

        if (current_pcb_ptr->fd_array[index].flags == 1 && (index == SEVEN_SYSCALL)){  /* if none of the flag is 0, fd_array is full*/
            return -1;
        }

        if (current_pcb_ptr->fd_array[index].flags == 0)    /* break once found the open slot*/
        {
            break;         
        }       
    }

    if(read_dentry_by_name(filename)){  /* if file does not exist, return error*/
        return -1;
    }

    /*type 0 is for RTC; if file is RTC, fill the op_table with RTC_table*/
    if(last_dentry->type==0){
        current_pcb_ptr->fd_array[index].file_op_table = rtc_table;
        current_pcb_ptr->fd_array[index].inode = 0;
        current_pcb_ptr->fd_array[index].file_position = 0;
        current_pcb_ptr->fd_array[index].flags = 1;
    }

    else if(last_dentry->type==1){    /*type 1 is for DIR; if file is DIR, fill the op_table with DIR_table*/
        current_pcb_ptr->fd_array[index].file_op_table = dir_table;
        current_pcb_ptr->fd_array[index].inode = 0;
        current_pcb_ptr->fd_array[index].file_position = 0;
        current_pcb_ptr->fd_array[index].flags = 1;
      
    }

    else{       /* else it's file, fill the op_table with file_table*/
        current_pcb_ptr->fd_array[index].file_op_table = file_table;
        current_pcb_ptr->fd_array[index].inode = last_dentry->inode_idx;
        current_pcb_ptr->fd_array[index].file_position = 0;
        current_pcb_ptr->fd_array[index].flags = 1;
        
        }

        current_pcb_ptr->fd_array[index].file_op_table.open(filename);  // do the corresponding open

    

    
    return index;
}

/*
 * read
 * DESCRIPTION: handling the read system call, following the instructions described
 * in the documents 
 * INPUTS: fd -- integer indicating the index to fd_array
 *          buf -- read data to this buffer
 *          nbytes -- read nbytes bytes data
 * OUTPUTS: number of bytes succesfully read, -1 for error
 * SIDEEFFECTS: read the file, modify the fd_array.inode
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    if (buf == NULL) {  // if buf invalid
        return -1;
    }

    // check if fd is in the valid range of 0-7 and if fd's flag is set to 1
    if(fd<0 || fd>SEVEN_SYSCALL || current_pcb_ptr->fd_array[fd].flags==0 ){
        return -1;
    }


    return current_pcb_ptr->fd_array[fd].file_op_table.read(fd,buf,nbytes);

}

/*
 * write
 * DESCRIPTION: handling the write system call, following the instructions described
 * in the documents 
 * INPUTS: fd -- integer indicating the index to fd_array
 *          buf -- write data from this buffer
 *          nbytes -- write nbytes bytes data
 * OUTPUTS: number of bytes succesfully write, -1 for error
 * SIDEEFFECTS: write to the file
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    //printf("write called \n");
    // if buf is invalid, return error
    if (buf == NULL) {
        return -1;
    }
    // check if fd is in the valid range of 0-7 and if fd's flag is set to 1
    if(fd>SEVEN_SYSCALL || fd < 0 || current_pcb_ptr->fd_array[fd].flags==0){
        return -1;
    }
    sti();

    return current_pcb_ptr->fd_array[fd].file_op_table.write(fd,buf,nbytes);
   
}

/*
 * close
 * DESCRIPTION: handling the close system call, following the instructions described
 * in the documents 
 * INPUTS: fd -- integer indicating the index to fd_array
 * OUTPUTS: 0 for success, -1 for error
 * SIDEEFFECTS: close the file, modify the fd_array
 */
int32_t close (int32_t fd){
    // check if fd is in the valid range of 2-7 and if fd's flag is set to 1
    if(fd<TWO_SYSCALL||fd>SEVEN_SYSCALL||(current_pcb_ptr->fd_array[fd].flags==0)){
        return -1;
    }

    current_pcb_ptr->fd_array[fd].file_op_table.close(fd);
    current_pcb_ptr->fd_array[fd].flags=0;

    return 0;
}

/*
 * badcall_open
 * DESCRIPTION: handling the bad open system call, used when the file does not support open function
 * INPUTS: filename -- the name of the file
 * OUTPUTS: -1 to indicate error
 * SIDEEFFECTS: none
 */
int32_t badcall_open(const uint8_t* filename){
    return -1;
}
/*
 * badcall_read
 * DESCRIPTION: handling the bad read system call, used when the file does not support read function
 * INPUTS: fd -- integer indicating the index to fd_array 
 *          buf -- read to the buffer
 *          nbytes -- number of bytes to read
 * OUTPUTS: -1 to indicate error
 * SIDEEFFECTS: none
 */
int32_t badcall_read(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}
/*
 * badcall_write
 * DESCRIPTION: handling the bad write system call, used when the file does not support write function
 * INPUTS: fd -- integer indicating the index to fd_array 
 *          buf -- write from the buffer
 *          nbytes -- number of bytes to write
 * OUTPUTS: -1 to indicate error
 * SIDEEFFECTS: none
 */
int32_t badcall_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}
/*
 * badcall_close
 * DESCRIPTION: handling the bad close system call, used when the file does not support close function
 * INPUTS: fd -- integer indicating the index to fd_array 
 * OUTPUTS: -1 to indicate error
 * SIDEEFFECTS: none
 */
int32_t badcall_close(int32_t fd){
    return -1;
}

/*
 * set_handler_handler
 * DESCRIPTION: NOT USED YET
 * INPUTS: none
 * OUTPUTS: none
 * SIDEEFFECTS: none
 */
int32_t set_handler_handler(){
    return -1;
}
/*
 * sigreturn_handler
 * DESCRIPTION: NOT USED YET
 * INPUTS: none
 * OUTPUTS: none
 * SIDEEFFECTS: none
 */
int32_t sigreturn_handler(){
    return -1;
}


/*
 * vidmap_handler
 * DESCRIPTION: to map the new virtual video memory to the same physical memory paging
 * INPUTS: screen_start - a double pointer which pointer to a pointer to the new virtual video mem addr
 * OUTPUTS: 0 for seccess, -1 for error
 * SIDEEFFECTS: called set_video_mem so changes the paging.
 */
int32_t vidmap_handler(uint8_t** screen_start){
    if (screen_start == NULL) {  // check for validity
        return -1;
    }
    if ((uint32_t)screen_start < ONE_TWO_EIGHT_MB || (uint32_t)screen_start > ONE_THREE_TWO_MB) { // if it's not in the range of 128-132 MB
        return -1;
    }
    pid_vidmap_flag[current_id] = 1;
    (*screen_start) = (uint8_t*)ONE_THREE_SIX_MB;         // set new video memory to 136 MB to not collide with user program

    current_pcb_ptr->vidmap_flag = 1;       // set the flag to 1 so we can undo the maping in halt
    set_video_mem(screen_start);            // call halper function set_video_mem to actually set the paging
    return 0;
}

/*
 * getargs_handler
 * DESCRIPTION: to read the arugment from current pcb and store it to buf
 * INPUTS: buf -- buffer to write the argument to
 *          nbytes -- number of bytes to read from argument
 * OUTPUTS: 0 for seccess, -1 for error
 * SIDEEFFECTS: called set_video_mem so changes the paging.
 */
int32_t getargs_handler(uint8_t* buf, int32_t nbytes){
    if (buf == NULL) {      // if buf is at invalid address
        return -1;
    }

    int i;
    if (current_pcb_ptr->execute_arg[0] == '\0' || nbytes < 0) { // if there is no argument or nbytes is negative, return -1
        return -1;
    }
    for (i = 0; i < nbytes - 1; i++) {      // the first nbytes-1 will be for argument, the nbytes-th byte will be used for NULL byte
        //strcpy(buf+i, &current_pcb_ptr->execute_arg[i]);
        if (i < MAXIMUM_FILENAME){
            *(buf+i) = current_pcb_ptr->execute_arg[i];  // write to buf
        } else {
            *(buf+i) = '\0';    // if exceed the max length of argument, fill in rest bytes with \0
        }
    }
    *(buf+i) = '\0';      // mannually add terminal byte at the end for nbytes-th byte

    return 0;
}



/* int32_t halt(uint8_t status)
 * Inputs: uint8_t status
 * Return Value: 0 for success -1 otherwise
 * Function: 
 * This function will restore the previously saved parent data, 
 * close any relavent FDs, and jump to the return label in execute. 
 */
int32_t halt_handler(uint8_t status)
{   
    int current_process_id;

    current_process_id = current_id;

    pcb_t *curr_pcb = current_pcb_ptr;

    
    int status_return;
    if (global_flag_exception == 1) {
        //status return equal 256 when exception happens

        //return 256 when exception happens
        status_return = 256;

        global_flag_exception = 0;
    } else {
        status_return = (uint32_t)status;
    }


    if (curr_pcb->parent_process_id == -1) {
        id_array[current_id] = 0;
        pcb_arr[current_id] = 0;
        current_id = -1;
        execute_handler((uint8_t *)"shell");
    } else {
        current_id = curr_pcb->parent_process_id;
        current_pcb_ptr = pcb_arr[curr_pcb->parent_process_id];
    }

    //insert code to set tss esp0
    tss.esp0 = EIGHT_MEGABYTES - EIGHT_KILOBYTES*(curr_pcb->parent_process_id) -  FOUR;
    tss.ss0 = KERNEL_DS;

    //make room for halt?
    id_array[current_process_id] = 0;
    pcb_arr[current_process_id] = 0;
    term_pid_array[cur_execute_term] = curr_pcb->parent_process_id;
    
    
    if (cur_execute_term == 0) {
        term1.curr_execute_pid_pcb = current_pcb_ptr;
    } else if (cur_execute_term == 1) {
        term2.curr_execute_pid_pcb = current_pcb_ptr;
    } else {
        term3.curr_execute_pid_pcb = current_pcb_ptr;
    }


    //clear paging
    set_page(curr_pcb->parent_process_id);

    if (curr_pcb->vidmap_flag == 1) {  // if vidmap is called for this pcb, undo the video mem paging
        undo_vidmap();
        curr_pcb->vidmap_flag = 0;
        pid_vidmap_flag[current_process_id] = 0;
    }

    //potentially need this
    //page_directory[32] = ((0x1 + 0x2 + 0x4 + 0x80)) + (uint32_t)((current_process_id * 0x400000 + 0x800000))
    //printf("halt ebp esp %d %d \n", curr_pcb->parent_ptr->ebp_when_execute, curr_pcb->parent_ptr->esp_when_execute);

    int k = 0;
    for(k = 0; k < ARRAY_SIZE; k++){
        //0 and 1 doesn't need to be -1
        if (k == 0 || k == 1) {
            curr_pcb->fd_array[k].flags = 0;
        } else {
            close(k);
        }
        
    }
    
    
    asm volatile("                          \n\
        movl    %0, %%esp                   \n\
        movl    %1, %%ebp                   \n\
        movl    %2, %%eax                   \n\
        leave                               \n\
        ret                                 \n\
        "
        :/* no outputs */
        :"r"(curr_pcb->parent_ptr->esp_when_execute), "r"(curr_pcb->parent_ptr->ebp_when_execute), "r"(status_return)
        :"cc" 
    );
    //save esp ebp

    return -1;
}


