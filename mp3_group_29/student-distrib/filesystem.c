#include "filesystem.h"
#include "systemcall_handler.h"

uint8_t* file_name;
dentry_t* open_file_entry;
//save dentry in the last open file
dentry_t * temp_dentry;

/* add one extra byte (null) at the end of the name array*/
uint8_t name_for_null[33];

uint32_t dir_open_file_idx = 0;

/* max length of the name array*/
#define THIRTY_TWO  32

/*
 * file_system_init
 *   DESCRIPTION:  initialize the pointer needed for file system operation
 *                     
 *   INPUTS: (void * file_ptr)
 *            file_ptr: the address of file store in the system , call from kernel.c 
 * 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

extern void    file_system_init(void * file_ptr){

    //initialize the file system
    fs_ptr = file_ptr;
    boot_ptr = file_ptr;
    den_ptr = file_ptr + DENTRY_OFFSET;
    inode_ptr = file_ptr + BLOCK_OFFSET;
    last_read = 0;
    last_dentry = NULL;
    
     
}


/*
 * read_dentry_by_name
 *   DESCRIPTION:  read the filename , find the its dir entry in the filesystem and store it 
 *  in the last_dentry pointer for the use of file read.
 *                     
 *   INPUTS: (const uint8_t* filename)
 *            filename: the name of the file
 * 
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if the file is in the system , else return -1
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_name(const uint8_t* filename){

    int i;
   //printf("open the file : %s\n" , filename);

   for (i = 0; i < 33; i++) {
       if (filename[i] == '\0' || filename[i] == '\n') {
           break;
       }
       if (i == 32) {
           printf("file name exceeding 32 bytes \n");
           return -1;       // the length is longer than 32 bytes 
       }
   }
    
    
    for (i = 0 ; i < boot_ptr->num_entry;i++){

       temp_dentry = den_ptr + i;
       file_name = temp_dentry->name;
       
       //if the filename match the file in the system return 0
       if (strncmp((int8_t*)filename,(int8_t*)file_name,THIRTY_TWO)==0)
       {
           last_dentry = temp_dentry;
           return 0;
       }
    }

    //file not found return 1
    printf("file not found\n");
    return -1;

}


/*
 * read_dentry_by_index
 *   DESCRIPTION:  read the index , find the its dir entry in the filesystem and store it 
 *  in the last_dentry pointer for the use of file read.
 *                     
 *   INPUTS: (uint32_t index)
 *            filename: the index of the dir entry in the file system
 * 
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if the file is in the system , else return 1
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_index(uint32_t index){

    if(index > boot_ptr->num_entry){
        printf("file not found\n");
        return 1;
    }

    temp_dentry = den_ptr + index;
    printf("find the file\n");
    printf("find the file : %s\n" , temp_dentry->name);
    return 0;

}

//rtc
//frame0.txt

//rame0.txt


/*
 * dir_open
 *   DESCRIPTION: open the directory according to the input filename
 *                     
 *   INPUTS: (const uint8_t* filename)
 *            filename: the name of the directory 
 * 
 *   OUTPUTS: none
 *   RETURN VALUE: 0 
 *   SIDE EFFECTS: none
 */
extern int32_t dir_open (const uint8_t* filename){
    
    // if(read_dentry_by_name(filename)==0){
    //     printf("open the directory\n");
    // //printf("open the directory : %s\n" , temp_dentry->name);
    // }
    return 0;
}


/*
 * file_open
 *   DESCRIPTION: open the file according to the input filename
 *                     
 *   INPUTS: (const uint8_t* filename)
 *            filename: the name of the file 
 * 
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if sucess , if not return -1 
 *   SIDE EFFECTS: none
 */
extern int32_t file_open (const uint8_t* filename){
    
    int i;
    if(read_dentry_by_name(filename)==0){
    //remember the last file open for the file read
    last_read = 0;
    last_dentry = temp_dentry;
    for (i = 0; i < THIRTY_TWO; i++) {
        name_for_null[i] = temp_dentry->name[i];
    }
    name_for_null[THIRTY_TWO] = '\0';   /* add null to the end of the array*/
    //printf("open the file : %s\n",name_for_null);
    //printf("open the file : %s\n" , temp_dentry->name);
    return 0;
    }

    else{
        return -1;
    }
   
}


/*
 * read_data
 *   DESCRIPTION: copy the data from data block to the input buffer
 *                     
 *   INPUTS: (inode_t * inode,uint8_t* buf , int32_t nbytes)
 *            inode: the inode of last open file
 *            buf: the buffer to store the data
 *            nbytes: the number of bytes store in the buf
 *   OUTPUTS: none
 *   RETURN VALUE: nbytes 
 *   SIDE EFFECTS: none
 */
