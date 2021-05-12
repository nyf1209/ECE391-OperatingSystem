/*
 * This portion of code is contributed by Bill, correct the code if I was wrong
 * 
 * for reference check: https://wiki.osdev.org/Keyboard
 * and https://wiki.osdev.org/%228042%22_PS/2_Controller
 * 
 * Assuming we are using "8042" PS/2 controller 
 * 
 */

#include "keyboard.h"

#define KB_DATA_PORT 0x60
#define KB_STAT_PORT 0x64
#define SC_BACKSP    0x0E
#define SC_TAB       0x0F
#define SC_ENTR      0x1C
#define KEYBD_IRQ    0x01
#define SC_LCTRL     0x1D
#define SC_LCTRL_R   0x9D
#define SC_ALT       0x38
#define SC_ALT_R     0xB8
#define SC_LSHIFT    0x2A
#define SC_RSHIFT    0x36
#define SC_LSHIFT    0x2A
#define SC_RSHIFT    0x36
#define SC_LSHIFT_R  0xAA
#define SC_RSHIFT_R  0xB6
#define SC_CAPS      0x3A
#define SC_CAPS_R    0xBA
#define SHIFT_BIT    0x08
#define CAPS_BIT     0x04
#define ALT_BIT      0x02
#define CTRL_BIT     0x01
#define SHIFT_CAPS   0x0C
#define SCANCODE_ST  0x01
#define SCANCODE_ED  0x3A
#define CLEAR        0xFF
#define SCAN_PRECENT 0x04
#define ASCI_PERCENT 0x25
#define SCAN_SET_RG  52
#define SC_SPACE     0x37
#define BUF_SIZE     128
#define KEY_F1      0x3B
#define KEY_F2      0x3C
#define KEY_F3      0x3D
#define KEY_F4      0x3E




static int8_t scancode_l[] = "1234567890-=\b qwertyuiop[]\n asdfghjkl;'` \\zxcvbnm,./";
static int8_t scancode_u[] = "!@#$ ^&*()_+\b QWERTYUIOP{}\n ASDFGHJKL:\"~ |ZXCVBNM<>?";
static int8_t scancode_C[] = "1234567890-=\b QWERTYUIOP[]\n ASDFGHJKL;'` \\ZXCVBNM,./";
static int8_t scancode_Cu[] = "!@#$ ^&*()_+\b qwertyuiop{}\n asdfghjkl:\"~ |zxcvbnm<>?";
static int CAPSLOCK_HOLDON;
// SCAC stands for shift, capslock, alt and ctrl
static int8_t SCAC;


int CAPS_state;
int8_t line_buf1[BUF_SIZE];
int8_t line_buf2[BUF_SIZE];
int8_t line_buf3[BUF_SIZE];
int8_t key_buf1[BUF_SIZE];
int8_t key_buf2[BUF_SIZE];
int8_t key_buf3[BUF_SIZE];

int curr_idx1;
int curr_idx2;
int curr_idx3;
int read_flag1;
int read_flag2;
int read_flag3;

/*
 * keyboard_init
 * DESCRIPTION: enable the IRQ1 and then
 * sending necessary command to the command port
 * INPUTS:none
 * OUTPUTS:none
 * SIDEEFFECTS: enable the IRQ1 keyboard interrupt
 */
void keyboard_init(void){
    //clear flags
    cli();
    // clear variables 
    CAPSLOCK_HOLDON = 0;
    SCAC = 0;
    curr_idx1 = 0;
    curr_idx2 = 0;
    curr_idx3 = 0;
    read_flag1 = 0;
    read_flag2 = 0;
    read_flag3 = 0;
    // enable IRQ1
    enable_irq(KEYBD_IRQ);
}

/*
 * keyboard_handler
 * DESCRIPTION: The handler function should be 
 * called when keyboard IRQ is initiallized
 * INPUTS: none
 * OUTPUTS: EOI signal 
 * SIDEEFFECTS: put the character or string on the console
 * based on user input
 */
