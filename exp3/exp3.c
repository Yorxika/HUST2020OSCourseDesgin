#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char** argv) {
	int fd;
	char buf[1024];
	int start, len;
	//fd = open("/dev/chardev0", O_RDWR, S_IRUSR | S_IWUSR);
	fd = open("/dev/mydev", O_RDWR);
	if (fd == -1) {
		printf("Open device error!\n");
		return -1;
	}
	if (argc == 4 && strncmp(argv[1], "read", 4) == 0) { 
		//static ssize_t my_read(struct file* filp, char* buf, size_t len, loff_t* off) 
		start = atoi(argv[2]);
		len = atoi(argv[3]);
		lseek(fd, start, SEEK_SET);
		read(fd, buf, len);
		printf("Character device read : %s.\n", buf);
	}
	else if (argc == 4 && strncmp(argv[1], "write", 5) == 0) {
		start = atoi(argv[2]);
		lseek(fd, start, SEEK_CUR);
		write(fd, argv[3], strlen(argv[3]));
	}
	else {
		printf("Usage : ./executable file <read | write> <start_offset> <len | string>\n");
	}
	close(fd);
	return 0;
}