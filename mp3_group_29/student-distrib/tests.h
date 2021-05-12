#ifndef TESTS_H
#define TESTS_H

// test launcher
void launch_tests();

/* divide by zero exception test */
void divide_exception();

/* double fault exception test and paging */
void double_fault_test();

/* overflow exception test */
void overflow_test();

/* file open test*/
void file_open_test();

/*file read test*/
void file_read_test();

/*directory read test*/
void dir_read_test();

/* RTC_read test*/
void rtc_read_test();

/* RTC_open test*/
void rtc_open_test();

/* RTC_write test*/
int rtc_write_test();

/*keyboard test*/
void keyboard_test();

#endif /* TESTS_H */
