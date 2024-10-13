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
#include <arpa/inet.h>

#define PORT 8080

void customer_program(int sock, const char *sessionid) {
    int choice, amount;
    char buffer[1024], receiver_id[100];

    while (1) {
        printf("\nCustomer Menu:\n1. View Balance\n2. Deposit Money\n3. Withdraw Money\n4. Transfer Funds\n5. Apply for a Loan\n6. Change Password\n7. Adding Feedback\n8. View Transaction History\n9. Logout\nChoose an option: ");
        scanf("%d", &choice);
        snprintf(buffer, sizeof(buffer), "%d", choice);
        send(sock, buffer, strlen(buffer), 0);

        if (choice == 2) {
        	// Deposit Money
            printf("\nEnter amount to deposit: ");
            scanf("%d", &amount);
            snprintf(buffer, sizeof(buffer), "%d", amount);
            send(sock, buffer, strlen(buffer), 0);
            int balance_after;
            recv(sock,balance_after,sizeof(balance_after),0);
            printf("\nAvailable Balance: $%d",balance_after);
        }
        else if (choice == 4) {
        	// Transfer Funds
            printf("\nEnter receiver ID: ");
            scanf("%s", receiver_id);
            printf("\nEnter amount to transfer: ");
            scanf("%d", &amount);
            snprintf(buffer, sizeof(buffer), "%s %d", receiver_id, amount);
            send(sock, buffer, strlen(buffer), 0);
            recv(sock,buffer,strlen(buffer),0);
           	printf("\n%s",buffer);
        }
        else if (choice == 1) {
        	// View Balance
            snprintf(buffer, sizeof(buffer), "%s %d", receiver_id, amount);
            send(sock, buffer, strlen(buffer), 0);
            int balance_after;
            recv(sock,balance_after,sizeof(balance_after),0);
            printf("\nAvailable Balance: $%d",balance_after);
        }
        else if (choice == 3) {
        	// Withdraw Money
            printf("\nEnter receiver ID: ");
            scanf("%s", receiver_id);
            printf("\nEnter amount to transfer: ");
            scanf("%d", &amount);
            snprintf(buffer, sizeof(buffer), "%d", amount);
            send(sock, buffer, strlen(buffer), 0);

            recv(sock,buffer,sizeof(buffer),0);
            printf("\n%s",buffer);
        }
        else if (choice == 5) {
        	// Apply for Loan
            printf("\nEnter receiver ID: ");
            scanf("%s", receiver_id);
            printf("\nEnter amount to transfer: ");
            scanf("%d", &amount);
            snprintf(buffer, sizeof(buffer), "%s %d", receiver_id, amount);
            send(sock, buffer, strlen(buffer), 0);
            recv();
        }
        else if (choice == 6) {
        	// Change Password
        	char password[10];
            printf("\nEnter new password(10 characters): ");
            scanf("%s", password);
            snprintf(buffer, sizeof(buffer), "%s", password);
            send(sock, buffer, strlen(buffer), 0);
            recv(sock,buffer,sizeof(buffer),0);
            printf("%s",buffer);
        }
        else if (choice == 7) {
        	// Adding Feedback
        	char feedback[100];
            printf("\nEnter Feedback: ");
            scanf("%s", feedback);
            snprintf(buffer, sizeof(buffer), "%s", feedback);
            send(sock, buffer, strlen(buffer), 0);
            printf("\nAdded your Feedback.");
        }
        else if (choice == 9) {
        	// View transaction History
            printf("\nTransaction History: ");
            send(sock, buffer, strlen(buffer), 0);
            recv();
        }
        else {
        	// Logout
            break;
        }

        recv(sock, buffer, 1024, 0);
        printf("Server: %s\n", buffer);
    }
}

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[1024], userid[100], password[10];
    int role_choice;
    char sessionid[100];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected to server\n");

    while (1) {
        printf("Enter role \n1.Customer\n2. Employee\n3. Manager\n4. Administrator ");
        scanf("%d", &role_choice);
        printf("\nEnter User ID: ");
        scanf("%s", userid);
        printf("\nEnter Password: ");
        scanf("%s", password);

        snprintf(buffer, sizeof(buffer), "%d %s %s", role_choice, userid, password);
        send(sock, buffer, strlen(buffer), 0);

        recv(sock, buffer, 1024, 0);
        printf("Server: %s\n", buffer);

        if (strstr(buffer, "successful")) {
            customer_menu(sock, sessionid);
        }
    }

    close(sock);
    return 0;
}
