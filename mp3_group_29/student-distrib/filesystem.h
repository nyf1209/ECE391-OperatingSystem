#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H


#include "lib.h"

#define BLOCK_OFFSET 4096 //4kb for each block 1024 *4 = 4096
#define DENTRY_OFFSET 64 //64 bytes for each directory entry
#define DATA_SIZE     4096 // 4kb for each data block



//64bytes dir-block
typedef struct dentry{

    uint8_t name[32];
    uint32_t  type;
    uint32_t  inode_idx;
    uint8_t   reserved[24];

} dentry_t; 


//64bytes data-block
typedef struct {

    uint32_t  num_entry;
    uint32_t  num_inode;
    uint32_t  num_datablock;
    uint8_t   reserved[52];

} boot_t;

//4kbytes inode-block
typedef struct {
    //1
    int32_t length;
    //1024-1
    uint32_t data_add[1023];

}inode_t;

//4kb data block
typedef struct{

    uint8_t data[4096];

}data_t;

//start of file system
void * fs_ptr;
//start of boot block
boot_t * boot_ptr;
//start of dentry
dentry_t * den_ptr;
//start of inode
inode_t * inode_ptr;
//start of data block
data_t * data_ptr;


//initialize the file system
extern void    file_system_init(void * file_ptr);

//directory read , success return 0 
extern int32_t dir_read (int32_t fd, void* buf, int32_t nbytes);
//write , success return 0
extern int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes);
//directory open , success return 0 
extern int32_t dir_open (const uint8_t* filename);
//directory close , success return 0
extern int32_t dir_close (int32_t fd);

//directory close , success return 0
extern int32_t file_read (int32_t fd, void* buf, int32_t nbytes);
//write , success return 0
extern int32_t file_write (int32_t fd, const void* buf, int32_t nbytes);
//directory close , success return 0
extern int32_t file_open (const uint8_t* filename);
//directory close , success return 0
extern int32_t file_close (int32_t fd);
//helper function to read dentry by name
int32_t read_dentry_by_name(const uint8_t* filename);
//helper function to read dentry by index
int32_t read_dentry_by_index(uint32_t index);
//helper function to read data 
int32_t read_data(inode_t * inode,uint8_t* buf , int32_t nbytes);
//helper function to read data block in execute handler
int32_t read_data_execute(inode_t * inode,uint8_t* buf , int32_t nbytes);
//helper function to read in execute handler
extern int32_t execute_read (int32_t fd, uint8_t* buf, int32_t nbytes);




//record how many bytes already read from last read_file
int32_t last_read;
//record the last dentry opened from file_open
dentry_t * last_dentry;


#endif

