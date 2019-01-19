#include <errno.h>
#include <minix/drivers.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Bitmap.h"
#include "utilities.h"
#include "rtc.h"
#include "video_gr.h"
#include "vbe.h"
#include "kbc.h"
#include "i8042.h"

static Bitmap * ad_week;
static Bitmap * ad_weekend;

int show_ad(char * path_images){
	char * path=malloc(sizeof(char)*256);
	strcpy(path, path_images);
	strcat(path, "ad_week.bmp");
	ad_week=loadBitmap(path);
	strcpy(path, path_images);
	strcat(path, "ad_weekend.bmp");
	ad_weekend=loadBitmap(path);
	date_t date;
	rtc_read_date(& date);
	if(vg_init(VBE_117_MODE)==NULL){
		vg_exit();
		printf("It wasn't possible to initiate the 117 Mode\n");
		return 1;
	}
	if(Isweekend(date)){
		drawBitmap(ad_weekend, 0, 0, ALIGN_LEFT);
	}
	else{
		drawBitmap(ad_week, 0, 0, ALIGN_LEFT);
	}
	swap_double_video();
	int ipc_status; //ipc_status
		message msg; //message
		int r; //variable that stores the error
		int kbd_hook_id = KBC_IRQ; //kbd_hook
		long irq_set_kbd = BIT(kbd_hook_id);

		if (kbd_subscribe_int(&kbd_hook_id) == -1) {
			return -1;
		}

		unsigned long scancode = 0; //scancode of the key that the user is going to press
		int counter=0;//counts the time
		while (scancode != 0x81 ) {
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) {
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					if (msg.NOTIFY_ARG & irq_set_kbd) {
							kbc_c_to_ass(& scancode);
					}
					break;
				default:
					break;
				}
			}
		}

		vg_exit();
		kbd_unsubscribe_int(&kbd_hook_id);

	return 0;
}


int Isweekend(date_t date){
	int months[13]={0, 31,28+Isleap(date.year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int sum=0;
	int i;//counter
	for(i=2017;i<date.year; i++){
		sum=sum+365+Isleap(i);
	}
	for(i=1;i<date.month; i++){
		sum=sum+months[i];
	}
	sum=sum+date.day-1;
	if((sum+6)%7==5 || (sum+6)%7==6){
		return 1;
	}
	else{
		return 0;
	}
}

int Isleap(int year){
	if(year%400==0){
		return 1;
	}
	else if(year%100==0){
		return 0;
	}
	else if(year%4==0){
		return 1;
	}
}
