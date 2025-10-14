#ifndef DATA_MANAGEMENT_H
#define DATA_MANAGEMENT_H

#include "student_data.h"

// 函数声明
int load_data_from_file();
int save_data_to_file();
int add_student(int id, const char* name, const char* major, int grade);
int delete_student(int id);
Student* find_student(int id);
int add_subject_score(int id, const char* subject, float score);
int delete_subject_score(int id, const char* subject);
SubjectScore* find_subject_scores(int id);
void get_all_students(Student** result, int* count);
void cleanup_data();

#endif
