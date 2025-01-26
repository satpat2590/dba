// Author: Satyam Patel
// Date: 11/27/2024
// Goal: Practice interfacing with SQLite using C structs 

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <stdlib.h>
#include <string.h>
#include <../lib/cJSON.h>

typedef struct {
    int tid;
    char name[100];
    char description[255];
    int due_date;
    char status[20];
    char category[100];
    int points;
    char directory[200];
    int p_tid;
    int created_at;
} Task;


typedef struct {
    Task *tasks;
    int count;
    int capacity; 
} TaskList; 

Task create_task(char *name, int duration, char *category, int points);

TaskList *get_tasks(sqlite3 *db); 

int insert(sqlite3 *db, Task task);

int db_open(const char *db_path, sqlite3 **db);

int db_close(sqlite3 *db);

Task create_task(char *name, int duration, char *category, int points);

int create_task_with_json(cJSON *json, Task *ntask);

void print_task(Task* row);

void print_tasks(TaskList *tlist);