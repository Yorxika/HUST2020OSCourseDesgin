#include <gtk/gtk.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <wait.h>

void set_widget_font_size(GtkWidget* widget, int size, int is_button)
{
	GtkWidget* labelChild;
	PangoFontDescription* font;
	gint fontSize = size;

	font = pango_font_description_from_string("Sans");//"Sans"������   
	pango_font_description_set_size(font, fontSize * PANGO_SCALE);//���������С   

	if (is_button) {
		labelChild = gtk_bin_get_child(GTK_BIN(widget));//ȡ��GtkButton���label  
	}
	else {
		labelChild = widget;
	}

	//����label�����壬�������GtkButton������ʾ������ͱ���
	gtk_widget_modify_font(GTK_WIDGET(labelChild), font);
	pango_font_description_free(font);
}

gboolean deal_time_1(gpointer* label)
{
	time_t t;
	struct tm *cur_time = NULL;
	time(&t);
	cur_time = localtime(&t);
	char buf[30] ;
	sprintf(buf, "%d-%d-%d\n\t    %d:%d", 1900 + cur_time->tm_year, 1 + cur_time->tm_mon, cur_time->tm_mday,cur_time->tm_hour,cur_time->tm_min);

	//printf("%s \n", buf);
	gtk_label_set_text(GTK_LABEL(label), buf);

	return TRUE;//��������TRUE
}

gboolean deal_time_2(gpointer* label)
{
	time_t t;
	struct tm* cur_time = NULL;
	time(&t);
	cur_time = localtime(&t);
	char buf[3];
	sprintf(buf, "%d", cur_time->tm_sec);

	//printf("%s \n", buf);
	gtk_label_set_text(GTK_LABEL(label), buf);

	return TRUE;//��������TRUE
}

gboolean deal_time_3(gpointer* label)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	char buf[4];
	sprintf(buf, "%d", tv.tv_usec / 1000);

	//printf("%s \n", buf);
	gtk_label_set_text(GTK_LABEL(label), buf);

	return TRUE;//��������TRUE
}

int main(int argc, char** argv) {
	int child1, child2;
	int rtn;

	switch (child1 = fork())
	{  //�����ӽ���1
	case -1:
		fprintf(stderr, "Fork Error:%s\n",  strerror(errno));
		return -1;
	case 0:
		gtk_init(&argc, &argv);
		GtkWidget *window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window1), "Child Progress 1 : show time end to minute");
		gtk_container_set_border_width(GTK_CONTAINER(window1), 0);
		//���ô���Ĭ�ϴ�С������һ����С��С
		gtk_window_set_default_size(GTK_WINDOW(window1), 300, 400);
		//���ô�������ʾ���е�λ��Ϊ���С�
		gtk_window_set_position(GTK_WINDOW(window1), GTK_WIN_POS_CENTER);
		g_signal_connect(G_OBJECT(window1), "destroy",G_CALLBACK(gtk_main_quit), NULL); //���رհ�ť���԰ѳ����ж�

		GtkWidget* label = gtk_label_new("Cur time:");		// label�Ĵ���
		set_widget_font_size(label, 50, FALSE);	// ����label�������С
		gtk_container_add(GTK_CONTAINER(window1), label);

		// ��ʱ���Ĵ���, label�����ص�����
		guint timer = g_timeout_add(1000, (GSourceFunc)deal_time_1, (gpointer)label);

		gtk_widget_show_all(window1); // ��ʾ���в���

		gtk_main();

		return 0;
	default:
		break;
	}

	switch (child2 = fork())
	{  //�����ӽ���2
	case -1:
		fprintf(stderr, "Fork Error:%s\n", strerror(errno));
		return -1;
	case 0:
		gtk_init(&argc, &argv);
		GtkWidget* window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window2), "Child Progress 2 : show current time sec");
		gtk_container_set_border_width(GTK_CONTAINER(window2), 0);
		//���ô���Ĭ�ϴ�С������һ����С��С
		gtk_window_set_default_size(GTK_WINDOW(window2), 300, 400);
		//���ô�������ʾ���е�λ��Ϊ���С�
		gtk_window_set_position(GTK_WINDOW(window2), GTK_WIN_POS_CENTER);
		g_signal_connect(G_OBJECT(window2), "destroy", G_CALLBACK(gtk_main_quit), NULL); //���رհ�ť���԰ѳ����ж�

		GtkWidget* label2 = gtk_label_new("Current time");		// label�Ĵ���
		set_widget_font_size(label2, 50, FALSE);	// ����label�������С
		gtk_container_add(GTK_CONTAINER(window2), label2);

		// ��ʱ���Ĵ���, label�����ص�����
		guint timer2 = g_timeout_add(1000, (GSourceFunc)deal_time_2, (gpointer)label2);

		gtk_widget_show_all(window2); // ��ʾ���в���

		gtk_main();

		return 0;
	default:
		break;
	}

	gtk_init(&argc, &argv);
	GtkWidget* window3 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window3), "Parent Progress: show current time msec");
	gtk_container_set_border_width(GTK_CONTAINER(window3), 0);
	//���ô���Ĭ�ϴ�С������һ����С��С
	gtk_window_set_default_size(GTK_WINDOW(window3), 300, 400);
	//���ô�������ʾ���е�λ��Ϊ���С�
	gtk_window_set_position(GTK_WINDOW(window3), GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(window3), "destroy", G_CALLBACK(gtk_main_quit), NULL); //���رհ�ť���԰ѳ����ж�

	GtkWidget* label3 = gtk_label_new("Current time");		// label�Ĵ���
	set_widget_font_size(label3, 50, FALSE);	// ����label�������С
	gtk_container_add(GTK_CONTAINER(window3), label3);

	// ��ʱ���Ĵ���, label�����ص�����
	guint timer3 = g_timeout_add(1, (GSourceFunc)deal_time_3, (gpointer)label3);

	gtk_widget_show_all(window3); // ��ʾ���в���

	gtk_main();

	waitpid(child1, &rtn, 0);
	waitpid(child2, &rtn, 0);
	return 0;
}