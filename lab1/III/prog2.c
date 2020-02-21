#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

// The great guide on how to compile this mess

// Start by generating Position Independent Code objects from the .c files
// gcc -fpic -c lib1.c lib2.c

// Then create shared libraries from the object files
// -shared 	: makes the libraries shared
// -ldl 	: links the libdl library
// gcc -fpic -shared -ldl -o lib1.so lib1.o
// gcc -fpic -shared -ldl -o lib2.so lib2.o

// Compiling a program that makes use of these libraries requires you link them
// -L		: locates the libraries, in this case it's the current directory
// -l1 		: links "lib1.so", notice the "lib" prefix and ".so" suffix are added automatically
// -l2 		: links "lib2.so"
// gcc prog2.c -L. -l1 -l2 -ldl -o prog2

int main(){
	int a;
	char line[100];
	char library_name[100];
	char *error = NULL;

	printf("What version of the functions you whant to use?\n");
	printf("\t1 - Normal    (lib1)\n");
	printf("\t2 - Optimized (lib2)\n");
	fgets(line, 100, stdin);
	sscanf(line,"%d", &a);
	if (a == 1){
		strcpy(library_name, "./lib1.so");
		printf("running the normal version from %s \n", library_name);
	}else{
		if(a== 2){
			strcpy(library_name, "./lib2.so");
			printf("running the optimized version from %s \n", library_name);
		}else{
			printf("Not running anything\n");
			exit(EXIT_FAILURE);
		}
	}

	// Opens the library, resolves symbols as the references occur
	void* handle = dlopen(library_name, RTLD_LAZY);
	if (!handle)
	{
		// Print the error that prevented the library from opening
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	// Links the functions from the selected library
	void (*func_1)(void) = dlsym(handle, "func_1");
	if (error = dlerror())
	{
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}
	void (*func_2)(void) = dlsym(handle, "func_2");
	if (error = dlerror())
	{
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	func_1();
	func_2();

	return 0;
}
