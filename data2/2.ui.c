#include <ctype.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;

// 栈操作函数
void initStack(Stack* s)
{
    s->top = -1;
}

int isEmpty(Stack* s)
{
    return s->top == -1;
}

int push(Stack* s, int value)
{
    if (s->top >= MAX_SIZE - 1) {
        return 0;
    }
    s->top++;
    s->data[s->top] = value;
    return 1;
}

int pop(Stack* s)
{
    if (isEmpty(s)) {
        return -1;
    }
    return s->data[(s->top)--];
}

int gettop(Stack* s)
{
    if (s->top == -1) {
        return 0;
    }
    return s->data[s->top];
}

int isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int getPriority(char op)
{
    switch (op) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    default:
        return 0;
    }
}

int safeIsDigit(char c)
{
    return isdigit((unsigned char)c);
}

// 输入验证函数
int isValidExpression(char* string)
{
    int len = strlen(string);
    int parenthesis = 0;

    for (int i = 0; i < len; i++) {
        char c = string[i];

        if (!safeIsDigit(c) && !isOperator(c) && c != '(' && c != ')' && c != ' ') {
            return 0;
        }

        if (c == '(')
            parenthesis++;
        if (c == ')')
            parenthesis--;

        if (parenthesis < 0) {
            return 0;
        }
    }

    if (parenthesis != 0) {
        return 0;
    }

    return 1;
}

// 中缀转后缀表达式
void infixToPostfix(char* infix, char* postfix)
{
    Stack opStack;
    initStack(&opStack);
    int i = 0, j = 0;
    char c;

    while (infix[i] != '\0') {
        c = infix[i];

        if (c == ' ') {
            i++;
            continue;
        }

        if (safeIsDigit(c)) {
            while (safeIsDigit(infix[i])) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' ';
            continue;
        }

        if (c == '(') {
            push(&opStack, c);
            i++;
            continue;
        }

        if (c == ')') {
            while (!isEmpty(&opStack) && gettop(&opStack) != '(') {
                postfix[j++] = pop(&opStack);
                postfix[j++] = ' ';
            }
            if (!isEmpty(&opStack) && gettop(&opStack) == '(') {
                pop(&opStack);
            }
            i++;
            continue;
        }

        if (isOperator(c)) {
            while (!isEmpty(&opStack) && getPriority(gettop(&opStack)) >= getPriority(c) && gettop(&opStack) != '(') {
                postfix[j++] = pop(&opStack);
                postfix[j++] = ' ';
            }
            push(&opStack, c);
            i++;
            continue;
        }

        i++;
    }

    while (!isEmpty(&opStack)) {
        postfix[j++] = pop(&opStack);
        postfix[j++] = ' ';
    }

    postfix[j] = '\0';
}

// 计算后缀表达式
int evaluatePostfix(char* postfix)
{
    Stack numStack;
    initStack(&numStack);
    int i = 0;
    char c;

    while (postfix[i] != '\0') {
        c = postfix[i];

        if (c == ' ') {
            i++;
            continue;
        }

        if (safeIsDigit(c)) {
            int num = 0;
            while (safeIsDigit(postfix[i])) {
                num = num * 10 + (postfix[i] - '0');
                i++;
            }
            push(&numStack, num);
            continue;
        }

        if (isOperator(c)) {
            if (isEmpty(&numStack)) {
                return -1;
            }
            int b = pop(&numStack);
            if (isEmpty(&numStack)) {
                return -1;
            }
            int a = pop(&numStack);
            int result;

            switch (c) {
            case '+':
                result = a + b;
                break;
            case '-':
                result = a - b;
                break;
            case '*':
                result = a * b;
                break;
            case '/':
                if (b == 0) {
                    return -1;
                }
                result = a / b;
                break;
            default:
                return -1;
            }

            push(&numStack, result);
            i++;
            continue;
        }

        i++;
    }

    if (isEmpty(&numStack)) {
        return -1;
    }

    int result = pop(&numStack);

    if (!isEmpty(&numStack)) {
        return -1;
    }

    return result;
}

