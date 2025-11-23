#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100
#define MAX_PHONE_LENGTH 12

// 节点
typedef struct HashNode {
    char phone[MAX_PHONE_LENGTH];
    char name[50];
    struct HashNode* next;
} HashNode;

// 哈希表结构
typedef struct {
    HashNode** buckets; // 节点
    int size; // 哈希字节数
    int count; // 计数
    double average_search_length;
} HashTable;

HashTable* phone_table = NULL;

// GUI
GtkWidget* window;
GtkWidget *name_entry, *phone_entry, *search_entry;
GtkWidget* result_text;
GtkWidget* asl_label;

// 计算字符串哈希
unsigned int hash_function(const char* phone)
{
    unsigned int hash = 0;
    while (*phone) { // 指针移动
        hash = (hash * 31) + *phone++; // 按位幂乗后相加
    }
    return hash % TABLE_SIZE; // 取余以固定范围
}

// 创建哈希表
HashTable* create_hash_table()
{
    HashTable* table = malloc(sizeof(HashTable));
    table->size = TABLE_SIZE;
    table->count = 0;
    table->average_search_length = 0.0;
    table->buckets = calloc(TABLE_SIZE, sizeof(HashNode*)); // 连续分配，类似数组
    return table;
}

// 插入数据
int hash_table_insert(HashTable* table, const char* name, const char* phone)
{
    if (table->count >= table->size) {
        return -1; // 表满
    }

    // 处理节点
    unsigned int index = hash_function(phone); // 计算哈希
    HashNode* new_node = malloc(sizeof(HashNode));
    strcpy(new_node->name, name);
    strcpy(new_node->phone, phone);
    new_node->next = NULL;

    //
    if (table->buckets[index] == NULL) { // 当前哈希节点为空
        table->buckets[index] = new_node;
    } else {
        HashNode* current = table->buckets[index]; // 当前哈希节点已占用
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    table->count++;
    return 0;
}

// 查找
HashNode* hash_table_search(HashTable* table, const char* phone, int* search_steps) // 最终修改search_steps
{
    unsigned int index = hash_function(phone); // 计算哈希
    HashNode* current = table->buckets[index]; // 节点定位
    *search_steps = 1;

    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) { // 对比数据
            return current;
        }
        current = current->next;
        (*search_steps)++;
    }

    return NULL;
}

// 计算平均查找长度
void calculate_average_search_length(HashTable* table)
{
    if (table->count == 0) {
        table->average_search_length = 0.0;
        return;
    }

    double total_steps = 0;
    int total_searches = 0;

    for (int i = 0; i < table->size; i++) {
        HashNode* current = table->buckets[i]; // 对接节点
        int position = 1;

        while (current != NULL) {
            int steps;
            hash_table_search(table, current->phone, &steps); // 更新当前节点的查找长度
            total_steps += steps;
            total_searches++; // 总查找次数
            current = current->next;
        }
    }

    table->average_search_length = total_steps / total_searches;
}

// GUI 显示哈希表
void display_hash_table(HashTable* table, GtkTextBuffer* buffer)
{
    gtk_text_buffer_set_text(buffer, "哈希表内容:\n\n", -1);

    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);

    char buffer_text[1000];
    int empty_buckets = 0;

    for (int i = 0; i < table->size; i++) {
        if (table->buckets[i] != NULL) {
            HashNode* current = table->buckets[i];
            int chain_length = 0;

            sprintf(buffer_text, "桶[%d]: ", i);
            gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);

            while (current != NULL) {
                sprintf(buffer_text, "(%s, %s) -> ", current->name, current->phone);
                gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
                current = current->next;
                chain_length++;
            }

            sprintf(buffer_text, "NULL [链长: %d]\n", chain_length);
            gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
        } else {
            empty_buckets++;
        }
    }

    sprintf(buffer_text, "\n统计信息:\n");
    gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
    sprintf(buffer_text, "总容量: %d\n", table->size);
    gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
    sprintf(buffer_text, "已使用: %d\n", table->count);
    gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
    sprintf(buffer_text, "空桶数: %d\n", empty_buckets);
    gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
    sprintf(buffer_text, "装载因子: %.3f\n", (double)table->count / table->size);
    gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
    sprintf(buffer_text, "平均查找长度: %.3f\n", table->average_search_length);
    gtk_text_buffer_insert(buffer, &iter, buffer_text, -1);
}

// GUI 响应
void on_insert_clicked(GtkWidget* widget, gpointer data)
{
    const char* name = gtk_entry_get_text(GTK_ENTRY(name_entry));
    const char* phone = gtk_entry_get_text(GTK_ENTRY(phone_entry));

    if (strlen(name) == 0 || strlen(phone) == 0) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text)),
            "错误: 姓名和手机号不能为空!", -1);
        return;
    }

    if (strlen(phone) != 11) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text)),
            "错误: 手机号必须是11位!", -1);
        return;
    }

    int result = hash_table_insert(phone_table, name, phone);

    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text));

    if (result == 0) {
        calculate_average_search_length(phone_table);
        char asl_text[100];
        sprintf(asl_text, "当前平均查找长度: %.3f", phone_table->average_search_length);
        gtk_label_set_text(GTK_LABEL(asl_label), asl_text);

        gtk_text_buffer_set_text(buffer, "插入成功!\n", -1);
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);

        char info_text[100];
        sprintf(info_text, "姓名: %s, 手机号: %s\n", name, phone);
        gtk_text_buffer_insert(buffer, &iter, info_text, -1);
    } else {
        gtk_text_buffer_set_text(buffer, "插入失败! 哈希表可能已满。", -1);
    }
}

