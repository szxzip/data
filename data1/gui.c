#include "data_management.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

// 全局组件
GtkWidget* main_window;
GtkTextBuffer* output_buffer;

void update_output_display(const char* text)
{
    gtk_text_buffer_set_text(output_buffer, text, -1);
}

void on_add_student_clicked(GtkButton* button, gpointer user_data)
{
    GtkWidget* dialog;
    GtkWidget* content_area;
    GtkWidget* grid;
    GtkWidget *id_entry, *name_entry, *major_entry, *grade_entry;

    dialog = gtk_dialog_new_with_buttons("添加学生",
        GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "确定",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // 创建输入字段
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("学号:"), 0, 0, 1, 1);
    id_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("姓名:"), 0, 1, 1, 1);
    name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("专业:"), 0, 2, 1, 1);
    major_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(major_entry), "计算机科学与技术");
    gtk_grid_attach(GTK_GRID(grid), major_entry, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("年级:"), 0, 3, 1, 1);
    grade_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(grade_entry), "2011");
    gtk_grid_attach(GTK_GRID(grid), grade_entry, 1, 3, 1, 1);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* id_text = gtk_entry_get_text(GTK_ENTRY(id_entry));
        const char* name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char* major = gtk_entry_get_text(GTK_ENTRY(major_entry));
        const char* grade_text = gtk_entry_get_text(GTK_ENTRY(grade_entry));

        int id = atoi(id_text);
        int grade = atoi(grade_text);

        if (id <= 0) {
            update_output_display("错误：学号必须为正整数");
        } else if (strlen(name) == 0) {
            update_output_display("错误：姓名不能为空");
        } else {
            int result = add_student(id, name, major, grade);
            if (result == 0) {
                char message[256];
                snprintf(message, sizeof(message), "成功添加学生: %s (学号: %d)", name, id);
                update_output_display(message);
            } else if (result == -2) {
                update_output_display("添加学生失败: 学号已存在");
            } else {
                update_output_display("添加学生失败: 容量已满");
            }
        }
    }

    gtk_widget_destroy(dialog);
}

void on_delete_student_clicked(GtkButton* button, gpointer user_data)
{
    GtkWidget* dialog;
    GtkWidget* content_area;
    GtkWidget* id_entry;

    dialog = gtk_dialog_new_with_buttons("删除学生",
        GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "确定",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    id_entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content_area), gtk_label_new("请输入要删除的学生学号:"));
    gtk_container_add(GTK_CONTAINER(content_area), id_entry);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* id_text = gtk_entry_get_text(GTK_ENTRY(id_entry));
        int id = atoi(id_text);

        if (id <= 0) {
            update_output_display("错误：学号必须为正整数");
        } else {
            int result = delete_student(id);
            if (result == 0) {
                char message[256];
                snprintf(message, sizeof(message), "成功删除学生 (学号: %d)", id);
                update_output_display(message);
            } else {
                update_output_display("删除学生失败: 未找到该学号");
            }
        }
    }

    gtk_widget_destroy(dialog);
}

void on_query_student_clicked(GtkButton* button, gpointer user_data)
{
    GtkWidget* dialog;
    GtkWidget* content_area;
    GtkWidget* id_entry;

    dialog = gtk_dialog_new_with_buttons("查询学生",
        GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "确定",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    id_entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content_area), gtk_label_new("请输入要查询的学生学号:"));
    gtk_container_add(GTK_CONTAINER(content_area), id_entry);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* id_text = gtk_entry_get_text(GTK_ENTRY(id_entry));
        int id = atoi(id_text);

        if (id <= 0) {
            update_output_display("错误：学号必须为正整数");
        } else {
            Student* student = find_student(id);
            if (student != NULL) {
                char output[1024];
                snprintf(output, sizeof(output),
                    "学生信息:\n"
                    "学号: %d\n"
                    "姓名: %s\n"
                    "专业: %s\n"
                    "年级: %d\n"
                    "科目成绩:\n",
                    student->id, student->name, student->major, student->grade);

                SubjectScore* subject = student->subjects;
                if (subject == NULL) {
                    strcat(output, "  暂无科目成绩\n");
                } else {
                    while (subject != NULL) {
                        char subject_info[100];
                        snprintf(subject_info, sizeof(subject_info), "  %s: %.1f\n", subject->subject, subject->score);
                        strcat(output, subject_info);
                        subject = subject->next;
                    }
                }

                update_output_display(output);
            } else {
                update_output_display("查询失败: 未找到该学号的学生");
            }
        }
    }

    gtk_widget_destroy(dialog);
}

