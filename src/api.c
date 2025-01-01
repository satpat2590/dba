// Author: Satyam Patel
// Date: 12/22/2024
// Description: Logic for handling API requests

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "src/sql.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <../lib/cJSON.h>


/*
    Internal struct to JSON functionality
*/
cJSON *tasks_to_json(TaskList *tasklist) {
    printf("\n[api.c::tasks_to_json()] - Looping through %d rows in TaskList & the capacity is %d\n", tasklist->count, tasklist->capacity);
    cJSON *json = cJSON_CreateObject();
    for (int i = 0; i < tasklist->count; i++) {
        cJSON *task = cJSON_CreateObject();

     // Retrieve the Task ID for the current row
        int tid = tasklist->tasks[i].id;
        char s_tid[10];
        sprintf(s_tid, "%d", tid);
        
     // Create the task to add to the response json variable
        cJSON_AddItemToObject(json, s_tid, task);
        cJSON_AddNumberToObject(task, "task_id", tasklist->tasks[i].id);
        cJSON_AddStringToObject(task, "task_name", tasklist->tasks[i].name);
        cJSON_AddNumberToObject(task, "duration", tasklist->tasks[i].duration);
        cJSON_AddStringToObject(task, "category", tasklist->tasks[i].category);
        cJSON_AddNumberToObject(task, "points", tasklist->tasks[i].points);
        //cJSON_AddNumberToObject(task, "p_tid", tasklist->tasks[i].p_tid);
    }
}



/*
GET /tasks - Get a list of all of the current tasks you are working on

POST /add_task - Add a new task with following body: 
    {
        "name": str
        "duration": int
        "p_tid": int
        "description": str
    }
*/





/*
    Take in the request and then parse it to understand...
        1. Is it a 'GET' or 'POST' request?
        2. If POST, what is the request body?
        3. Is the request endpoint valid?
*/
void handle_request(int client_socket, sqlite3 *db) {
    char buffer[2048] = {0};
    read(client_socket, buffer, sizeof(buffer) - 1);

    printf("Request received:\n%s\n", buffer);

    // Parse the HTTP method and route
    char method[8], route[256];
    sscanf(buffer, "%s %s", method, route);

    // Check for GET or POST
    if (strcmp(method, "GET") == 0) {
     // Handle the /tasks route
        if (strcmp(route, "/tasks") == 0) {
            route_get_tasks(client_socket, db);
        } else {
            send_response(client_socket, "404 Not Found", "text/plain", "Route not found");
        }
    } else if (strcmp(method, "POST") == 0) {
     // Handle the /add_task route
        if (strcmp(route, "/add_task") == 0) {
            // Find the body of the POST request
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                body += 4; // Skip the "\r\n\r\n"
                route_add_task(client_socket, body, db);
            } else {
                send_response(client_socket, "400 Bad Request", "text/plain", "Missing body");
            }
        } else {
            send_response(client_socket, "404 Not Found", "text/plain", "Route not found");
        }
    } else {
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Only GET and POST supported");
    }

    close(client_socket);
}

// Handler for GET /tasks
void route_get_tasks(int client_socket, sqlite3 *db) {
    const char *errMsg; 
    TaskList *tasks = get_tasks(db, errMsg);

    printf("\n[api.c::route_get_tasks()] - Retrieved TaskList from SQL command...\n");

    cJSON *json_res = tasks_to_json(tasks);

    const char *json_res_printform = cJSON_PrintUnformatted(json_res);

    send_response(client_socket, "200 OK", "application/json", json_res_printform);
}

// Handler for POST /add_task
void route_add_task(int client_socket, const char *body, sqlite3 *db) { 
    cJSON *json_request = cJSON_Parse(body);
    printf("Request POST data: \n", cJSON_Print(json_request));

    cJSON *json_response = cJSON_CreateObject();
    cJSON_AddStringToObject(json_response, "State", "Horny");
    cJSON_AddStringToObject(json_response, "Condition", "Freshly Jerked");
    cJSON_AddNumberToObject(json_response, "Number of Jerks in a Single Day", 12);
    send_response(client_socket, "201 Created", "application/json", cJSON_PrintUnformatted(json_response));
}

// Utility function to send an HTTP response
void send_response(int client_socket, const char *status, const char *content_type, const char *body) {
    char response[2048];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",
             status, content_type, strlen(body), body);

    write(client_socket, response, strlen(response));
}
