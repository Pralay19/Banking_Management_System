/* 
====================================
* PROGRAM: client2.c
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "allStructures.h"


#define PORT 8080


int main() {
	int sock=0;
    struct sockaddr_in server;
    char buffer[1024], userid[100], password[10];
    int role_choice; 

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
	    perror("inet_pton");
	    exit(EXIT_FAILURE);
	}
	// Connect to the server
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        return 1;
    }
    printf("\nConnected to BANK server\n");


    while(1){

    	printf("\nEnter role \n1.Customer\n2. Employee\n3. Manager\n4. Administrator\n5. Exit \n\n");
    	scanf("%d",&role_choice);
    	if(role_choice==5){
    		// The client program is closed

    		send(sock, );
    		close(sock);
    		exit(0);
    	}

    	printf("\nEnter User ID: ");
        scanf("%s", userid);
        printf("\nEnter Password: ");
        scanf("%s", password);

        snprintf(buffer, sizeof(buffer), "%d %s %s", role_choice, userid, password);
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        recv(sock,buffer,sizeof(buffer),0);
        printf("\n%s",buffer);

        if (strstr(buffer, "Successful")) {
        	if(role_choice==1){
        		//customer_program(sock);
        	}
        	else if(role_choice==2) {
        		//employee_program(sock);
        	}
        	else if(role_choice==3) {
        		// manager_program(sock);
        	}
        	else if(role_choice==4) {
        		//admin_program(sock);
        	}
        	else {
        		// Invalid choice
        		continue;
        	}
        }
        memset(buffer, 0, sizeof(buffer));
    }
}