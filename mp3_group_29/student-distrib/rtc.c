/*
 * This portion of code is contributed by Bill, the overnighter
 * 
 * Assuming using DS12887 RTC
 * 
 * for reference to datasheet:
 * https://courses.grainger.illinois.edu/ece391/sp2021/secure/references/ds12887.pdf
 * 
 * 
 */ 

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "pit.h"

#define RTC_PORT    0x70
#define CMOS_PORT   0x71
#define CRT_A       0x8A
#define CRT_B       0x8B
#define CRT_C       0x8C
#define CRT_D       0x8D
#define SET_PIE     0x40
#define RTC_IRQ_NUM 0x08

/* for different frequencies*/
#define HZ_2        0x0F
#define HZ_4        0x0E
#define HZ_8        0x0D
#define HZ_16       0x0C
#define HZ_32       0x0B
#define HZ_64       0x0A
#define HZ_128      0x09
#define HZ_256      0x08
#define HZ_512      0x07
#define HZ_1024     0x06

/* the maximum frequency we use for this mp is 1024 hz*/
#define MAX_FREQ    1024

/* rtc_interrupt_received flag; default set to 0*/
volatile int32_t rtc_itr_rec1 = 0;
volatile int32_t rtc_itr_rec2 = 0;
volatile int32_t rtc_itr_rec3 = 0;

/* counter for virtualizing RTC*/
volatile int32_t vir_counter1 = 0;
volatile int32_t vir_counter2 = 0;
volatile int32_t vir_counter3 = 0;



/*
 *
 * Control register A 
 * +---+---+---+---+---+---+---+---+
 *  uip dv2 dv1 dv0 rs3 rs2 rs1 rs0
 * +---+---+---+---+---+---+---+---+
 * 
 * rs3 - rs0 can select the rate or disable
 * the rate
 * 
 * Control register B
 * +---+---+---+---+----+--+-----+---+
 *  set pie aie uie sqwe dm 24/12 dse
 * +---+---+---+---+----+--+-----+---+
 * 
 * pie - periodic interrupt enable
 * set to 1 to generate periodic interrupt
 * 
 * Control register C
 * +----+--+--+--+-+-+-+-+
 *  irqf pf af uf 0 0 0 0
 * +----+--+--+--+-+-+-+-+
 * 
 * basically indicate current mode 
 */ 

/* RTC_init
 * DESCRIPTION: Initialize the RTC interrupt by setting the Control register
 * and enable IRQ8
 * INPUTS:none
 * OUTPUTS:none
 * SIDEEFFECTS: enable the RTC interrupt
 */
void RTC_init(void){
    // set PIE to 1 and follow the instruction 
    uint8_t value_d;
    cli();


    outb(CRT_B, RTC_PORT);      // select control register B and disable NMI
    outb(SET_PIE, CMOS_PORT);   // set the PIE bit to be 1 enable periodic interrupt

    outb(CRT_A, RTC_PORT);      // select control register A 
    outb(HZ_256, CMOS_PORT);    // set the frequency to be 256hz

    // enable IRQ8
    enable_irq(RTC_IRQ_NUM);

    // do a check on register d
    outb(CRT_D, RTC_PORT);      // select control register D
    value_d = inb(CMOS_PORT);
    // restore_flags(flags);
}

/* RTC_handler
 * DESCRIPTION: the handler function of RTC interrupt, should be called
 * when RTC interrupt raised
 * INPUTS: none
 * OUTPUTS: EOI signal 
 * SIDEEFFECTS: in mp 3.1 test_interrupt is called.
 */
void RTC_handler(void){
    //test_interrupts();
    cli();

    /* to see the interrupts*/
    //printf("1");

    /*do a read on control register C*/
    outb(CRT_C, RTC_PORT);
    inb(CMOS_PORT);

    /* change the rtc_interrupt_received flag to 1 */
    if (cur_execute_term == 0) {
        rtc_itr_rec1++;
    } else if (cur_execute_term == 1) {
        rtc_itr_rec2++;
    } else {
        rtc_itr_rec3++;
    }
    /* increase the counter used by virtualizing RTC */
    send_eoi(RTC_IRQ_NUM);
    sti();

}

