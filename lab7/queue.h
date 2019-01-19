#ifndef __QUEUE_H
#define __QUEUE_H
/*
 * @brief This struct is used to represent a queue
 */

typedef struct{
	char * buf;//pointer to the array that stores the elements in the queue
	int out;//indice of the last element of the array
} queue_t;

/*
 * @brief return a new queue with an array of size 200
 * @return pointer to new queue
 */
queue_t * new_queue();

/*
 * @brief frees the memoru allocated by q
 * @param q queue whose memory we want to deallocate
 *
 */
void delete_queue(queue_t * q);
/*
 * @brief adds a new element to the queue
 * @param q queue where a new element is going to be added
 * @param c char that is going to be added to the queue
 * @return 0 in case of success 1 otherwise
 */
int push(queue_t * q, char c);
/*
 * @brief returns the top of the queue
 * @param q queue whose top element we want to access
 * @return top element
 */
char top(queue_t * q);
/*
 * @brief pops the first element of the queue
 * @param q queue whose first element we want to pop
 */
void pop(queue_t * q);
/*
 * @brief tells if a queue is empty
 * @param q queue
 * @return 1 if the queue is empty, 0 otherwise
 */
int empty(queue_t * q);
#endif
