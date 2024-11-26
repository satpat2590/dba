#include <stdio.h> 
#include <sqlite3.h> 

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    printf("\n");
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc; 
    const char *sql; 

    // Open database connection
    rc = sqlite3_open("test.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    sql = "CREATE TABLE IF NOT EXISTS TASKS(" \
          "ID INT PRIMARY KEY NOT NULL," \
          "NAME TEXT NOT NULL," \
          "DURATION INT," \
          "CATEGORY CHAR(50)," \
          "POINTS INT NOT NULL );";

    rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    sql = "INSERT INTO TASKS (ID, NAME, DURATION, CATEGORY, POINTS) " \
          "VALUES (2, 'Creating C interface for database', 5, 'Satya', 7);";

    rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stdout, "Task created successfully.");
    }

    sql = "SELECT * FROM TASKS";
    rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stdout, "Operation done successfully.\n");
    }

    sqlite3_close(db);
    return 0;
}