void on_add_score_clicked(GtkButton* button, gpointer user_data)
{
    GtkWidget* dialog;
    GtkWidget* content_area;
    GtkWidget* grid;
    GtkWidget *id_entry, *subject_entry, *score_entry;

    dialog = gtk_dialog_new_with_buttons("添加成绩",
        GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "确定",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    gtk_container_add(GTK_CONTAINER(content_area), grid);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("学号:"), 0, 0, 1, 1);
    id_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("科目:"), 0, 1, 1, 1);
    subject_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), subject_entry, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("成绩:"), 0, 2, 1, 1);
    score_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), score_entry, 1, 2, 1, 1);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* id_text = gtk_entry_get_text(GTK_ENTRY(id_entry));
        const char* subject = gtk_entry_get_text(GTK_ENTRY(subject_entry));
        const char* score_text = gtk_entry_get_text(GTK_ENTRY(score_entry));

        int id = atoi(id_text);
        float score = atof(score_text);

        if (id <= 0) {
            update_output_display("错误：学号必须为正整数");
        } else if (strlen(subject) == 0) {
            update_output_display("错误：科目名称不能为空");
        } else if (score < 0 || score > 100) {
            update_output_display("错误：成绩必须在0-100之间");
        } else {
            int result = add_subject_score(id, subject, score);
            if (result == 0) {
                char message[256];
                snprintf(message, sizeof(message), "成功为学号 %d 添加科目 %s 的成绩: %.1f", id, subject, score);
                update_output_display(message);
            } else {
                update_output_display("添加成绩失败: 学生不存在");
            }
        }
    }

    gtk_widget_destroy(dialog);
}

void on_list_all_clicked(GtkButton* button, gpointer user_data)
{
    Student* all_students;
    int count;
    get_all_students(&all_students, &count);

    if (count == 0) {
        update_output_display("没有学生记录");
        return;
    }

    char output[8192] = "所有学生信息:\n\n";

    for (int i = 0; i < count; i++) {
        Student* student = &all_students[i];
        char student_info[512];
        snprintf(student_info, sizeof(student_info),
            "学号: %d, 姓名: %s, 专业: %s, 年级: %d\n",
            student->id, student->name, student->major, student->grade);
        strcat(output, student_info);

        SubjectScore* subject = student->subjects;
        if (subject == NULL) {
            strcat(output, "  暂无科目成绩\n");
        } else {
            while (subject != NULL) {
                char subject_info[100];
                snprintf(subject_info, sizeof(subject_info), "  %s: %.1f\n", subject->subject, subject->score);
                strcat(output, subject_info);
                subject = subject->next;
            }
        }
        strcat(output, "\n");
    }

    update_output_display(output);
}

void show_main_window(GtkApplication* app)
{
    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget *add_button, *delete_button, *query_button;
    GtkWidget *add_score_button, *list_all_button;
    GtkWidget* scrolled_window;
    GtkWidget* output_view;

    // 创建主窗口
    window = gtk_application_window_new(app);
    main_window = window;
    gtk_window_set_title(GTK_WINDOW(window), "学生成绩管理系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // 创建网格布局
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 创建按钮
    add_button = gtk_button_new_with_label("添加学生");
    delete_button = gtk_button_new_with_label("删除学生");
    query_button = gtk_button_new_with_label("查询学生");
    add_score_button = gtk_button_new_with_label("添加成绩");
    list_all_button = gtk_button_new_with_label("列出所有学生");

    // 将按钮添加到网格
    gtk_grid_attach(GTK_GRID(grid), add_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), delete_button, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), query_button, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), add_score_button, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), list_all_button, 4, 0, 1, 1);

    // 创建输出显示区域
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 1, 5, 1);

    output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(output_view), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled_window), output_view);

    output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));

    // 连接信号
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_student_clicked), NULL);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_student_clicked), NULL);
    g_signal_connect(query_button, "clicked", G_CALLBACK(on_query_student_clicked), NULL);
    g_signal_connect(add_score_button, "clicked", G_CALLBACK(on_add_score_clicked), NULL);
    g_signal_connect(list_all_button, "clicked", G_CALLBACK(on_list_all_clicked), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 显示所有组件
    gtk_widget_show_all(window);

    update_output_display("欢迎使用学生成绩管理系统！\n数据已从 students.txt 文件加载。\n请选择操作按钮开始使用。");
}

void activate(GtkApplication* app, gpointer user_data)
{
    int loaded_count = load_data_from_file();
    show_main_window(app);

    if (loaded_count > 0) {
        char message[256];
        snprintf(message, sizeof(message), "欢迎使用学生成绩管理系统！\n已从文件加载 %d 条学生记录。\n请选择操作按钮开始使用。", loaded_count);
        update_output_display(message);
    } else {
        update_output_display("欢迎使用学生成绩管理系统！\n当前没有学生记录。\n请使用'添加学生'按钮开始添加数据。");
    }
}
