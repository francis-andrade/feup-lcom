#include "utilities.h"

//generic functions


void stringcpy(char * dest, char * src){
	printf("now: %s\n fim: ",src);
	int i=0;//counter
	do{
		dest[i]=src[i];
		//printf("%d: %d\n",i, src[i]);
		i++;
	}while(src[i-1]!=0 && i<50);
}

void stringcat(char * dest, char * src){
	int i=0;//counter
	do{
		i++;
	}while(dest[i]!=0);

	int j=0;
	do{
		dest[i+j]=src[j];
		j++;
	}while(src[j-1]!=0);
}

int hex_to_dec(int x){
	int r=0;//value to return
	int mult=1;//power of 10 to be multiplied by the corresponding decimal place in x
	while(x>0){
		r=r+(x%16)*mult;
		x=x/16;
		mult=mult*10;
	}
	return r;
}

int floor2(double x){
	unsigned int i=0;//counter;
	while(i<=x){
		i++;
	}
	return i-1;
}

int round(double x){
	if((x-floor2(x))<0.5){
		return floor2(x);
	}
	else{
		return floor2(x)+1;
	}
}

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
