#ifndef __UTILITIES_H
#define __UTILITIES_H


//macros

#define BIT(n) (0x01<<(n))
//generic functions
/**

 * @brief returns the absolute value of a number

 *

 *

 * @param n  number whose absolute value we want to know

 *

 * @return Absolute value of n

 */
int abs(int n);
/**

 * @brief returns sign of a number

 *

 *

 * @param n  number whose sign we want to know

 *

 * @return sign of n

 */
int sgn(int n);
/**

 * @brief checks if two number have the same sign

 *

 *

 * @param x, y  numbers whose signs we want to compare

 *

 * @return Returns 1 if both numbers have the same sign, 0 otherwise

 */

int samesign(int x, int y);
/**

 * @brief checks if all the elements of an array have the same sign

 *

 *

 * @param size size of an array
 *

 * @return Returns 1 if all numbers have the same sign, 0 otherwise

 */

int samesignarray(int size, int * arr);
/**

 * @brief returns the opposite number in two complement to c

 *

 *

 * @param c value whose symetric in two complement we want to know

 *

 * @return the symetric in two_complement for c

 */

char two_complement_sym(char c);
/**

 * @brief converts a char to decimal

 *

 *

 * @param c  char to be converted to decimal

 *

 * @return Returns the conversion of c to decimal

 */


int conv_to_decimal(char c);



#endif
