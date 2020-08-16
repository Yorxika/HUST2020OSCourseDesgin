#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <time.h>

void create_progress(GtkWidget* notebook);  //用于创建进程页
void read_progress(char(*stat)[255], char* tmp); //用于从/proc/pid/stat读取进程信息
void get_progressInfo(void);  //用于从/info中读取进程信息

void select_progress(GtkWidget* clist, gint row, gint column, GdkEventButton* event, gpointer data);  //选择进程
void search_PID(GtkButton* button, gpointer data);  //按照PID搜索进程
gboolean deal_mouse_press(GtkWidget* widget, GdkEventButton* event, gpointer data);
gboolean get_progressNum(gpointer label);
void kill_progress(void);  //结束进程
void refresh_proc(void);
void _shutdown(void);


