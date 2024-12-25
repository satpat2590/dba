// Author: Satyam Patel
// Date: 11/27/2024
// Goal: Practice interfacing with SQLite using C structs 

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <stdlib.h>
#include <string.h>
#include "src/sql.h"

int main() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc; 
    const char *sql; 
    char *res = 0; 
    TaskList *results;

 // Open database connection
    rc = sqlite3_open("test.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stdout, "\nOpened database successfully\n");
    }

 // Create Tasks table
    sql = "CREATE TABLE IF NOT EXISTS TASKS(" \
          "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
          "NAME TEXT UNIQUE NOT NULL," \
          "DURATION DOUBLE," \
          "CATEGORY CHAR(50)," \
          "POINTS INT NOT NULL );";
    query(db, sql, &errMsg);


 // Insert a row into the Tasks table
    Task new_task = create_task("Clean the apartment", 1, "Lifestyle", 5);
    insert(db, new_task, &errMsg);
    
 // Retrieve all rows from the Tasks table
    results = get_tasks(db, &errMsg);

    sqlite3_close(db);
    return 0;
}


