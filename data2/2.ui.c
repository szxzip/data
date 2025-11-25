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
    return s->top == -1; // 空的
}

int push(Stack* s, int value)
{
    if (s->top >= MAX_SIZE - 1) {
        return 0; // 满了
    }
    s->top++;
    s->data[s->top] = value;
    return 1;
}

int pop(Stack* s)
{
    if (isEmpty(s)) {
        return -1; // 栈为空
    }
    return s->data[(s->top)--];
}

int gettop(Stack* s)
{
    if (s->top == -1) {
        return 0; // 空的
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
    // 将字符转换为无符号字符以避免负数问题
    return isdigit((unsigned char)c);
}

// 输入验证函数
int isValidExpression(char* string)
{
    int len = strlen(string);
    int parenthesis = 0;

    for (int i = 0; i < len; i++) {
        char c = string[i];

        // 允许的字符：数字、运算符、括号、空格
        if (!safeIsDigit(c) && !isOperator(c) && c != '(' && c != ')' && c != ' ') {
            return 0; // 错误：包含非法字符
        }

        // 检查括号匹配
        if (c == '(')
            parenthesis++;
        if (c == ')')
            parenthesis--;

        if (parenthesis < 0) {
            return 0; // 错误：括号不匹配
        }
    }

    if (parenthesis != 0) {
        return 0; // 错误：括号不匹配
    }

    return 1;
}

// 中缀转后缀表达式
void infixToPostfix(char* infix, char* postfix)
{
    Stack opStack; // 运算符栈
    initStack(&opStack);
    int i = 0, j = 0;
    char c;

    while (infix[i] != '\0') {
        c = infix[i];

        // 跳过空格
        if (c == ' ') {
            i++;
            continue;
        }

        // 如果是数字，直接输出到后缀表达式
        if (safeIsDigit(c)) {
            while (safeIsDigit(infix[i])) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' '; // 用空格分隔数字
            continue;
        }

        // 如果是左括号，直接入栈
        if (c == '(') {
            push(&opStack, c);
            i++;
            continue;
        }

        // 如果是右括号，弹出栈中元素直到遇到左括号
        if (c == ')') {
            while (!isEmpty(&opStack) && gettop(&opStack) != '(') {
                postfix[j++] = pop(&opStack);
                postfix[j++] = ' ';
            }
            if (!isEmpty(&opStack) && gettop(&opStack) == '(') {
                pop(&opStack); // 弹出左括号
            }
            i++;
            continue;
        }

        // 如果是运算符
        if (isOperator(c)) {
            // 弹出优先级更高或相等的运算符
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

    // 弹出栈中所有剩余运算符
    while (!isEmpty(&opStack)) {
        postfix[j++] = pop(&opStack);
        postfix[j++] = ' ';
    }

    postfix[j] = '\0'; // 添加字符串结束符
}

// 计算后缀表达式
int evaluatePostfix(char* postfix)
{
    Stack numStack; // 操作数栈
    initStack(&numStack);
    int i = 0;
    char c;

    while (postfix[i] != '\0') {
        c = postfix[i];

        // 跳过空格
        if (c == ' ') {
            i++;
            continue;
        }

        // 如果是数字，解析整个数字并入栈
        if (safeIsDigit(c)) {
            int num = 0;
            while (safeIsDigit(postfix[i])) {
                num = num * 10 + (postfix[i] - '0');
                i++;
            }
            push(&numStack, num);
            continue;
        }

        // 如果是运算符，弹出两个操作数进行计算
        if (isOperator(c)) {
            if (isEmpty(&numStack)) {
                return -1; // 错误：表达式不合法
            }
            int b = pop(&numStack);
            if (isEmpty(&numStack)) {
                return -1; // 错误：表达式不合法
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
                    return -1; // 错误：除数不能为零
                }
                result = a / b;
                break;
            default:
                return -1; // 错误：未知运算符
            }

            push(&numStack, result);
            i++;
            continue;
        }

        i++; // 处理下一个字符
    }

    if (isEmpty(&numStack)) {
        return -1; // 错误：表达式为空
    }

    int result = pop(&numStack);

    // 检查栈中是否还有多余的操作数
    if (!isEmpty(&numStack)) {
        return -1; // 错误：表达式不完整
    }

    return result;
}

// GTK
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
    gtk_label_set_text(result_label, "-");
    gtk_label_set_text(postfix_label, "-");
}

int main(int argc, char* argv[])
{
    GtkWidget* window;
    GtkWidget* vbox;
    GtkWidget* hbox;
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

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "算术表达式求值器");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    title_label = gtk_label_new("算术表达式求值器");
    gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 0);

    // 输入输入
    input_label = gtk_label_new("请输入表达式:");
    gtk_box_pack_start(GTK_BOX(vbox), input_label, FALSE, FALSE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    // 按钮盒子
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    calculate_btn = gtk_button_new_with_label("计算");
    clear_btn = gtk_button_new_with_label("清空");

    gtk_box_pack_start(GTK_BOX(hbox), calculate_btn, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), clear_btn, TRUE, TRUE, 0);

    // 创建后缀表达式显示区域
    postfix_title_label = gtk_label_new("后缀表达式:");
    gtk_box_pack_start(GTK_BOX(vbox), postfix_title_label, FALSE, FALSE, 0);

    postfix_label = gtk_label_new("-");
    gtk_label_set_line_wrap(GTK_LABEL(postfix_label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(postfix_label), TRUE);

    // 边框
    GtkWidget* postfix_frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(postfix_frame), postfix_label);
    gtk_box_pack_start(GTK_BOX(vbox), postfix_frame, FALSE, FALSE, 0);

    // 结果显示区域
    result_title_label = gtk_label_new("计算结果:");
    gtk_box_pack_start(GTK_BOX(vbox), result_title_label, FALSE, FALSE, 0);

    result_label = gtk_label_new("-");
    gtk_label_set_selectable(GTK_LABEL(result_label), TRUE);

    GtkWidget* result_frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(result_frame), result_label);
    gtk_box_pack_start(GTK_BOX(vbox), result_frame, FALSE, FALSE, 0);

    // 存储控件指针
    GtkWidget* widgets[3] = { entry, result_label, postfix_label };

    // 连接信号
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(calculate_btn, "clicked", G_CALLBACK(on_calculate_clicked), widgets);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_clear_clicked), widgets);

    // 显示所有widgets
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
