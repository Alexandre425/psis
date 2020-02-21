#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(){
	char v1[100];
	char *v2;
	int i;

	puts("Write a word");
	fgets(v1, 100, stdin);

	v2 = malloc(sizeof(char)*strlen(v1)+1);
	// +1 ensures the terminating character is copied
	for (i = 0; i < strlen(v1) + 1; i++){
		v2[i] = toupper(v1[i]);
	}

	printf("Converted string: %s", v2);
	free(v2);

}