#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAX_SIZE 100
typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;
void initStack(Stack* s) {
    s->top = -1;
}
int isEmpty(Stack* s) {
    if (s->top == -1) {
        printf("空的\n");
        return 1;
    }
}
int push(Stack* s, int value) {
    if (s->top >= MAX_SIZE - 1) {
        printf("满了\n");
        return  0;
    }
    s->top++;
    s->data[s->top] = value;
    return  1;
}
int pop(Stack* s) {
    if (isEmpty(s)) {
        printf("栈为空！\n");
        return -1;
    }
    return s->data[(s->top)--];
}
int gettop(Stack* s) {
    if (s->top == -1) {
        printf("空的\n");
        return 0;
   }
    return  s->data[s->top];
    return  1;
}
int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}
int getPriority(char op) {
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
int safeIsDigit(char c) {
    // 将字符转换为无符号字符以避免负数问题
    return isdigit((unsigned char)c);
}
void infixToPostfix(char* infix, char* postfix) {
    Stack opStack;  // 运算符栈
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
            postfix[j++] = ' ';  // 用空格分隔数字
            continue;
        }

        // 如果是左括号，直接入栈
        if (c== '(') {
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
                pop(&opStack);  // 弹出左括号
            }
            else {
                printf("错误：括号不匹配！\n");
                return;
            }
            i++;
            continue;
        }

        // 如果是运算符
        if (isOperator(c)) {
            // 弹出优先级更高或相等的运算符
            while (!isEmpty(&opStack) &&
                getPriority(gettop(&opStack)) >= getPriority(c) &&
                gettop(&opStack) != '(') {
                postfix[j++] = pop(&opStack);
                postfix[j++] = ' ';
            }
            push(&opStack, c);
            i++;
            continue;
        }

        // 如果遇到未知字符
        printf("错误：遇到未知字符 '%c'\n", c);
        i++;
    }

    // 弹出栈中所有剩余运算符
    while (!isEmpty(&opStack)) {
        postfix[j++] = pop(&opStack);
        postfix[j++] = ' ';
    }

    postfix[j] = '\0';  // 添加字符串结束符
}

// 计算后缀表达式
int evaluatePostfix(char* postfix) {
    Stack numStack;  // 操作数栈
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
                printf("错误：表达式不合法！\n");
                return -1;
            }
            int b = pop(&numStack);
            if (isEmpty(&numStack)) {
                printf("错误：表达式不合法！\n");
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
                    printf("错误：除数不能为零！\n");
                    return -1;
                }
                result = a / b;
                break;
            default:
                printf("错误：未知运算符！\n");
                return -1;
            }

            push(&numStack, result);
            i++;
            continue;
        }

        i++;  // 处理下一个字符
    }

    if (isEmpty(&numStack)) {
        printf("错误：表达式为空！\n");
        return -1;
    }

    int result = pop(&numStack);

    // 检查栈中是否还有多余的操作数
    if (!isEmpty(&numStack)) {
        printf("错误：表达式不完整！\n");
        return -1;
    }

    return result;
}

// 输入验证函数
int isValidExpression(char* string) {
    int len = strlen(string);
    int parenthesis = 0;

    for (int i = 0; i < len; i++) {
        char c = string[i];

        // 允许的字符：数字、运算符、括号、空格
        if (!safeIsDigit(c) && !isOperator(c) &&
            c != '(' && c != ')' && c != ' ') {
            printf("错误：包含非法字符 '%c'\n", c);
            return 0;
        }

        // 检查括号匹配
        if (c == '(') parenthesis++;
        if (c == ')') parenthesis--;

        if (parenthesis < 0) {
            printf("错误：括号不匹配！\n");
            return 0;
        }
    }

    if (parenthesis != 0) {
        printf("错误：括号不匹配！\n");
        return 0;
    }

    return 1;
}

// 主函数
int main() {
    char infix[MAX_SIZE];
    char postfix[MAX_SIZE * 2];  // 后缀表达式可能更长

    printf("算术表达式求值程序\n");
    printf("==================\n");
    printf("请输入算术表达式（支持+、-、*、/和括号）：\n");

    // 读取输入
    if (fgets(infix, sizeof(infix), stdin) == NULL) {
        printf("读取输入失败！\n");
        return 1;
    }

    // 移除换行符
    infix[strcspn(infix, "\n")] = 0;

    // 验证输入表达式
    if (!isValidExpression(infix)) {
        return 1;
    }

    printf("\n输入的中缀表达式：%s\n", infix);

    // 转换为后缀表达式
    infixToPostfix(infix, postfix);
    printf("转换的后缀表达式：%s\n", postfix);

    // 计算后缀表达式
    int result = evaluatePostfix(postfix);
    if (result != -1) {
        printf("计算结果：%d\n", result);
    }

    return 0;
}