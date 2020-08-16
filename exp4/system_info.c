#include "exp4.h"

void create_sysInfo(GtkWidget* notebook) { //用于创建系统信息页

	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);
	cpu_use = gtk_frame_new(_("CPU使用情况"));
	gtk_container_set_border_width(GTK_CONTAINER(cpu_use), 5);
	gtk_widget_set_size_request(cpu_use, 550, 300);
	gtk_box_pack_start(GTK_BOX(hbox), cpu_use, TRUE, FALSE, 5);

	hbox = gtk_hbox_new(FALSE, 0);
	GtkWidget* label1 = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
	g_timeout_add(1000, (GtkFunction)get_cpuRatio, (gpointer)label1);
	gtk_widget_set_size_request(hbox, 550, 30);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

	//绘制CPU使用曲线
	cpu_draw = gtk_drawing_area_new();
	gtk_widget_set_size_request(cpu_draw, 0, 0);
	g_signal_connect(G_OBJECT(cpu_draw), "expose_event", G_CALLBACK(cpu_startDraw), NULL);
	gtk_container_add(GTK_CONTAINER(cpu_use), cpu_draw);

	//系统信息
	frame = gtk_frame_new(_("系统信息"));
	label4 = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(frame), label4);
	//gtk_widget_set_size_request(frame, 500, 200);
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
	get_cpuInfo();  //读取CPU信息
	g_timeout_add(1000, (GtkFunction)get_sysInfo, (gpointer)label4);;
	gtk_container_add(GTK_CONTAINER(frame), label4);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new(_("系统信息")));
}

gboolean cpu_startDraw(GtkWidget* widget, GdkEventExpose* event, gpointer data) {
	static int flag = 0;
	draw_cpuUse((gpointer)widget);
	if (flag == 0) {
		g_timeout_add(1000, (GtkFunction)draw_cpuUse, (gpointer)widget);
		flag = 1;
	}
	return TRUE;
}

//取得cpu利用率
gboolean get_cpuRatio(gpointer label){
	FILE* fp;
	char buffer[1024];
	size_t buf;
	static int flag = 0;  //用于区分第一次还是第二次调用函数
	static long old_idle, old_total; 
	float t1, t2;

	long user = 0, nice = 0, sys = 0, idle = 0, iowait = 0, irq = 0, softirq = 0;
	long total;
	//CPU指标：user，nice, system, idle, iowait, irq, softirq
	// 分别为用户态时间，低优先级用户态时间，内核态时间，空闲的处理器时间 ,I/O等待时间

	fp = fopen("/proc/stat", "r");
	/* 从fp中读取sizeof(buffer)块数据放入buffer，每次读一块 */
	buf = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);

	if (buf == 0)
		return 0;
	buffer[buf] == '\0';

	sscanf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &sys, &idle, &iowait,&irq,&softirq);

	if (flag == 0) {
		flag ++;
		old_idle = idle;
		old_total = user + nice + sys + idle + iowait + irq + softirq;
	}
	else {
		total = user + nice + sys + idle + iowait + irq + softirq;
		t1 = total - old_total;
		t2 = idle - old_idle;
		cpu_ratio = 100 * (t1 - t2) / (t1);
		total = old_total;
		idle = old_idle;
	}
	sprintf(buffer, "CPU使用率:  %0.1f%%\0", cpu_ratio);
	//sprintf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld\n %f %f %ld", user, nice, sys, idle, iowait, irq, softirq,t1,t2,total);
	gtk_label_set_text(GTK_LABEL(label), _(buffer));
	return TRUE;
}

