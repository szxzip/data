#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 100
#define NAME_LEN 20
#define ID_LEN 15
#define FILENAME "students.dat" // 文件名

typedef struct {
    char id[ID_LEN];
    char name[NAME_LEN];
    float score[3]; // 0 语文 1 数学 2 英语
} Student;

typedef struct {
    Student data[MAXSIZE];
    int len;
} List;

typedef struct Node {
    int index;
    struct Node* next;
} Node;

Node* subjHead[3] = { NULL, NULL, NULL };
List L;

void initList(List* L) { L->len = 0; }
int full(List* L) { return L->len == MAXSIZE; }
int empty(List* L) { return L->len == 0; }

// 数据验证
int is_valid_id(const char* id)
{
    if (strlen(id) == 0 || strlen(id) >= ID_LEN)
        return 0;
    for (int i = 0; id[i]; i++) {
        if (!isalnum(id[i]) && id[i] != '_')
            return 0;
    }
    return 1;
}

int is_valid_name_simple(const char* name)
{
    return strlen(name) > 0 && strlen(name) < NAME_LEN;
}

int is_valid_score(float score)
{
    return score >= 0 && score <= 100;
}

int is_valid_position(int pos, int max_pos)
{
    return pos >= 1 && pos <= max_pos + 1;
}

// id查重
int findById(List* L, const char* id)
{
    for (int i = 0; i < L->len; i++)
        if (strcmp(L->data[i].id, id) == 0)
            return i;
    return -1;
}

// 添加学生
void addStudent(List* L, const char* id, const char* name, float chinese, float math, float english)
{
    if (full(L)) // 顺序表已满
        return;

    Student s;
    strcpy(s.id, id);
    strcpy(s.name, name);
    s.score[0] = chinese;
    s.score[1] = math;
    s.score[2] = english;

    L->data[L->len++] = s;

    for (int k = 0; k < 3; k++) {
        Node* p = (Node*)malloc(sizeof(Node));
        if (p == NULL) // 内存分配失败
            return;
        p->index = L->len - 1;
        p->next = subjHead[k];
        subjHead[k] = p;
    }
}

// 删除学生
void delStudent(List* L, const char* id)
{
    int pos = findById(L, id);
    if (pos == -1) // 学号不存在
        return;

    for (int k = 0; k < 3; k++) {
        Node **h = &subjHead[k], *p;
        while ((p = *h) != NULL) {
            if (p->index == pos) {
                *h = p->next;
                free(p);
                break;
            }
            h = &p->next;
        }
        for (p = subjHead[k]; p; p = p->next)
            if (p->index > pos)
                p->index--;
    }
    L->data[pos] = L->data[--L->len];
}

// 查询学生
void searchStudent(List* L, const char* id, char* result)
{
    int pos = findById(L, id);
    if (pos == -1) {
        strcpy(result, "学号不存在！");
        return;
    }
    Student* s = &L->data[pos];
    sprintf(result, "学号：%s\n姓名：%s\n语文：%.1f\n数学：%.1f\n英语：%.1f",
        s->id, s->name, s->score[0], s->score[1], s->score[2]);
}

// 插入学生
void insertStudent(List* L, int pos, const char* id, const char* name, float chinese, float math, float english)
{
    if (full(L)) // 顺序表已满
        return;
    if (pos < 1 || pos > L->len + 1) // 非法位置
        return;
    pos--;

    Student s;
    strcpy(s.id, id);
    strcpy(s.name, name);
    s.score[0] = chinese;
    s.score[1] = math;
    s.score[2] = english;

    for (int i = L->len; i > pos; i--)
        L->data[i] = L->data[i - 1];
    L->data[pos] = s;
    L->len++;

    for (int k = 0; k < 3; k++) {
        Node* p = (Node*)malloc(sizeof(Node));
        if (p == NULL) // 内存分配失败
            return;
        p->index = pos;
        p->next = subjHead[k];
        subjHead[k] = p;
    }
}

// 按学科显示
void getSubjectList(List* L, int k, char* result)
{
    const char* name[3] = { "语文", "数学", "英语" };
    char temp[1024] = "";
    sprintf(temp, "=== 按 %s 成绩链表 ===\n", name[k]);

    Node* p = subjHead[k];
    if (!p) {
        strcat(temp, "暂无学生"); // 连接字符串
        strcpy(result, temp);
        return;
    }

    strcat(temp, "学号\t\t姓名\t\t成绩\n");
    strcat(temp, "--------------------------------\n");

    while (p) {
        Student* s = &L->data[p->index];
        char line[100];
        sprintf(line, "%s\t\t%s\t\t%.1f\n", s->id, s->name, s->score[k]);
        strcat(temp, line);
        p = p->next;
    }
    strcpy(result, temp);
}

