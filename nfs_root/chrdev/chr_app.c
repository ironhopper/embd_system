#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILE_PATH "/dev/chr_dev" // device file path

int main(){
	int device;
	char buf[128] = "buffer data";
	device = open(DEVICE_FILE_PATH, O_RDWR);
	if( device < 0){
		printf("device file open fail\n");
		return device;
	}
	else{
		printf("device file open success\n");
		write(device,buf,1);
		read(device,buf,1);
		close(device);
		return 0;
	}
}
