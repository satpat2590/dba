// Author: Satyam Patel
// Date: 11/27/2024
// Goal: Practice interfacing with SQLite using C structs 

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <stdlib.h>
#include <string.h>
#include <../lib/cJSON.h>
#include <time.h>


/*

CREATE TABLE IF NOT EXISTS TASKS(
    0 TID INTEGER PRIMARY KEY AUTOINCREMENT, 
    1 NAME TEXT NOT NULL, 
    2 p_tid INT,
    3 CATEGORY CHAR(50),
    4 POINTS INT NOT NULL,
    5 DIRECTORY TEXT,
    6 P_TID INT,
    FOREIGN KEY (P_TID) REFERENCES TASKS (TID)
);

*/

/*
    TID: Unique code given to a task
    NAME: The name of the task at hand
    DESCRIPTION: Lengthy explanation of what the task entails
    DUE DATE: When is this task due to be finished?
    CATEGORY: The category that this task belongs to
    POINTS: Amount of points, X, that accomplishing task will grant
    DIRECTORY: Location where all files pertaining to the task are stored
    P_TID: The parent task's TID
*/
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


void print_task(Task* row) {
    printf("\n(%d, %s, %s, %d, %s, %d, %s, %s, %d, %d)", row->tid, row->name, row->description, row->due_date, row->category, row->points, row->status, row->directory, row->p_tid, row->created_at);
}

/*
    Print a TaskList's tasks attribute.
*/
void print_tasks(TaskList *tlist) {
    //fprintf(stdout, "\nNumber of elements in sql_data: %d\n", tlist->count);
    //fprintf(stdout, "Capacity of the sql_data struct: %d\n", tlist->capacity);

    for (int o = 0; o < tlist->count; o++) { // Outer loop on the rows in resultant set
        print_task(&tlist->tasks[o]);
    }
    printf("\n");
}

/*
    Procedure:
    1. Check if the TASKS table exists in the SQLite database (taskm.db)
    2. Check if the TASKS table is not empty (should have at least one row)
    3. If both above pass, then allocate memory for a TaskList struct
    4. Run the 'SELECT * FROM TASKS' query 
    5. Step through each row of the executed statement
        a. Ensure Task struct is able to be filled by results
        b. For all char-based data types, ensure you free all allocated space
        c. Add Task struct onto TaskList.tasks Task array 
    6. Ensure that TaskList is allocated properly. Print out the memory mapping if possible.
*/

