#ifndef STUDENT_DATA_H
#define STUDENT_DATA_H

#define MAX_NAME_LENGTH 50
#define MAX_SUBJECT_LENGTH 30
#define MAX_STUDENTS 1000
#define DATA_FILE "students.txt"

// 科目成绩节点
typedef struct SubjectScore {
    char subject[MAX_SUBJECT_LENGTH];
    float score;
    struct SubjectScore* next;
} SubjectScore;

// 学生信息结构
typedef struct Student {
    int id; // 学号
    char name[MAX_NAME_LENGTH]; // 姓名
    char major[50]; // 专业
    int grade; // 年级
    SubjectScore* subjects; // 科目成绩链表头指针
} Student;

// 全局学生数组
extern Student students[];
extern int student_count;

#endif
