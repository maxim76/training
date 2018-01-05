/*
	Tests gethostname() function
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
	size_t len = 1024;
	char buf[len];

	gethostname(buf, len);
	printf( "Hostname: %s\n", buf );
}
