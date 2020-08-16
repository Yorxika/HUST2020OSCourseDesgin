asmlinkage long sys_mycp(char* src, char* dst) {
	int from_td, to_td;
	char buf[1024];
	long bytes_read;
	mm_segment_t fs = get_fs();  //设置内存地址访问范围
	set_fs(get_ds());

	from_td = ksys_open(src, O_RDONLY, S_IRUSR);
	if (from_td == -1) {  //打开源文件
		printk("Open source file erre!\n");
		set_fs(fs);
		ksys_close(from_td);
		return -1;
	}
	to_td = ksys_open(dst, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if (to_td == -1) {  //打开目的文件
		printk("Open destination file error!\n");
		set_fs(fs);
		ksys_close(from_td);
		ksys_close(to_td);
		return -1;
	}
	set_fs(KERNEL_DS);
	while (1) {
		bytes_read = ksys_read(from_td, buf, 1024);
		if (bytes_read == -1) {
			printk("Read Error!\n");
			set_fs(fs);
			ksys_close(from_td);
			ksys_close(to_td);
			return -1;
		}
		else if (bytes_read == 0)
			break;
		ksys_write(to_td, buf, bytes_read);
	}

	printk("Finish copy from %s to %s !", src, dst);
	set_fs(fs);
	ksys_close(from_td);
	ksys_close(to_td);
	return 0;
}