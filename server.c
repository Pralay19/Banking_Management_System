/* 
====================================
* PROGRAM: Server.c
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "allStructures.h"

#define PORT 8080

long authenticate_user(const char* role, const char* user_id, const char* password) {
    char filename[100], line[256], stored_user[50], stored_pass[50];
    FILE *file;
    struct flock lock;
    int reqSize;
    // Determine the file based on role
    if (strcmp(role, "customer") == 0) {
        strcpy(filename, "customers.txt");
        struct Customer who;
        reqSize=sizeof(who);
    }
    else if (strcmp(role, "employee") == 0) {
        strcpy(filename, "employees.txt");
        struct Employee who;
        reqSize=sizeof(who);
    }
    else if (strcmp(role, "manager") == 0) {
        strcpy(filename, "managers.txt");
        struct Manager who;
        reqSize=sizeof(who);
    }
    else if (strcmp(role, "admin") == 0) {
        strcpy(filename, "admins.txt");
        struct Admin who;
        reqSize=sizeof(who);
    }
    else {
        return 0; // Invalid role
    }

    // Open the role-specific file
    file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    // Lock the file to avoid concurrent access issues
    int fd = fileno(file);


    long position=0;
    // Searching in the file
    while (fread(&who, sizeof(reqSize), 1, file) == 1) {
        if (strcmp(who.userid, user_id) == 0 && strcmp(who.password, password) == 0) {
            if(who.active!=true){
                position=ftell(file);//storing the current position of the head in file
                lock.l_type = F_WRLCK;// F_WRLCK for exclusive lock
                lock.l_whence = SEEK_SET;
                lock.l_start = position;
                lock.l_len = reqSize;
                lock.l_pid = getpid();
                fcntl(fd, F_SETLK, &lock);// LOCKING the part where user block is present          
                
                who.active=true;// making the client-user online
                // Updating the changes to file
                fseek(file, -sizeof(reqSize), SEEK_CUR);
                fwrite(&who, sizeof(reqSize), 1, file);// Writing the changes to file
                fflush(file);// Ensuring data is written to disk
                lock.l_type = F_UNLCK; // Unlock
                if (fcntl(fd, F_SETLCK, &lock) == -1) {
                    perror("fcntl");
                    return 1;
                }
                fclose(file);
                return position;  // Success
            }
            else{
                return 0;
            }
        }
    }

    fclose(file);
    
    return 0;  // Failure
}

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    char *welcome_message = "Choose your role:\n1. Customer\n2. Employee\n3. Manager\n4. Administrator\n";
    char user_id[50], password[50];
    
    send(client_socket, welcome_message, strlen(welcome_message), 0);
    read(client_socket, buffer, 1024);
    int choice = atoi(buffer);
    
    char *role;
    if (choice == 1) role = "customer";
    else if (choice == 2) role = "employee";
    else if (choice == 3) role = "manager";
    else if (choice == 4) role = "admin";
    else {
        send(client_socket, "Invalid choice.\n", 15, 0);
        return;
    }

    send(client_socket, "Enter User ID: ", 15, 0);
    read(client_socket, user_id, 50);
    send(client_socket, "Enter Password: ", 16, 0);
    read(client_socket, password, 50);

    long position=authenticate_user(role, user_id, password);

    if (position) {
        send(client_socket, "Login Successful\n", 18, 0);
        char position_str[32];
        sprintf(position_str, "%ld", position);
        if (strcmp(role, "customer") == 0) {
            execlp("./customer", "customer",user_id,password,position_str, NULL);
        } else if (strcmp(role, "employee") == 0) {
            execlp("./employee", "employee",user_id,password,position_str, NULL);
        } else if (strcmp(role, "manager") == 0) {
            execlp("./manager", "manager",user_id,password,position_str, NULL);
        } else if (strcmp(role, "admin") == 0) {
            execlp("./admin", "admin",user_id,password,position_str, NULL);
        }
    } else {
        send(client_socket, "Invalid credentials or You are already online from another device. Try again.\n", 55, 0);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started, waiting for connections...\n");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        if (fork() == 0) {
            handle_client(new_socket);
            close(new_socket);
            exit(0);
        } else {
            close(new_socket);
        }
    }

    return 0;
}
