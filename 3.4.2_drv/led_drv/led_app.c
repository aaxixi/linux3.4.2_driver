
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* firstdrvtest on
  * firstdrvtest off
  */
int main(int argc, char **argv)
{
	int fd;
	int val = 7;
	fd = open("/dev/led_drv_dev", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	write(fd, &val, 4);
	return 0;
}
