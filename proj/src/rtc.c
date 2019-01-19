#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "rtc.h"
#include "utilities.h"

#define BIT(n) (0x01<<(n))

int rtc_subscribe_int(unsigned int * rtc_hook_id) {

        int old_hook = *rtc_hook_id; //new variable that preserves the value of the original hook

        if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, rtc_hook_id) == OK) {

                if (sys_irqenable(rtc_hook_id) == OK) {


                        return BIT(old_hook);

                }

        }

        return -1;

}

int rtc_unsubscribe_int(unsigned int * rtc_hook_id) {

        if (sys_irqdisable(rtc_hook_id) == OK) {

                if (sys_irqrmpolicy(rtc_hook_id) == OK) {

                           return 0;

                }

        }

        return -1;

}


int rtc_read(unsigned int reg, unsigned int * inf){
	if(sys_outb(RTC_ADDR_REG, reg)!=OK){
		return 1;
	}
	if(sys_inb(RTC_DATA_REG, (long unsigned int *) inf)!=OK){
		return 1;
	}
	return 0;
}

int rtc_read_date(date_t * date){
	int year, month, day;//year, month and day date
	if(rtc_read(7, &day)!=0){
		return 1;
	}
	if(rtc_read(8, &month)!=0){
		return 1;
	}
	if(rtc_read(9, &year)!=0){
			return 1;
	}
	date->year=hex_to_dec(year)+2000;
	date->month=hex_to_dec(month);
	date->day=hex_to_dec(day);
	return 0;

}


