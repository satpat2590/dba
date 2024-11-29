#include <stdio.h> 
#include <sqlite3.h> 
#include <stdlib.h>

typedef struct {
    char **columns;
    int numColumns;
} Row;

typedef struct {
    Row *rows;
    int count;
    int capacity; 
} ResultantSet;

void print(char *text) {
    fprintf(stdout, "\n%s", text);
}

void print_set(ResultantSet *sql_data) {
    fprintf(stdout, "Number of elements in sql_data: %d\n", sql_data->count);
    fprintf(stdout, "Capacity of the sql_data struct: %d\n", sql_data->capacity);

    for (int o = 0; o < sql_data->count; o++) { // Outer loop on the rows in resultant set
        printf("\n---------Row %d---------", o);
        for (int i = 0; i < sql_data->rows[o].numColumns; i++) { // Inner loop on the different columns in row
            printf("\n%s", sql_data->rows[o].columns[i]);
        }
    }

    printf("\n");
}

void print_row(Row* row) {
    printf("\n(");
    printf("%s", row->columns[0]);
    for (int i = 1; i < row->numColumns; i++) {
        printf(", %s", row->columns[i]);
    }
    printf(")\n");
}

// Runs for each row of output data returned from a SQL query
int callback(void *data, int argc, char **argv, char **azColName) {
    ResultantSet *result = (ResultantSet *)data; // Type casting the parameter to be of type ResultantSet 
    
    // If the number of elements in the resultant set is greater than or equal to the max capacity, then... 
    if (result->count >= result->capacity) { 
        result->capacity *= 2; // Double our max capacity 
        result->rows = realloc(result->rows, result->capacity * sizeof(Row)); // Reallocate memory to account for new capacity
    }

    // Process and store the row in the Row struct
    Row row;
    row.numColumns = argc; 
    row.columns = malloc(argc * sizeof(char*));

    for (int i = 0; i < argc; i++) {
        row.columns[i] = strdup(argv[i] ? argv[i] : "NULL");
    }

    // Store the completed Row struct as an index in the 'rows' field of ResultantSet struct 
    result->rows[result->count] = row; 
    result->count++; 

    print_row(&row); 

    return 0; 
}

ResultantSet *query(sqlite3 *db, const char *sql, char **errMsg) {    
    fprintf(stdout, "\n--------Executing: %s--------\n", sql);

    ResultantSet *results = malloc(sizeof(ResultantSet)); // Creating a struct to store query results
    results->rows = malloc(10 * sizeof(Row)); // Initial capacity of the rows stored in the ResultantSet struct
    results->count = 0; 
    results->capacity = 10; // How many rows are able to be stored? 

    int rc; 
    rc = sqlite3_exec(db, sql, callback, results, errMsg);

    if (rc != SQLITE_OK) { // FAILURE CASE
        fprintf(stderr, "\nSQL Error: %s\n", *errMsg);
        sqlite3_free(*errMsg);

        // Free everything
        for (int o = 0; o < results->count; o++) {
            for (int i = 0; i < results->rows->numColumns; i++) {
                free(results->rows[o].columns[i]); // Free individual columns of each row
            }
            free(results->rows[o].columns); // Free the char ** in Rows struct
        }
        free(results->rows); // Free the Rows struct from ResultantSet struct
        free(results); // Free the ResultantSet struct

        return NULL;
    } else { // SUCCESS CASE
        fprintf(stdout, "\nQuery executed successfully\n");
        return results;
    }
}

int main() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc; 
    const char *sql; 
    char *res = 0; 
    ResultantSet *results;
    ResultantSet *create_results;  

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
          "ID INT PRIMARY KEY NOT NULL," \
          "NAME TEXT NOT NULL," \
          "DURATION INT," \
          "CATEGORY CHAR(50)," \
          "POINTS INT NOT NULL );";
    create_results = query(db, sql, &errMsg);

    // Insert a row into the Tasks table
    //sql = "INSERT INTO TASKS (ID, NAME, DURATION, CATEGORY, POINTS) " \
          "VALUES (2, 'Creating C interface for database', 5, 'Satya', 7);";
    //res = query(db, sql, &errMsg);
    //printf(stdout, "%s", res);
    
    // Retrieve all rows from the Tasks table
    sql = "SELECT * FROM TASKS";
    results = query(db, sql, &errMsg);

    sqlite3_close(db);
    return 0;
}


