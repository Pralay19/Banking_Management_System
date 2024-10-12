/* 
====================================
* PROGRAM: client.c
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

// #include "allStructures.h"

#define PORT 8080

void login(int socket,int role) {
    char user_id[50], password[50], buffer[1024];
    
    printf("Enter User ID: ");
    scanf("%s", user_id);
    send(socket, user_id, strlen(user_id), 0);

    printf("Enter Password: ");
    scanf("%s", password);
    send(socket, password, strlen(password), 0);
    
    read(socket, buffer, 1024);
    printf("%s", buffer);
    
}

int main() {
    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to Bank Server...\n");
    
    read(sock, buffer, 1024);
    printf("%s", buffer);

    int role_choice;
    scanf("%d", &role_choice);
    sprintf(buffer, "%d", role_choice);
    send(sock, buffer, strlen(buffer), 0);

    login(sock,role_choice);

    return 0;
}
