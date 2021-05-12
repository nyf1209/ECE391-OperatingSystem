/*
 * This portion of code is contributed by Bill, the overnighter.
 * 
 * 
 * The RTC will keeps computer's clock up-to-date
 * 
 * Assuming using DS12887 RTC
 * 
 * for reference to datasheet:
 * https://courses.grainger.illinois.edu/ece391/sp2021/secure/references/ds12887.pdf
 */ 

#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "i8259.h"


/*
 * we should disable NMI interrupt if RTC interrupt is enabled, 
 * the question is that do we have NMI interrupt at this stage 
 */

/* Initialize the RTC_handler */
void RTC_init(void);

/* The RTC_handler */
void RTC_handler(void);

/* change the rate of RTC without change the proper timing (virtualizing RTC) */
void RTC_vir_rate(uint8_t rate);

/* initialize the frequency to 2hz and return 0*/
int32_t RTC_open(const uint8_t* filename);

/* do nothing for this checkpoint and return 0*/
int32_t RTC_close(int32_t fd);

/* RTC_read*/
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes);
/*RTC_write */
int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes);

/* a helper function to check if the input is power of 2*/
int32_t check_power(int32_t number);

#endif

