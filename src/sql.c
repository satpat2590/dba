// Author: Satyam Patel
// Date: 11/27/2024
// Goal: Practice interfacing with SQLite using C structs 

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <stdlib.h>
#include <string.h>

/*
ID: Unique code given to a task
DURATION: The amount of hours X it will take to finish task
CATEGORY: The category that this task belongs to
POINTS: Amount of points, X, that accomplishing task will grant
*/
typedef struct {
    int tid;
    char name[100];
    int duration;
    char category[100];
    int points;
    char directory[200];
    int p_tid;
} Task;

/*

CREATE TABLE IF NOT EXISTS TASKS(
    0 TID INTEGER PRIMARY KEY AUTOINCREMENT, 
    1 NAME TEXT NOT NULL, 
    2 DURATION INT,
    3 CATEGORY CHAR(50),
    4 POINTS INT NOT NULL,
    5 DIRECTORY TEXT,
    6 P_TID INT,
    FOREIGN KEY (P_TID) REFERENCES TASKS (TID)
);

*/

/*
TASKS: Array of Task objects 
COUNT: Number of Task objects in array
CAPACITY: Maximum number of Task objects able to be stored
*/
typedef struct {
    Task *tasks;
    int count;
    int capacity; 
} TaskList; 


void print_row(Task* row) {
    printf("\n(%d, %s, %d, %s, %d)", row->tid, row->name, row->duration, row->category, row->points);
}

void print_set(TaskList *sql_data) {
    fprintf(stdout, "\nNumber of elements in sql_data: %d\n", sql_data->count);
    fprintf(stdout, "Capacity of the sql_data struct: %d\n", sql_data->capacity);

    for (int o = 0; o < sql_data->count; o++) { // Outer loop on the rows in resultant set
        print_row(&sql_data->tasks[o]);
    }
    printf("\n");
}


// Runs for each row of output data returned from a SQL query
int tasks_callback(void *data, int argc, char **argv, char **azColName) {
    printf("\n[sql.c::tasks_callback()] - Looping through a SELECT query resultant set\n");
    for (int i = 0; i < argc; i++) {
        printf("\n%s\n", argv[i]);
    }

    TaskList *result = (TaskList *)data; // Type casting the parameter to be of type TaskList 
    
 // If the number of elements in the resultant set is greater than or equal to the max capacity, then... 
    if (result->count >= result->capacity) { 
        result->capacity *= 2; // Double our max capacity 
        printf("\n[sql.c::tasks_callback()] - Number of elements in TaskList is above capacity...\n\n...Increasing capacity by double from %d to %d\n", result->count, result->capacity);
        result->tasks = realloc(result->tasks, result->capacity * sizeof(Task)); // Reallocate memory to account for new capacity
        if (result->tasks == NULL) {
            fprintf(stderr, "[sql.c::tasks_callback()] - Memory allocation failed for Tasks struct\n");
            return 1; // Non-zero return in sqlite callback stops the query execution
        }
    }

 // Process and store the row in the Row struct
    Task row;
    // TID 
    printf("\n[sql.c::tasks_callback()] - Adding TID from SQL query to Task struct...\n");
    row.tid = argv[0] ? atoi(argv[0]) : 0;
    // NAME
    printf("\n[sql.c::tasks_callback()] - Adding NAME from SQL query to Task struct...\n");
    if (argv[1]) strncpy(row.name, argv[1], sizeof(row.name) - 1), row.name[sizeof(row.name) - 1] = '\0';
    else row.name[0] = '\0';
    // DURATION
    printf("\n[sql.c::tasks_callback()] - Adding DURATION from SQL query to Task struct...\n");
    row.duration = argv[2] ? atoi(argv[2]) : 0;
    // CATEGORY
    printf("\n[sql.c::tasks_callback()] - Adding CATEGORY from SQL query to Task struct...\n");
    if (argv[3]) strncpy(row.category, argv[3], sizeof(row.name) - 1), row.name[sizeof(row.name) - 1] = '\0';
    else row.name[0] = '\0';
    // POINTS
    printf("\n[sql.c::tasks_callback()] - Adding POINTS from SQL query to Task struct...\n");
    row.points = argv[4] ? atoi(argv[4]) : 0;
    // DIRECTORY
    printf("\n[sql.c::tasks_callback()] - Adding DIRECTORY from SQL query to Task struct...\n");
    if (argv[5]) strncpy(row.name, argv[5], sizeof(row.name) - 1), row.name[sizeof(row.name) - 1] = '\0';
    else row.name[0] = '\0';
    // P_TID
    printf("\n[sql.c::tasks_callback()] - Adding P_TID from SQL query to Task struct...\n");
    row.p_tid = argv[6] ? atoi(argv[6]) : -1;

 // Store the completed Task struct as an index in the 'tasks' field of Tasks struct 
    result->tasks[result->count] = row; 
    result->count++; 

    print_row(&row); 
    return 0; 
}

