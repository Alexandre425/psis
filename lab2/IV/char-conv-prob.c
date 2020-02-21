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

	// +1 to fit the string terminator character
	// calloc takes care of the terminator (0) by zeroing out the memory.
	v2 = calloc(strlen(v1) + 1, sizeof(char));
	for (i=0; v1[i] != 0; i++)
	{
		v2[i] = toupper(v1[i]);	
	}

	printf("Converted string: %s", v2);

	free(v2);
}