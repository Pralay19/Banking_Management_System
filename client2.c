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
	char buffer[1024], employee_id[100], password[10],manager_id[100];

	while(1){
		int choice;
		printf("\n-----------------------------------------------------------------------\n");
		printf("\nAdmin Menu:\n");
        printf("1. Add New Employee\n2. Logout\n3. Add new Manager\n4.Manage user roles\n5.Change Password\n6.Modify Customer Details\n7.Add new Admin\nChoose an option: ");
        printf("\n-----------------------------------------------------------------------\n");
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
            printf("Logging out...\n");
            memset(buffer, 0, sizeof(buffer));


            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));
            return;
        }
        else if(choice==3){
        	//Add New manager
        	printf("Enter new Manager UserID: ");
            scanf("%s", employee_id);
            printf("Enter password for the new Manager: ");
            scanf("%s", password);
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s %s", employee_id, password);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock, buffer, sizeof(buffer), 0);
            printf("\n%s", buffer);
            memset(buffer, 0, sizeof(buffer));

        }
        else if(choice==4){
        	//Manage user roles
        	printf("\nPROMOTE EMPLOYEE -> MANAGER:\n");
        	printf("\nEnter employee UserID: ");
            scanf("%s", employee_id);
            
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s", employee_id);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));

        }
        else if(choice==5){
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
        else if(choice==6){
        	//Modify Customer details
        	int decision;
        	char change[100],cust_id[100];
        	printf("\nEnter the Customer userID: ");
        	scanf("%s",cust_id);
        	memset(buffer,0,sizeof(buffer));
        	snprintf(buffer,sizeof(buffer),"%s",cust_id);
        	send(sock,buffer,sizeof(buffer),0);
        	recv(sock, buffer, sizeof(buffer), 0);
        	printf("\n%s\n",buffer);

        	printf("\nWhat do you want to change?");
        	
        	printf("\n1->Name of Customer: ");
        	printf("\n2-> Customer mobile no.: \n");
        	scanf("%d",&decision);

        	printf("\nEnter the modified info: ");
        	scanf(" %[^\n]", change);
        	memset(buffer,0,sizeof(buffer));
        	snprintf(buffer,sizeof(buffer),"%d %s",decision,change);

        	send(sock,buffer,sizeof(buffer),0);
        	memset(buffer,0,sizeof(buffer));
        	recv(sock, buffer, sizeof(buffer), 0);
        	printf("%s",buffer);
        	memset(buffer,0,sizeof(buffer));
        }
        else if(choice==7){
        	//Add new admin

        }
        else {
            printf("\nInvalid choice. Please try again.\n");
        }

	}
}

