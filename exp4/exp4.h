#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <errno.h>

#include "progress.h"
#include "system_info.h"

#define MAXNUM 100

//ȫ�ֱ���
GtkWidget* window;  //������
GtkWidget* vbox; //��ֱ��������
GtkWidget* hbox; //��ֱ��������
GtkWidget* notebook;   //�ʼǱ�
GtkWidget* entry;  //�б༭�ؼ�
GtkWidget* button1;  //��ť
GtkWidget* button2;
GtkWidget* button3;
GtkWidget* button4;
GtkWidget* popup_window;  //����
GtkWidget* frame;
GtkWidget* popup_label;
GtkWidget* clist;  //�б�
GtkWidget* scrolled_window;

char* cur_pid;  //���浱ǰ������̵�PID
static gint progress_num = 0;  //��������

gchar* _(char* c);  //������ʾ�����ַ���
void scroll_to_line(gpointer scrolled_window, gint line_num, gint to_line_index);//ʹ�ù������ڵĹ����������б��ĳһ��
