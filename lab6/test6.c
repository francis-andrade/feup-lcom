#include "rtc.h"
int rtc_test_conf(void) {
	/* To be completed */
}

int rtc_test_date(void) {
	date_t date;
	rtc_read_date(& date);
	printf("%d/%d/%d",date.day,date.month,date.year);
	return 0;
}

int rtc_test_int(/* to be defined in class */) { 
	/* To be completed */
}

