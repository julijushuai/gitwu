#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
	char a[6];
	int i = 0;
	memset(a, 0, sizeof(a));
	for (i = 250; i < 260; i++)
	{
		a[5] = i;
		printf("0x%x\n", a[5]);
		printf("%d\n", a[5]);
	}

}
