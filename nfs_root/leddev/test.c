#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc , char **argv)
{
	int data = 0;
	int fd;
	if (argc < 2 )
	{
		printf("command format: ./a.out 3 \n");
		return -1;
	}

	data = atoi(argv[1]);
	fd = open("/dev/periled", O_WRONLY);
	if ( fd < 0 )
	{
		printf("file open error.\n");
		return fd;
	}
	write(fd, &data, 4);
	close(fd);
	return 0;
}