// 显示全部学生
void getAllStudents(List* L, char* result)
{
    char temp[4096] = "";
    if (empty(L)) {
        strcpy(temp, "暂无学生信息！");
        strcpy(result, temp);
        return;
    }

    strcpy(temp, "=== 所有学生信息 ===\n");
    strcat(temp, "学号\t\t姓名\t\t语文\t数学\t英语\n");
    strcat(temp, "--------------------------------------------\n");

    for (int i = 0; i < L->len; i++) {
        Student* s = &L->data[i];
        char line[100];
        sprintf(line, "%s\t\t%s\t\t%.1f\t%.1f\t%.1f\n",
            s->id, s->name, s->score[0], s->score[1], s->score[2]);
        strcat(temp, line);
    }
    strcpy(result, temp);
}

// 释放内存
void freeAllLists()
{
    for (int k = 0; k < 3; k++) {
        Node* p = subjHead[k];
        while (p) {
            Node* q = p->next;
            free(p);
            p = q;
        }
        subjHead[k] = NULL;
    }
}

// 文件读写
void saveToFile(List* L)
{
    FILE* fp = fopen(FILENAME, "wb");
    if (fp == NULL) {
        return;
    }

    fwrite(&L->len, sizeof(int), 1, fp);
    fwrite(L->data, sizeof(Student), L->len, fp);
    fclose(fp);
}

void loadFromFile(List* L)
{
    FILE* fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        return;
    }

    fread(&L->len, sizeof(int), 1, fp);
    fread(L->data, sizeof(Student), L->len, fp);
    fclose(fp);

    // 重建链表
    for (int k = 0; k < 3; k++) {
        // 清空原有链表
        Node* p = subjHead[k];
        while (p) {
            Node* q = p->next;
            free(p);
            p = q;
        }
        subjHead[k] = NULL;

        // 重建链表
        for (int i = 0; i < L->len; i++) {
            Node* newNode = (Node*)malloc(sizeof(Node));
            if (newNode == NULL)
                continue;
            newNode->index = i;
            newNode->next = subjHead[k];
            subjHead[k] = newNode;
        }
    }
}

// GTK UI 组件
GtkWidget* text_view;
GtkTextBuffer* text_buffer;

// UI 刷新显示
void update_display(const char* text)
{
    gtk_text_buffer_set_text(text_buffer, text, -1);
}