int32_t read_data(inode_t * inode,uint8_t* buf , int32_t nbytes){
    int i,j;

    uint32_t block_begin , block_end ,block_idx;
    uint32_t begin_byte;
    uint32_t buf_idx; 
    uint32_t cur_byte;
    int32_t total_bytes;
    data_t * temp;
    //how many bytes left to transfer to the buffer
    total_bytes = nbytes;
    buf_idx = 0;
    //find the starting/ending block in inode and starting address in the data block
    block_begin = last_read/DATA_SIZE;
    begin_byte = last_read%DATA_SIZE; 
    block_end = ((last_read+nbytes)/DATA_SIZE); 
    //printf("begin %d , end %d ,byte %d \n", block_begin,block_end,begin_byte);
   
    
    cur_byte = begin_byte;
    //begin to read each data block 
    for (i = block_begin ; i < block_end+1; i++){

        //get the data block from inode
        block_idx = inode->data_add[i]; 
        temp = fs_ptr+ (BLOCK_OFFSET * (block_idx+1+(boot_ptr->num_inode)));
        
        //copy data from data block to buf byte by byte
        for(j = 0 ; j < total_bytes ; j++){
                
                
                
                buf[buf_idx] = *(temp->data+cur_byte);
                //printf("%c", buf[buf_idx]);
                buf_idx++;
                
                
               //printf("cur_byte %d\n",cur_byte);
               cur_byte++;
               
                //if the next byte to read is on the other block ,break
                //4096 means the end of a data block(4kb size) , should transfer to a new block
                if(cur_byte==4096){
                    //starting address of data in the  block reset to 0
                    cur_byte = 0;
                    // calculate how many bytes left
                    total_bytes = total_bytes - j;
                    break;
                }
                
                 }
            }
        
   
    
    
    //update the last_read to tell how many bytes already read
    last_read  = last_read +    nbytes ;
    //printf("last_read , %d",last_read);

    return nbytes;
}



/*
 * dir_read
 *   DESCRIPTION: read a directory , copy the name to the buf
 *                     
 *   INPUTS: (int32_t fd, uint8_t* buf, int32_t nbytes)
 *            fd: none
 *            buf: none
 *            nbytes: the index of dir entry in the file system
 *   OUTPUTS: none
 *   RETURN VALUE: index of file if open successfully , else return 0
 *   SIDE EFFECTS: none
 */ 
extern int32_t dir_read (int32_t fd, void* buf, int32_t nbytes){

    
    dentry_t *  temp;
    int i;
    
    //print out all the file in the directory
    if(dir_open_file_idx < boot_ptr->num_entry){

        temp = den_ptr + dir_open_file_idx;
        for (i = 0; i < THIRTY_TWO && i < nbytes; i++) {
            name_for_null[i] = temp->name[i];
        }
        name_for_null[i] = '\0';       /* add null to the end of the array*/
        //printf("%d %s\n",nbytes,name_for_null);
        memcpy(buf, &name_for_null, i);
        //printf("%d %s\n",nbytes,temp->name);
        dir_open_file_idx = dir_open_file_idx + 1;
        return i;
    }
    
    dir_open_file_idx = 0;
    return 0;  

}



/*
 * dir_read
 *   DESCRIPTION: read a file 
 *                     
 *   INPUTS: (int32_t fd, uint8_t* buf, int32_t nbytes)
 *            fd: none
 *            buf: the buffer to store the data from data block
 *            nbytes: bytes to copy data to the buffer
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if open successfully , else return 1
 *   SIDE EFFECTS: none
 */ 
extern int32_t file_read (int32_t fd, void* buf, int32_t nbytes){

    //printf("file_read");
    inode_t * inode;
    int32_t byte_left;
    
    

     //find the start of the datablock;
     //0th data block = (N+1)th block 
     data_ptr = fs_ptr + (boot_ptr->num_inode + 1)*BLOCK_OFFSET;
     //get last_dentry from last file opened !!!!!!! for demo
     inode = inode_ptr + (current_pcb_ptr->fd_array[fd].inode);

     last_read = current_pcb_ptr->fd_array[fd].file_position;
    //check if the bytes read exceed the length of file  
    if(nbytes>((inode->length)-last_read)){
        //set to the maximum file bytes can read
        byte_left = (inode->length)-last_read;
        //printf("too many bytes to read");
    }

    else{
        byte_left = nbytes;
        
    }

    //read data
    
    read_data(inode,(uint8_t*)buf,byte_left);
    current_pcb_ptr->fd_array[fd].file_position = last_read;
    //printf("%d bytes read\n",byte_left);
    
    

    return byte_left;

}

