
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE 100
#define NAME_LEN 20
#define ID_LEN 15
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

void initList(List* L) { L->len = 0; }
int full(List* L) { return L->len == MAXSIZE; }
int empty(List* L) { return L->len == 0; }

int findById(List* L, char* id)
{
    for (int i = 0; i < L->len; i++)
        if (strcmp(L->data[i].id, id) == 0)
            return i;
    return -1;
}
void addStudent(List* L)
{
    if (full(L)) {
        printf("顺序表已满！\n");
        return;
    }
    Student s;
    printf("输入学号：");
    scanf("%s", s.id);
    if (findById(L, s.id) != -1) {
        printf("学号已存在！\n");
        return;
    }
    printf("输入姓名：");
    scanf("%s", s.name);
    printf("语文 数学 英语 成绩：");
    scanf("%f%f%f", &s.score[0], &s.score[1], &s.score[2]);

    L->data[L->len++] = s;

    for (int k = 0; k < 3; k++) {
        Node* p = (Node*)malloc(sizeof(Node));
        if (p == NULL) {
            printf("内存分配失败！\n");
            return;
        }
        p->index = L->len - 1;
        p->next = subjHead[k];
        subjHead[k] = p;
    }
    printf("添加成功！\n");
}
void delStudent(List* L)
{
    char id[15];
    printf("输入要删除的学号：");
    scanf("%s", id);
    int pos = findById(L, id);
    if (pos == -1) {
        printf("学号不存在！\n");
        return;
    }

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
    puts("删除成功！");
}
void searchStudent(List* L)
{
    char id[15];
    printf("输入要查询的学号：");
    scanf("%s", id);
    int pos = findById(L, id);
    if (pos == -1) {
        printf("学号不存在！\n");
        return;
    }
    Student* s = &L->data[pos];
    printf("学号：%s  姓名：%s\n", s->id, s->name);
    printf("语文：%.1f  数学：%.1f  英语：%.1f\n",
        s->score[0], s->score[1], s->score[2]);
}

void insertStudent(List* L)
{
    if (full(L)) {
        printf("顺序表已满！\n");
        return;
    }
    int pos;
    printf("插入到哪个位置(1~%d)：", L->len + 1);
    scanf("%d", &pos);
    if (pos < 1 || pos > L->len + 1) {
        printf("请重新输入！\n");
        return;
    }
    pos--;

    Student s;
    printf("输入学号：");
    scanf("%s", s.id);
    if (findById(L, s.id) != -1) {
        printf("学号已存在！\n");
        return;
    }
    printf("输入姓名：");
    scanf("%s", s.name);
    printf("语文 数学 英语 成绩：");
    scanf("%f%f%f", &s.score[0], &s.score[1], &s.score[2]);

    for (int i = L->len; i > pos; i--)
        L->data[i] = L->data[i - 1];
    L->data[pos] = s;
    L->len++;

    for (int k = 0; k < 3; k++) {
        Node* p = (Node*)malloc(sizeof(Node));
        if (p == NULL) {
            printf("内存分配失败！\n");
            return;
        }
        p->index = pos;
        p->next = subjHead[k];
        subjHead[k] = p;
    }
    printf("插入成功！\n");
}

void print(List* L, int k)
{
    const char* name[3] = { "语文", "数学", "英语" };
    printf("\n=== 按 %s 成绩链表 ===\n", name[k]);
    Node* p = subjHead[k];
    if (!p) {
        puts("暂无学生");
        return;
    }
    printf("学号\t\t姓名\t\t%s成绩\n", name[k]);
    printf("--------------------------------\n");
    while (p) {
        Student* s = &L->data[p->index];
        printf("%s\t\t%s\t\t%.1f\n", s->id, s->name, s->score[k]);
        p = p->next;
    }
}
void displayAllStudents(List* L)
{
    if (empty(L)) {
        puts("暂无学生信息！");
        return;
    }
    printf("\n=== 所有学生信息 ===\n");
    printf("学号\t\t姓名\t\t语文\t数学\t英语\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < L->len; i++) {
        Student* s = &L->data[i];
        printf("%s\t\t%s\t\t%.1f\t%.1f\t%.1f\n",
            s->id, s->name, s->score[0], s->score[1], s->score[2]);
    }
}
void menu()
{
    puts("\n===== 学生成绩管理 =====");
    puts("1. 登记学生");
    puts("2. 查询学生");
    puts("3. 插入学生");
    puts("4. 删除学生");
    puts("5. 显示所有学生");
    puts("6. 显示语文成绩链表");
    puts("7. 显示数学成绩链表");
    puts("8. 显示英语成绩链表");
    puts("0. 退出");
    printf("请选择：");
}

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

int main()
{
    List L;
    initList(&L);
    int choice;
    do {
        menu();
        if (scanf("%d", &choice) != 1) {
            printf("输入错误，请输入数字！\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        switch (choice) {
        case 1:
            addStudent(&L);
            break;
        case 2:
            searchStudent(&L);
            break;
        case 3:
            insertStudent(&L);
            break;
        case 4:
            delStudent(&L);
            break;
        case 5:
            displayAllStudents(&L);
            break;
        case 6:
            print(&L, 0);
            break;
        case 7:
            print(&L, 1);
            break;
        case 8:
            print(&L, 2);
            break;
        case 0:
            puts("再见！");
            break;
        default:
            puts("输入错误，重试！");
        }
    } while (choice != 0);

    freeAllLists();
    return 0;
}
