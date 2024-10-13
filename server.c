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
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdbool.h>
#include "customer.h"
#include "allStructures.h"
#include "employee.h"
#include "admin.h"

#define PORT 8080

long authenticate_user(int role, const char* user_id, const char* password) {
    char filename[100], line[256], stored_user[50], stored_pass[50];
    FILE *file;
    struct flock lock;

    int reqSize;
    // Determine the file based on role
    if (role == 1) {
        strcpy(filename, "customers.txt");
        // Open the role file
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Customer who1;
        reqSize=sizeof(who1);

        int fd = fileno(file);

        long position=0;
        // Searching in the file
        while (fread(&who1, sizeof(reqSize), 1, file) == 1) {
            if (strcmp(who1.userid, user_id) == 0 && strcmp(who1.password, password) == 0) {
                    position=ftell(file);//storing the current position of the head in file
                    lock.l_type = F_RDLCK;// F_WRLCK for exclusive lock
                    lock.l_whence = SEEK_SET;
                    lock.l_start = position;
                    lock.l_len = reqSize;
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLK, &lock);// LOCKING the part where user block is present          
                    
                    // who.active=true;// making the client-user online
                    // Updating the changes to file
                    // fseek(file, -sizeof(reqSize), SEEK_CUR);
                    // fwrite(&who, sizeof(reqSize), 1, file);// Writing the changes to file
                    // fflush(file);// Ensuring data is written to disk
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return 1;
                    }
                    fclose(file);
                    return position;  // Success
            }
        }
    }
    else if (role == 2) {
        strcpy(filename, "employees.txt");
        // Open the role file
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Employee who2;
        reqSize=sizeof(who2);
        int fd = fileno(file);

        long position=0;
        // Searching in the file
        while (fread(&who2, sizeof(reqSize), 1, file) == 1) {
            if (strcmp(who2.userid, user_id) == 0 && strcmp(who2.password, password) == 0) {
                    position=ftell(file);//storing the current position of the head in file
                    lock.l_type = F_RDLCK;// F_WRLCK for exclusive lock
                    lock.l_whence = SEEK_SET;
                    lock.l_start = position;
                    lock.l_len = reqSize;
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLK, &lock);// LOCKING the part where user block is present          
                    
                    // who.active=true;// making the client-user online
                    // Updating the changes to file
                    // fseek(file, -sizeof(reqSize), SEEK_CUR);
                    // fwrite(&who, sizeof(reqSize), 1, file);// Writing the changes to file
                    // fflush(file);// Ensuring data is written to disk
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return 1;
                    }
                    fclose(file);
                    return position;  // Success
            }
        }
    }
    else if (role == 3) {
        strcpy(filename, "managers.txt");
        // Open the role file
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Manager who3;
        reqSize=sizeof(who3);
        int fd = fileno(file);

        long position=0;
        // Searching in the file
        while (fread(&who3, sizeof(reqSize), 1, file) == 1) {
            if (strcmp(who3.userid, user_id) == 0 && strcmp(who3.password, password) == 0) {
                    position=ftell(file);//storing the current position of the head in file
                    lock.l_type = F_RDLCK;// F_WRLCK for exclusive lock
                    lock.l_whence = SEEK_SET;
                    lock.l_start = position;
                    lock.l_len = reqSize;
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLK, &lock);// LOCKING the part where user block is present          
                    
                    // who.active=true;// making the client-user online
                    // Updating the changes to file
                    // fseek(file, -sizeof(reqSize), SEEK_CUR);
                    // fwrite(&who, sizeof(reqSize), 1, file);// Writing the changes to file
                    // fflush(file);// Ensuring data is written to disk
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return 1;
                    }
                    fclose(file);
                    return position;  // Success
            }
        }
    }
    else if (role == 4) {
        strcpy(filename, "admins.txt");
        // Open the role file
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Admin who4;
        strcpy(who4.userid,"pralay11");
        strcpy(who4.password,"123");
        reqSize=sizeof(who4);
        return 1; // TEMPORARY! FOR CHECKING ONLY.
        int fd = fileno(file);

        long position=0;
        // Searching in the file
        while (fread(&who4, sizeof(reqSize), 1, file) == 1) {
            if (strcmp(who4.userid, user_id) == 0 && strcmp(who4.password, password) == 0) {
                    position=ftell(file);//storing the current position of the head in file
                    lock.l_type = F_RDLCK;// F_WRLCK for exclusive lock
                    lock.l_whence = SEEK_SET;
                    lock.l_start = position;
                    lock.l_len = reqSize;
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLK, &lock);// LOCKING the part where user block is present          
                    
                    // who.active=true;// making the client-user online
                    // Updating the changes to file
                    // fseek(file, -sizeof(reqSize), SEEK_CUR);
                    // fwrite(&who, sizeof(reqSize), 1, file);// Writing the changes to file
                    // fflush(file);// Ensuring data is written to disk
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return 1;
                    }
                    fclose(file);
                    return position;  // Success
            }
        }
    }
    else {
        return 0; // Invalid role
    }

    // int fd = fileno(file);

    // long position=0;
    // // Searching in the file
    // while (fread(&who, sizeof(reqSize), 1, file) == 1) {
    //     if (strcmp(who.userid, user_id) == 0 && strcmp(who.password, password) == 0) {
    //             position=ftell(file);//storing the current position of the head in file
    //             lock.l_type = F_RDLCK;// F_WRLCK for exclusive lock
    //             lock.l_whence = SEEK_SET;
    //             lock.l_start = position;
    //             lock.l_len = reqSize;
    //             lock.l_pid = getpid();
    //             fcntl(fd, F_SETLK, &lock);// LOCKING the part where user block is present          
                
    //             // who.active=true;// making the client-user online
    //             // Updating the changes to file
    //             // fseek(file, -sizeof(reqSize), SEEK_CUR);
    //             // fwrite(&who, sizeof(reqSize), 1, file);// Writing the changes to file
    //             // fflush(file);// Ensuring data is written to disk
    //             lock.l_type = F_UNLCK; // Unlock
    //             if (fcntl(fd, F_SETLK, &lock) == -1) {
    //                 perror("fcntl");
    //                 return 1;
    //             }
    //             fclose(file);
    //             return position;  // Success
    //     }
    // }

    fclose(file);
    
    return 0;  // Failure
}

