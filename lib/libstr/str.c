#include <stdio.h>
#include <unistd.h>

#include "str.h"

int
str_trim(char *buf, int len)
{
	while ((len) != 0 &&
	    (buf[len - 1] == '\r' || buf[len - 1] == '\n'))
		len--;

	return len;
}