void employee_program(int sock){
	char buffer[1024];

	while(1){
		int choice;
		printf("\n-----------------------------------------------------------------------\n");
		printf("\nEmployee Menu:\n");
        printf("1. Add New Customer\n2. Modify Customer Details\n3. Process Loan Application\n4. View Assigned Loan Applications\n5. Change Password\n6. Logout\nChoose an option: ");
        printf("\n-----------------------------------------------------------------------\n");
        scanf("%d",&choice);

        snprintf(buffer, sizeof(buffer), "%d", choice);
        send(sock, buffer, strlen(buffer), 0);

        if (choice == 1) {
            // Add New Customer
            char userid[100],password[10],name[50],mobile[10];
            printf("\nEnter new Customer UserID: ");
            scanf("%s", userid);
            printf("\nEnter password for the new Customer: ");
            scanf("%s", password);
            printf("\nEnter Name of Customer: ");
            scanf(" %[^\n]", name);
            printf("\nEnter Customer mobile no.: ");
            scanf(" %[^\n]", mobile);
            snprintf(buffer, sizeof(buffer), "%s %s %s %s", userid, password,name,mobile);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock, buffer, sizeof(buffer), 0);
            printf("\n%s", buffer);
            memset(buffer,0,sizeof(buffer));
        }
        else if(choice==2){
        	//Modify Customer Details
        	int decision;
        	char change[100],cust_id[100];
        	printf("\nEnter the Customer userID: ");
        	scanf("%s",cust_id);
        	memset(buffer,0,sizeof(buffer));
        	snprintf(buffer,sizeof(buffer),"%s",cust_id);
        	send(sock,buffer,sizeof(buffer),0);
        	recv(sock, buffer, sizeof(buffer), 0);
        	printf("\n%s\n",buffer);

        	printf("\nWhat do you want to change?");
        	
        	printf("\n1->Name of Customer: ");
        	printf("\n2-> Customer mobile no.: \n");
        	scanf("%d",&decision);

        	printf("\nEnter the modified info: ");
        	scanf(" %[^\n]", change);
        	memset(buffer,0,sizeof(buffer));
        	snprintf(buffer,sizeof(buffer),"%d %s",decision,change);

        	send(sock,buffer,sizeof(buffer),0);
        	memset(buffer,0,sizeof(buffer));
        	recv(sock, buffer, sizeof(buffer), 0);
        	printf("%s",buffer);
        	memset(buffer,0,sizeof(buffer));
        }
        else if(choice==3){
        	//Process Loan Application
        	char loanid[110];
        	int decision;

        	printf("\nEnter Loan ApplicationId to process: ");
        	scanf("%s",loanid);
        	memset(buffer,0,sizeof(buffer));
        	snprintf(buffer,sizeof(buffer),"%s",loanid);
        	send(sock, buffer, sizeof(buffer), 0);

        	memset(buffer,0,sizeof(buffer));
        	recv(sock,buffer,sizeof(buffer),0);
        	printf("\n%s",buffer);
        	printf("\nOptions:\n1->Approve the loan\n2->Reject the loan");
        	scanf("%d",&decision);
        	memset(buffer,0,sizeof(buffer));
        	snprintf(buffer,sizeof(buffer),"%d",decision);
        	send(sock, buffer, strlen(buffer), 0);
        	printf("\nThe Loan status has been changed");
        	memset(buffer,0,sizeof(buffer));
        }
        else if(choice==4){
        	//View Assigned Loan Applications
        	memset(buffer, 0, sizeof(buffer));
        	char temp[1500];
		    
		    int result;
		    memset(buffer, 0, sizeof(buffer));
		    recv(sock,buffer,sizeof(buffer),0);
		    sscanf(buffer, "%d", &result);
		    memset(buffer, 0, sizeof(buffer));
		    if(result){
			    int bytes_received = recv(sock, temp, sizeof(temp), 0);
			    if (bytes_received <= 0) {
			        printf("Error receiving loan Applications available.\n");
			        
			    }
			    printf("%s",temp);
			    memset(buffer, 0, sizeof(buffer));
			}
			else{
				recv(sock,buffer,sizeof(buffer),0);
				printf("%s",buffer);

			}
		    memset(buffer, 0, sizeof(buffer));
        }
        else if(choice==5){
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
		printf("\n-----------------------------------------------------------------------\n");
		printf("\nCustomer Menu:\n1. View Balance\n2. Deposit Money\n3. Withdraw Money\n4. Transfer Funds\n5. Apply for a Loan\n6. Change Password\n7. Adding Feedback\n8. View Transaction History\n9. Logout\nChoose an option: ");
        printf("\n-----------------------------------------------------------------------\n");
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
        	int amount;
        	printf("\nEnter the Loan Amount: $");
        	scanf("%d",&amount);
        	snprintf(buffer, sizeof(buffer), "%d", amount);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
            memset(buffer, 0, sizeof(buffer));
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
            fgets(feedback, sizeof(feedback), stdin);
            feedback[strcspn(feedback, "\n")] = '\0';
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s", feedback);
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(sock,buffer,sizeof(buffer),0);
            printf("%s",buffer);
            memset(buffer, 0, sizeof(buffer));

        }
        else if(choice==8){
            //View Transactions
            memset(buffer, 0, sizeof(buffer));
        	struct Transaction transArray[100];
		    memset(transArray, 0, sizeof(transArray));

		    // Receive the transaction array from the server
		    int result;
		    memset(buffer, 0, sizeof(buffer));
		    recv(sock,buffer,sizeof(buffer),0);
		    sscanf(buffer, "%d", &result);
		    memset(buffer, 0, sizeof(buffer));
		    if(result){
			    int bytes_received = recv(sock, transArray, sizeof(transArray), 0);
			    if (bytes_received <= 0) {
			        printf("Error receiving transactions or no transactions available.\n");
			        
			    }

			    printf("\nYour Transactions:\n");
			    for (int i = 0; i < 100; i++) {
			        if (transArray[i].transac_id != 0) {
			            printf("*Transaction ID: %d| Type: %s| Receiver ID: %s| Debit/Credit: %s| Amount: %d| Balance After: %d\n",
			                   transArray[i].transac_id, transArray[i].transaction_type, transArray[i].receiver_id,
			                   transArray[i].debitCredit, transArray[i].amount, transArray[i].balance_after);
			        }
			    }
			}
			else{
				recv(sock,buffer,sizeof(buffer),0);
				printf("%s",buffer);

			}
		    memset(buffer, 0, sizeof(buffer));
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
            printf("\nInvalid Choice\n");
        }

	}
}