/*
    Retrieve all of the current tasks from the TASKS table.

    :param db: A pointer to a sqlite3 struct, used as an interface for database.
    :return: A pointer to a TaskList struct. 
*/
TaskList *get_tasks(sqlite3 *db) { 
 // QUERY if the TASKS table exists in the database    
    const char *check_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='TASKS';";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, check_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) { // Failed to prepare the statement
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

 // Step through the prepared statement. If it does not return a SQLITE_ROW return status, then it has failed to find TASK table. 
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "Error: TASKS table does not exist.\n");
        sqlite3_finalize(stmt);
        return NULL;
    }
    sqlite3_finalize(stmt); // Free the prepared statement

 // QUERY if the TASKS table is empty or not  
    const char *count_sql = "SELECT COUNT(*) FROM TASKS;";
    int count = 0;
    rc = sqlite3_prepare_v2(db, count_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) { // Failed to prepare the statement
        fprintf(stderr, "Error preparing count statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

 // Step through the prepared statement. The return should be an integer with number of rows in TASKS table. 
    if (sqlite3_step(stmt) == SQLITE_ROW) { 
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt); // Free the prepared statement

 // If the number of rows in the TASKS table is zero, then return NULL
    if (count == 0) {
        fprintf(stdout, "\n[sql.c::get_tasks()] - No entries in TASKS table.\n");
        return NULL;
    }

    printf("\n[sql.c::get_tasks()] - Number of tasks in the TASKS table: %d\n", count);

 // THERE ARE TASKS IN THE TABLE. QUICK, PUT YOUR THINKING CAP ON. IT'S TIME TO RAW DOG MEMORY MANAGEMENT
    TaskList *results = malloc(sizeof(TaskList)); // Creating a struct to store query results
    if (results == NULL) { // Malloc failure case
        fprintf(stderr, "\n[sql.c::get_tasks()] - Memory allocation failed for TaskList\n");
        return NULL; 
    }

    results->count = 0; 
    results->capacity = 20; // How many rows are able to be stored? 

    results->tasks = malloc(results->capacity * sizeof(Task)); // Initial capacity of the rows stored in the TaskList struct
    if (results->tasks == NULL) { // Malloc failure case
        fprintf(stderr, "\n[sql.c::get_tasks()] - Memory allocation failed for tasks array in TaskList\n");
        free(results);
        return NULL;
    }

 // If the TASKS table exists and the number of elements in table is above 0, then run SELECT query...
    const char *sql = "SELECT * FROM TASKS;";   

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) { // Failed to prepare statement
        fprintf(stderr, '\n[sql.c::get_tasks()] - Failed to prepare statement: %s\n', sqlite3_errmsg(db));
        free(results->tasks);
        free(results);
        return NULL;
    }

 // Loop through each row of the resultant set
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
     // Expand the array if needed
        if (results->count >= (results->capacity - 1)) {
            results->capacity *= 2;
            Task *new_tasks = realloc(results->tasks, results->capacity * sizeof(Task));
         // Failed to reallocate TaskList::tasks
            if (results->tasks == NULL) { 
                fprintf(stderr, "\n[sql.c::get_tasks()] - Reallocation of results->tasks FAILED...\n");
                sqlite3_finalize(stmt);
                free(results->tasks);
                free(results);
                return NULL;
            }
            results->tasks = new_tasks;
        }

     // Populate a new Task object
        Task *task = &results->tasks[results->count++];
        memset(task, 0, sizeof(Task)); // Zero out all fields before use

     // Retrieve all INT/FLOAT-BASED data types WHICH ARE NOT NULL
        task->tid = sqlite3_column_int(stmt, 0);
        task->due_date = sqlite3_column_int(stmt, 3); 
        task->points = sqlite3_column_int(stmt, 6);
        task->created_at = sqlite3_column_int(stmt, 9);

     // Retrieve all INT/FLOAT-BASED data types WHICH CAN BE NULL
        if (sqlite3_column_type(stmt, 6) == SQLITE_NULL) {
            task->p_tid = -1;
        } else {
            task->p_tid = sqlite3_column_int(stmt, 6);
        }

     // Retrieve all CHAR-BASED data types
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        if (name != NULL) {
            strncpy(task->name, name ? name : "", sizeof(task->name) - 1);
            task->name[sizeof(task->name) - 1] = '\0';
        } else {
            task->name[0] = '\0'; // Handle NULL case
        }

        const char *status = (const char *)sqlite3_column_text(stmt, 4);
        if (status != NULL) {
            strncpy(task->status, status ? status : "", sizeof(task->status) - 1);
            task->status[sizeof(task->status) - 1] = '\0';
        } else {
            task->status[0] = '\0';
        }

        const char *category = (const char *)sqlite3_column_text(stmt, 5);
        if (category != NULL) {
            strncpy(task->category, category ? category : "", sizeof(task->category) - 1);
            task->category[sizeof(task->category) - 1] = '\0';
        } else {
            task->category[0] = '\0';
        }

        const char *directory = (const char *)sqlite3_column_text(stmt, 7);
        if (directory != NULL) {
            strncpy(task->directory, directory ? directory : "", sizeof(task->directory) - 1);
            task->directory[sizeof(task->directory) - 1] = '\0';
        } else {
            task->directory[0] = '\0';
        }
    }

    if (rc != SQLITE_DONE) { // If query isn't fully executed, then print error message
        fprintf(stderr, "\n[sql.c::get_tasks()] - rc = %d; Error executing statement: %s\n", rc, sqlite3_errmsg(db));
        free(results->tasks);
        free(results);
        return NULL;
    }

    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) { // If the statement isn't fully finalized and cleaned up, then...
        fprintf(stderr, "\n[sql.c::get_tasks()] - rc = %d; Error finalizing prepared statement %s\n", rc, sqlite3_errmsg(db));
        free(results->tasks);
        free(results);
        return NULL;
    }

    return results;
}

/*
    TID: Unique code given to a task
    NAME: The name of the task at hand
    DESCRIPTION: Lengthy explanation of what the task entails
    DUE DATE: When is this task due to be finished?
    CATEGORY: The category that this task belongs to
    POINTS: Amount of points, X, that accomplishing task will grant
    DIRECTORY: Location where all files pertaining to the task are stored
    P_TID: The parent task's TID
*/

/*
    Insert a Task object into the Task table within the SQLite DB. 

    No two tasks can have the same ID or Names. 
*/
int insert(sqlite3 *db, Task task) {
    sqlite3_stmt *stmt; 
    const char *sql = "INSERT INTO TASKS (NAME, DESCRIPTION, DUE_DATE, CATEGORY, POINTS, DIRECTORY, P_TID) VALUES (?, ?, ?, ?, ?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) { // FAILURE CASE
        fprintf(stderr, "\n[sql.c::insert()] - Failed to Prepare Statement: %s\n", sqlite3_errmsg(db));
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
    sqlite3_bind_text(stmt, 2, task.description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, task.due_date);
    sqlite3_bind_text(stmt, 4, task.category, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, task.points);
    sqlite3_bind_text(stmt, 6, task.directory, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, task.p_tid);


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
    fprintf(stdout, "\nPOST /add_task - Insert executed successfully...\n");
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

/*
    TID: Unique code given to a task
    NAME: The name of the task at hand
    DESCRIPTION: Lengthy explanation of what the task entails
    DUE DATE: When is this task due to be finished?
    CATEGORY: The category that this task belongs to
    POINTS: Amount of points, X, that accomplishing task will grant
    DIRECTORY: Location where all files pertaining to the task are stored
    P_TID: The parent task's TID
*/

int create_task_with_json(cJSON *json, Task *ntask) {
    if (json == NULL || ntask == NULL) {
        fprintf(stderr, "\n[sql.c::create_task_with_json()] - Invalid input parameters.\n");
        return -1;
    }

    // Initialize the Task struct
    memset(ntask, 0, sizeof(Task));

    // Retrieve all numerical items from JSON
    cJSON *due_date = cJSON_GetObjectItemCaseSensitive(json, "due_date");
    if (cJSON_IsNumber(due_date)) {
        ntask->due_date = due_date->valueint;

        // Validate that the due_date is in the future
        time_t now = time(NULL);
        if (ntask->due_date <= now) {
            fprintf(stderr, "\n[sql.c::create_task_with_json()] - Due date must be in the future.\n");
            return -1;
        }
    } else {
        fprintf(stderr, "\n[sql.c::create_task_with_json()] - Due Date is not a valid integer. Task creation failed...\n");
        return -1;
    }

    cJSON *points = cJSON_GetObjectItemCaseSensitive(json, "points");
    if (cJSON_IsNumber(points)) {
        ntask->points = points->valueint;
    } else {
        fprintf(stderr, "\n[sql.c::create_task_with_json()] - Points is not a valid integer. Task creation failed...\n");
        return -1;
    }

    cJSON *p_tid = cJSON_GetObjectItemCaseSensitive(json, "p_tid");
    if (cJSON_IsNumber(p_tid)) {
        ntask->p_tid = p_tid->valueint;
    } else {
        ntask->p_tid = -1; // Default value for no parent task
    }

    // Retrieve all character-based items from JSON
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        strncpy(ntask->name, name->valuestring, sizeof(ntask->name) - 1);
        ntask->name[sizeof(ntask->name) - 1] = '\0'; // Null-terminate
    } else {
        fprintf(stderr, "\n[sql.c::create_task_with_json()] - Name is not a valid string. Task creation failed...\n");
        return -1;
    }

    cJSON *category = cJSON_GetObjectItemCaseSensitive(json, "category");
    if (cJSON_IsString(category) && (category->valuestring != NULL)) {
        strncpy(ntask->category, category->valuestring, sizeof(ntask->category) - 1);
        ntask->category[sizeof(ntask->category) - 1] = '\0'; // Null-terminate
    } else {
        fprintf(stderr, "\n[sql.c::create_task_with_json()] - Category is not a valid string. Task creation failed...\n");
        return -1;
    }

    cJSON *directory = cJSON_GetObjectItemCaseSensitive(json, "directory");
    if (cJSON_IsString(directory) && (directory->valuestring != NULL)) {
        strncpy(ntask->directory, directory->valuestring, sizeof(ntask->directory) - 1);
        ntask->directory[sizeof(ntask->directory) - 1] = '\0'; // Null-terminate
    } else {
        fprintf(stderr, "\n[sql.c::create_task_with_json()] - Directory is not a valid string. Task creation failed...\n");
        return -1;
    }

    cJSON *description = cJSON_GetObjectItemCaseSensitive(json, "description");
    if (cJSON_IsString(description) && (description->valuestring != NULL)) {
        strncpy(ntask->description, description->valuestring, sizeof(ntask->description) - 1);
        ntask->description[sizeof(ntask->description) - 1] = '\0'; // Null-terminate
    } else {
        ntask->description[0] = '\0'; // Default value for no description
    }

    // Handle the STATUS field (default to 'NOT_STARTED')
    cJSON *status = cJSON_GetObjectItemCaseSensitive(json, "status");
    if (cJSON_IsString(status) && (status->valuestring != NULL)) {
        strncpy(ntask->status, status->valuestring, sizeof(ntask->status) - 1);
        ntask->status[sizeof(ntask->status) - 1] = '\0'; // Null-terminate
    } else {
        strncpy(ntask->status, "NOT_STARTED", sizeof(ntask->status) - 1); // Default status
        ntask->status[sizeof(ntask->status) - 1] = '\0';
    }

    return 0; // Success
}

// Create a new Task object 
Task create_task(char *name, char *description, int due_date, char *status, char *category, int points, char *directory, int p_tid) {
    Task new_task;
    memset(&new_task, 0, sizeof(Task)); // Zero out all fields before use

    // Copy 'name', 'description', 'status', 'category', and 'directory' into the new_task
    if (name != NULL) {
        strncpy(new_task.name, name, sizeof(new_task.name) - 1);
        new_task.name[sizeof(new_task.name) - 1] = '\0';
    } else {
        new_task.name[0] = '\0'; // Handle NULL case
    }

    if (description != NULL) {
        strncpy(new_task.description, description, sizeof(new_task.description) - 1);
        new_task.description[sizeof(new_task.description) - 1] = '\0';
    } else {
        new_task.description[0] = '\0'; // Handle NULL case
    }

    if (status != NULL) {
        strncpy(new_task.status, status, sizeof(new_task.status) - 1);
        new_task.status[sizeof(new_task.status) - 1] = '\0';
    } else {
        new_task.status[0] = '\0'; // Handle NULL case
    }

    if (category != NULL) {
        strncpy(new_task.category, category, sizeof(new_task.category) - 1);
        new_task.category[sizeof(new_task.category) - 1] = '\0';
    } else {
        new_task.category[0] = '\0'; // Handle NULL case
    }

    if (directory != NULL) {
        strncpy(new_task.directory, directory, sizeof(new_task.directory) - 1);
        new_task.directory[sizeof(new_task.directory) - 1] = '\0';
    } else {
        new_task.directory[0] = '\0'; // Handle NULL case
    }

    // Assign numerical values
    new_task.due_date = due_date;
    new_task.points = points;
    new_task.p_tid = p_tid;

    return new_task;
}