// UI 错误弹窗
void show_error_dialog(GtkWindow* parent, const char* message)
{
    GtkWidget* dialog = gtk_message_dialog_new(parent,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// UI 信息弹窗
void show_info_dialog(GtkWindow* parent, const char* message)
{
    GtkWidget* dialog = gtk_message_dialog_new(parent,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// UI 添加学生
void on_add_student_clicked(GtkWidget* widget, gpointer data)
{
    GtkWidget *dialog, *content_area, *grid;
    GtkWidget *id_entry, *name_entry, *chinese_entry, *math_entry, *english_entry;

    // 窗口
    dialog = gtk_dialog_new_with_buttons("添加学生",
        GTK_WINDOW(data),
        (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
        "确定",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    // 布局
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // 5 个条目
    id_entry = gtk_entry_new();
    name_entry = gtk_entry_new();
    chinese_entry = gtk_entry_new();
    math_entry = gtk_entry_new();
    english_entry = gtk_entry_new();

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("学号:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("姓名:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("语文 (0-100):"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chinese_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("数学 (0-100):"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), math_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("英语 (0-100):"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), english_entry, 1, 4, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    // 获取数据
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* id = gtk_entry_get_text(GTK_ENTRY(id_entry));
        const char* name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char* chinese_text = gtk_entry_get_text(GTK_ENTRY(chinese_entry));
        const char* math_text = gtk_entry_get_text(GTK_ENTRY(math_entry));
        const char* english_text = gtk_entry_get_text(GTK_ENTRY(english_entry));

        // 数据验证
        if (!is_valid_id(id)) {
            show_error_dialog(GTK_WINDOW(data), "学号格式错误！\n只能包含字母、数字和下划线，且不能为空。");
        } else if (!is_valid_name_simple(name)) {
            char error_msg[100];
            sprintf(error_msg, "姓名格式错误！\n姓名不能为空且长度不能超过%d个字符。", NAME_LEN - 1);
            show_error_dialog(GTK_WINDOW(data), error_msg);
        } else {
            float chinese = atof(chinese_text);
            float math = atof(math_text);
            float english = atof(english_text);

            if (!is_valid_score(chinese) || !is_valid_score(math) || !is_valid_score(english)) {
                show_error_dialog(GTK_WINDOW(data), "成绩必须在0-100之间！");
            } else if (findById(&L, id) != -1) {
                show_error_dialog(GTK_WINDOW(data), "学号已存在！");
            } else {
                addStudent(&L, id, name, chinese, math, english);
                saveToFile(&L); // 保存到文件
                char result[256];
                sprintf(result, "添加学生成功！\n学号：%s\n姓名：%s\n语文：%.1f\n数学：%.1f\n英语：%.1f",
                    id, name, chinese, math, english);
                update_display(result);
                show_info_dialog(GTK_WINDOW(data), "添加成功！");
            }
        }
    }
    gtk_widget_destroy(dialog);
}

// UI 搜索学生
void on_search_student_clicked(GtkWidget* widget, gpointer data)
{
    GtkWidget *dialog, *content_area, *id_entry;

    // 窗口
    dialog = gtk_dialog_new_with_buttons("查询学生",
        GTK_WINDOW(data),
        (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
        "查询",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    // 布局
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    id_entry = gtk_entry_new();

    // 条目
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("学号:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 0, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    // 接收数据
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* id = gtk_entry_get_text(GTK_ENTRY(id_entry));

        // 验证、处理数据
        if (!is_valid_id(id)) {
            show_error_dialog(GTK_WINDOW(data), "学号格式错误！");
        } else {
            char result[256];
            searchStudent(&L, id, result);
            update_display(result);
        }
    }
    gtk_widget_destroy(dialog);
}

// UI 插入学生
void on_insert_student_clicked(GtkWidget* widget, gpointer data)
{
    GtkWidget *dialog, *content_area, *grid;
    GtkWidget *pos_entry, *id_entry, *name_entry, *chinese_entry, *math_entry, *english_entry;

    // 窗口
    dialog = gtk_dialog_new_with_buttons("插入学生",
        GTK_WINDOW(data),
        (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
        "确定",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    // 布局
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    pos_entry = gtk_entry_new();
    id_entry = gtk_entry_new();
    name_entry = gtk_entry_new();
    chinese_entry = gtk_entry_new();
    math_entry = gtk_entry_new();
    english_entry = gtk_entry_new();

    char pos_label[50];
    sprintf(pos_label, "位置 (1-%d):", L.len + 1);

    // 条目
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new(pos_label), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pos_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("学号:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("姓名:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("语文 (0-100):"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chinese_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("数学 (0-100):"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), math_entry, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("英语 (0-100):"), 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), english_entry, 1, 5, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    // 接收数据
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* pos_text = gtk_entry_get_text(GTK_ENTRY(pos_entry));
        const char* id = gtk_entry_get_text(GTK_ENTRY(id_entry));
        const char* name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char* chinese_text = gtk_entry_get_text(GTK_ENTRY(chinese_entry));
        const char* math_text = gtk_entry_get_text(GTK_ENTRY(math_entry));
        const char* english_text = gtk_entry_get_text(GTK_ENTRY(english_entry));

        int pos = atoi(pos_text);

        // 数据验证、处理
        if (!is_valid_position(pos, L.len)) {
            show_error_dialog(GTK_WINDOW(data), "位置无效！");
        } else if (!is_valid_id(id)) {
            show_error_dialog(GTK_WINDOW(data), "学号格式错误！");
        } else if (!is_valid_name_simple(name)) {
            char error_msg[100];
            sprintf(error_msg, "姓名格式错误！\n姓名不能为空且长度不能超过%d个字符。", NAME_LEN - 1);
            show_error_dialog(GTK_WINDOW(data), error_msg);
        } else {
            float chinese = atof(chinese_text);
            float math = atof(math_text);
            float english = atof(english_text);

            if (!is_valid_score(chinese) || !is_valid_score(math) || !is_valid_score(english)) {
                show_error_dialog(GTK_WINDOW(data), "成绩必须在0-100之间！");
            } else if (findById(&L, id) != -1) {
                show_error_dialog(GTK_WINDOW(data), "学号已存在！");
            } else {
                insertStudent(&L, pos, id, name, chinese, math, english);
                saveToFile(&L); // 保存到文件
                char result[256];
                sprintf(result, "插入学生成功！\n位置：%d\n学号：%s\n姓名：%s", pos, id, name);
                update_display(result);
                show_info_dialog(GTK_WINDOW(data), "插入成功！");
            }
        }
    }
    gtk_widget_destroy(dialog);
}

// UI 删除学生
void on_delete_student_clicked(GtkWidget* widget, gpointer data)
{
    GtkWidget *dialog, *content_area, *id_entry;

    // 窗口
    dialog = gtk_dialog_new_with_buttons("删除学生",
        GTK_WINDOW(data),
        (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
        "删除",
        GTK_RESPONSE_ACCEPT,
        "取消",
        GTK_RESPONSE_REJECT,
        NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    id_entry = gtk_entry_new();

    // 条目
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("学号:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 0, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    // 接收、处理数据
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* id = gtk_entry_get_text(GTK_ENTRY(id_entry));

        if (!is_valid_id(id)) {
            show_error_dialog(GTK_WINDOW(data), "学号格式错误！");
        } else if (findById(&L, id) == -1) {
            show_error_dialog(GTK_WINDOW(data), "学号不存在！");
        } else {
            delStudent(&L, id);
            saveToFile(&L); // 保存到文件
            char result[256];
            sprintf(result, "删除学生成功！\n学号：%s", id);
            update_display(result);
            show_info_dialog(GTK_WINDOW(data), "删除成功！");
        }
    }
    gtk_widget_destroy(dialog);
}

// UI 显示所有学生
void on_show_all_clicked(GtkWidget* widget, gpointer data)
{
    char result[4096];
    getAllStudents(&L, result);
    update_display(result);
}

// UI 分科显示学生
void on_show_chinese_clicked(GtkWidget* widget, gpointer data)
{
    char result[1024];
    getSubjectList(&L, 0, result);
    update_display(result);
}

void on_show_math_clicked(GtkWidget* widget, gpointer data)
{
    char result[1024];
    getSubjectList(&L, 1, result);
    update_display(result);
}

void on_show_english_clicked(GtkWidget* widget, gpointer data)
{
    char result[1024];
    getSubjectList(&L, 2, result);
    update_display(result);
}

// UI 读写数据
void on_save_data_clicked(GtkWidget* widget, gpointer data)
{
    saveToFile(&L);
    show_info_dialog(GTK_WINDOW(data), "数据保存成功！");
    update_display("数据已成功保存到文件。");
}

void on_load_data_clicked(GtkWidget* widget, gpointer data)
{
    loadFromFile(&L);
    show_info_dialog(GTK_WINDOW(data), "数据加载成功！");
    update_display("数据已从文件加载。");
}

// UI 清除、退出
void on_clear_clicked(GtkWidget* widget, gpointer data)
{
    update_display("欢迎使用学生成绩管理系统！\n\n请选择左侧的功能按钮进行操作。");
}

void on_quit_clicked(GtkWidget* widget, gpointer data)
{
    saveToFile(&L);
    freeAllLists();
    gtk_main_quit();
}

int main(int argc, char* argv[])
{
    GtkWidget *window, *main_box, *vbox, *button_box, *scrolled_window;
    GtkWidget* button;

    gtk_init(&argc, &argv);
    initList(&L);

    // 启动时自动加载数据
    loadFromFile(&L);

    // 创建主窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "学生成绩管理系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 创建主布局
    main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // 创建左侧按钮面板
    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 5);
    gtk_widget_set_size_request(button_box, 150, -1);

    // 创建右侧显示区域
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), vbox, TRUE, TRUE, 5);

    // 创建文本显示区域
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);

    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(text_buffer, "欢迎使用学生成绩管理系统！\n\n程序启动时自动加载数据，退出时自动保存。\n请选择左侧的功能按钮进行操作。", -1);

    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

    // 创建按钮
    button = gtk_button_new_with_label("登记学生");
    g_signal_connect(button, "clicked", G_CALLBACK(on_add_student_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("查询学生");
    g_signal_connect(button, "clicked", G_CALLBACK(on_search_student_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("插入学生");
    g_signal_connect(button, "clicked", G_CALLBACK(on_insert_student_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("删除学生");
    g_signal_connect(button, "clicked", G_CALLBACK(on_delete_student_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    // 分隔线
    GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(button_box), separator, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("显示所有学生");
    g_signal_connect(button, "clicked", G_CALLBACK(on_show_all_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("语文成绩链表");
    g_signal_connect(button, "clicked", G_CALLBACK(on_show_chinese_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("数学成绩链表");
    g_signal_connect(button, "clicked", G_CALLBACK(on_show_math_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("英语成绩链表");
    g_signal_connect(button, "clicked", G_CALLBACK(on_show_english_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    // 分隔线
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(button_box), separator, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("保存数据");
    g_signal_connect(button, "clicked", G_CALLBACK(on_save_data_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("加载数据");
    g_signal_connect(button, "clicked", G_CALLBACK(on_load_data_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("清空显示");
    g_signal_connect(button, "clicked", G_CALLBACK(on_clear_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    button = gtk_button_new_with_label("退出系统");
    g_signal_connect(button, "clicked", G_CALLBACK(on_quit_clicked), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 5);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
