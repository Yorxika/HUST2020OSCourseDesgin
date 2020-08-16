#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <time.h>

void create_progress(GtkWidget* notebook);  //���ڴ�������ҳ
void read_progress(char(*stat)[255], char* tmp); //���ڴ�/proc/pid/stat��ȡ������Ϣ
void get_progressInfo(void);  //���ڴ�/info�ж�ȡ������Ϣ

void select_progress(GtkWidget* clist, gint row, gint column, GdkEventButton* event, gpointer data);  //ѡ�����
void search_PID(GtkButton* button, gpointer data);  //����PID��������
gboolean deal_mouse_press(GtkWidget* widget, GdkEventButton* event, gpointer data);
gboolean get_progressNum(gpointer label);
void kill_progress(void);  //��������
void refresh_proc(void);
void _shutdown(void);


