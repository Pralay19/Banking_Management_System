/* 
====================================
* PROGRAM: server2.c
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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <stdbool.h>

#include "allStructures.h"
#include "employee.h"
#include "admin.h"

#define PORT 8080


int number_of_custm=0;
int number_of_emp=0;


int authentication(int role,const char *userid, const char *password){
	char filename[100];
	FILE*file;
	if(role==1){
		//CUSTOMER
		strcpy(filename, "customers.txt");
		file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Customer who;
        while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            fclose(file);
	            return 1; // Authentication successful
	        }
    	}
	}
	else if(role==2){
		//EMPLOYEE
		strcpy(filename, "employees.txt");
		file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Employee who;
        while (fread(&who, sizeof(struct Employee), 1, file) == 1) {
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            fclose(file);
	            return 1; // Authentication successful
	        }
    	}
	}
	else if(role==3){
		//MANAGER
		strcpy(filename, "managers.txt");
		file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Manager who;
        while (fread(&who, sizeof(struct Manager), 1, file) == 1) {
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            fclose(file);
	            return 1; // Authentication successful
	        }
    	}
	}
	else if(role==4){
		//ADMIN
        if(strstr("pralay",userid) && strstr("123",password)){
            return 1;
        }
	}

	return 0;
}


void handle_client(int client_sock) {
    char buffer[1024];
    char userid[100], password[10];
    int role;
    int bytes_received;

    while((bytes_received=recv(client_sock, buffer, sizeof(buffer), 0))>0){
    	//Receive role,userid and password
    	
    	if (bytes_received == 0) {
            printf("Client disconnected\n");
        } 
        else {
            perror("recv");
        }
    	   

    	//Initializing Role,Userid and Password
    	sscanf(buffer, "%d %s %s", &role, userid, password);

    	//Checking if client initiated TERMINATION PROCESS
    	if (role==5){
            printf("\nClient requested termination\n");
            close(client_sock);
            return;
        }
    	

    	//check if the user is online or not


    	//Authenticate the user
    	int auth=authentication(role,userid,password);
        printf("\nAuth done %d",auth);
    	if(auth==0){
    		snprintf(buffer,sizeof(buffer),"Invalid Credentials!");
    		send(client_sock,buffer,sizeof(buffer),0);
    		memset(buffer, 0, sizeof(buffer));
    		continue;
    	}

    	snprintf(buffer,sizeof(buffer),"Login Successful !\nWelcome %s",userid);
    	send(client_sock,buffer,sizeof(buffer),0);
    	memset(buffer, 0, sizeof(buffer));

    	if(role==1){
    		//CUSTOMER
            while(1){
                memset(buffer, 0, sizeof(buffer));
                recv(client_sock,buffer,sizeof(buffer),0);
                int choice;
                sscanf(buffer,"%d",&choice);

                if(choice==1){
                    // //View balance
                    // int amount=0;
                    // amount=view_account_balance(userid);
                    // snprintf(buffer,sizeof(buffer),"Available Balance: $%d",amount);
                    // send(client_sock,buffer,sizeof(buffer),0);
                }
                else if(choice==2){

                }
                else if(choice==3){
                    
                }
                else if(choice==4){
                    
                }else if(choice==2){
                    
                }


            }

    	}
    	else if(role==2){
    		// EMPLOYEE
    		while(1){
    			recv(client_sock,buffer,sizeof(buffer),0);
    			int choice;
    			sscanf(buffer,"%d",&choice);

    			if(choice==1){
    				//Add new customer
    				char new_customer_id[100], new_customer_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s %s", new_customer_id, new_customer_password);
                    add_new_customer(new_customer_id, new_customer_password,number_of_emp);
                    number_of_custm+=1;
                    strcpy(buffer,"New Customer Added.");
                    send(client_sock, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
    			}
    			else if(choice==6){
    				// Logout
                    //remove_session(userid);
                    
                    char *msg = "Logged Out";
                    send(client_sock, msg, strlen(msg), 0);
                    break;
    			}

    		}

    	}
    	else if(role==3){
    		//MANAGER
    	}
    	else if(role==4){
    		//ADMIN
    		while(1){
	    		recv(client_sock,buffer,sizeof(buffer),0);
	    		int choice;
	    		sscanf(buffer,"%d",&choice);

	    		if (choice == 1) {
                    // Add New Employee
                    char new_employee_id[100], new_employee_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s %s", new_employee_id, new_employee_password);
                    add_new_employee(new_employee_id, new_employee_password,number_of_emp);
                    number_of_emp+=1;
                    memset(buffer, 0, sizeof(buffer));
                    strcpy(buffer,"New Employee Added.");
                    send(client_sock, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));

                } else if (choice == 2) {
                    // Logout
                    //remove_session(userid);
                    char *msg = "Logged Out";
                    send(client_sock, msg, strlen(msg), 0);
                    break;
                } else if (choice == 3) {
                    // Other functionalities

                } else {
                    // Invalid choice
                    char *msg1 = "Invalid option.";
                    send(client_sock, msg1, strlen(msg1), 0);
                }
    		}

    	}
    	else{
    		//INVALID CHOICE

    	}

    }

    close(client_sock);
    return;
}



int main() {
    int server_sock,client_sock,new_sock;
    struct sockaddr_in server;
    int addrlen=sizeof(server);
    pid_t pid;
    int opt=1;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_sock, 10);
    printf("\nServer listening on port %d",PORT);
    printf("\nWaiting for connections...\n");

    while(1){
    	// Accept a new connection
    	client_sock = accept(server_sock, (struct sockaddr *)&server, (socklen_t *)&addrlen);
        new_sock=client_sock;
        // Create a new process to handle the client connection
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { 
        // Child process
            close(server_sock); // Close the parent socket
            handle_client(new_sock);
            exit(0); 
        } else { 
            // Parent process
            close(new_sock); // Close the child's socket
        }

        printf("New Client connected\n");
        
    }

}