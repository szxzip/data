#include "data_management.h"
#include "gui.h"
#include <gtk/gtk.h>

int main(int argc, char** argv)
{
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.gtk.studentmanagement", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // 程序退出前清理数据
    cleanup_data();

    return status;
}