/* RTC_vir_rate
 * DESCRIPTION: To virtualize the RTC
 * INPUTS: rate -- the new frequency to be set
 * OUTPUTS: none
 * SIDEEFFECTS: change the virtualized RTC frequency
 */ 
/* not completely done for checkpoint 2 since it's not used in checkpoint 2*/
void RTC_vir_rate(uint8_t rate){
    /* this is what number the counter has to count to before we stop blocking
        if the input number is not a power of 2, then the decimal portion of rate_idx is discarded*/
    int i;
    uint32_t rate_idx = 1;
    for (i = 0; i < rate; i++) {
        rate_idx = rate_idx*2;
    }
    rate_idx = MAX_FREQ / rate_idx;
    if (cur_execute_term == 0) {
        vir_counter1 = rate_idx - 1;
    } else if (cur_execute_term == 1) {
        vir_counter2 = rate_idx - 1;
    } else if (cur_execute_term == 2) {
        vir_counter3 = rate_idx - 1;
    }

    /* first, set frequency to the maximum frequency allowed (1024 hz)*/
    outb(CRT_A, RTC_PORT);           // select control register A 
    outb(HZ_1024, CMOS_PORT);        // set the frequency to be 1024 hz

    /* keep blocking until the counter reached the desired value*/
    // vir_counter = 0;
    // while (vir_counter < rate_idx) {
    //     // keep blocking and do nothing
    // }
    // vir_counter = 0;
}

/* RTC_open
 * DESCRIPTION: changes the frequency to 2 hz
 * INPUTS: filename -- the name of the file to be opened; not used in this function
 * OUTPUTS: 0 -- indicate successfully set requency to 2 hz
 * SIDEEFFECTS: changes the frequency to 2hz
 */ 
int32_t RTC_open(const uint8_t* filename) {
    // outb(CRT_A, RTC_PORT);      // select control register A 
    // outb(HZ_2, CMOS_PORT);      // set the frequency to be 2 hz
    RTC_vir_rate(1);
    return 0;
}

/* RTC_close
 * DESCRIPTION: close rtc; do nothing and only returns 0
 * INPUTS: fd -- file des1criptor to be closed; not used in this function
 * OUTPUTS: 0 -- to indicate success
 * SIDEEFFECTS: none
 */ 
int32_t RTC_close(int32_t fd) {
    return 0;
}

/* RTC_read
 * DESCRIPTION: blocks until next inerrupt
 * INPUTS: fd -- the file descriptor; not used in this function
 *         buf -- data stored in this buffer; not used in this function
 *         nbytes -- number of bytes to be read; not used in this function
 * OUTPUTS: 0 -- to indicate success
 * SIDEEFFECTS: none
 */ 
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes) {
    sti();
    if (cur_execute_term == 0) {
        rtc_itr_rec1 = 0;
        while (rtc_itr_rec1 < vir_counter1) {
            // block until next interrupt, so do nothing here
        }
        /* reset rtc_interrupt_received flag back to 0*/
        rtc_itr_rec1 = 0;
    } else if (cur_execute_term == 1) {
        rtc_itr_rec2 = 0;
        while (rtc_itr_rec2 < vir_counter2) {
            // block until next interrupt, so do nothing here
        }
        /* reset rtc_interrupt_received flag back to 0*/
        rtc_itr_rec2 = 0;
    } else {
        rtc_itr_rec3 = 0;
        while (rtc_itr_rec3 < vir_counter3) {
            // block until next interrupt, so do nothing here
        }
        /* reset rtc_interrupt_received flag back to 0*/
        rtc_itr_rec3 = 0;
    }

    //printf("2");
    return 0;
}

