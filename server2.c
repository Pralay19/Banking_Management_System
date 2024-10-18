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
#include "customer.h"

#define PORT 8080


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
        printf("\nFile Opened");
        struct Customer who;
        fseek(file, 0, SEEK_SET);
        while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
            printf("\nChecking %s %s",who.userid,who.password);
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            
                fclose(file);
	            return 1; // Authentication successful
	        }
    	}
        fclose(file);
        printf("\nUser Not Found");
	}
	else if(role==2){
		//EMPLOYEE
        printf("\nINSIDE EMP %s\n",userid);
		strcpy(filename, "employees.txt");
		file = fopen(filename, "r");
        if (file == NULL) {
            printf("\nError opening role file\n");
            return 0;
        }
        struct Employee who;
        fseek(file, 0, SEEK_SET);
        while (fread(&who, sizeof(struct Employee), 1, file) == 1) {
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            
                fclose(file);
	            return 1; // Authentication successful
	        }
    	}
        fclose(file);
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
        fseek(file, 0, SEEK_SET);
        while (fread(&who, sizeof(struct Manager), 1, file) == 1) {
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            
                fclose(file);
	            return 1; // Authentication successful
	        }
    	}
        fclose(file);
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
            //perror("recv");
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
        if(isOnline(userid)){
            strcpy(buffer,"User already logged in.");
            send(buffer,sizeof(buffer),0);
            continue;
        }

    	//Authenticate the user
    	int auth=authentication(role,userid,password);
        printf("\nAuth done %d",auth);
    	if(auth==0){
    		snprintf(buffer,sizeof(buffer),"Invalid Credentials!");
    		send(client_sock,buffer,sizeof(buffer),0);
    		memset(buffer, 0, sizeof(buffer));
    		continue;
    	}
        memset(buffer, 0, sizeof(buffer));
        int sessionid = add_session(userid);
    	snprintf(buffer,sizeof(buffer),"\nLogin Successful !\nWelcome %s\n Session Id:%d",userid,sessionid);
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
                    int amount=0;
                    amount=view_account_balance(userid);
                    snprintf(buffer,sizeof(buffer),"Available Balance: $%d",amount);
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==2){
                    //Deposit Money
                    int amount;
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%d", &amount);
                    int result=deposit_money(userid,amount);
                    char*msg1="\nAvailable Balance: $";
                    snprintf(buffer,sizeof(buffer),"%s%d",msg1,result);
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==3){
                    //Withdraw Money
                    int amount;
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%d",&amount);
                    int result=withdraw_money(userid,amount);
                    if(result>0){
                        char*msg1="\nAvailable Balance: $";
                        snprintf(buffer,sizeof(buffer),"%s%d",msg1,result);
                        send(client_sock,buffer,sizeof(buffer),0);
                    }
                    else{
                        char*msg1="\nInsufficient Balance.";
                        send(client_sock,msg1,sizeof(msg1),0);
                    }
                    memset(buffer, 0, sizeof(buffer));
                    
                }
                else if(choice==4){
                    //Transfer Funds
                    char receiver_id[100];
                    int amount;
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s %d", receiver_id, &amount);
                    memset(buffer, 0, sizeof(buffer));
                    int result=transfer_funds(userid, receiver_id, amount);
                    if(result!=0){
                        if(result>0){
                            memset(buffer, 0, sizeof(buffer));
                            snprintf(buffer,sizeof(buffer),"\nTransfer successful. Available Balance: $%d",result);
                            //printf("\n%s\n",buffer);
                            send(client_sock,buffer,sizeof(buffer),0);
                            memset(buffer, 0, sizeof(buffer));
                        }
                        else{
                            memset(buffer, 0, sizeof(buffer));
                            snprintf(buffer,sizeof(buffer),"Insuffecient Balance.");
                            send(client_sock,buffer,sizeof(buffer),0);
                            memset(buffer, 0, sizeof(buffer));
                        }
                    }
                    else{
                        
                        snprintf(buffer,sizeof(buffer),"\nNo such Account found. Please check the User Id of the receiver and try again later.");
                        send(client_sock,buffer,sizeof(buffer),0);
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==5){
                    //Apply for a loan
                    int amount;
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%d",&amount);
                    apply_for_loan(userid,amount);
                    memset(buffer, 0, sizeof(buffer));
                    strcpy(buffer,"\nYour Loan application has been submitted.\n");
                    send(buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==6){
                    //Change password
                    char new_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s", new_password);
                    int result=change_password(userid, new_password);
                    if(result){
                        memset(buffer, 0, sizeof(buffer));
                        snprintf(buffer,sizeof(buffer),"\nPassword changed successfully.");
                        send(client_sock,buffer,sizeof(buffer),0);
                    }
                    else{
                        memset(buffer, 0, sizeof(buffer));
                        snprintf(buffer,sizeof(buffer),"\nError in database");
                        send(client_sock,buffer,sizeof(buffer),0);
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==7){
                    //Add a feedback
                    char feedback[300];
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    strcpy(feedback,buffer);
                    memset(buffer, 0, sizeof(buffer));
                    add_feedback(userid,feedback);
                    snprintf(buffer,sizeof(buffer),"\nAdded your feedback.\n");
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==8){
                    //View Transactions
                    memset(buffer, 0, sizeof(buffer));
                    struct Transaction transArray[100];
                    memset(transArray, 0, sizeof(transArray));

                    int result = view_transaction(userid, transArray);
                    snprintf(buffer,sizeof(buffer),"%d",result);
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                    if (result==0) {
                        
                        snprintf(buffer,sizeof(buffer),"No transactions found or an error occurred.");
                        send(client_sock, buffer, sizeof(buffer), 0);
                        memset(buffer, 0, sizeof(buffer));
                        
                    }
                    else{
                        send(client_sock, transArray, sizeof(transArray), 0);
                        memset(buffer, 0, sizeof(buffer));
                    }
                }
                else if(choice==9){
                    // Logout
                    remove_session(userid);
                    char *msg = "Logged Out";
                    send(client_sock, msg, strlen(msg), 0);
                    break;

                }
                else{
                    //Invalid Choice

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
                    add_new_customer(new_customer_id, new_customer_password);

                    strcpy(buffer,"New Customer Added.");
                    send(client_sock, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
    			}
                else if(choice==2){
                    //Modify Customer Details
                }
                else if(choice==3){
                    //Process Loan Application
                }
                else if(choice==4){
                    //View Assigned Loan Applications
                    memset(buffer, 0, sizeof(buffer));
                    char temp[1500];

                    int result = view_assigned_loans(userid, temp);
                    snprintf(buffer,sizeof(buffer),"%d",result);
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                    if (result==0) {
                        
                        snprintf(buffer,sizeof(buffer),"No Loans found or an error occurred.");
                        send(client_sock, buffer, sizeof(buffer), 0);
                        memset(buffer, 0, sizeof(buffer));
                        
                    }
                    else{
                        send(client_sock, temp, sizeof(temp), 0);
                        memset(buffer, 0, sizeof(buffer));
                    }
                }
                else if(choice==5){
                    //Change Password
                }
    			else if(choice==6){
    				// Logout
                    remove_session(userid);
                    
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
                    add_new_employee(new_employee_id, new_employee_password);

                    memset(buffer, 0, sizeof(buffer));
                    strcpy(buffer,"New Employee Added.");
                    send(client_sock, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));

                } 
                else if (choice == 2) {
                    // Logout
                    remove_session(userid);
                    char *msg = "Logged Out";
                    send(client_sock, msg, strlen(msg), 0);
                    break;
                } 
                else if (choice == 3) {
                    // Add new Manager
                    char new_manager_id[100], new_manager_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s %s", new_manager_id, new_manager_password);
                    add_new_manager(new_manager_id, new_manager_password);

                    memset(buffer, 0, sizeof(buffer));
                    strcpy(buffer,"New Manager Added.");
                    send(client_sock, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));

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