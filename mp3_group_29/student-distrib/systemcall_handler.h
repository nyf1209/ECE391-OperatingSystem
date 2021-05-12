
#ifndef SYSTEMCALL_HANDLER_H
#define SYSTEMCALL_HANDLER_H

#include "types.h"
#include "lib.h"
#include "filesystem.h"
#include "rtc.h"
#include "terminal.h"
#include "exception_handler.h"
//jump table for system call handler

#define MAXIMUM_PROCESS     6
#define TWO_FIFTY_FIVE      255
#define MAX_FD_ARRAY_SIZE   8
typedef struct files_op{
    int32_t (*open)(const uint8_t* filename);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*close)(int32_t fd);
}file_op_t
;
//struct of block in file descriptor array
typedef struct file_d {
    file_op_t file_op_table;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
} file_d_t
;

//struct of pcb block
/**
 * 
 * 
 */
typedef struct pcb {
    file_d_t fd_array[MAX_FD_ARRAY_SIZE];
    //whatever you need 
    uint32_t ebp_when_execute;
    uint32_t esp_when_execute;
    uint32_t parent_process_id;
    struct pcb*   parent_ptr;
    uint8_t execute_arg[TWO_FIFTY_FIVE];
    uint32_t vidmap_flag;
} pcb_t
;
// external variable indicate current process ID
extern int current_id;

//variable to store current pcb
extern pcb_t* current_pcb_ptr;
extern pcb_t* pcb_arr[MAXIMUM_PROCESS];

int32_t pid_vidmap_flag[MAXIMUM_PROCESS];


//system call open
int32_t open (const uint8_t* filename);
//system call read
int32_t read (int32_t fd, void* buf, int32_t nbytes);
//system call write
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
//system call close
int32_t close (int32_t fd);
//system call execute
int32_t execute_handler (const uint8_t* command);
//system call halt
int32_t halt_handler(uint8_t status);

//helper function used to init shell
int32_t init_shell();


//not used in checkpoint 3
int32_t set_handler_handler();
int32_t sigreturn_handler();

// system call vidmap
int32_t vidmap_handler(uint8_t** screen_start);
// system call getargs
int32_t getargs_handler(uint8_t* buf, int32_t nbytes);

//bad call open happens in stdin , stdout
int32_t badcall_open(const uint8_t* filename);
//bad call read happens in stdin 
int32_t badcall_read(int32_t fd, void* buf, int32_t nbytes);
//bad call write happens in  stdout
int32_t badcall_write(int32_t fd, const void* buf, int32_t nbytes);
//bad call close happens in stdin , stdout
int32_t badcall_close(int32_t fd);





#endif
