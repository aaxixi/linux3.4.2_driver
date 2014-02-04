#if 1
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
	int val;
	
	if(argc!=2)
	  return 0;
	
	sscanf(argv[1],"%d",&val);
	fd = open("/dev/led_drv_dev", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	write(fd, &val, sizeof(int));
	return 0;
}
#else


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char **argv)
{
	int on;
	int led_no;
	int fd;
	if (argc != 3 || sscanf(argv[1], "%d", &led_no) != 1 || sscanf(argv[2],"%d", &on) != 1 ||
	    on < 0 || on > 1 || led_no < 1 || led_no > 4) {
		fprintf(stderr, "Usage: leds led_no 0|1\n");
		exit(1);
	}
	fd = open("/dev/led_drv_dev", 0);
	if (fd < 0) {
		perror("open device leds");
		exit(1);
	}
	ioctl(fd, on, (led_no-1));
	close(fd);
	return 0;
}




#endif
