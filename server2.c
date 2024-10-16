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
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdbool.h>

#include "allStructures.h"


#define PORT 8080



int client_count = 0;

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
	else if(role=2){
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
	else if(role=3){
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
	else if(role=4){
		//ADMIN
		strcpy(filename, "admins.txt");
		file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Admin who;
        while (fread(&who, sizeof(struct Admin), 1, file) == 1) {
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            fclose(file);
	            return 1; // Authentication successful
	        }
    	}
	}

	return 0;
}


void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[1024];
    
    
    while(1){
    	//Again receive role,userid and password
    	char userid[100], password[10];
    	int role;

    	memset(buffer, 0, sizeof(buffer));
    	int bytes_received=0;
    	bytes_received=recv(client_sock, buffer, sizeof(buffer), 0);
    	if(bytes_received<=0){
    		continue;
    	}

    	
    	sscanf(buffer, "%d %s %s", &role, userid, password);
    	//Checking if client initiated TERMINATION PROCESS
    	if (role==5){
            printf("\nClient requested termination\n");
            close(client_sock);
            client_count--;
            return NULL;
        }
    	

    	//check if the user is online or not


    	//Authenticate the user
    	int auth=authentication(role,userid,password);
    	if(auth==0){
    		snprintf(buffer,sizeof(buffer),"Invalid Credentials!");
    		send(client_sock,buffer,sizeof(buffer),0);
    		memset(buffer, 0, sizeof(buffer));
    		continue;
    	}

    	snprintf(buffer,sizeof(buffer),"Login Successful !\nWelcome %s",userid);
    	send(client_sock,buffer,sizeof(buffer),0);



    }

    close(client_sock);
    return NULL;
}



int main() {
    int server_sock,client_sock,*new_sock;
    struct sockaddr_in server;
    int addrlen=sizeof(server);
    pthread_t thread_id[10];
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
        new_sock=malloc(1);
        *new_sock=client_sock;
        // Create a new thread to handle the client connection
        if (pthread_create(&thread_id[client_count], NULL, handle_client, (void *)&new_sock) < 0) {
            perror("Could not create thread");
            exit(EXIT_FAILURE);
        }
        client_count++;
        printf("New Client connected\n");
        printf("\nNumber of Clients online: %d\n",client_count);
    }

}