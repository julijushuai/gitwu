#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEV "/dev/" 
 int main() {
	int length = 0; 
	char *firstName = "Hello";
	char *file  = (char *) malloc(20);
	char *lastName = "World";
	char *name = (char *) malloc(strlen(firstName) + strlen(lastName));
	int len = strlen(DEV);	
	length = strlen(DEV) + strlen(lastName);
	sprintf(name, "%s%s", firstName, lastName);
	snprintf(file, length+1, "%s%s", DEV, lastName);
	
	printf("%s\n", name);
	printf("%s\n", file);
	printf("%d\n", length);
	
	printf("%x\n", *name);
	printf("%p\n", &name);
	free(name);
	printf("%x\n", *name);
	printf("%p\n", &name);

	name =NULL;
	return 0;
	}