// Base callback for all "write" queries
int callback(void *data, int argc, char **argv, char **azColName) {    
    return 0; 
}

TaskList *get_tasks(sqlite3 *db, char **errMsg) { 
 // Check if the TASKS table exists in the database    
    sqlite3_stmt *stmt;
    const char *check_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='TASKS';";
    if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "Error: TASKS table does not exist.\n");
        sqlite3_finalize(stmt);
        return NULL;
    }
    sqlite3_finalize(stmt);

 // Check if the TASKS table is empty or not  
    const char *count_sql = "SELECT COUNT(*) FROM TASKS;";
    int count = 0;
    sqlite3_prepare_v2(db, count_sql, -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (count == 0) {
        fprintf(stdout, "No entries in TASKS table.\n");
        return NULL;
    }

 // If the TASKS table exists and the number of elements in table is above 0, then run SELECT query...
    const char *sql = "SELECT * FROM TASKS;";   
    fprintf(stdout, "\n--------%s--------\n", sql);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)) {
        fprintf(stderr, "\n[sql.c::get_tasks()] - Error in creating a prepared statement...\n");
    }

 // Memory allocation portion
    printf("\n[sql.c::get_tasks(db, errMsg)] - Entering memory allocation portion...\n");
    TaskList *results = malloc(sizeof(TaskList)); // Creating a struct to store query results
    if (results == NULL) {
        fprintf(stderr, "\n[sql.c::get_tasks()] - Memory allocation failed for TaskList\n");
        return NULL; 
    }

    results->tasks = malloc(10 * sizeof(Task)); // Initial capacity of the rows stored in the TaskList struct
    if (results->tasks == NULL) {
        fprintf(stderr, "\n[sql.c::get_tasks()] - Memory allocation failed for tasks array in TaskList\n");
    }

    results->count = 0; 
    results->capacity = 20; // How many rows are able to be stored? 

    int rc; 
    printf("\n[sql.c::get_tasks(db, errMsg)] - ...Executing query: %s\n", sql);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, 'Failed to prepare statement: %s\n', sqlite3_errmsg(db));
        free(results->tasks);
        free(results);
        return NULL;
    }

 // Loop through each row of the resultant set
    while((rc == sqlite3_step(stmt)) == SQLITE_ROW) {
     // Expand the array if needed
        if (results->count >= results->capacity) {
            results->capacity *= 2;
            results->tasks = realloc(results->tasks, results->capacity * sizeof(Task));
            if (results->tasks == NULL) {
                fprintf(stderr, "\n[sql.c::get_tasks()] - Reallocation of results->tasks FAILED...\n");
                sqlite3_finalize(stmt);
                free(results->tasks);
                free(results);
                return NULL;
            }
        }
     // Populate a new Task object
        Task *task = &results->tasks[results->count++];
        
        task->tid = sqlite3_column_int(stmt, 0);

        //if (argv[1]) strncpy(row.name, argv[1], sizeof(row.name) - 1), row.name[sizeof(row.name) - 1] = '\0';
        //else row.name[0] = '\0';

        task->duration = sqlite3_column_int(stmt, 2);
        
        
        //task->category = strdup((const char *)sqlite3_column_text(stmt, 3));


    }
    rc = sqlite3_exec(db, sql, tasks_callback, results, errMsg);

    if (rc != SQLITE_OK) { // FAILURE CASE
        fprintf(stderr, "\nSQL Error: %s\n", *errMsg);
        sqlite3_free(*errMsg);

     // Free everything
        for (int o = 0; o < results->count; o++) { // Free each row
            free(&results->tasks[o].tid);
            free(results->tasks[o].name);
            free(&results->tasks[o].duration);
            free(results->tasks[o].category);                                                            
            free(&results->tasks[o].points);
            free(results->tasks[o].directory);
            free(&results->tasks[o].p_tid);
        }
        free(results->tasks); // Free the array of Task structs from Tasks struct
        free(results); // Free the Tasks struct

        return NULL;
    } else { // SUCCESS CASE
        fprintf(stdout, "\nQuery executed successfully.\n");
        return results;
    }
}

