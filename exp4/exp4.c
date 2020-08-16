#include "exp4.h"

//用于显示中文字符串
gchar* _(char* c)
{
	//return(g_locale_to_utf8(c, -1, NULL, NULL, NULL));
	return g_convert(c,- 1, "UTF-8", "GB2312", NULL, NULL, NULL);
}


//使得滚动窗口的滚动条滚到列表的某一行
void scroll_to_line(gpointer scrolled_window, gint line_num, gint to_line_index) {
	GtkAdjustment* adj;
	gdouble lower_value, upper_value, page_size, max_value, line_height, to_value;
	adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
	lower_value = gtk_adjustment_get_lower(adj);
	upper_value = gtk_adjustment_get_upper(adj);
	page_size = gtk_adjustment_get_page_size(adj);
	max_value = upper_value - page_size;
	line_height = upper_value / line_num;
	to_value = line_height * to_line_index;
	if (to_value < lower_value)
		to_value = lower_value;
	if (to_value > max_value)
		to_value = max_value;
	gtk_adjustment_set_value(adj, to_value);
	return;
}

int main(int argc, char** argv) {

	gtk_init(&argc, &argv); 	// 初始化

	char buf[MAXNUM];  //字符缓冲区

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), _("系统监控器"));
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 700);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL); //按关闭按钮可以把程序中断
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	notebook = gtk_notebook_new();  //创建笔记本
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);  //标签置于上方
	gtk_container_add(GTK_WINDOW(window), notebook);

	//新建第一页，用于显示进程
	create_progress(notebook);

	//创建第二页，用于显示系统信息
	create_sysInfo(notebook);

	gtk_widget_show_all(window); //显示所有控件
	gtk_main();
	return 0;
}
