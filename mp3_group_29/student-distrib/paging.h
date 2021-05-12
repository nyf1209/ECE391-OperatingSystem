/* paging.h - Defines paging
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "systemcall_handler.h"

#define VIDEO_ADDR       0xB8000
#define BB1_ADDR 0xB9000
#define BB2_ADDR 0xBA000
#define BB3_ADDR 0xBB000
/* page table and page directory should all have 1024 entries*/


/* bit shift by 12 to get the highest 20 bits of a 32 bits data*/
#define BIT_SHIFT_TWELVE    12
/* bit shify by 22 to get the highest 10 bits of a 32 bits data*/
#define BIT_SHIFT_TWENTY_TWO    22

/* used to get the last ten bits after bit shift*/
#define TEN_BITS            0x3FF
/* used to get the last twenty bits after bit shift*/
#define TWENTY_BITS         0xFFFFF


/* pointer to the page_table*/
pt_entry *pt_ptr_vid;
pt_entry *pt_ptr;
uint32_t ptr_pd;


/* function to intialize paging*/
extern void paging_init();
/* helper function to set paging for excute*/
extern void set_page(int pid);

/* helper function to set paging for vidmap*/
extern void set_video_mem(uint8_t** screen_start);
/* helper function to undo paging for halt (for vidmap)*/
extern void undo_vidmap();

/* helper function used for scheduling and terminal Switch*/
void switch_term_vim(int32_t vis_term, int32_t exe_term);

#endif

