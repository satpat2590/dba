// Author: Satyam Patel
// Date: 12/21/2024
// Goal: Create API logic for connecting client sockets

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <stdlib.h>
#include <string.h>

void handle_request(int client_socket);

void route_get_tasks(int client_socket);

void route_add_task(int client_socket, const char *body);

void send_response(int client_socket, const char *status, const char *content_type, const char *body);