#include <stdlib.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
	const char* pHex = "F80";
	int num = 0;
	short int num2 =0;
	sscanf(pHex,"%x",&num) ;
	sscanf(pHex,"%hx",&num2) ;
	printf("num=%d\n",num);
	printf("num2=%d\n",num2);
	printf("num2=0x%x\n",num2);
	printf("num2=0x%x\n",num);
//	return 1;
	exit(0);
}