void on_search_clicked(GtkWidget* widget, gpointer data)
{
    const char* phone = gtk_entry_get_text(GTK_ENTRY(search_entry));

    if (strlen(phone) == 0) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text)),
            "错误: 请输入要查找的手机号!", -1);
        return;
    }

    int search_steps;
    HashNode* result = hash_table_search(phone_table, phone, &search_steps);

    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text));

    if (result != NULL) {
        char result_text[200];
        sprintf(result_text, "查找成功!\n姓名: %s\n手机号: %s\n查找步数: %d\n当前平均查找长度: %.3f",
            result->name, result->phone, search_steps, phone_table->average_search_length);
        gtk_text_buffer_set_text(buffer, result_text, -1);
    } else {
        char result_text[100];
        sprintf(result_text, "未找到手机号: %s\n查找步数: %d", phone, search_steps);
        gtk_text_buffer_set_text(buffer, result_text, -1);
    }
}

void on_display_clicked(GtkWidget* widget, gpointer data)
{
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text));
    display_hash_table(phone_table, buffer);
}

void on_clear_clicked(GtkWidget* widget, gpointer data)
{
    // 清空哈希表
    for (int i = 0; i < phone_table->size; i++) {
        HashNode* current = phone_table->buckets[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
        phone_table->buckets[i] = NULL;
    }
    phone_table->count = 0;
    phone_table->average_search_length = 0.0;

    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text)),
        "哈希表已清空!", -1);
    gtk_label_set_text(GTK_LABEL(asl_label), "当前平均查找长度: 0.000");

    // 清空输入框
    gtk_entry_set_text(GTK_ENTRY(name_entry), "");
    gtk_entry_set_text(GTK_ENTRY(phone_entry), "");
    gtk_entry_set_text(GTK_ENTRY(search_entry), "");
}

void create_gui()
{
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "学生手机号哈希表管理系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 标题
    GtkWidget* title_label = gtk_label_new("学生手机号哈希表管理系统");
    gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 0);

    // 平均查找长度
    asl_label = gtk_label_new("当前平均查找长度: 0.000");
    gtk_box_pack_start(GTK_BOX(vbox), asl_label, FALSE, FALSE, 0);

    // 分隔线
    GtkWidget* separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator1, FALSE, FALSE, 5);

    // 插入
    GtkWidget* insert_frame = gtk_frame_new("插入学生信息");
    gtk_box_pack_start(GTK_BOX(vbox), insert_frame, FALSE, FALSE, 0);

    GtkWidget* insert_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(insert_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(insert_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(insert_grid), 5);
    gtk_container_add(GTK_CONTAINER(insert_frame), insert_grid);

    GtkWidget* name_label = gtk_label_new("姓名:");
    GtkWidget* phone_label = gtk_label_new("手机号:");
    name_entry = gtk_entry_new();
    phone_entry = gtk_entry_new();

    gtk_grid_attach(GTK_GRID(insert_grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(insert_grid), name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(insert_grid), phone_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(insert_grid), phone_entry, 1, 1, 1, 1);

    GtkWidget* insert_button = gtk_button_new_with_label("插入");
    gtk_grid_attach(GTK_GRID(insert_grid), insert_button, 2, 0, 1, 2);

    // 查找
    GtkWidget* search_frame = gtk_frame_new("查找学生信息");
    gtk_box_pack_start(GTK_BOX(vbox), search_frame, FALSE, FALSE, 0);

    GtkWidget* search_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(search_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(search_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(search_grid), 5);
    gtk_container_add(GTK_CONTAINER(search_frame), search_grid);

    GtkWidget* search_phone_label = gtk_label_new("手机号:");
    search_entry = gtk_entry_new();
    GtkWidget* search_button = gtk_button_new_with_label("查找");

    gtk_grid_attach(GTK_GRID(search_grid), search_phone_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), search_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), search_button, 2, 0, 1, 1);

    // 操作按钮
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    GtkWidget* display_button = gtk_button_new_with_label("显示哈希表");
    GtkWidget* clear_button = gtk_button_new_with_label("清空哈希表");

    gtk_box_pack_start(GTK_BOX(button_box), display_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), clear_button, TRUE, TRUE, 0);

    // 结果
    GtkWidget* result_frame = gtk_frame_new("结果");
    gtk_box_pack_start(GTK_BOX(vbox), result_frame, TRUE, TRUE, 0);

    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(result_frame), scrolled_window);

    result_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(result_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled_window), result_text);

    // 连接信号
    g_signal_connect(insert_button, "clicked", G_CALLBACK(on_insert_clicked), NULL);
    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_clicked), NULL);
    g_signal_connect(display_button, "clicked", G_CALLBACK(on_display_clicked), NULL);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_clicked), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    phone_table = create_hash_table(); // 创建哈希表

    create_gui();
    gtk_main();

    // 释放内存
    for (int i = 0; i < phone_table->size; i++) {
        HashNode* current = phone_table->buckets[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(phone_table->buckets);
    free(phone_table);

    return 0;
}
