#include "exp4.h"

//创建第一页，用于显示进程信息
void create_progress(GtkWidget* notebook) {

	vbox = gtk_vbox_new(FALSE, 0); // 创建垂直布局
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);  //建一个新的滚动窗口(scrolled window)
	gtk_widget_set_size_request(scrolled_window, 550, 500);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gchar* name[5] = { _("进程名称"),"PID",_("进程状态"),_("占用内存"),_("进程优先级") };
	clist = gtk_clist_new(5);  //分栏列表用于显示进程具体信息
	gtk_clist_set_column_title(GTK_CLIST(clist), 0, name[1]);  //设置每一列的名字
	gtk_clist_set_column_title(GTK_CLIST(clist), 1, name[0]);
	gtk_clist_set_column_title(GTK_CLIST(clist), 2, name[2]);
	gtk_clist_set_column_title(GTK_CLIST(clist), 3, name[4]);
	gtk_clist_set_column_title(GTK_CLIST(clist), 4, name[3]);

	gtk_clist_set_column_width(GTK_CLIST(clist), 0, 75);  //设置每一列宽度
	gtk_clist_set_column_width(GTK_CLIST(clist), 1, 125);
	gtk_clist_set_column_width(GTK_CLIST(clist), 2, 75);
	gtk_clist_set_column_width(GTK_CLIST(clist), 3, 75);
	gtk_clist_set_column_width(GTK_CLIST(clist), 4, 125);
	gtk_clist_column_titles_show(GTK_CLIST(clist));

	get_progressInfo();

	//gtk_widget_add_events(clist, GDK_BUTTON_PRESS_MASK); //clist接收鼠标事件
	// "button-press-event" 与 deal_mouse_event 连接，鼠标点击事件
	//g_signal_connect(GTK_OBJECT(window), "button-press-event", G_CALLBACK(deal_mouse_press), NULL);
	gtk_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(select_progress), NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), clist);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

	hbox = gtk_hbox_new(FALSE, 10);
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 0);
	button1 = gtk_button_new_with_label(_("搜索进程PID"));
	button3 = gtk_button_new_with_label(_("刷新进程信息"));
	button4 = gtk_button_new_with_label(_("关机"));


	GtkWidget* label3 = gtk_label_new("");
	g_timeout_add(1000, (GtkFunction)get_progressNum, (gpointer)label3);

	g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(search_PID), scrolled_window);
	g_signal_connect(G_OBJECT(button3), "clicked", G_CALLBACK(refresh_proc), NULL);
	g_signal_connect(G_OBJECT(button4), "clicked", G_CALLBACK(_shutdown), NULL);

	gtk_widget_set_size_request(entry, 150, 30);
	gtk_widget_set_size_request(button1, 70, 30);
	gtk_widget_set_size_request(button2, 120, 30);
	gtk_widget_set_size_request(button3, 80, 30);

	gtk_box_pack_start(GTK_BOX(hbox), label3, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button1, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button3, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button4, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new(_("进程信息")));
}

//读取进程信息
void get_progressInfo(void) {

	DIR* dir = opendir("/proc");
	struct dirent* dir_info;
	int fd;
	progress_num = 0;
	char file[1024];  //保存PID
	char pro_stat[5][255];  //依次存放PID,进程名,状态,内存,优先级
	char tmp[1024];
	char* stat;
	gchar* temp[5];

	while (dir_info = readdir(dir)) {
		if ((dir_info->d_name[0] >= '0') && (dir_info->d_name[0] <= '9')) {
			sprintf(file, "/proc/%s/stat", dir_info->d_name);  //将此文件的全部路径写入file中
			//打开这个文件，并且从中读取到有用的信息放在相应的数组中
			fd = open(file, O_RDONLY);
			read(fd, tmp, 1024);
			close(fd);
			stat = tmp;
			read_progress(pro_stat, stat);
			for (int i = 0; i < 5; i++)
				temp[i] = pro_stat[i];
			gtk_clist_append(GTK_CLIST(clist),temp);	//将上述信息加入clist中
			//gtk_clist_thaw(GTK_CLIST(clist));   //更新list列表显示
			progress_num++;
		}
	}
		closedir(dir);
}

//读取进程信息
void read_progress(char(*stat)[255], char* tmp) {

	char memory[255];

	//从/proc/pid/stat中读取信息
	//pid (name) status ......(14 data) priority (5 data) memory
	int offset = 0;
	while (offset < 1024 && tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(stat[0], tmp);  //保存进程PID
	tmp  += (offset + 1);

	offset = 0;
	while (offset < 1024 && tmp[offset++] != ')');
	tmp[offset-1] = '\0';
	strcpy(stat[1], tmp);  //保存进程名
	tmp += (offset + 1);

	offset = 0;
	while (offset < 1024 && tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(stat[2], tmp);  //保存状态
	tmp += (offset);

	offset = 0;  //跳过中间14种数据类型，如父进程ID,线程组号等
	for (int i = 0; i < 14; i++) {
		while (offset < 1024 && tmp[offset++] != ' ');
		//offset++;
	}
	tmp[offset -1] = '\0';
	tmp += (offset);

	offset = 0;
	while (tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(stat[3], tmp);  //保存优先级
	tmp += (offset);

	offset = 0;  //跳过中间5种数据类型，如静态优先级,该任务所在的线程组里线程的个数等
	for (int i = 0; i < 5; i++) {
		while (offset < 1024 && tmp[offset++] != ' ');
		//offset++;
	}
	tmp[offset-1] = '\0';
	tmp += (offset);

	offset = 0;
	while (offset < 1024 && tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(memory, tmp);  //保存内存使用情况
	if (atoi(tmp) > 1024)
		sprintf(memory, "%.1f MB\0", atoi(tmp) / 1024.0);
	else
		sprintf(memory, "%d KB\0", atoi(tmp));
	strcpy(stat[4], memory);  //保存内存使用情况
}

//选择某一行时
void select_progress(GtkWidget* clist, gint row, gint column, GdkEventButton* event, gpointer data) {
	char title[50];
	gtk_clist_get_text(GTK_CLIST(clist), row, 1, title);
	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &cur_pid);
	//gtk_entry_set_text(GTK_ENTRY(entry), (gchar*)cur_pid);
	char buf[35];
	sprintf(buf, "确定结束进程 <%s> ? \0", title);
	GtkWidget* dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,_("确定结束该进程？"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("警告"));	
	GtkResponseType result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (result == GTK_RESPONSE_YES || result == GTK_RESPONSE_APPLY)
	{
		kill_progress();
	}
	return;
}

//按照PID搜索进程
void search_PID(GtkButton* button, gpointer data) {
	const gchar* entry_text;
	gchar* text;
	gint rtn, i = 0;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	while ((rtn = gtk_clist_get_text(GTK_CLIST(clist), i, 0, &text)) != 0) {
		if (!strcmp(text, entry_text))
			break;
		i++;
	}
	gtk_clist_select_row(GTK_CLIST(clist), i, 0);
	scroll_to_line(data, progress_num, i);
	return;
}

//结束进程
void kill_progress(void) {
	int rtn;
	if (cur_pid != NULL) {
		rtn = kill(atoi(cur_pid), SIGKILL);
		if (rtn == -EPERM) {
			popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			popup_label = gtk_label_new(_("你需要超级用户权限\n来结束该进程！"));
			gtk_widget_set_size_request(popup_window, 300, 180);
			gtk_container_add(GTK_CONTAINER(popup_window), popup_label);
			gtk_window_set_title(GTK_WINDOW(popup_window), _("错误信息"));
			gtk_widget_show_all(popup_window);
		}
	}
	return;
}

void refresh_proc(void) {
	gtk_clist_freeze(GTK_CLIST(clist));
	gtk_clist_clear(GTK_CLIST(clist));
	get_progressInfo();
	gtk_clist_thaw(GTK_CLIST(clist));
	return;
}

void _shutdown(void) {
	//char buf[20] = "确定要现在关机吗？"
	GtkWidget* dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("确定要现在关机吗？"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("警告"));
	GtkResponseType result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (result == GTK_RESPONSE_YES || result == GTK_RESPONSE_APPLY)
	{
		system("shutdown -h now");
	}
	return;
}

// 鼠标点击事件处理函数
gboolean deal_mouse_press(GtkWidget* widget, GdkEventButton* event, gpointer data){
	switch (event->button) {	// 判断鼠标点击的类型
	case 1:
		printf("Left Button!!\n");
		break;
	case 2:
		printf("Middle Button!!\n");
		break;
	case 3:
		printf("Right Button!!\n");
		gtk_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(select_progress), NULL);
		break;
	default:
		printf("Unknown Button!!\n");
	}
	/*if (event->type == GDK_2BUTTON_PRESS)
		gtk_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(select_progress), NULL);*/
	return TRUE;
}

gboolean get_progressNum(gpointer label){
	/*struct sysinfo info;
	sysinfo(&info);
	progress_num = info.procs;*/
	char temp[50];
	sprintf(temp, "当前系统共有\n进程数:%d\0", progress_num);
	gtk_label_set_text(GTK_LABEL(label), _(temp));
	return TRUE;
}