/*
 * execute_read
 *   DESCRIPTION: copy data to the buf in execute systemcall handler 
 *                     
 *   INPUTS: (int32_t fd, uint8_t* buf, int32_t nbytes)
 *            fd: none
 *            buf: the buffer to store the data from data block
 *            nbytes: bytes to copy data to the buffer
 *   OUTPUTS: none
 *   RETURN VALUE: 0 
 *   SIDE EFFECTS: none
 */ 
extern int32_t execute_read (int32_t fd, uint8_t* buf, int32_t nbytes){

    //printf("file_read");
    inode_t * inode;
    int32_t byte_left;


     //find the start of the datablock;
     //0th data block = (N+1)th block 
     data_ptr = fs_ptr + (boot_ptr->num_inode + 1)*BLOCK_OFFSET;
     //get last_dentry from last file opened !!!!!!! for demo
     inode = inode_ptr + (last_dentry->inode_idx);

    //check if the bytes read exceed the length of file  
    if(nbytes>((inode->length))){
        //set to the maximum file bytes can read
        byte_left = (inode->length);
        //printf("too many bytes to read");
    }

    else{
        byte_left = nbytes;
        
    }

    //read data
    read_data_execute(inode,buf,byte_left);
    //printf("%d bytes read\n",byte_left);
    //last_read_execute = 0;
    

    return 0;

}

/*
 * read_data_execute
 *   DESCRIPTION: copy the data from data block to the input buffer in execute systemcall handler
 *                     
 *   INPUTS: (inode_t * inode,uint8_t* buf , int32_t nbytes)
 *            inode: the inode of last open file
 *            buf: the buffer to store the data
 *            nbytes: the number of bytes store in the buf
 *   OUTPUTS: none
 *   RETURN VALUE: nbytes 
 *   SIDE EFFECTS: none
 */
int32_t read_data_execute(inode_t * inode,uint8_t* buf , int32_t nbytes){
    int i,j;

    uint32_t block_begin , block_end ,block_idx;
    uint32_t begin_byte;
    uint32_t buf_idx; 
    uint32_t cur_byte;
    int32_t total_bytes;
    data_t * temp;
    //how many bytes left to transfer to the buffer
    total_bytes = nbytes;
    buf_idx = 0;
    //find the starting/ending block in inode and starting address in the data block
    block_begin = 0;
    begin_byte = 0; 
    block_end = (nbytes)/DATA_SIZE; 
    //printf("begin %d , end %d ,byte %d \n", block_begin,block_end,begin_byte);
   
    
    cur_byte = begin_byte;
    //begin to read each data block 
    for (i = block_begin ; i < block_end+1; i++){

        //get the data block from inode
        block_idx = inode->data_add[i]; 
        temp = fs_ptr+ (BLOCK_OFFSET * (block_idx+1+(boot_ptr->num_inode)));
        
        //copy data from data block to buf byte by byte
        for(j = 0 ; j < total_bytes ; j++){
                
                
                
                buf[buf_idx] = *(temp->data+cur_byte);
                //printf("%c", buf[buf_idx]);
                buf_idx++;
                
                
               //printf("cur_byte %d\n",cur_byte);
               cur_byte++;
               
                //if the next byte to read is on the other block ,break
                //4096 means the end of a data block(4kb size) , should transfer to a new block
                if(cur_byte==4096){
                    //starting address of data in the  block reset to 0
                    cur_byte = 0;
                    // calculate how many bytes left
                    total_bytes = total_bytes - j;
                    break;
                }
                
                 }
            }
        
   
    
    
    //update the last_read to tell how many bytes already read
    //last_read_execute  = last_read_execute +    nbytes ;
    //printf("last_read , %d",last_read);

    return nbytes;
}

/*
 * dir_write
 *   DESCRIPTION: write directory 
 *                     
 *   INPUTS: (int32_t fd, uint8_t* buf, int32_t nbytes)
 *            fd: none
 *            buf: none
 *            nbytes: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */ 
extern int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/*
 * dir_close
 *   DESCRIPTION: close directory 
 *                     
 *   INPUTS: (int32_t fd, uint8_t* buf, int32_t nbytes)
 *            fd: none
 *            buf: none
 *            nbytes: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */ 
extern int32_t dir_close (int32_t fd){
    return 0;
}


/*
 * file_write
 *   DESCRIPTION: write file 
 *                     
 *   INPUTS: (int32_t fd, uint8_t* buf, int32_t nbytes)
 *            fd: none
 *            buf: none
 *            nbytes: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */ 
extern int32_t file_write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}


/*
 * file_close
 *   DESCRIPTION: close file 
 *                     
 *   INPUTS: (int32_t fd, uint8_t* buf, int32_t nbytes)
 *            fd: none
 *            buf: none
 *            nbytes: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
extern int32_t file_close (int32_t fd){
    return 0;
}


