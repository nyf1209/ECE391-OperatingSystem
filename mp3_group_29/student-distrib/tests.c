#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystem.h"
#include "rtc.h"
#include "systemcall_handler.h"
#include "terminal.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
/* check point 1 tests */

/* divide by zero exception test
 * 
 * Inputs: None
 * Outputs: none
 * Side Effects: enter infinite loop and
 * show exception message if sucess
 * Coverage: Load IDT, exception handler and wrap
 * Files: idt.c/h exception_handler.c/h
 * exception_wrap.h/S
 */
void divide_exception(){
	int a;
	int b;
	int c;
	a = 1;
	b = 0;
	c = a/b;
}

/* double fualt exception test
 * 
 * Inputs: None
 * Outputs: none
 * Side Effects: enter infinite loop and
 * show exception message if sucess
 * Coverage: Load IDT, exception handler and wrap, paging
 * Files: idt.c/h exception_handler.c/h
 * exception_wrap.h/S paging.c/h
 */
void double_fault_test(){
	int* ptr = NULL; 
 	int testVar; 
 	testVar = *(ptr); 
}

/* overflow exception test
 * 
 * Inputs: None
 * Outputs: none
 * Side Effects: enter infinite loop and
 * show exception message if sucess
 * Coverage: Load IDT, exception handler and wrap
 * Files: idt.c/h exception_handler.c/h
 * exception_wrap.h/S 
 */
void overflow_test(){
	// this should raise overflow exception
	asm ("		\n\
	int $0x04	\n\
	");
}




/* Checkpoint 2 tests */
/* file open test
 * 
 * Inputs: None
 * Outputs: none
 * Side Effects: open three text file , print out the file name
 * Coverage: filesystem
 * Files: filesystem.c/h
 * 
 */
void file_open_test(){

	//file name
	uint8_t a[32] = "frame0.txt";
	uint8_t b[32] = "frame1.txt";
	uint8_t c[33] = "verylargetextwithverylongname.txt";
	
	
	file_open(a);
	file_open(b);
	file_open(c);

}

/* file read test
 * 
 * Inputs: None
 * Outputs: none
 * Side Effects: open three text file and print the data 
 * of last file verylargetextwithverylongname.txt for two times
 * Coverage: filesystem
 * Files: filesystem.c/h
 * 
 */
void file_read_test(){
	//file name
	uint8_t a[32] = "frame0.txt";
	uint8_t b[32] = "frame1.txt";
	file_open(a);
	file_open(b);
	
	
	uint8_t d[6000];
	

	
	//read verylargetextwithverylongname.txt

	uint8_t c1[33] = "verylargetextwithverylongname.txt";
	file_open(c1);
	file_read(1,d,5999);
	printf("%s\n",d);	
	
	
	
	
	
	//read frame0.txt

	// uint8_t c2[33] = "frame0.txt";
	// file_open(c2);
	// file_read(1,d,300);
	// printf("%s\n",d);
	
	
	 

	
	//read excutable grep

	// uint8_t c3[33] = "grep";
	// file_open(c3);
	// file_read(1,d,300);
	// printf("%s\n",d);
	
	



	
	//read verylargetextwithverylongname.txt two times continously
	// uint8_t e[6000];

	// uint8_t c1[33] = "verylargetextwithverylongname.txt";
	// file_open(c1);
	
	// //first read 100 bytes
	// file_read(1,d,100);
	// printf("%s\n",d);

	// printf("\n");	
	
	// //second read another 100 bytes
	// file_read(1,e,100);
	// printf("%s", e);
	
	



}


/* dir read test
 * 
 * Inputs: None
 * Outputs: none
 * Side Effects: print out all the file name in the
 * directory which is same as the ls , each time the 
 * dir_read will only read one object 
 * Coverage: filesystem
 * Files: filesystem.c/h
 * 
 */
void dir_read_test(){

	/*directory read test*/
	
	uint8_t c[32] = "dummy.txt";
	int a;
	for (a = 0 ; a < 17 ; a ++){
		dir_read(1,c,a);
	}

	
	

}


