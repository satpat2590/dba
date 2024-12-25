// Author: Satyam Patel
// Date: 12/19/2024
// Goal: Learn how to build your own web server API 

#include <stdio.h> 
#include <lib/sqlite3.h> 
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "src/sql.h"
#include "src/api.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <lib/cJSON.h>

#define PORT 8080

/* 
    Return current time in time_t format. 
    Print out time in string format.
*/
time_t get_currtime() {
 // Instantiate both time_t for return and char array for printing out formatted timestamp
    time_t current_time; 
    struct tm *time_info; 
    char time_string[100];

 // Getting the current time and assigning it to current_time
    time(&current_time);

 // Get the local timezone information from current timestamp and then format into string 
    time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);
    printf("%s\n", time_string);

    return current_time; 
}


// Main function
int main() {
 // Program Start
    time_t currtime = get_currtime();

 // Define the server socket fd and the client socket fd
    int server_fd, client_socket;

 // The memory address which the socket will point to 
    struct sockaddr_in server_addr;

 // Create a new socket with a random protocol (don't ask me what a protocol is)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

 // We all sin in our lives. We must define them here.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

 // Bind the socket to a port and force feed it the mem address at server_addr (real ones will understand the meaning of an ampersand)
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

 // Keep your ears open, Mr. Socket. They're coming.
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d\n", PORT);

 // False alarm, Mr. Socket. We enabled incoming socket connections
    while ((client_socket = accept(server_fd, NULL, NULL)) >= 0) {
        handle_request(client_socket);
    }

    close(server_fd);
    return 0;
}