/* RTC_write
 * DESCRIPTION: the write function that changes the frequency to the number stored in buf
 * INPUTS: fd -- the file descriptor; not used in this function
 *         buf -- data stored in this buffer
 *         nbytes -- number of bytes to be written; not used in this function
 * OUTPUTS: 0 -- succesfully set the frequency
 *          -1 -- if the input is not power of 2, so invalid input
 * SIDEEFFECTS: none
 */ 
int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes) {
    if (buf == NULL) {      /* return unseccessful (-1) if input pointer is NULL*/
        return -1;
    }
    //sti();
    int32_t input_num = *(int32_t*) buf;
    int32_t power_num = check_power(input_num);
    if (power_num == -1) {      /* if power_num = -1, input is not a power of 2*/
        return -1;              /* return -1 if input is not a power of 2*/
    }

    if (power_num < 1 || power_num > 10) {      // if frequency is below 2hz (2^1) or above 1024hz (2^10), return invalid
        return -1;
    }
    //virtualized rtc
    RTC_vir_rate(power_num);
    return 0;
    // cli();
    // outb(CRT_A, RTC_PORT);            // select control register A 
    // char prev_freq = inb(CMOS_PORT);
    // prev_freq = prev_freq & 0xF0;
    // outb(CRT_A, RTC_PORT);            // select control register A 
    // //printf("prev_frequency = %d ", prev_freq);
    // printf("input = %d, power = %d", input_num, power_num);

    // if (power_num == 1) {             // if input is 2 --> 2^1
    //     outb(HZ_2 | prev_freq, CMOS_PORT);        // set the frequency to be 2 hz
    // } else if (power_num == 2) {      // if input is 4 --> 2^2
    //     outb(HZ_4 | prev_freq, CMOS_PORT);        // set the frequency to be 4 hz
    // } else if (power_num == 3) {      // if input is 8 --> 2^3
    //     outb(HZ_8 | prev_freq, CMOS_PORT);        // set the frequency to be 8 hz
    // } else if (power_num == 4) {      // if input is 16 --> 2^4
    //     outb(HZ_16 | prev_freq, CMOS_PORT);       // set the frequency to be 16 hz
    // } else if (power_num == 5) {      // if input is 32 --> 2^5
    //     outb(HZ_32 | prev_freq, CMOS_PORT);       // set the frequency to be 32 hz
    // } else if (power_num == 6) {      // if input is 64 --> 2^6
    //     outb(HZ_64 | prev_freq, CMOS_PORT);       // set the frequency to be 64 hz
    // } else if (power_num == 7) {      // if input is 128 --> 2^7
    //     outb(HZ_128 | prev_freq, CMOS_PORT);      // set the frequency to be 128 hz
    // } else if (power_num == 8) {      // if input is 256 --> 2^8
    //     outb(HZ_256 | prev_freq, CMOS_PORT);      // set the frequency to be 256 hz
    // } else if (power_num == 9) {      // if input is 512 --> 2^9
    //     outb(HZ_512 | prev_freq, CMOS_PORT);      // set the frequency to be 512 hz
    // } else if (power_num == 10) {     // if input is 1024 --> 2^10
    //     outb(HZ_1024 | prev_freq, CMOS_PORT);     // set the frequency to be 1024 hz
    // }
    // sti();
    // return 4;

}

/* check_power
 * DESCRIPTION: to check if the input is a power of 2
 * INPUTS: number -- the number to be checked
 * OUTPUTS: count -- if input is "count"th power of 2
 *          -1 -- if the input is not a power of 2
 * SIDEEFFECTS: none
 */ 
int32_t check_power(int32_t number) {
    int32_t temp = number;
    int32_t count = 0;
    while ((temp > 1)) {        /* while temp still can be divided by 2 (greater than 1)*/
        if ((temp % 2) == 1) {      /* if temp is divisible by 2 --> even number; if not, reminder = 1 and it's an odd number*/
            return -1;            /* if temp is an odd number, it can't be power of 2, so return -1*/
        }
        temp = temp / 2;        /* divide temp by 2 since it's an even number*/
        count++;                /*increment count to indicate how many times number can be divided by 2 and still get an even number*/
    }
    return count;
}
