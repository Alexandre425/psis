#include <stdio.h>
#include <stdlib.h>


int main(){
	int v[100];
	int i;

	for (i=0; i<100; ++i){
		v[i] = random();
	}

	printf("vector initialized\n");

}