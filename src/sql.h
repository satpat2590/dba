// Author: Satyam Patel
// Date: 11/27/2024
// Goal: Practice interfacing with SQLite using C structs 

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[100];
    int duration;
    char category[100];
    int points;
} Task;

typedef struct {
    Task *tasks;
    int count;
    int capacity; 
} TaskList; 

Task create_task(char *name, int duration, char *category, int points);

TaskList *get_tasks(sqlite3 *db, char **errMsg); 

TaskList *query_tasks(sqlite3 *db, const char *sql, char **errMsg);

int query(sqlite3 *db, const char *sql, char **errMsg);

int insert(sqlite3 *db, Task task, char **errMsg);

int db_open(const char *db_path, sqlite3 **db);

int db_close(sqlite3 *db);

Task create_task(char *name, int duration, char *category, int points);