// GTK回调函数
void on_calculate_clicked(GtkWidget* widget, gpointer data)
{
    GtkWidget** widgets = (GtkWidget**)data;
    GtkEntry* entry = GTK_ENTRY(widgets[0]);
    GtkLabel* result_label = GTK_LABEL(widgets[1]);
    GtkLabel* postfix_label = GTK_LABEL(widgets[2]);

    const gchar* infix = gtk_entry_get_text(entry);

    if (strlen(infix) == 0) {
        gtk_label_set_text(result_label, "错误：请输入表达式！");
        gtk_label_set_text(postfix_label, "");
        return;
    }

    if (!isValidExpression((char*)infix)) {
        gtk_label_set_text(result_label, "错误：表达式不合法！");
        gtk_label_set_text(postfix_label, "");
        return;
    }

    char postfix[MAX_SIZE * 2];
    infixToPostfix((char*)infix, postfix);

    gtk_label_set_text(postfix_label, postfix);

    int result = evaluatePostfix(postfix);
    if (result == -1) {
        gtk_label_set_text(result_label, "错误：计算失败！");
    } else {
        char result_str[50];
        snprintf(result_str, sizeof(result_str), "结果: %d", result);
        gtk_label_set_text(result_label, result_str);
    }
}

void on_clear_clicked(GtkWidget* widget, gpointer data)
{
    GtkWidget** widgets = (GtkWidget**)data;
    GtkEntry* entry = GTK_ENTRY(widgets[0]);
    GtkLabel* result_label = GTK_LABEL(widgets[1]);
    GtkLabel* postfix_label = GTK_LABEL(widgets[2]);

    gtk_entry_set_text(entry, "");
    gtk_label_set_text(result_label, "结果将显示在这里");
    gtk_label_set_text(postfix_label, "后缀表达式将显示在这里");
}

int main(int argc, char* argv[])
{
    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget* entry;
    GtkWidget* calculate_btn;
    GtkWidget* clear_btn;
    GtkWidget* result_label;
    GtkWidget* postfix_label;
    GtkWidget* title_label;
    GtkWidget* input_label;
    GtkWidget* postfix_title_label;
    GtkWidget* result_title_label;

    gtk_init(&argc, &argv);

    // 创建主窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "算术表达式求值器");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 300);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // 创建网格布局
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 创建标题
    title_label = gtk_label_new("算术表达式求值器");
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 2, 1);

    // 创建输入标签和输入框
    input_label = gtk_label_new("请输入表达式:");
    gtk_grid_attach(GTK_GRID(grid), input_label, 0, 1, 2, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 2, 2, 1);

    // 创建按钮
    calculate_btn = gtk_button_new_with_label("计算");
    clear_btn = gtk_button_new_with_label("清空");

    gtk_grid_attach(GTK_GRID(grid), calculate_btn, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), clear_btn, 1, 3, 1, 1);

    // 创建后缀表达式显示区域
    postfix_title_label = gtk_label_new("后缀表达式:");
    gtk_grid_attach(GTK_GRID(grid), postfix_title_label, 0, 4, 2, 1);

    postfix_label = gtk_label_new("后缀表达式将显示在这里");
    gtk_label_set_line_wrap(GTK_LABEL(postfix_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), postfix_label, 0, 5, 2, 1);

    // 创建结果显示区域
    result_title_label = gtk_label_new("计算结果:");
    gtk_grid_attach(GTK_GRID(grid), result_title_label, 0, 6, 2, 1);

    result_label = gtk_label_new("结果将显示在这里");
    gtk_grid_attach(GTK_GRID(grid), result_label, 0, 7, 2, 1);

    // 存储需要传递的控件指针
    GtkWidget* widgets[3] = { entry, result_label, postfix_label };

    // 连接信号
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(calculate_btn, "clicked", G_CALLBACK(on_calculate_clicked), widgets);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_clear_clicked), widgets);

    // 显示所有控件
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
