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

void admin_program(int sock){
	char buffer[1024], employee_id[100], password[10];

	while(1){
		int choice;
		printf("\nAdmin Menu:\n");
        printf("1. Add New Employee\n2. Logout\nChoose an option: ");
        scanf("%d",&choice);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d", choice);
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));

        if (choice == 1) {
            // Add New Employee
            printf("Enter new employee UserID: ");
            scanf("%s", employee_id);
            printf("Enter password for the new employee: ");
            scanf("%s", password);
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s %s", employee_id, password);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock, buffer, sizeof(buffer), 0);
            printf("\n%s", buffer);
            memset(buffer, 0, sizeof(buffer));
            
        } 
        else if (choice == 2) {
            // Logout
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%d", choice);
            send(sock, buffer, strlen(buffer), 0);
            printf("Logging out...\n");
            memset(buffer, 0, sizeof(buffer));
            // remove it from session


            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));
            return;
        } 
        else {
            printf("Invalid choice. Please try again.\n");
        }

	}
}

void employee_program(int sock){
	char buffer[1024];

	while(1){
		int choice;
		printf("\nEmployee Menu:\n");
        printf("1. Add New Customer\n2. Modify Customer Details\n3. Process Loan Application\n4. View Assigned Loan Applications\n5. Change Password\n6. Logout\nChoose an option: ");
        scanf("%d",&choice);

        snprintf(buffer, sizeof(buffer), "%d", choice);
        send(sock, buffer, strlen(buffer), 0);

        if (choice == 1) {
            // Add New Customer
            char userid[100],password[10];
            printf("Enter new customer UserID: ");
            scanf("%s", userid);
            printf("Enter password for the new customer: ");
            scanf("%s", password);
            snprintf(buffer, sizeof(buffer), "%s %s", userid, password);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock, buffer, sizeof(buffer), 0);
            printf("\n%s", buffer);
            memset(buffer,0,sizeof(buffer));
        }
        else if(choice==6){
        	// Logout
            printf("Logging out...\n");
            memset(buffer, 0, sizeof(buffer));

            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));
            return;
        }
	}

}

void customer_program(int sock){
	char buffer[1024];

	while(1){
		int choice;
		printf("\nCustomer Menu:\n1. View Balance\n2. Deposit Money\n3. Withdraw Money\n4. Transfer Funds\n5. Apply for a Loan\n6. Change Password\n7. Adding Feedback\n8. View Transaction History\n9. Logout\nChoose an option: ");
        scanf("%d", &choice);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d", choice);
        send(sock, buffer, strlen(buffer), 0);

        if(choice==1){
        	//View Balance
        	memset(buffer, 0, sizeof(buffer));
        	recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));

        }
        else if(choice==2){
            //Deposit Money
            int amount;
        	printf("\nEnter amount to deposit: ");
            scanf("%d", &amount);
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%d", amount);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));

        }
        else if(choice==3){
            //Withdraw Money
            int amount;
            printf("\nEnter amount: ");
            scanf("%d", &amount);
            snprintf(buffer, sizeof(buffer), "%d", amount);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));
        }
        else if(choice==4){
            //Transfer Funds
            char receiver_id[100];
            int amount;
            printf("\nEnter receiver ID: ");
            scanf("%s", receiver_id);
            printf("\nEnter amount to transfer: ");
            scanf("%d", &amount);

            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s %d", receiver_id, amount);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));

            int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
			if (bytes_received < 0) {
			    perror("recv failed");
			} else {
			    printf("%s\n", buffer);
			}
			memset(buffer, 0, sizeof(buffer));
        }
        else if(choice==5){
            //Apply for a loan

        }
        else if(choice==6){
            //Change Password
            char password[10];
            printf("\nEnter new password(10 characters): ");
            scanf("%s", password);
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s", password);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock,buffer,sizeof(buffer),0);
            printf("%s",buffer);
            memset(buffer, 0, sizeof(buffer));
        }
        else if(choice==7){
            // Adding Feedback
        	char feedback[300];
            printf("\nEnter Feedback(300 characters): ");
            scanf("%s", feedback);
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s", feedback);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock,buffer,sizeof(buffer),0);
            printf("%s",buffer);
            memset(buffer, 0, sizeof(buffer));

        }
        else if(choice==8){
            
        }
        else if(choice==9){
            // Logout
            printf("Logging out...\n");
            memset(buffer, 0, sizeof(buffer));

            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));
            return;
            
        }
        else{
            //Invalid Choice

        }

	}
}



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
    	memset(buffer, 0, sizeof(buffer));
    	printf("\nEnter role \n1.Customer\n2. Employee\n3. Manager\n4. Administrator\n5. Exit \n\n");
    	scanf("%d",&role_choice);
    	if(role_choice==5){
    		// The client program is closed
    		strcpy(userid,"a");
    		strcpy(password,"b");
    		snprintf(buffer, sizeof(buffer), "%d %s %s", role_choice,userid,password);
    		send(sock, buffer,sizeof(buffer),0);
    		close(sock);
    		exit(0);
    	}

    	printf("\nEnter User ID: ");
        scanf("%s", userid);
        printf("\nEnter Password: ");
        scanf("%s", password);

        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d %s %s", role_choice, userid, password);
        printf("%s\n",buffer );
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        recv(sock,buffer,sizeof(buffer),0);
        printf("\n%s",buffer);

        if (strstr(buffer, "Successful")) {
        	if(role_choice==1){
        		customer_program(sock);
        	}
        	else if(role_choice==2) {
        		employee_program(sock);
        	}
        	else if(role_choice==3) {
        		// manager_program(sock);
        	}
        	else if(role_choice==4) {
        		admin_program(sock);
        	}
        	else {
        		// Invalid choice
        		continue;
        	}
        }
        memset(buffer, 0, sizeof(buffer));
    }
}