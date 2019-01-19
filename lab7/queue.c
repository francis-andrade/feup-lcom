#include "queue.h"
#include <stdlib.h>
queue_t * new_queue(){
	printf("queue");
	char * buff=malloc(100*sizeof(char));
	queue_t * queue=malloc(sizeof(queue_t));
	queue->out=-1;
	queue->buf=buff;
	printf("queue");
	return queue;
}

void delete_queue(queue_t * q){
	free(q->buf);
	free(q);
}

int push(queue_t * q, char c){
	if(q->out>=200){
		return 1;
	}
	q->buf[q->out+1]=c;
	q->out=q->out+1;
	return 0;
}

char top(queue_t * q){
	return q->buf[0];
}

void pop(queue_t * q){
	if(q->out==-1){
		return;
	}
	else{
		int i;//counter
		for(i=0;i<q->out;i++){
			q->buf[i]=q->buf[i+1];
		}
		q->out=q->out-1;
	}
}

int empty(queue_t * q){
	if(q->out==-1){
		return 1;
	}
	else{
		return 0;
	}
}
