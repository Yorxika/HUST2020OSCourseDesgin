#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <time.h>

struct CPUinfo
{
	/*char modeName[50]; //line 5
	char cpuMHz[10]; //line 7
	char cacheSize[10];//line 8
	char cpuCores[10]; //line 12
	char addrSize[50]; //line 30*/
}cpu;

static int cpu_curve_start = 20;
float cpu_ratio;     //保存CPU使用率
char cpu_info[5][255];  //用来存储CPU型号，主频，缓存大小，核心数，线程
float cpu_ratio_data[120];

GtkWidget* cpu_use;  //cpu框架
GtkWidget* cpu_draw;   //cpu使用曲线
GtkWidget* label4;

void create_sysInfo(GtkWidget* notebook);  //用于创建系统信息页
void get_cpuInfo();  //取得CPU信息

gboolean get_cpuRatio(gpointer label);  //保存CPU使用率
gboolean get_sysInfo(gpointer label);  //保存系统信息
gboolean cpu_startDraw(GtkWidget* widget, GdkEventExpose* event, gpointer data);  //画图
gboolean draw_cpuUse(gpointer widget);  //具体绘图函数
