#include "utilities.h"

//generic functions
int abs(int n){
	if(n>0){
		return n;
	}
	else{
		return -n;
	}
}

int sgn(int n){
	if(n>0){
		return 1;
	}
	else if(n==0){
		return 0;
	}
	else{
		return -1;
	}
}

int samesign(int x, int y){
	if(abs(sgn(x)-sgn(y))==2){
		return 0;
	}
	else{
		return 1;
	}
}

int samesignarray(int size, int * arr){
	unsigned int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			if(samesign(arr[i],arr[j])==0){
				return 0;
			}
		}
	}
	return 1;
}

char two_complement_sym(char c){
	char r=0;//value to return
	unsigned int i;//counter
	char first_one=0;//it will be set to 1 when the first one appears
	for(i=0;i<8;i++){
		if(first_one==1){
			if((c & BIT(i))==0){
				r=(r | BIT(i));
			}
		}
		else if(((c & BIT(i))==BIT(i)) && (first_one==0)){
					r=BIT(i);
					first_one=1;
		}
	}
	return r;
}

int conv_to_decimal(char c){
	unsigned int i=0;//counter
	int p=1;//power of two used to obtain the final value
	int sum=0;//value to return
	for(i=0;i<8;i++){
		sum=sum+((c & BIT(i))>>i)*p;
		p=2*p;
	}
	return sum;
}
