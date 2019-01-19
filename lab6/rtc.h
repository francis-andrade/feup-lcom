#ifndef __RTC_H
#define __RTC_H

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_IRQ 8

typedef struct{
	int year;
	int month;
	int day;
}__attribute__((packed)) date_t;

/**

 * @brief subscribes a RTC interruption

  * @param rtc_hook_id rtc hook

 * @return Return 0 upon success and non-zero otherwise

 */

int rtc_subscribe_int(unsigned int * rtc_hook_id);

/**

 * @brief unsubscribes a RTC interruption

 * @param rtc_hook_id rtc hook

 * @return Return 0 upon success and non-zero otherwise

 */

int rtc_unsubscribe_int(unsigned int * rtc_hook_id);
/**

 * @brief reads from the rtc

 * @param reg register that is going to be read

 * @param inf variable where the information read is going to be stored

 * @return Return 0 upon success and 1 otherwise

 */
int rtc_read(unsigned int reg, unsigned int * inf);
/**

 * @brief gets the date from the rtc

 * @param date variable where the date is going to be stored

 * @return Return 0 upon success and 1 otherwise

 */
int rtc_read_date(date_t * date);
/**

 * @brief converts a number from hexadecimal to decimal

 * @param x number that is going to be converted

 * @return number converted

 */
int hex_to_dec(int x);

#endif
