#include "data_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Student students[MAX_STUDENTS];
int student_count = 0;

int load_data_from_file()
{
    FILE* file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        // 文件不存在，创建新文件
        file = fopen(DATA_FILE, "w");
        if (file != NULL) {
            fclose(file);
        }
        student_count = 0;
        return 0;
    }

    student_count = 0;
    char line[1024];

    while (fgets(line, sizeof(line), file) && student_count < MAX_STUDENTS) {
        // 解析学生基本信息
        int id, grade;
        char name[MAX_NAME_LENGTH], major[50];
        char subjects_str[512];

        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%511[^\n]",
                &id, name, major, &grade, subjects_str)
            == 5) {

            Student* student = &students[student_count];
            student->id = id;
            strcpy(student->name, name);
            strcpy(student->major, major);
            student->grade = grade;
            student->subjects = NULL;

            // 解析科目成绩
            char* token = strtok(subjects_str, ";");
            while (token != NULL) {
                char subject[MAX_SUBJECT_LENGTH];
                float score;

                if (sscanf(token, "%29[^:]:%f", subject, &score) == 2) {
                    SubjectScore* new_score = (SubjectScore*)malloc(sizeof(SubjectScore));
                    if (new_score != NULL) {
                        strcpy(new_score->subject, subject);
                        new_score->score = score;
                        new_score->next = student->subjects;
                        student->subjects = new_score;
                    }
                }
                token = strtok(NULL, ";");
            }

            student_count++;
        }
    }

    fclose(file);
    return student_count;
}

int save_data_to_file()
{
    FILE* file = fopen(DATA_FILE, "w");
    if (file == NULL) {
        return -1;
    }

    for (int i = 0; i < student_count; i++) {
        Student* student = &students[i];

        // 写入学生基本信息
        fprintf(file, "%d,%s,%s,%d,", student->id, student->name, student->major, student->grade);

        // 写入科目成绩
        SubjectScore* subject = student->subjects;
        int first = 1;
        while (subject != NULL) {
            if (!first) {
                fprintf(file, ";");
            }
            fprintf(file, "%s:%.1f", subject->subject, subject->score);
            subject = subject->next;
            first = 0;
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

int add_student(int id, const char* name, const char* major, int grade)
{
    if (student_count >= MAX_STUDENTS) {
        return -1; // 容量已满
    }

    // 检查学号是否已存在
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
            return -2; // 学号已存在
        }
    }

    Student* new_student = &students[student_count];
    new_student->id = id;
    strcpy(new_student->name, name);
    strcpy(new_student->major, major);
    new_student->grade = grade;
    new_student->subjects = NULL;

    student_count++;

    // 保存到文件
    save_data_to_file();
    return 0; // 成功
}

int delete_student(int id)
{
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
            // 释放科目链表内存
            SubjectScore* current = students[i].subjects;
            while (current != NULL) {
                SubjectScore* temp = current;
                current = current->next;
                free(temp);
            }

            // 移动后续元素
            for (int j = i; j < student_count - 1; j++) {
                students[j] = students[j + 1];
            }

            student_count--;

            // 保存到文件
            save_data_to_file();
            return 0; // 成功
        }
    }
    return -1; // 未找到
}

Student* find_student(int id)
{
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
            return &students[i];
        }
    }
    return NULL;
}

int add_subject_score(int id, const char* subject, float score)
{
    Student* student = find_student(id);
    if (student == NULL) {
        return -1; // 学生不存在
    }

    SubjectScore* new_score = (SubjectScore*)malloc(sizeof(SubjectScore));
    if (new_score == NULL) {
        return -2; // 内存分配失败
    }

    strcpy(new_score->subject, subject);
    new_score->score = score;
    new_score->next = student->subjects;
    student->subjects = new_score;

    // 保存到文件
    save_data_to_file();
    return 0; // 成功
}

int delete_subject_score(int id, const char* subject)
{
    Student* student = find_student(id);
    if (student == NULL) {
        return -1; // 学生不存在
    }

    SubjectScore** current = &(student->subjects);
    while (*current != NULL) {
        if (strcmp((*current)->subject, subject) == 0) {
            SubjectScore* temp = *current;
            *current = (*current)->next;
            free(temp);

            // 保存到文件
            save_data_to_file();
            return 0; // 成功
        }
        current = &((*current)->next);
    }

    return -2; // 科目未找到
}

SubjectScore* find_subject_scores(int id)
{
    Student* student = find_student(id);
    return (student != NULL) ? student->subjects : NULL;
}

void get_all_students(Student** result, int* count)
{
    *result = students;
    *count = student_count;
}

void cleanup_data()
{
    for (int i = 0; i < student_count; i++) {
        SubjectScore* current = students[i].subjects;
        while (current != NULL) {
            SubjectScore* temp = current;
            current = current->next;
            free(temp);
        }
    }
    student_count = 0;
}
