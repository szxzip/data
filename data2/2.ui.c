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
        return -1; // 满了
    }
    s->top++;
    s->data[s->top] = value;
    return 0;
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
    if (isEmpty(s)) {
        return -1; // 空的
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

int infixToPostfix(const char* infix, char* postfix, char* error_msg) // error_msg: GUI 错误报告
{
    Stack opStack; // 运算符栈
    initStack(&opStack);
    int i = 0, j = 0;
    char c;

    error_msg[0] = '\0';

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
            } else {
                strcpy(error_msg, "错误：括号不匹配！");
                return -1;
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

        // 如果遇到未知字符
        sprintf(error_msg, "错误：遇到未知字符 '%c'", c);
        return -1;
    }

    // 弹出栈中所有剩余运算符
    while (!isEmpty(&opStack)) {
        if (gettop(&opStack) == '(') {
            strcpy(error_msg, "错误：括号不匹配！");
            return -1;
        }
        postfix[j++] = pop(&opStack);
        postfix[j++] = ' ';
    }

    postfix[j] = '\0'; // 添加字符串结束符
    return 0;
}

// 计算后缀表达式
int evaluatePostfix(char* postfix, int* result, char* error_msg)
{
    Stack numStack; // 操作数栈
    initStack(&numStack);
    int i = 0;
    char c;

    error_msg[0] = '\0'; // 清空错误信息

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
                strcpy(error_msg, "错误：表达式不合法（操作数不足）！");
                return -1;
            }
            int b = pop(&numStack);
            if (isEmpty(&numStack)) {
                strcpy(error_msg, "错误：表达式不合法（操作数不足）！");
                return -1;
            }
            int a = pop(&numStack);
            int res;

            switch (c) {
            case '+':
                res = a + b;
                break;
            case '-':
                res = a - b;
                break;
            case '*':
                res = a * b;
                break;
            case '/':
                if (b == 0) {
                    strcpy(error_msg, "错误：除数不能为零！");
                    return -1;
                }
                res = a / b;
                break;
            default:
                strcpy(error_msg, "错误：未知运算符！");
                return -1;
            }
            push(&numStack, res);
            i++;
            continue;
        }
        i++; // 处理下一个字符
    }

    if (isEmpty(&numStack)) {
        strcpy(error_msg, "错误：表达式为空！");
        return -1;
    }

    *result = pop(&numStack);

    // 检查栈中是否还有多余的操作数
    if (!isEmpty(&numStack)) {
        strcpy(error_msg, "错误：表达式不完整（栈中-操作数过多）！");
        return -1;
    }

    return 0;
}

int isValidExpression(const char* string, char* error_msg)
{
    int len = strlen(string);
    int parenthesis = 0;
    error_msg[0] = '\0';

    for (int i = 0; i < len; i++) {
        char c = string[i];

        // 允许的字符：数字、运算符、括号、空格
        if (!safeIsDigit(c) && !isOperator(c) && c != '(' && c != ')' && c != ' ') {
            sprintf(error_msg, "错误：包含非法字符 '%c'", c);
            return -1;
        }

        // 检查括号匹配
        if (c == '(')
            parenthesis++;
        if (c == ')')
            parenthesis--;

        if (parenthesis < 0) {
            strcpy(error_msg, "错误：括号不匹配！");
            return -1;
        }
    }

    if (parenthesis != 0) {
        strcpy(error_msg, "错误：括号不匹配！");
        return -1;
    }

    return 0;
}

// 用于在回调函数中传递 Gtk 小部件
typedef struct {
    GtkEntry* entry;
    GtkLabel* postfix_label;
    GtkLabel* result_label;
} AppWidgets;

// “计算”按钮的点击事件处理程序
static void on_calculate_clicked(GtkButton* button, gpointer user_data)
{
    AppWidgets* widgets = (AppWidgets*)user_data;
    const char* infix_expression;
    char postfix_expression[MAX_SIZE * 2]; // 后缀表达式可能更长
    char error_message[MAX_SIZE];
    int result;

    // 1. 获取输入
    infix_expression = gtk_entry_get_text(widgets->entry);

    // 清空上次的结果
    gtk_label_set_text(widgets->postfix_label, "后缀表达式：");
    gtk_label_set_text(widgets->result_label, "计算结果：");

    // 2. 验证输入
    if (isValidExpression(infix_expression, error_message) != 0) {
        gtk_label_set_text(widgets->result_label, error_message);
        return;
    }

    // 3. 转换为后缀表达式
    if (infixToPostfix(infix_expression, postfix_expression, error_message) != 0) {
        gtk_label_set_text(widgets->result_label, error_message);
        return;
    }

    // 4. 显示后缀表达式
    char postfix_buffer[MAX_SIZE * 2 + 50];
    sprintf(postfix_buffer, "后缀表达式：%s", postfix_expression);
    gtk_label_set_text(widgets->postfix_label, postfix_buffer);

    // 5. 计算后缀表达式
    if (evaluatePostfix(postfix_expression, &result, error_message) != 0) {
        gtk_label_set_text(widgets->result_label, error_message);
        return;
    }

    // 6. 显示最终结果
    char result_buffer[MAX_SIZE];
    sprintf(result_buffer, "计算结果：%d", result);
    gtk_label_set_text(widgets->result_label, result_buffer);
}

// 激活 GTK 应用程序
static void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget* window;
    GtkWidget* vbox;
    GtkWidget* info_label;
    GtkWidget* button;

    // 为 Gtk 小部件分配内存
    AppWidgets* widgets = (AppWidgets*)g_malloc(sizeof(AppWidgets));

    // 创建主窗口
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "算术表达式求值器");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // 创建一个垂直布局容器
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 1. 信息标签
    info_label = gtk_label_new("请输入算术表达式（支持+、-、*、/和括号）：");
    gtk_box_pack_start(GTK_BOX(vbox), info_label, FALSE, FALSE, 0);

    // 2. 表达式输入框
    widgets->entry = GTK_ENTRY(gtk_entry_new());
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(widgets->entry), FALSE, FALSE, 0);

    // 3. 计算按钮
    button = gtk_button_new_with_label("计算");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    // 4. 后缀表达式标签
    widgets->postfix_label = GTK_LABEL(gtk_label_new("后缀表达式："));
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(widgets->postfix_label), FALSE, FALSE, 0);

    // 5. 结果标签
    widgets->result_label = GTK_LABEL(gtk_label_new("计算结果："));
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(widgets->result_label), FALSE, FALSE, 0);

    // 连接 "clicked" 信号到回调函数
    g_signal_connect(button, "clicked", G_CALLBACK(on_calculate_clicked), widgets);

    // 当窗口关闭时，释放 widgets 内存
    g_signal_connect(window, "destroy", G_CALLBACK(g_free), widgets);

    // 显示所有窗口和控件
    gtk_widget_show_all(window);
}

int main(int argc, char** argv)
{
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.example.calculator", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