TaskList *query_tasks(sqlite3 *db, const char *sql, char **errMsg) {    
    fprintf(stdout, "\n--------%s--------\n", sql);

    TaskList *results = malloc(sizeof(TaskList)); // Creating a struct to store query results
    results->tasks = malloc(10 * sizeof(Task)); // Initial capacity of the rows stored in the ResultantSet struct
    results->count = 0; 
    results->capacity = 50; // How many rows are able to be stored? 

    int rc; 
    rc = sqlite3_exec(db, sql, tasks_callback, results, errMsg);

    if (rc != SQLITE_OK) { // FAILURE CASE
        fprintf(stderr, "\nSQL Error: %s\n", *errMsg);
        sqlite3_free(*errMsg);
     // Free everything
        for (int o = 0; o < results->count; o++) { // Free each row
            free(&results->tasks[o].tid);
            free(results->tasks[o].name);
            free(&results->tasks[o].duration);
            free(results->tasks[o].category);
            free(&results->tasks[o].points);
        }
        free(results->tasks); // Free the array of Task structs from Tasks struct
        free(results); // Free the Tasks struct
        return NULL;

    } else { // SUCCESS CASE
        fprintf(stdout, "\nQuery executed successfully.\n");
        return results;
    }
}

int query(sqlite3 *db, const char *sql, char **errMsg) {
    fprintf(stdout, "\n--------%s--------\n", sql);

    char *data = "";
    int rc; 
    rc = sqlite3_exec(db, sql, callback, (void*)data, errMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "\n[SQL.C] Error while querying: %s\n", *errMsg);
        sqlite3_free(*errMsg);
    } else {
        fprintf(stdout, "\nQuery executed successfully.\n");
    }
    return rc; 
}

/*
Insert a Task object into the Task table within the SQLite DB. 

No two tasks can have the same ID or Names. 
*/
int insert(sqlite3 *db, Task task, char **errMsg) {
    sqlite3_stmt *stmt; 
    const char *sql = "INSERT INTO TASKS (NAME, DURATION, CATEGORY, POINTS) VALUES (?, ?, ?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) { // FAILURE CASE
        fprintf(stderr, "\n[SQL.C] Failed to Prepare Statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

 // Bind the values
    // sqlite3_bind_text arguments: 
    // 1. Statement handle
    // 2. Parameter index (1-based)
    // 3. Text to bind
    // 4. Length of text (-1 means strlen)
    // 5. Destructor (SQLITE_STATIC if string will remain valid)
    sqlite3_bind_text(stmt, 1, task.name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, task.duration);
    sqlite3_bind_text(stmt, 3, task.category, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, task.points);

    fprintf(stdout, "\n--------%s--------\n", sql);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "\n[SQL.C] Task Insert Failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    fprintf(stdout, "\nQuery executed successfully.\n");
    return SQLITE_OK;
}

// Instantiate a sqlite3 object using a predefined database file path
int db_open(const char *db_path, sqlite3 **db) {
    return sqlite3_open(db_path, db);
}

// Close the sqlite3 object by de-allocating memory
int db_close(sqlite3 *db) {
    return sqlite3_close(db);
}

// Create a new Task object 
Task create_task(char *name, int duration, char *category, int points, int p_tid) {
    Task new_task;
 // Copy 'name' and 'category' into the const char * attributes of new_task
    strncpy(new_task.name, name, sizeof(new_task.name) - 1), new_task.name[sizeof(new_task.name) - 1] = '\0';
    strncpy(new_task.category, category, sizeof(new_task.category) - 1), new_task.category[sizeof(new_task.category) - 1] = '\0';
 // Assign duration and points as integers   
    new_task.duration = duration; 
    new_task.points = points;
    if (p_tid != -1) {
        new_task.p_tid = p_tid; 
    } else {
        new_task.p_tid = -1; 
    }

    return new_task;
}

