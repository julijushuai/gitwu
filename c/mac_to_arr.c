#include<stdio.h>
#include<string.h>
#include<stdlib.h>
static int st_mac_2a2t(const char *str, char *bin)
{
	int	i;
	char	*ptr, *strtok_ctx;

	if ((bin == NULL) || (str == NULL))
	{
		return -1;
	}

	ptr = (char *) str; 
	for (i = 0; i < 6; ++i)
	{
		bin[i] = ptr ? strtoul(ptr, &strtok_ctx, 16) : 0;
		//printf("%s,%s,bin[%d]:%02x\n",ptr,strtok_ctx,i,bin[i]);
		if (ptr)
		{
			ptr = (*strtok_ctx) ? strtok_ctx + 1 : strtok_ctx;
		}
	}

	return 0;
}



/******************************************************************************\
Function   : st_mac_4ap2t
Description: 
Return     : 
Others     : 0012.34AB.CDEF -> Array[6] = {00, 12, 34, AB, CD, EF}
\***************************************************************************/
int st_mac_4ap2t(const char *str, char *bin)
{
	char	*ptr;
	int	tmp[6] = {0};

	if ((bin == NULL) || (str == NULL))
	{
		return -1;
	}

	ptr = (char *) str; 

	sscanf(ptr, "%02x%02x.%02x%02x.%02x%02x", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
	bin[0] = (ct_uint8) tmp[0];
	bin[1] = (ct_uint8) tmp[1];
	bin[2] = (ct_uint8) tmp[2];
	bin[3] = (ct_uint8) tmp[3];
	bin[4] = (ct_uint8) tmp[4];
	bin[5] = (ct_uint8) tmp[5];   

	return 0;
}