void keyboard_handler(void){
    // When IRQ1 occurs we just need to read from IO port 0x60
    // no need to check status register
    //unsigned long(flags);
    int i;
    uint8_t scan_code;
    uint8_t ascii_char;
    // clear IF and save
    // IRQ1
    send_eoi(KEYBD_IRQ);
    scan_code = inb(KB_DATA_PORT);
    // make sure within range 0x02 - 02C
    if(cur_vis_term == 0){
        switch(scan_code){
            case SC_LSHIFT: case SC_RSHIFT:
                SCAC |= SHIFT_BIT;
                return;
            case SC_LSHIFT_R: case SC_RSHIFT_R:
                SCAC &= (CLEAR - SHIFT_BIT);
                return;
            case SC_CAPS:
                if(CAPSLOCK_HOLDON == 0){
                    CAPSLOCK_HOLDON = 1;
                    if(SCAC >= CAPS_BIT){
                        SCAC &= (CLEAR - CAPS_BIT);
                    }
                    else{
                        SCAC |= CAPS_BIT;
                    }
                }
                return;
            case SC_CAPS_R:
                CAPSLOCK_HOLDON = 0;
                return;
            case SC_ALT:
                SCAC |= ALT_BIT;
                return;
            case SC_ALT_R:
                SCAC &= (CLEAR - ALT_BIT);
                return;
            case SC_LCTRL:
                SCAC |= CTRL_BIT;
                return;
            case SC_LCTRL_R:
                SCAC &= (CLEAR - CTRL_BIT);
                return;
            default:
                /* writing to the buffer*/
                if(scan_code > SCANCODE_ED || scan_code < SCANCODE_ST){
                    if(scan_code != KEY_F1 && scan_code != KEY_F2 && scan_code != KEY_F3)
                        return;
                }
                /* scancode table offset 2*/
                ascii_char = convert_to_ascii(scan_code - 2);
                /* enable keyboard only when flag is set*/
                if(terminal_enabled1 == 1){
                    if(ascii_char != 0){
                        if(ascii_char == '\n'){
                            /* do copy to line_buf */
                            switch_term_vim(0, 0);
                            key_buf1[curr_idx1] = ascii_char;
                            for(i = 0; i < BUF_SIZE; i++){
                                line_buf1[i] = key_buf1[i];
                                key_buf1[i] = 0;
                            }
                            /* put on the screen */
                            /* temporily switch maping of virtual 0xB8000 */
                            putc(ascii_char);
                            /* clear curr_idx */
                            curr_idx1 = 0;
                            /* set read_flag */
                            read_flag1 = 1;
                            switch_term_vim(0, cur_execute_term);
                        }
                        else if(ascii_char == '\b'){
                            if(curr_idx1 == 0){
                                /* do nothing */
                            }
                            else{
                                /* decrement idx */
                                curr_idx1--;
                                /* put on the screen */
                                switch_term_vim(0, 0);
                                putc(ascii_char);
                                switch_term_vim(0, cur_execute_term);
                            }
                        }
                        else if(curr_idx1 == BUF_SIZE - 1){
                            // do nothing 
                        }
                        else{
                            key_buf1[curr_idx1] = ascii_char;
                            switch_term_vim(0, 0);
                            putc(ascii_char);
                            switch_term_vim(0, cur_execute_term);
                            curr_idx1++;
                        }
                    }
                }
                break;
        }
    }
    else if(cur_vis_term == 1){
        switch(scan_code){
            case SC_LSHIFT: case SC_RSHIFT:
                SCAC |= SHIFT_BIT;
                return;
            case SC_LSHIFT_R: case SC_RSHIFT_R:
                SCAC &= (CLEAR - SHIFT_BIT);
                return;
            case SC_CAPS:
                if(CAPSLOCK_HOLDON == 0){
                    CAPSLOCK_HOLDON = 1;
                    if(SCAC >= CAPS_BIT){
                        SCAC &= (CLEAR - CAPS_BIT);
                    }
                    else{
                        SCAC |= CAPS_BIT;
                    }
                }
                return;
            case SC_CAPS_R:
                CAPSLOCK_HOLDON = 0;
                return;
            case SC_ALT:
                SCAC |= ALT_BIT;
                return;
            case SC_ALT_R:
                SCAC &= (CLEAR - ALT_BIT);
                return;
            case SC_LCTRL:
                SCAC |= CTRL_BIT;
                return;
            case SC_LCTRL_R:
                SCAC &= (CLEAR - CTRL_BIT);
                return;
            default:
                /* writing to the buffer*/
                if(scan_code > SCANCODE_ED || scan_code < SCANCODE_ST){
                    if(scan_code != KEY_F1 && scan_code != KEY_F2 && scan_code != KEY_F3)
                        return;
                }
                /* scancode table offset 2*/
                ascii_char = convert_to_ascii(scan_code - 2);
                /* enable keyboard only when flag is set*/
                if(terminal_enabled2 == 1){
                    if(ascii_char != 0){
                        if(ascii_char == '\n'){
                            /* do copy to line_buf */
                            switch_term_vim(0, 0);
                            key_buf2[curr_idx2] = ascii_char;
                            for(i = 0; i < BUF_SIZE; i++){
                                line_buf2[i] = key_buf2[i];
                                key_buf2[i] = 0;
                            }
                            /* put on the screen */
                            /* temporily switch maping of virtual 0xB8000 */
                            putc(ascii_char);
                            /* clear curr_idx */
                            curr_idx2 = 0;
                            /* set read_flag */
                            read_flag2 = 1;
                            switch_term_vim(1, cur_execute_term);
                        }
                        else if(ascii_char == '\b'){
                            if(curr_idx2 == 0){
                                /* do nothing */
                            }
                            else{
                                /* decrement idx */
                                curr_idx2--;
                                /* put on the screen */
                                switch_term_vim(1, 1);
                                putc(ascii_char);
                                switch_term_vim(1, cur_execute_term);
                            }
                        }
                        else if(curr_idx2 == BUF_SIZE - 1){
                            // do nothing 
                        }
                        else{
                            key_buf2[curr_idx2] = ascii_char;
                            switch_term_vim(1, 1);
                            putc(ascii_char);
                            switch_term_vim(1, cur_execute_term);
                            curr_idx2++;
                        }
                    }
                }
                break;
        }
    }
    else if(cur_vis_term == 2){
        switch(scan_code){
            case SC_LSHIFT: case SC_RSHIFT:
                SCAC |= SHIFT_BIT;
                return;
            case SC_LSHIFT_R: case SC_RSHIFT_R:
                SCAC &= (CLEAR - SHIFT_BIT);
                return;
            case SC_CAPS:
                if(CAPSLOCK_HOLDON == 0){
                    CAPSLOCK_HOLDON = 1;
                    if(SCAC >= CAPS_BIT){
                        SCAC &= (CLEAR - CAPS_BIT);
                    }
                    else{
                        SCAC |= CAPS_BIT;
                    }
                }
                return;
            case SC_CAPS_R:
                CAPSLOCK_HOLDON = 0;
                return;
            case SC_ALT:
                SCAC |= ALT_BIT;
                return;
            case SC_ALT_R:
                SCAC &= (CLEAR - ALT_BIT);
                return;
            case SC_LCTRL:
                SCAC |= CTRL_BIT;
                return;
            case SC_LCTRL_R:
                SCAC &= (CLEAR - CTRL_BIT);
                return;
            default:
                /* writing to the buffer*/
                if(scan_code > SCANCODE_ED || scan_code < SCANCODE_ST){
                    if(scan_code != KEY_F1 && scan_code != KEY_F2 && scan_code != KEY_F3)
                        return;
                }
                /* scancode table offset 2*/
                ascii_char = convert_to_ascii(scan_code - 2);
                /* enable keyboard only when flag is set*/
                if(terminal_enabled3 == 1){
                    if(ascii_char != 0){
                        if(ascii_char == '\n'){
                            /* do copy to line_buf */
                            switch_term_vim(0, 0);
                            key_buf3[curr_idx3] = ascii_char;
                            for(i = 0; i < BUF_SIZE; i++){
                                line_buf3[i] = key_buf3[i];
                                key_buf3[i] = 0;
                            }
                            /* put on the screen */
                            /* temporily switch maping of virtual 0xB8000 */
                            putc(ascii_char);
                            /* clear curr_idx */
                            curr_idx3 = 0;
                            /* set read_flag */
                            read_flag3 = 1;
                            switch_term_vim(2, cur_execute_term);
                        }
                        else if(ascii_char == '\b'){
                            if(curr_idx3 == 0){
                                /* do nothing */
                            }
                            else{
                                /* decrement idx */
                                curr_idx3--;
                                /* put on the screen */
                                switch_term_vim(2, 2);
                                putc(ascii_char);
                                switch_term_vim(2, cur_execute_term);
                            }
                        }
                        else if(curr_idx3 == BUF_SIZE - 1){
                            // do nothing 
                        }
                        else{
                            key_buf3[curr_idx3] = ascii_char;
                            switch_term_vim(2, 2);
                            putc(ascii_char);
                            switch_term_vim(2, cur_execute_term);
                            curr_idx3++;
                        }
                    }
                }
                break;
        }
    }
}


