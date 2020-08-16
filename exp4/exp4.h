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

//全局变量
GtkWidget* window;  //主窗口
GtkWidget* vbox; //垂直布局容器
GtkWidget* hbox; //垂直布局容器
GtkWidget* notebook;   //笔记本
GtkWidget* entry;  //行编辑控件
GtkWidget* button1;  //按钮
GtkWidget* button2;
GtkWidget* button3;
GtkWidget* button4;
GtkWidget* popup_window;  //弹窗
GtkWidget* frame;
GtkWidget* popup_label;
GtkWidget* clist;  //列表
GtkWidget* scrolled_window;

char* cur_pid;  //保存当前点击进程的PID
static gint progress_num = 0;  //进程总数

gchar* _(char* c);  //用于显示中文字符串
void scroll_to_line(gpointer scrolled_window, gint line_num, gint to_line_index);//使得滚动窗口的滚动条滚到列表的某一行