//保存CPU有关信息
void get_cpuInfo(){
	int fd = open("/proc/cpuinfo", O_RDONLY);
	char temp[2048];
	int offset = 0,i = 0;
	char* p;
	char name[5][20] = {"model name","cache size","cpu MHz" ,"cpu cores" ,"address sizes"};

	read(fd, temp, sizeof(temp));
	close(fd);
	
	/*for (int i = 0; i < 5; i++) {
		p = strstr(temp,name[i]);  // 取得所需要信息所在行首尾指针 
		offset = 0;
		while (p[offset++] != ':');  //跳过标题
		p[offset] = '\0';
		p += (offset + 1);
		offset = 0;
		while (p[offset++] != '\n');
		p[offset - 1] = '\0';
		strcpy(cpu_info[i], p);  //保存对应CPU信息
	}*/

	p = strstr(temp, "model name");  //读取CPU型号
	while ((*p++) != ':');
	p += 1;
	while ((*p) != '\n') {
		cpu_info[0][i] = *p;
		i++;
		p++;
	}
	cpu_info[0][i] = '\0';

	i = 0;  
	p = strstr(temp, "cache size");  //读取CPU缓存大小
	while ((*p++) != ':');
	p += 1;
	while ((*p) != '\n') {
		cpu_info[1][i] = *p;
		i++;
		p++;
	}
	cpu_info[1][i] = '\0';

	i = 0;
	p = strstr(temp, "cpu MHz");
	while ((*p++) != ':');
	p += 1;
	while ((*p) != '\n') {
		cpu_info[2][i] = *p;
		i++;
		p++;
	}
	cpu_info[2][i] = '\0';

	i = 0;
	p = strstr(temp, "cpu cores");
	while ((*p++) != ':');
	p += 1;
	while ((*p) != '\n') {
		cpu_info[3][i] = *p;
		i++;
		p++;
	}
	cpu_info[3][i] = '\0';

	i = 0;
	p = strstr(temp, "address sizes");
	while ((*p++) != ':');
	p += 1;
	while ((*p) != '\n') {
		cpu_info[4][i] = *p;
		i++;
		p++;
	}
	cpu_info[4][i] = '\0';

}

//获取系统信息
gboolean get_sysInfo(gpointer label){
	int fd;
	FILE* fp;
	char buf[1024], hostname[128], osname[128], kernel_version[128];
	char memtotal[255], memfree[255];
	char* pos = NULL;
	int os_type;
	int setup_time,uphour, upminute, upsecond;

	fp = fopen("/proc/sys/kernel/hostname", "r");  //保存主机名
	fgets(hostname, sizeof(hostname), fp);
	fclose(fp);

	memset(buf, 0, sizeof(buf));  //保存操作系统名
	fd = open("/etc/issue", O_RDONLY);
	read(fd, buf, sizeof(buf));
	close(fd);
	int i = 0;
	while (buf[i++] != '\\');
	buf[i - 1] = '\0';
	strcpy(osname, buf);

	os_type = sizeof(char*) * 8;  //保存操作系统位数

	fp = fopen("/proc/sys/kernel/osrelease", "r");
	fgets(kernel_version, sizeof(kernel_version), fp);  //保存内核版本
	fclose(fp);

	memset(buf, 0, sizeof(buf));
	fd = open("/proc/uptime", O_RDONLY);  //读取系统运行时间
	read(fd, buf, sizeof(buf));
	close(fd);
	while (buf[i++] != ' ');
	buf[i-1] = '\0';
	setup_time = atoi(buf);
	upsecond = setup_time % 60;
	upminute = (setup_time / 60) % 60;
	uphour = setup_time / 3600;

	//读取内存信息
	memset(buf, 0, sizeof(buf));
	fd = open("/proc/meminfo", O_RDONLY);
	read(fd, buf, sizeof(buf));
	close(fd);

	i = 0;
	pos = strstr(buf, "MemAvailable");  //读取总内存
	while ((*pos++) != ':');
	while ((*pos++) == ' ');
	pos--;
	while (*pos != ' '){
		memfree[i] = *pos;
		i++;
		pos++;
	}
	memfree[i] = '\0';
	
	i = 0;
	pos = strstr(buf, "MemTotal");  //读取总内存
	while ((*pos++) != ':');
	while ((*pos++) == ' ');
	pos--;
	while (*pos != ' ') {
		memtotal[i] = *pos;
		i++;
		pos++;
	}
	memtotal[i] = '\0';
	float mem_total = atof(memtotal) / (1024 * 1024);
	float mem_free = atof(memfree) / (1024 * 1024);

	//保存当前时间
	struct tm* gmt, * local;
	time_t tt;
	tzset();//void tzset(void);设置时间环境变量-时区
	tt = time(NULL);//等价于time(&tt);
	char* str = ctime(&tt);
	local = localtime(&tt);
	sprintf(str,"%4d年%02d月%02d日 %2d:%2d:%2d\n", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	sprintf(buf, "主机名:   %s操作系统版本:  %s %d-bit\n内存(可用):  %0.2f / %0.2f GB\n内核版本:   %s当前系统时间: %s\
系统运行时间:  %02d:%02d:%02d\nCPU型号:  %s\nCPU主频:	 %s MHz\nCPU二级缓存大小:  %s\n\
CPU核心数:  %s\nCPU线程数:	%s\n",
hostname, osname, os_type, mem_total - mem_free,mem_total,kernel_version, str,uphour, upminute, upsecond, cpu_info[0], cpu_info[2], cpu_info[1], cpu_info[3], cpu_info[4]);


	//sprintf(buf, "%s  | %s ", memtotal, memfree);
	gtk_label_set_text(GTK_LABEL(label), _(buf));
	return TRUE;
}

gboolean draw_cpuUse(gpointer widget) {
	GtkWidget* cpu_curve = (GtkWidget*)widget;
	GdkColor color;
	GdkGC* gc = cpu_curve->style->fg_gc[GTK_WIDGET_STATE(widget)];
	static int flag = 0;
	static int now_pos = 0;
	int draw_pos = 0;

	color.red = 0;
	color.green = 0;
	color.blue = 0;
	gdk_gc_set_rgb_fg_color(gc, &color);  //设置背景颜色
	gdk_draw_rectangle(cpu_curve->window, gc, TRUE, 15, 30, 480, 200);

	color.red = 0;
	color.green = 20000;
	color.blue = 0;
	gdk_gc_set_rgb_fg_color(gc, &color);  //设置背景线条
	for (int i = 30; i <= 220; i += 20)
		gdk_draw_line(cpu_curve->window, gc, 15, i, 495, i);
	for (int i = 15; i <= 480; i += 20)
		gdk_draw_line(cpu_curve->window, gc, i + cpu_curve_start, 30, i + cpu_curve_start, 230);

	cpu_curve_start -= 4;  //设置CPU折线位置
	if (cpu_curve_start == 0)
		cpu_curve_start = 20;

	if (flag == 0) {
		for (int i = 0; i < 120; i++) {
			cpu_ratio_data[i] = 0;
			flag = 1;
		}
	}

	cpu_ratio_data[now_pos] = cpu_ratio / 100;
	now_pos++;
	if (now_pos == 120)
		now_pos = 0;

	color.red = 0;  //开始画线
	color.green = 65535;
	color.blue = 0;
	gdk_gc_set_rgb_fg_color(gc, &color);
	draw_pos = now_pos;
	for (int i = 0; i < 119; i++) {
		gdk_draw_line(cpu_curve->window, gc,
			15 + i * 4, 230 - 200 * cpu_ratio_data[draw_pos % 120],
			15 + (i + 1) * 4, 230 - 200 * cpu_ratio_data[(draw_pos + 1) % 120]);
		draw_pos++;
		if (draw_pos == 120)
			draw_pos = 0;
	}

	color.red = 25000;
	color.green = 25000;
	color.blue = 25000;  //重新设置颜色
	gdk_gc_set_rgb_fg_color(gc, &color);

	return TRUE;
}