void manager_program(int sock){
	char buffer[1024];

	while(1){
		int choice;
		printf("\n-----------------------------------------------------------------------\n");
		printf("\nManager Menu:\n1. Activate/Deactivate Customer accounts\n2. Assign Loan Applications\n3. Review Customer Feedback\n4. Change Password\n5. Logout\nChoose an option:");
        printf("\n-----------------------------------------------------------------------\n");
        scanf("%d", &choice);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d", choice);
        send(sock, buffer, strlen(buffer), 0);

        if(choice==1){
        	//Activate/Deactivate Customer accounts
        	char customer_id[100];
            int result;
            printf("\nEnter Customer userID: ");
            scanf("%s", customer_id);
            printf("\n1->Activate ");
            printf("\n2->Deactivate ");
            printf("\nEnter your option: ");
            scanf("%d", &result);

            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s %d", customer_id, result);
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
        else if(choice==2){
        	//Assign Loan Applications
        	memset(buffer, 0, sizeof(buffer));
        	char temp[2000];
        	recv(sock, temp, sizeof(temp), 0);
        	printf("\nAll the Loan Applications:\n");
        	printf("\n%s",temp);
        	memset(buffer, 0, sizeof(buffer));
        	char employeeid[100];
            char loanid[110];

            printf("\nEnter LoanID: ");
            scanf("%s", loanid);
            printf("\nEnter the employeeid to which to assign: ");
            scanf("%s", employeeid);

            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s %s", employeeid, loanid);
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
        else if(choice==3){
        	//Review Customer feedback
        	memset(buffer, 0, sizeof(buffer));
        	char temp[1500];
		    
		    recv(sock,temp,sizeof(temp),0);
		    printf("%s\n",temp);
		    memset(buffer, 0, sizeof(buffer));
        }
        else if(choice==4){
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
        else if(choice==5){
        	//Logout
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
    	printf("\n-----------------------------BANK OF BHAI-----------------------------\n");
    	printf("\nEnter role \n1.Customer\n2. Employee\n3. Manager\n4. Administrator\n5. Exit \n\n");
    	printf("\n-----------------------------------------------------------------------\n");
    	scanf("%d",&role_choice);
    	snprintf(buffer,sizeof(buffer),"%d",role_choice);
    	send(sock,buffer,sizeof(buffer),0);
    	if(role_choice==5){
    		// The client program is closed
    		close(sock);
    		exit(0);
    	}

    	printf("\nEnter User ID: ");
        scanf("%s", userid);
        printf("\nEnter Password: ");
        scanf("%s", password);

        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%s %s",userid, password);
        
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        recv(sock,buffer,sizeof(buffer),0);
        
        printf("\n%s",buffer);
        printf("\n######################################################################\n");


        if (strstr(buffer, "Successful")) {
        	if(role_choice==1){
        		customer_program(sock);
        	}
        	else if(role_choice==2) {
        		employee_program(sock);
        	}
        	else if(role_choice==3) {
        		manager_program(sock);
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