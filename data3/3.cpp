
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- 二叉树结点定义 ---------- */
typedef struct Node {
    char op; // 运算符或 '#' 表示操作数
    int val; // 当 op=='#' 时存放数值
    struct Node *left, *right;
} Node;

typedef struct {
    Node* data[1000];
    int top;
} Stack;

void stack_init(Stack* s) { s->top = -1; }
int stack_empty(Stack* s) { return s->top == -1; }
void stack_push(Stack* s, Node* p) { s->data[++s->top] = p; }
Node* stack_pop(Stack* s) { return stack_empty(s) ? NULL : s->data[s->top--]; }
Node* stack_top(Stack* s) { return stack_empty(s) ? NULL : s->data[s->top]; }

Node* new_node(char op, int val)
{
    Node* p = (Node*)malloc(sizeof(Node));
    p->op = op;
    p->val = val;
    p->left = p->right = NULL;
    return p;
}

int priority(char op)
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

/* ---------- 中缀表达式 → 二叉表达式树 ---------- */
Node* build_expr_tree(const char* expr)
{
    Stack opnd, optr; // 操作数栈、运算符栈
    stack_init(&opnd);
    stack_init(&optr);

    int i = 0, len = strlen(expr);
    while (i < len) {
        if (isspace(expr[i])) { // 跳过空白
            ++i;
            continue;
        }

        if (isdigit(expr[i])) { // 读取整数
            int v = 0;
            while (i < len && isdigit(expr[i]))
                v = v * 10 + (expr[i++] - '0');
            stack_push(&opnd, new_node('#', v));
        } else if (expr[i] == '(') { // 左括号直接入栈
            stack_push(&optr, new_node(expr[i], 0));
            ++i;
        } else if (expr[i] == ')') { // 右括号：弹到 '('
            while (!stack_empty(&optr) && stack_top(&optr)->op != '(') {
                Node* r = stack_pop(&opnd);
                Node* l = stack_pop(&opnd);
                Node* op = stack_pop(&optr);
                op->left = l;
                op->right = r;
                stack_push(&opnd, op);
            }
            stack_pop(&optr); // 弹出 '('
            ++i;
        } else { // 运算符
            while (!stack_empty(&optr) && priority(stack_top(&optr)->op) >= priority(expr[i])) {
                Node* r = stack_pop(&opnd);
                Node* l = stack_pop(&opnd);
                Node* op = stack_pop(&optr);
                op->left = l;
                op->right = r;
                stack_push(&opnd, op);
            }
            stack_push(&optr, new_node(expr[i], 0));
            ++i;
        }
    }

    /* 剩余运算符全部弹出 */
    while (!stack_empty(&optr)) {
        Node* r = stack_pop(&opnd);
        Node* l = stack_pop(&opnd);
        Node* op = stack_pop(&optr);
        op->left = l;
        op->right = r;
        stack_push(&opnd, op);
    }

    return stack_empty(&opnd) ? NULL : stack_pop(&opnd);
}

/* ---------- 后序遍历求值 ---------- */
int eval(Node* root)
{
    if (!root)
        return 0;
    if (root->op == '#')
        return root->val;
    int lv = eval(root->left);
    int rv = eval(root->right);
    switch (root->op) {
    case '+':
        return lv + rv;
    case '-':
        return lv - rv;
    case '*':
        return lv * rv;
    case '/':
        if (rv == 0) {
            fprintf(stderr, "Error: divide by zero!\n");
            exit(1);
        }
        return lv / rv;
    default:
        fprintf(stderr, "Unknown operator %c\n", root->op);
        exit(1);
    }
}

/* ---------- 释放内存 ---------- */
void free_tree(Node* root)
{
    if (!root)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* ---------- 主函数 ---------- */
int main()
{
    char expr[1024];
    printf("Input expression: ");
    if (!fgets(expr, sizeof(expr), stdin))
        return 0;

    Node* root = build_expr_tree(expr);
    if (!root) {
        printf("Invalid expression!\n");
        return 0;
    }
    int result = eval(root);
    printf("Result = %d\n", result);
    free_tree(root);
    return 0;
}
