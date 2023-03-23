#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct
{
	int b;
}test_t;
int main() {
	char c[34] = {0};	
	test_t *b;
	char *a = NULL;
	b = (test_t *)malloc(sizeof(test_t));
//	memset(a , 0 , sizeof(a));
	//	strncpy(a, "nihao", 5);
	printf("%5s %d\n","nih", 10);
	printf("%-5s %d\n","nih", 10);
	printf("Hello, World!\n");
	//	printf("%s", a);

//		memset(b ,0 , sizeof(b));
	b->b = 6;
//	memcpy(&b->a, "ss", 2);
		printf("b = %d\n", b->b);
	//	printf("%s\n", b->a);
	return 0;
}
