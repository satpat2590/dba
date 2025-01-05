// Author: Satyam Patel
// Date: 12/21/2024
// Goal: Create API logic for connecting client sockets

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <stdlib.h>
#include <string.h>
#include <../lib/cJSON.h>


void handle_request(int client_socket, sqlite3* db);

void route_get_tasks(int client_socket, sqlite3 *db);

void route_add_task(int client_socket, const char *body, sqlite3 *db);

void send_response(int client_socket, const char *status, const char *content_type, const char *body);