// void handle_client(int client_socket) {
//     char buffer[1024] = {0};
//     char *welcome_message = "Choose your role:\n1. Customer\n2. Employee\n3. Manager\n4. Administrator\n";
//     char user_id[50], password[50];
    
//     send(client_socket, welcome_message, strlen(welcome_message), 0);
//     read(client_socket, buffer, 1024);
//     int choice = atoi(buffer);
    
//     char *role;
//     if (choice == 1) role = "customer";
//     else if (choice == 2) role = "employee";
//     else if (choice == 3) role = "manager";
//     else if (choice == 4) role = "admin";
//     else {
//         send(client_socket, "Invalid choice.\n", 15, 0);
//         return;
//     }

//     // send(client_socket, "Enter User ID: ", 15, 0);
//     read(client_socket, user_id, 50);
//     // send(client_socket, "Enter Password: ", 16, 0);
//     read(client_socket, password, 50);

//     long position=authenticate_user(role, user_id, password);

//     if (position) {
//         send(client_socket, "Login Successful\n", 18, 0);
//         char position_str[32];
//         sprintf(position_str, "%ld", position);
//         if (strcmp(role, "customer") == 0) {
//             execlp("./customer", "customer",user_id,password,position_str, NULL);
//         } else if (strcmp(role, "employee") == 0) {
//             execlp("./employee", "employee",user_id,password,position_str, NULL);
//         } else if (strcmp(role, "manager") == 0) {
//             execlp("./manager", "manager",user_id,password,position_str, NULL);
//         } else if (strcmp(role, "admin") == 0) {
//             execlp("./admin", "admin",user_id,password,position_str, NULL);
//         }
//     } else {
//         send(client_socket, "Invalid credentials or You are already online from another device. Try again.\n", 55, 0);
//     }
// }


struct Session {
    char userid[100];
    int sessionid;
};

struct Session activeSessions[100];
int sessionCount = 0;

pthread_mutex_t session_lock = PTHREAD_MUTEX_INITIALIZER;

bool isOnline(const char *userid) {
    pthread_mutex_lock(&session_lock);
    for (int i = 0; i < sessionCount; i++) {
        if (strcmp(activeSessions[i].userid, userid) == 0) {
            pthread_mutex_unlock(&session_lock);
            return true;
        }
    }
    pthread_mutex_unlock(&session_lock);
    return false;
}

int add_session(const char *userid) {
    pthread_mutex_lock(&session_lock);
    int sessionid = rand();
    strcpy(activeSessions[sessionCount].userid, userid);
    activeSessions[sessionCount].sessionid = sessionid;
    sessionCount++;
    pthread_mutex_unlock(&session_lock);
    return sessionid;
}

