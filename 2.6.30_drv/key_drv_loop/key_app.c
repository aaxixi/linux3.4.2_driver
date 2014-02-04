#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd;
	unsigned int val[4];
	int cnt=0;

	fd = open("/dev/key_drv_dev", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	while(1)
	{	
		memset(val,0,sizeof(val));
		read(fd, val, sizeof(val));
		//if( (!val[0]) || (!val[1]) || (!val[2]) || (!val[3]) )
		if (val[0] || val[1] || val[2] || val[3])
		{
			printf("Cnt:%04d  key-%d %d %d %d\n",cnt,val[0],val[1],val[2],val[3]);
			cnt++;
		}
		usleep(500000);
	}
	
	return 0;
}

