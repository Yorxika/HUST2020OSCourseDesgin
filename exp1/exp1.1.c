#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MAXSIZE 1024 //缓冲区大小

int main(int argc, char** argv) {
	int from_fd, to_fd;
	int bytes_read;
	char buf[MAXSIZE];

	if (argc != 3) {  //输入为3个参数
		fprintf(stderr, "Usage:%s fromfile tofile \n\a", argv[0]);
		return -1;
	}
	if ((from_fd = open(argv[1], O_RDONLY)) == -1) {  //打开源文件
		fprintf(stderr, "Open %s Error:%s\n", argv[1], strerror(errno));
		return -1;
	}
	if ((to_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {  //创建目的文件
		fprintf(stderr, "Open %s Error:%s\n", argv[2], strerror(errno));
		return -1;
	}
	while (1) {
		bytes_read = read(from_fd, buf, MAXSIZE);
		if (bytes_read == -1) {
			fprintf(stderr, "Copy error: %s \n", strerror(errno));
			return -1;
		}
		else if (bytes_read == 0)
			break;
		write(to_fd, buf, bytes_read);
	}
	close(from_fd);
	close(to_fd);
	return 0;
}