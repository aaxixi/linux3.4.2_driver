#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <poll.h>
#include <signal.h>
#include <unistd.h>

#include <sys/time.h>

int fd;
unsigned int rd_cnt=0;
void signal_handle()
{
	unsigned long val;
	rd_cnt++;
	read(fd,&val,sizeof(unsigned long));
	if( (rd_cnt>47000))
	//if(0)
	{
		//printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		//printf("rd:%d-IRQ:%06d\n",rd_cnt,val);
		printf("%d\n",rd_cnt);
	}
}

int main(int argc , char **argv)
{

	int ret;
    unsigned long val;
	 int Oflags;
	 struct timeval start, end;

    int interval;
    fd=open("/dev/irqTest" , O_RDWR);
    if(fd<0)
	{
		printf("can't open /dev/irqTest\n");
		return -1;
	}
	
	 signal(SIGIO, signal_handle);
	 //设置将接收SIGIO和SIGURG信号的进程id,这里就是本进程
	 fcntl(fd, F_SETOWN, getpid());
			//取得fd的文件状态标志
	 Oflags = fcntl(fd, F_GETFL); 
			//为设备文件设置同步属性，即FASYNC，此时就会调用.fasync函数
	 fcntl(fd, F_SETFL, Oflags | FASYNC);
	
	printf("start %s!\n",__TIME__);
	
	ret = read(fd,&val,sizeof(unsigned long));
	rd_cnt = 0;
	//if(ret)
	{
		//start = time(NULL);
		gettimeofday(&start, NULL);
		for(ret=0;ret<100;ret++)
		{
			//read(fd,&val,sizeof(unsigned long));
			printf("%d-IRQ_CNT:%ld\n",ret,val);
		}
		//end = time(NULL);
		 gettimeofday(&end, NULL);
		 interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
		printf("val = %f\n", interval/1000.0);
	}
	printf("---\n");
	   
    while(1)
	{
	   //read(fd,&val,sizeof(unsigned long));
	   //printf("rd:cnt:%d--val:%ld",rd_cnt,val);
	   sleep(1);
	}
    return 0;
}