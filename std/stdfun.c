

#include "typedef.h"
#include "error.h"

void strncpy(char *dest, char *src, int len)
{
	int i = 0;

	for(i = 0; i < len; i++)
	{
		dest[i] = src[i];	
	}

	dest[i] = 0;
}

int strcmp(char *s1, char *s2)
{
	char tmp = 0;
	
	while((*s1 != 0) && (*s2 != 0))
	{
		if(0 != (tmp = (*s1 - *s2)))
		{
			return tmp;
		}
		
		s1++;s2++;
	}

	return (*s1 - *s2);
}

int strlen(char *s)
{
	int i = 0;

	while(*s != 0)
	{
		i++;
		s++;
	}

	return i;
}

void strcpy(char *dest, char *src)
{
	while(*src != 0)
	{
		*dest = *src;
		dest++;
		src++;
	}
}

void memset(void *dest, char c, unsigned int num)
{
	int i = 0;
	char *p = (char *)dest;

	for(i = 0; i < num; i++)
	{
		p[i] = c;
	}
}



