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
float cpu_ratio;     //����CPUʹ����
char cpu_info[5][255];  //�����洢CPU�ͺţ���Ƶ�������С�����������߳�
float cpu_ratio_data[120];

GtkWidget* cpu_use;  //cpu���
GtkWidget* cpu_draw;   //cpuʹ������
GtkWidget* label4;

void create_sysInfo(GtkWidget* notebook);  //���ڴ���ϵͳ��Ϣҳ
void get_cpuInfo();  //ȡ��CPU��Ϣ

gboolean get_cpuRatio(gpointer label);  //����CPUʹ����
gboolean get_sysInfo(gpointer label);  //����ϵͳ��Ϣ
gboolean cpu_startDraw(GtkWidget* widget, GdkEventExpose* event, gpointer data);  //��ͼ
gboolean draw_cpuUse(gpointer widget);  //�����ͼ����
