#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
	char name[32];
	memset(name, 0, sizeof(name));
	char name1[5];
	memset(name1, 0, sizeof(name1));
	int i = 0;

	printf("%s\n", argv[1]);
	printf("%s\n", argv[2]);
//	memcpy(name, argv[1], 32);
	strncpy(name1, argv[2], 5);

//	printf("name:");
//	for (i = 0; i < 32; i++)
//	{
//		printf("%c",name[i]);
//	}

//	printf("\nname %s\n", name);
	printf("name1:");
	for (i = 0; i < 7; i++)
	{
		printf("%c\n",name1[i]);
	
	}
	printf("\nname1:%s\n", name1);
	printf("\n");



}
