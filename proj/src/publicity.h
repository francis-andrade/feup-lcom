#ifndef __VBE_H
#define __VBE_H

/** @defgroup publicity publicity
 * @{
 * Functions for publicity
 */

/**
 * @brief shows an ad. If today is a day of the week shows a certain ad, if it's weekend shows another one
 * @param path_images path of the images directory
 * @return 1 in case of success, 0 otherwise
 */
int show_ad(char * path_images);

/**
 * @brief Determines if date is a weekend or not
 * @param date
 * @return 1 if a date is weekend, 0 otherwise
 */
int Isweekend(date_t date);

/**
 * @brief Determines is year is leap or not
 * @param year
 * @return 1 if a year is leap, 0 otherwise
 */
int Isleap(int year);
/** @}* end of publicity */
#endif