void remove_session(const char *userid) {
    pthread_mutex_lock(&session_lock);
    for (int i = 0; i < sessionCount; i++) {
        if (strcmp(activeSessions[i].userid, userid) == 0) {
            for (int j = i; j < sessionCount - 1; j++) {
                activeSessions[j] = activeSessions[j + 1];
            }
            sessionCount--;
            break;
        }
    }
    pthread_mutex_unlock(&session_lock);
}

void *client_handler(void *socket_desc) {
    int client_sock = *(int *)socket_desc;
    free(socket_desc);

    char buffer[1024];
    char userid[100], password[10];
    int role;
    int sessionid;

    while(1) {
        // Receive role, userid, and password
        recv(client_sock, buffer, 1024, 0);
        sscanf(buffer, "%d %s %s", &role, userid, password);

        if (isOnline(userid)) {
            char *msg = "User already logged in.\n";
            send(client_sock, msg, strlen(msg), 0);
            continue;
        }

        // Authenticate user
        long position=authenticate_user(role,userid,password);
        if(position){
            // That means user exists so don't do anything
        }
        else{
            snprintf(buffer, sizeof(buffer), "Invalid Credentials! Please try again later.\n");
            send(client_sock, buffer, strlen(buffer), 0);
            continue;
        }


        sessionid = add_session(userid);
        snprintf(buffer, sizeof(buffer), "Login successful. Session ID: %d\n", sessionid);
        send(client_sock, buffer, strlen(buffer), 0);

        while (1) {
            recv(client_sock, buffer, 1024, 0);  // Receive request

            // Process client request based on role
            if (role == 1) {  
                // Customer
                int choice;
                sscanf(buffer, "%d", &choice);

                if (choice == 2) {
                    // Deposit Money
                    int amount;
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%d", &amount);
                    int result=deposit_money(position, amount);
                    char*msg1="\nAvailable Balance: $";
                    snprintf(buffer,sizeof(buffer),"%s%d",msg1,result);
                    send(client_sock,buffer,sizeof(buffer),0);
                }
                else if (choice == 1) {
                    // View account balance
                    //sscanf() takes out of buffer->variable
                    int result=view_account_balance(position);
                    char*msg1="\nAvailable Balance: $";
                    snprintf(buffer,sizeof(buffer),"%s%d",msg1,result);
                    send(client_sock,buffer,sizeof(buffer),0);                
                }
                else if (choice == 3) {
                    // Withdraw Money
                    
                    int amount;
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%d",&amount);
                    int result=withdraw_money(position,amount);
                    if(result){
                        if(result>0){
                            char*msg1="\nAvailable Balance: $";
                            snprintf(buffer,sizeof(buffer),"%s%d",msg1,result);
                            send(client_sock,buffer,sizeof(buffer),0);
                        }
                        else{
                            char*msg1="\nInsufficient Balance.";
                            send(client_sock,msg1,sizeof(msg1),0);
                        }
                    }
                    else{
                        char*msg1="\nERROR opening file";
                        send(client_sock,msg1,sizeof(msg1),0);
                    }
                }
                else if (choice == 4) {
                    // Transfer Funds
                    char receiver_id[100];
                    int amount;
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s %d", receiver_id, &amount);
                    int result=transfer_funds(position, receiver_id, amount);
                    if(result){
                        if(result>0){
                            char tempBuffer[1024];
                            char * msg1="\nTransfer successful. Available Balance: $";
                            snprintf(tempBuffer,sizeof(tempBuffer),"%s%d",msg1,result);
                            send(client_sock,tempBuffer,sizeof(tempBuffer),0);
                        }
                        else{
                            char *msg1="Insuffecient Balance.";
                            send(client_sock,msg1,sizeof(msg1),0);
                        }
                    }
                    else{
                        char *msg1="\nNo such Account found. Please check the User Id of the receiver and try again later.";
                        send(client_sock,msg1,sizeof(msg1),0);
                    }
                }
                else if (choice == 5) {
                    // Aplly for Loan
                    char receiver_id[100];
                    int amount;
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s %d", receiver_id, &amount);
                    transfer_funds(position, receiver_id, amount);
                }
                else if (choice == 6) {
                    // Change Password
                    char password[100];
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s", password);
                    int result=change_password(position,password);
                    if(result){
                        char*msg1="\nPassword changed successfully.";
                        send(client_sock,msg1,sizeof(msg1),0);
                    }
                    else{
                        char*msg1="\nError in database";
                        send(client_sock,msg1,sizeof(msg1),0);
                    }
                }
                else if (choice == 7) {
                    // Add feedback
                    char feedback[100];
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(feedback, "%s", buffer);
                    add_feedback(feedback);
                }
                else if (choice == 8) {
                    // View Transaction History
                    view_transaction_history();
                }
                else if (choice == 9) {
                    remove_session(userid);
                    char*msg1="Logged Out";
                    send(client_sock,msg1,sizeof(msg1),0);
                    break;
                }
            }
            else if(role == 2) {
                // Employee
                int choice;
                sscanf(buffer, "%d", &choice);

                if (choice == 1) { 
                    // Add New Customer
                    char new_customer_id[100], new_customer_password[10];
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s %s", new_customer_id, new_customer_password);
                    add_new_customer(new_customer_id, new_customer_password); // Calling function from employee.h
                    char *msg = "New customer added successfully.";
                    send(client_sock, msg, strlen(msg), 0);

                } 
                else if (choice == 2) { 
                    // Modify Customer Details
                    char customer_id[100], new_password[10];
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s %s", customer_id, new_password);
                    modify_customer_details(customer_id, new_password); // Calling function from employee.h
                    char *msg = "Customer details updated successfully.";
                    send(client_sock, msg, strlen(msg), 0);

                } 
                else if (choice == 3) { 
                    // Process Loan Application
                    char loan_customer_id[100];
                    int decision; // 1 for Accept, 0 for Reject
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s %d", loan_customer_id, &decision);
                    process_loan_appl(loan_customer_id, decision == 1); // Calling function from employee.h
                    char *msg;
                    if (decision == 1) {
                        msg = "Loan application accepted.";
                    } else {
                        msg = "Loan application rejected.";
                    }
                    send(client_sock, msg, strlen(msg), 0);

                } 
                else if (choice == 4) { 
                    // View Assigned Loan Applications
                    view_assigned_loan_appl(userid); // Call the function from employee.h
                    char *msg = "Assigned Loan Applications:\n1. Loan #12345 - Pending\n2. Loan #67890 - Approved"; // Placeholder for actual data
                    send(client_sock, msg, strlen(msg), 0);

                } 
                else if (choice == 5) { 
                    // Change Password
                    char new_password[10];
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s", new_password);
                    change_password_employee(position, new_password);
                    char *msg = "Password changed successfully.";
                    send(client_sock, msg, strlen(msg), 0);

                } 
                else if (choice == 6) { 
                    // Logout
                    remove_session(userid);
                    char *msg = "Logged Out";
                    send(client_sock, msg, strlen(msg), 0);
                    break;

                } 
                else {
                    // Invalid choice
                    char *msg = "Invalud option. Please try again.";
                    send(client_sock, msg, strlen(msg), 0);
                }
            }
            else if(role == 3) {
                // Manager
                int choice;
                sscanf(buffer, "%d", &choice);
            }
            else if (role == 4) { 
                // Admin
                int choice;
                sscanf(buffer, "%d", &choice);

                if (choice == 1) {
                    // Add New Employee
                    char new_employee_id[100], new_employee_password[10];
                    recv(client_sock, buffer, 1024, 0);
                    sscanf(buffer, "%s %s", new_employee_id, new_employee_password);
                    add_new_employee(new_employee_id, new_employee_password);
                    char msg1[100]= "New Employee Added.";
                    //printf("%s",msg1);
                    send(client_sock, msg1, sizeof(msg1), 0);

                } else if (choice == 2) {
                    // Logout
                    // remove_session(userid);
                    // char *msg = "Logged Out";
                    // send(client_sock, msg, strlen(msg), 0);
                    // break;
                } else if (choice == 3) {
                    // Other functionalities

                } else {
                    // Invalid choice
                    char *msg1 = "Invalid option. Please try again.";
                    send(client_sock, msg1, strlen(msg1), 0);
                }
            }
            else if(role == 5) {
                // Exit the program
                close(client_sock);
                return NULL;

            }
            else {
                // Invalid Role
                char* msg1="Invalid role selected. Please choose from the following role.";
                send(client_sock, msg1,sizeof(msg1),0);
                continue;
            }
        }
    }
    
    return NULL;
}

int main() {
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server, client;
    socklen_t c;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_sock, 100);

    printf("Waiting for connections...\n");
    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, &c))) {
        pthread_t client_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&client_thread, NULL, client_handler, (void *)new_sock) < 0) {
            perror("Could not create thread");
            return 0;
        }
        printf("Client connected\n");
    }

    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    
    return 0;
}