/* rtc_read_test
 * Description: To test the RTC_read function; RTC_write is used to change the frequency
 * Inputs: None
 * Outputs: none
 * Side Effects: print out many 1 and 2 on the screen if printf("1") and printf("2")
 * 				in RTC_read and RTC_handler are uncommented; RTC frequency is changed
 * 
 */
void rtc_read_test() {
	/* use different frequencies to test read function
		so we can see "12" printed on the screen at different frequency */
	int rtc_i;
	int vir_test = 0;
	void * vir_ptr = & vir_test;
	for (rtc_i = 0; rtc_i < 100; rtc_i++) {		/* 100 here is a random number so this test won't run forever*/
		if ((rtc_i / 10) == 0) {			/* for the first 10 loops, set frequency to 2 hz*/
			vir_test = 2;	
		} else if ((rtc_i / 10) == 9) {		/* for the last 10 loops, set frequency to 8 hz*/
			vir_test = 8;
		} else {					/* for the middle 80 iterations, set frequency to 256 hz*/
			vir_test = 256;
		}
		RTC_write(0, vir_ptr, 0);
		RTC_read(0, vir_ptr, 0);
		printf("1");
	}
}

/* rtc_open_test
 * Description: To test the RTC_open function
 * Inputs: None
 * Outputs: None
 * Side Effects: since this calls RTC_open, RTC_open function will change the RTC frequency to 2hz
 * 
 */
void rtc_open_test() {
	uint8_t rtc_open_file = 0;
	uint8_t* rtc_open_ptr = &rtc_open_file;		/* create dummy argument to be passed into the RTC_open*/
	RTC_open(rtc_open_ptr);
}

/* rtc_write_test
 * Description: To test the RTC_write function
 * Inputs: None
 * Outputs: None
 * Side Effects: since this calls RTC_write, RTC_write will change the RTC_frequency
 * 
 */
int rtc_write_test() {
	/* this is the frequency, must be the power of 2*/
	/* if frequency is invalid , the frequency will stay at 256 Hz*/
	int vir_test = 2;			
	void * vir_ptr = & vir_test;
	int rtc_write_ret;
	rtc_write_ret = RTC_write(0, vir_ptr, 0);
	return rtc_write_ret;
}
/* keyboard test
 * 
 * Inputs: None
 * Outputs: none
 * Side Effects: 
 */
void keyboard_test(){
	int8_t buf[128];
	//printf("read bytes: %d\n", terminal_read(0, buf, 128));
	while(1) {
		terminal_read(0, &buf[0], 128);
		terminal_write(0, &buf[0], 128);
	}
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(){
	/* uncomment each portion for testing */
	//clear the screen
	reset_screen();
	
	//file system test

	
	// pcb_t temp;
	// current_pcb_ptr = &temp;
	// int i;
	// for(i = 0 ; i<8;i++){
	// 	if (i>1)
	// 	{
	// 		current_pcb_ptr->fd_array[i].flags = 0;
	// 		printf("flag0\n");
	// 	}

	// 	else{
	// 		current_pcb_ptr->fd_array[i].flags = 1;
	// 		printf("flag1\n");
	// 	}
		
		 
	// }
	

	// uint8_t d[6000];
	// uint8_t e[6000];
	// printf("%d\n",open((uint8_t*)"frame0.txt"));
	// //printf("%d\n",write(2,d,1024));
	// printf("%d\n",read(2,e,1024));
	// printf("%s\n",e);
	// printf("%d\n",read(3,e,600));
	// close(2);
	// printf("%d\n",read(2,e,600));
	
	/*file open test*/
	//file_open_test();

	/*file read test*/
	//file_read_test();

	/*directory read test*/
	//dir_read_test();
	

	/*rtc driver test*/ 
	/* test rtc_write*/
	// if frequency is not power of 2 , print error
	// if test go to rtc.c uncomment RTC_handler function printf(1)
	// if (rtc_write_test() == -1) {
	//	printf("rtc_write got invalid input");
	// }

	/* to test read and write*/
	// if test go to rtc.c comment RTC_handler function printf(1)
	//rtc_read_test();		

	
	//rtc_open_test();		/* to test open*/



	/* terminal driver test*/
	//keyboard_test();
}
