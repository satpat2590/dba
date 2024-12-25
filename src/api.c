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
#include <lib/cJSON.h>

/*
GET /tasks - Get a list of all of the current tasks you are working on

POST /add_task - Add a new task with following body: 
    {
        "name": str
        "p_taskid": int
        "description": str
        "days_to_finish": int
    }
*/





/*
    Take in the request and then parse it to understand...
        1. Is it a 'GET' or 'POST' request?
        2. If POST, what is the request body?
        3. Is the request endpoint valid?
*/
void handle_request(int client_socket) {
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
            route_get_tasks(client_socket);
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
                route_add_task(client_socket, body);
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
void route_get_tasks(int client_socket) {
    const char *tasks = "[{\"id\": 1, \"title\": \"Example Task\", \"completed\": false}]";
    send_response(client_socket, "200 OK", "application/json", tasks);
}

// Handler for POST /add_task
void route_add_task(int client_socket, const char *body) { 

    printf("Received POST data: %s\n", body);
    send_response(client_socket, "201 Created", "text/plain", "Task added successfully");
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
