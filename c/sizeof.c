#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
	int a[] = {1,2,3,4,5,6,7,8,9};
	int b = (sizeof(a)/sizeof(a[0]));
	int c = 23333333;
	printf("0x%-1x\n", c);
	printf("b=%d\n", b);
}