/*
 * convert_to_ascii
 * DESCRIPTION: convert the scan code into ascii code
 * based on look up table.Assuming we are using scan 
 * code set 1
 * INPUTS: scan
 * OUTPUTS: corresponding ascii character
 */ 
uint8_t convert_to_ascii(uint8_t scan){
    /* space check */
    if(scan == SC_SPACE)
        return ' ';
    /* range check */
    if(scan > SCAN_SET_RG){
        switch (SCAC)
        {
            case ALT_BIT: case ALT_BIT + CAPS_BIT:
                switch(scan + 2){
                    case KEY_F1:
                        //printf("enter alt+f1\n");
                        // set CAPS_state based on current SCAC
                        if((SCAC & CAPS_BIT) == CAPS_BIT){
                            CAPS_state = 1;
                        }
                        else{
                            CAPS_state = 0;
                        }
                        term_switch(0);
                        // change SCAC based on new CAPS_state
                        if(CAPS_state == 1){
                            SCAC = SCAC | CAPS_BIT;
                        }
                        else{
                            SCAC = SCAC & (0xFF - CAPS_BIT);
                        }
                        break;
                    case KEY_F2:
                        //printf("enter alt+f2\n");
                        // set CAPS_state based on current SCAC
                        if((SCAC & CAPS_BIT) == CAPS_BIT){
                            CAPS_state = 1;
                        }
                        else{
                            CAPS_state = 0;
                        }
                        term_switch(1);
                        // change SCAC based on new CAPS_state
                        if(CAPS_state == 1){
                            SCAC = SCAC | CAPS_BIT;
                        }
                        else{
                            SCAC = SCAC & (0xFF - CAPS_BIT);
                        }
                        break;
                    case KEY_F3:
                        //printf("enter alt+f3\n");
                        // set CAPS_state based on current SCAC
                        if((SCAC & CAPS_BIT) == CAPS_BIT){
                            CAPS_state = 1;
                        }
                        else{
                            CAPS_state = 0;
                        }
                        term_switch(2);
                        // change SCAC based on new CAPS_state
                        if(CAPS_state == 1){
                            SCAC = SCAC | CAPS_BIT;
                        }
                        else{
                            SCAC = SCAC & (0xFF - CAPS_BIT);
                        }
                        break;
                    default:
                        return 0;
                    //case KEY_F4:
                        // optionally, terminate current process
                }
                return 0;
            default:
                return 0;
        }
        return 0;
    }
    /* case check */
    switch(SCAC){
        case SHIFT_BIT:  // pressing shift
            if(scan == SCAN_PRECENT)
                return ASCI_PERCENT;
            return scancode_u[scan];
        case CAPS_BIT:   // CAPSLOCK ON
            return scancode_C[scan];\
        case CAPS_BIT + CTRL_BIT:
        case CTRL_BIT:   // pressing CTRL
            if (scancode_l[scan] == 'l'){
                /* reset screen */
                reset_screen();
            }
            return 0;
        case ALT_BIT + CAPS_BIT:
        case ALT_BIT:    // pressing ALT
            // disabled
        case SHIFT_CAPS: // CAPSLOCK ON and pressing shift, (why do this?)
            if(scan == SCAN_PRECENT)
                return ASCI_PERCENT;
            return scancode_Cu[scan];
        case 0:         // no special key pressing or pressed
            return scancode_l[scan];
        default:        // all other combo of special keys, disabled
            // disabled
            // only above cases allowed 
            return 0;
    }
    /* error check */
    printf("Aieee! there is bug!");
    return -1;
}

