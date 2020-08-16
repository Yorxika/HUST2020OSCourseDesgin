#include "exp4.h"

//������ʾ�����ַ���
gchar* _(char* c)
{
	//return(g_locale_to_utf8(c, -1, NULL, NULL, NULL));
	return g_convert(c,- 1, "UTF-8", "GB2312", NULL, NULL, NULL);
}


//ʹ�ù������ڵĹ����������б��ĳһ��
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

	gtk_init(&argc, &argv); 	// ��ʼ��

	char buf[MAXNUM];  //�ַ�������

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), _("ϵͳ�����"));
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 700);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL); //���رհ�ť���԰ѳ����ж�
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	notebook = gtk_notebook_new();  //�����ʼǱ�
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);  //��ǩ�����Ϸ�
	gtk_container_add(GTK_WINDOW(window), notebook);

	//�½���һҳ��������ʾ����
	create_progress(notebook);

	//�����ڶ�ҳ��������ʾϵͳ��Ϣ
	create_sysInfo(notebook);

	gtk_widget_show_all(window); //��ʾ���пؼ�
	gtk_main();
	return 0;
}
