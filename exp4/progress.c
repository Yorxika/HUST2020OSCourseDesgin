#include "exp4.h"

//������һҳ��������ʾ������Ϣ
void create_progress(GtkWidget* notebook) {

	vbox = gtk_vbox_new(FALSE, 0); // ������ֱ����
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);  //��һ���µĹ�������(scrolled window)
	gtk_widget_set_size_request(scrolled_window, 550, 500);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gchar* name[5] = { _("��������"),"PID",_("����״̬"),_("ռ���ڴ�"),_("�������ȼ�") };
	clist = gtk_clist_new(5);  //�����б�������ʾ���̾�����Ϣ
	gtk_clist_set_column_title(GTK_CLIST(clist), 0, name[1]);  //����ÿһ�е�����
	gtk_clist_set_column_title(GTK_CLIST(clist), 1, name[0]);
	gtk_clist_set_column_title(GTK_CLIST(clist), 2, name[2]);
	gtk_clist_set_column_title(GTK_CLIST(clist), 3, name[4]);
	gtk_clist_set_column_title(GTK_CLIST(clist), 4, name[3]);

	gtk_clist_set_column_width(GTK_CLIST(clist), 0, 75);  //����ÿһ�п��
	gtk_clist_set_column_width(GTK_CLIST(clist), 1, 125);
	gtk_clist_set_column_width(GTK_CLIST(clist), 2, 75);
	gtk_clist_set_column_width(GTK_CLIST(clist), 3, 75);
	gtk_clist_set_column_width(GTK_CLIST(clist), 4, 125);
	gtk_clist_column_titles_show(GTK_CLIST(clist));

	get_progressInfo();

	//gtk_widget_add_events(clist, GDK_BUTTON_PRESS_MASK); //clist��������¼�
	// "button-press-event" �� deal_mouse_event ���ӣ�������¼�
	//g_signal_connect(GTK_OBJECT(window), "button-press-event", G_CALLBACK(deal_mouse_press), NULL);
	gtk_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(select_progress), NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), clist);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

	hbox = gtk_hbox_new(FALSE, 10);
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 0);
	button1 = gtk_button_new_with_label(_("��������PID"));
	button3 = gtk_button_new_with_label(_("ˢ�½�����Ϣ"));
	button4 = gtk_button_new_with_label(_("�ػ�"));


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

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new(_("������Ϣ")));
}

//��ȡ������Ϣ
void get_progressInfo(void) {

	DIR* dir = opendir("/proc");
	struct dirent* dir_info;
	int fd;
	progress_num = 0;
	char file[1024];  //����PID
	char pro_stat[5][255];  //���δ��PID,������,״̬,�ڴ�,���ȼ�
	char tmp[1024];
	char* stat;
	gchar* temp[5];

	while (dir_info = readdir(dir)) {
		if ((dir_info->d_name[0] >= '0') && (dir_info->d_name[0] <= '9')) {
			sprintf(file, "/proc/%s/stat", dir_info->d_name);  //�����ļ���ȫ��·��д��file��
			//������ļ������Ҵ��ж�ȡ�����õ���Ϣ������Ӧ��������
			fd = open(file, O_RDONLY);
			read(fd, tmp, 1024);
			close(fd);
			stat = tmp;
			read_progress(pro_stat, stat);
			for (int i = 0; i < 5; i++)
				temp[i] = pro_stat[i];
			gtk_clist_append(GTK_CLIST(clist),temp);	//��������Ϣ����clist��
			//gtk_clist_thaw(GTK_CLIST(clist));   //����list�б���ʾ
			progress_num++;
		}
	}
		closedir(dir);
}

//��ȡ������Ϣ
void read_progress(char(*stat)[255], char* tmp) {

	char memory[255];

	//��/proc/pid/stat�ж�ȡ��Ϣ
	//pid (name) status ......(14 data) priority (5 data) memory
	int offset = 0;
	while (offset < 1024 && tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(stat[0], tmp);  //�������PID
	tmp  += (offset + 1);

	offset = 0;
	while (offset < 1024 && tmp[offset++] != ')');
	tmp[offset-1] = '\0';
	strcpy(stat[1], tmp);  //���������
	tmp += (offset + 1);

	offset = 0;
	while (offset < 1024 && tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(stat[2], tmp);  //����״̬
	tmp += (offset);

	offset = 0;  //�����м�14���������ͣ��縸����ID,�߳���ŵ�
	for (int i = 0; i < 14; i++) {
		while (offset < 1024 && tmp[offset++] != ' ');
		//offset++;
	}
	tmp[offset -1] = '\0';
	tmp += (offset);

	offset = 0;
	while (tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(stat[3], tmp);  //�������ȼ�
	tmp += (offset);

	offset = 0;  //�����м�5���������ͣ��羲̬���ȼ�,���������ڵ��߳������̵߳ĸ�����
	for (int i = 0; i < 5; i++) {
		while (offset < 1024 && tmp[offset++] != ' ');
		//offset++;
	}
	tmp[offset-1] = '\0';
	tmp += (offset);

	offset = 0;
	while (offset < 1024 && tmp[offset++] != ' ');
	tmp[offset-1] = '\0';
	strcpy(memory, tmp);  //�����ڴ�ʹ�����
	if (atoi(tmp) > 1024)
		sprintf(memory, "%.1f MB\0", atoi(tmp) / 1024.0);
	else
		sprintf(memory, "%d KB\0", atoi(tmp));
	strcpy(stat[4], memory);  //�����ڴ�ʹ�����
}

//ѡ��ĳһ��ʱ
void select_progress(GtkWidget* clist, gint row, gint column, GdkEventButton* event, gpointer data) {
	char title[50];
	gtk_clist_get_text(GTK_CLIST(clist), row, 1, title);
	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &cur_pid);
	//gtk_entry_set_text(GTK_ENTRY(entry), (gchar*)cur_pid);
	char buf[35];
	sprintf(buf, "ȷ���������� <%s> ? \0", title);
	GtkWidget* dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,_("ȷ�������ý��̣�"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("����"));	
	GtkResponseType result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (result == GTK_RESPONSE_YES || result == GTK_RESPONSE_APPLY)
	{
		kill_progress();
	}
	return;
}

//����PID��������
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

//��������
void kill_progress(void) {
	int rtn;
	if (cur_pid != NULL) {
		rtn = kill(atoi(cur_pid), SIGKILL);
		if (rtn == -EPERM) {
			popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			popup_label = gtk_label_new(_("����Ҫ�����û�Ȩ��\n�������ý��̣�"));
			gtk_widget_set_size_request(popup_window, 300, 180);
			gtk_container_add(GTK_CONTAINER(popup_window), popup_label);
			gtk_window_set_title(GTK_WINDOW(popup_window), _("������Ϣ"));
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
	//char buf[20] = "ȷ��Ҫ���ڹػ���"
	GtkWidget* dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("ȷ��Ҫ���ڹػ���"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("����"));
	GtkResponseType result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (result == GTK_RESPONSE_YES || result == GTK_RESPONSE_APPLY)
	{
		system("shutdown -h now");
	}
	return;
}

// ������¼�������
gboolean deal_mouse_press(GtkWidget* widget, GdkEventButton* event, gpointer data){
	switch (event->button) {	// �ж������������
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
	sprintf(temp, "��ǰϵͳ����\n������:%d\0", progress_num);
	gtk_label_set_text(GTK_LABEL(label), _(temp));
	return TRUE;
}