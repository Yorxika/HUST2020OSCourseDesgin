#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc,char ** argv) {
	if (argc != 3) {  //输入为3个参数
		fprintf(stderr, "Usage:%s fromfile tofile \n\a", argv[0]);
		return -1;
	}
	//printf("Copy:%s	->		%s\n", argv[1], argv[2]);
	int rt = syscall(336);
	//
	printf("Return code is: %d.\n", rt);
	int rtn = syscall(335, argv[1], argv[2]);
printf("Return code is: %d.\n", rt);
	return 0;
}
