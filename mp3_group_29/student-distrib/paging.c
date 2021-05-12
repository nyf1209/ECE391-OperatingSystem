/* Paging.c - To initialize paging
 * vim:ts=4 noexpandtab
 */

#include "paging.h"
#include "term_switch.h"
#include "pit.h"


uint32_t vidmap_pd_idx;
uint32_t vidmap_pt_idx;


uint32_t vim_pt_idx;
uint32_t user_vim_pt_idx;
uint32_t user_vim_pd_idx;

/* paging_init 
 * Inputs: none
 * Return Value: none
 * Function: initialize page directory and page table with video memory at 0xb8000 and
 *          kernel at 0x40000, and set CR0,CR3,CR4 to enable paging */
void paging_init() {
    /* temporary variables used*/
    unsigned int i, temp, bb1, bb2, bb3;
    /* pointer to the page_table*/
    pt_ptr = &page_table[0];

    /* for video memory (at virtual memory 0xb8000 = 0000 0000 0000 1011 1000 0000 0000 0000)
     second 10 bits are (00 1011 1000 = 0xb8) --> corresponds to Page_table[0xb8]*/
    temp = (VIDEO_ADDR >> BIT_SHIFT_TWELVE) & TEN_BITS;   /*bit shift 0xb8000 by 12 bits (discard the lowest 12 bits) to get 0xb8
                                    0x3FF = 11 1111 1111; it's making sure only the ten lowest bits are possbily 1*/

    vim_pt_idx = temp;
    
    page_table[temp].present = 1;   /* set to present*/
    page_table[temp].r_w = 1;
    page_table[temp].u_s = 0;
    page_table[temp].pwt = 0;
    page_table[temp].pcd = 0;
    page_table[temp].accessed = 0;
    page_table[temp].dirty = 0;
    page_table[temp].pat = 0;
    page_table[temp].global_bit = 1;
    page_table[temp].avail = 0;
    page_table[temp].page_base_addr = (VIDEO_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS; /*bit shift 0xb8000 by 12 bits (discard the lowest 12 bits)
                                                                0xFFFFF = 1111 1111 1111 1111 1111;
                                                                it's making sure only the 20 lowest bits are possbily 1;
                                                                this gives the 20 most significant bits of physical memory of video memory*/
    // map virtual memory address 0xB9000 to physical memory address 0xB9000
    bb1 = (BB1_ADDR >> BIT_SHIFT_TWELVE) & TEN_BITS;

    page_table[bb1].present = 1;   /* set to present*/
    page_table[bb1].r_w = 1;
    page_table[bb1].u_s = 0;
    page_table[bb1].pwt = 0;
    page_table[bb1].pcd = 0;
    page_table[bb1].accessed = 0;
    page_table[bb1].dirty = 0;
    page_table[bb1].pat = 0;
    page_table[bb1].global_bit = 1;
    page_table[bb1].avail = 0;
    page_table[bb1].page_base_addr = (BB1_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;

    // map virtual memory address 0xBA000 to physical memory address 0xBA000
    bb2 = (BB2_ADDR >> BIT_SHIFT_TWELVE) & TEN_BITS;
    page_table[bb2].present = 1;   /* set to present*/
    page_table[bb2].r_w = 1;
    page_table[bb2].u_s = 0;
    page_table[bb2].pwt = 0;
    page_table[bb2].pcd = 0;
    page_table[bb2].accessed = 0;
    page_table[bb2].dirty = 0;
    page_table[bb2].pat = 0;
    page_table[bb2].global_bit = 1;
    page_table[bb2].avail = 0;
    page_table[bb2].page_base_addr = (BB2_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;

    // map virtual memory address 0xBB000 to physical memory address 0xBB000
    bb3 = (BB3_ADDR >> BIT_SHIFT_TWELVE) & TEN_BITS;
    page_table[bb3].present = 1;   /* set to present*/
    page_table[bb3].r_w = 1;
    page_table[bb3].u_s = 0;
    page_table[bb3].pwt = 0;
    page_table[bb3].pcd = 0;
    page_table[bb3].accessed = 0;
    page_table[bb3].dirty = 0;
    page_table[bb3].pat = 0;
    page_table[bb3].global_bit = 1;
    page_table[bb3].avail = 0;
    page_table[bb3].page_base_addr = (BB3_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;


    /* for the rest of the page table*/
    for (i = 0; i < PAGE_ENTRY; i++) {
        if (i != temp && i != bb1 && i != bb2 && i != bb3) {        /* set everything else in the page table to be empty/not present*/
            page_table[i].present = 0;
            page_table[i].r_w = 0;
            page_table[i].u_s = 0;
            page_table[i].pwt = 0;
            page_table[i].pcd = 0;
            page_table[i].accessed = 0;
            page_table[i].dirty = 0;
            page_table[i].pat = 0;
            page_table[i].global_bit = 0;
            page_table[i].avail = 0;
            page_table[i].page_base_addr = 0;
        }
    }

    /* for video memory (0xb8000 = 0000 0000 0000 1011 1000 0000 0000 0000)
     first 10 bits are all 0s --> corresponds to Page directory[0]*/
    /* all bits except present, page_size, pt_addr/page_base_addr are set to 0 because not used for checkpoint 1*/
    page_directory[0].pd_entry_union.k.present = 1;    /* set to present*/
    page_directory[0].pd_entry_union.k.r_w = 1;
    page_directory[0].pd_entry_union.k.u_s = 0;
    page_directory[0].pd_entry_union.k.pwt = 0;
    page_directory[0].pd_entry_union.k.pcd = 0;
    page_directory[0].pd_entry_union.k.accessed = 0;
    page_directory[0].pd_entry_union.k.reserved = 0;
    page_directory[0].pd_entry_union.k.page_size = 0;    /* 0 because it's 4kb*/
    page_directory[0].pd_entry_union.k.global_bit = 1;
    page_directory[0].pd_entry_union.k.avail = 0;
    page_directory[0].pd_entry_union.k.pt_addr = ((uint32_t)pt_ptr >> BIT_SHIFT_TWELVE); /* only using the highest 20 (= 32-12) bits of addr of page_table
                                                    bit shift by 12 to get the highest 20 bits*/

    /* for kernel (0x0040000 = 0000 0000 0100 0000 0000 0000 0000 0000)
    first 10 bits are 0x1 --> corresponds to page directory[1]*/
    page_directory[1].pd_entry_union.m.present = 1;        /* set to present*/
    page_directory[1].pd_entry_union.m.r_w = 1;
    page_directory[1].pd_entry_union.m.u_s = 0;
    page_directory[1].pd_entry_union.m.pwt = 0;
    page_directory[1].pd_entry_union.m.pcd = 0;
    page_directory[1].pd_entry_union.m.accessed = 0; 
    page_directory[1].pd_entry_union.m.dirty = 0;
    page_directory[1].pd_entry_union.m.page_size = 1;    /* 1 because it's 4mb*/
    page_directory[1].pd_entry_union.m.global_bit = 0;
    page_directory[1].pd_entry_union.m.avail = 0; 
    page_directory[1].pd_entry_union.m.pat = 0;
    page_directory[1].pd_entry_union.m.reserved = 0; 
    page_directory[1].pd_entry_union.m.page_base_addr = 0x1;     /* the kernel should be mapped to physical memory at 0x0040000
                                                    the highest 10 bits are 0000 0000 01, which is 0x1 */


    /* the rest of the page_directory are not used for checkpoint 1, so set to not present
     */
    for (i = 2; i < PAGE_ENTRY; i++) {
        page_directory[i].pd_entry_union.m.present = 0;
    }
    ptr_pd = (uint32_t)(&page_directory[0]);
    /* set CR0, CR3, CR4*/
    asm ("                                                      \n\
        movl ptr_pd, %%eax                                      \n\
        movl %%eax, %%cr3                                       \n\
        movl %%cr4, %%eax                                       \n\
        orl $0x00000010, %%eax                                  \n\
        movl %%eax, %%cr4                                       \n\
        movl %%cr0, %%eax                                       \n\
        orl $0x80000001, %%eax                                  \n\
        movl %%eax, %%cr0                                       \n\
        "
        : /* no outputs */
        : /* no inputs*/
        : "eax", "memory", "cc"
    );
}

#define MEM  0x08000000
#define EIGHT_MEGA 8388608
#define FOUR_MEGA  4194304
/* set_page
 * Inputs: pid -- integer indicating the process id
 * Return Value: none
 * Function: set a 4mb paging for virtual addr 128 mb and flush CR3 */
void set_page(int pid){
    /*(0x08000000 = 0000 1000 0000 0000 0000 0000 0000 0000) --> virtual memory at 128 mb
    highest 10 bits = 0000 1000 00 = 32 in decimal, which is used below as index*/
    page_directory[32].pd_entry_union.m.present = 1;  
    page_directory[32].pd_entry_union.m.r_w = 1;
    page_directory[32].pd_entry_union.m.u_s = 1;
    page_directory[32].pd_entry_union.m.pwt = 0;
    page_directory[32].pd_entry_union.m.pcd = 0;
    page_directory[32].pd_entry_union.m.accessed = 0; 
    page_directory[32].pd_entry_union.m.dirty = 0;
    page_directory[32].pd_entry_union.m.page_size = 1;    /* 1 because it's 4mb*/
    page_directory[32].pd_entry_union.m.global_bit = 0;
    page_directory[32].pd_entry_union.m.avail = 0; 
    page_directory[32].pd_entry_union.m.pat = 0;
    page_directory[32].pd_entry_union.m.reserved = 0; 
    page_directory[32].pd_entry_union.m.page_base_addr = 0x2 + pid;     /* starting from physical memory 8mb --> highest 10 bits = 0x2*/

    /* to flush RC3*/
    asm("                               \n\
        movl %%cr3, %%eax               \n\
        movl %%eax, %%cr3               \n\
        "
        : /* no outputs*/
        : /* no inputs*/
        :"eax", "memory", "cc"
    );

}

/* set_video_mem
 * Inputs: screen_start - a double pointer which pointer to a pointer to the new virtual video mem addr
 * Return Value: none
 * Function: map this new virtual video memory to the same physical video memory paging
 * sideeffects: create another page table, modifies the page directory, flush CR3*/
void set_video_mem(uint8_t** screen_start) {
    int temp, i;
    uint32_t mem_addr = (uint32_t)*screen_start;
    int32_t video_mem_pd_idx = (mem_addr >> BIT_SHIFT_TWENTY_TWO) & TEN_BITS;
    pt_ptr_vid = &page_table_vidmap[0];

    user_vim_pd_idx = video_mem_pd_idx;
    /*use the second ten bits of mem_addr*/
    temp = (mem_addr >> BIT_SHIFT_TWELVE) & TEN_BITS;
    user_vim_pt_idx = temp;

    page_table_vidmap[temp].present = 1;   /* set to present*/
    page_table_vidmap[temp].r_w = 1;        /* read and write*/
    page_table_vidmap[temp].u_s = 1;        /* dpl = user level*/
    page_table_vidmap[temp].pwt = 0;
    page_table_vidmap[temp].pcd = 0;
    page_table_vidmap[temp].accessed = 0;
    page_table_vidmap[temp].dirty = 0;
    page_table_vidmap[temp].pat = 0;
    page_table_vidmap[temp].global_bit = 1;
    page_table_vidmap[temp].avail = 0;

    // set the paging depending on current executing and visible terminal
    if (cur_execute_term == cur_vis_term) {
        page_table_vidmap[temp].page_base_addr = (VIDEO_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
         /*bit shift 0xb8000 by 12 bits (discard the lowest 12 bits)
                                                                0xFFFFF = 1111 1111 1111 1111 1111;
                                                                it's making sure only the 20 lowest bits are possbily 1;
                                                                this gives the 20 most significant bits of physical memory of video memory*/
    } else if (cur_execute_term == 0) {
        page_table_vidmap[temp].page_base_addr = (BB1_ADDR>> BIT_SHIFT_TWELVE) & TWENTY_BITS;
    } else if (cur_execute_term == 1) {
        page_table_vidmap[temp].page_base_addr = (BB2_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
    } else {
        page_table_vidmap[temp].page_base_addr = (BB3_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
    }


    /* for the rest of the page table*/
    for (i = 0; i < PAGE_ENTRY; i++) {
        if (i != temp) {        /* set everything else in the page table to be empty/not present*/
            page_table_vidmap[i].present = 0;
        }
    }

    page_directory[video_mem_pd_idx].pd_entry_union.k.present = 1;    /* set to present*/
    page_directory[video_mem_pd_idx].pd_entry_union.k.r_w = 1;       /* set to read and write*/
    page_directory[video_mem_pd_idx].pd_entry_union.k.u_s = 1;      /* set DPL to user level*/
    page_directory[video_mem_pd_idx].pd_entry_union.k.pwt = 0;
    page_directory[video_mem_pd_idx].pd_entry_union.k.pcd = 0;
    page_directory[video_mem_pd_idx].pd_entry_union.k.accessed = 0;
    page_directory[video_mem_pd_idx].pd_entry_union.k.reserved = 0;
    page_directory[video_mem_pd_idx].pd_entry_union.k.page_size = 0;    /* 0 because it's 4kb*/
    page_directory[video_mem_pd_idx].pd_entry_union.k.global_bit = 1;
    page_directory[video_mem_pd_idx].pd_entry_union.k.avail = 0;
    page_directory[video_mem_pd_idx].pd_entry_union.k.pt_addr = ((uint32_t)pt_ptr_vid >> BIT_SHIFT_TWELVE) & TWENTY_BITS; 
                                                    /* only using the highest 20 (= 32-12) bits of addr of page_table
                                                    bit shift by 12 to get the highest 20 bits*/
    vidmap_pt_idx = temp;
    vidmap_pd_idx = video_mem_pd_idx;
    if (cur_vis_term == cur_execute_term) {

    }


    /* to flush RC3*/
    asm("                               \n\
        movl %%cr3, %%eax               \n\
        movl %%eax, %%cr3               \n\
        "
        : /* no outputs*/
        : /* no inputs*/
        :"eax", "memory", "cc"
    );

}
/* undo_vidmap
 * Inputs: none
 * Return Value: none
 * Function: unmap the new virtual video memory passed in vidmap to the physical video memory paging
 * sideeffects: modifies the page directory and page table, flush CR3*/
void undo_vidmap() {
    page_table_vidmap[vidmap_pt_idx].present = 0;   /* set to not present*/
    page_directory[vidmap_pd_idx].pd_entry_union.k.present = 0;    /* set to not present*/

    /* to flush RC3*/
    asm("                               \n\
        movl %%cr3, %%eax               \n\
        movl %%eax, %%cr3               \n\
        "
        : /* no outputs*/
        : /* no inputs*/
        :"eax", "memory", "cc"
    );

}

/* switch_term_vim
 * Inputs: vis_term -- integer indicating which terminal is currently visible
            exe_term -- integer indicating which terminal we are switching to
 * Return Value: none
 * Function: map the virtual B8000 to either physical B8000, BB1, BB2, or BB3 depending on the current visible and executing terminals
 * sideeffects: modifies the page table, flush CR3*/
void switch_term_vim(int32_t vis_term, int32_t exe_term) {
    if (pid_vidmap_flag[current_id] == 0) {
        page_table_vidmap[user_vim_pt_idx].present = 0;
        //page_directory[user_vim_pd_idx].pd_entry_union.k.present = 0;    /* set to not present*/

    } else {
        page_directory[user_vim_pd_idx].pd_entry_union.k.present = 1;
        page_table_vidmap[user_vim_pt_idx].present = 1;
    }

    if (vis_term == exe_term) {
        page_table[vim_pt_idx].page_base_addr = (VIDEO_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        if (pid_vidmap_flag[current_id] == 1) {
            page_table_vidmap[user_vim_pt_idx].page_base_addr = (VIDEO_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        }
    } else if (exe_term == 0) {
        page_table[vim_pt_idx].page_base_addr = (BB1_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        if (pid_vidmap_flag[current_id] == 1) {
            page_table_vidmap[user_vim_pt_idx].page_base_addr = (BB1_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        }

    } else if (exe_term == 1) {
        //set virtual mem B8000 to physical addr BA000 - BAFFF
        page_table[vim_pt_idx].page_base_addr = (BB2_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        if (pid_vidmap_flag[current_id] == 1) {
            page_table_vidmap[user_vim_pt_idx].page_base_addr = (BB2_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        }

    } else {
        page_table[vim_pt_idx].page_base_addr = (BB3_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        if (pid_vidmap_flag[current_id] == 1) {
            page_table_vidmap[user_vim_pt_idx].page_base_addr = (BB3_ADDR >> BIT_SHIFT_TWELVE) & TWENTY_BITS;
        }
    }
    /* to flush RC3*/
    asm("                               \n\
        movl %%cr3, %%eax               \n\
        movl %%eax, %%cr3               \n\
        "
        : /* no outputs*/
        : /* no inputs*/
        :"eax", "memory", "cc"
    );
    return;

}


