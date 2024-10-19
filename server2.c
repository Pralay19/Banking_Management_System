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
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/sem.h>


#include "allStructures.h"
#include "employee.h"
#include "admin.h"
#include "customer.h"
#include "manager.h"


#define PORT 8080


struct Session {
    char userid[100];
    int sessionid;
};

struct Session *activeSessions;
int *sessionCount;

int sem_id;

struct sembuf sem_lock = {0, -1, 0}; 
struct sembuf sem_unlock = {0, 1, 0};

void lock() {
    semop(sem_id, &sem_lock, 1);
}

void unlock() {
    semop(sem_id, &sem_unlock, 1);
}



bool isOnline(char *userid) {
    lock();
    for (int i = 0; i < *sessionCount; i++) {
        if (strcmp(activeSessions[i].userid, userid) == 0) {
            unlock();
            return true;
        }
    }
    unlock();
    return false;
}

int add_session(char *userid) {
    lock();
    int sessionid = rand();  
    strcpy(activeSessions[*sessionCount].userid, userid);
    activeSessions[*sessionCount].sessionid = sessionid;
    (*sessionCount)++;  
    unlock();
    return sessionid;
}

void remove_session(char *userid) {
    lock();
    for (int i = 0; i < *sessionCount; i++) {
        if (strcmp(activeSessions[i].userid, userid) == 0) {
            
            for (int j = i; j < (*sessionCount) - 1; j++) {
                activeSessions[j] = activeSessions[j + 1];
            }
            (*sessionCount)--; 
            break;
        }
    }
    unlock();
}



int authentication(int role,char *userid,char *password){
	
	if(role==1){
		//CUSTOMER
        char filename1[100];
        FILE*file1;
		strcpy(filename1, "customers.txt");
		file1 = fopen(filename1, "r+");
        if (file1 == NULL) {
            perror("Error opening role file");
            return 0;
        }
        
        struct Customer who;
        fseek(file1, 0, SEEK_SET);
        while (fread(&who, sizeof(struct Customer), 1, file1) == 1) {
            printf("\nChecking %s %s",who.userid,who.password);
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            
                fclose(file1);
	            return 1; // Authentication successful
	        }
    	}
        fclose(file1);
        
	}
	else if(role==2){
		//EMPLOYEE
        char filename2[100];
        FILE*file2;
        printf("\nINSIDE EMP %s\n",userid);
		strcpy(filename2, "employees.txt");
		file2 = fopen(filename2, "r+");
        if (file2 == NULL) {
            printf("\nError opening role file\n");
            return 0;
        }
        struct Employee who;
        fseek(file2, 0, SEEK_SET);
        while (fread(&who, sizeof(struct Employee), 1, file2) == 1) {
	        printf("\nChecking %s %s",who.userid,who.password);
            if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            
                fclose(file2);
	            return 1; // Authentication successful
	        }
    	}
        fclose(file2);
	}
	else if(role==3){
		//MANAGER
        char filename3[100];
        FILE*file3;
		strcpy(filename3, "managers.txt");
		file3 = fopen(filename3, "r+");
        if (file3 == NULL) {
            perror("Error opening role file");
            return 0;
        }
        struct Manager who;
        fseek(file3, 0, SEEK_SET);
        while (fread(&who, sizeof(struct Manager), 1, file3) == 1) {
	        if (strcmp(who.userid, userid) == 0 && strcmp(who.password, password) == 0) {
	            printf("\nChecking %s %s",who.userid,who.password);
                fclose(file3);
	            return 1; // Authentication successful
	        }
    	}
        fclose(file3);
	}
	else if(role==4){
		//ADMIN
        char admin1[100]="pralay";
        char admin2[100];
        char pass[10]="123";
        if(strcmp(admin1,userid)==0 && strcmp(pass,password)==0){
            return 1;
        }
	}

	return 0;
}


void handle_client(int client_sock) {

    while(1){
    char buffer[1024];
    char userid[100], password[10];
    int bytes_received;
    int role;
    	//Receive role
        memset(buffer, 0, sizeof(buffer));
        recv(client_sock,buffer,sizeof(buffer),0);
        sscanf(buffer,"%d",&role);

        //Checking if client initiated TERMINATION PROCESS
        if (role==5){
            printf("\nClient requested termination\n");
            close(client_sock);
            return;
        }

        //Receive userid,password
        memset(buffer, 0, sizeof(buffer));
    	bytes_received=recv(client_sock, buffer, sizeof(buffer), 0);
    	if (bytes_received == 0) {
            printf("Client disconnected\n");
        } 
        else {
            //perror("recv");
        }
    	printf("\n%s -> %d\n",buffer,role);
        
    	//Initializing Role,Userid and Password
    	sscanf(buffer, "%s %s", userid, password);

    	//check if the user is online or not
        if(isOnline(userid)){
            strcpy(buffer,"User already logged in.");
            send(client_sock,buffer,sizeof(buffer),0);
            continue;
        }

    	//Authenticate the user
    	int auth=authentication(role,userid,password);
        printf("\nAuth done %d",role);
    	if(auth==0){
    		snprintf(buffer,sizeof(buffer),"Invalid Credentials!");
    		send(client_sock,buffer,sizeof(buffer),0);
    		memset(buffer, 0, sizeof(buffer));
    		continue;
    	}
        memset(buffer, 0, sizeof(buffer));
        int sessionid = add_session(userid);
    	snprintf(buffer,sizeof(buffer),"\nLogin Successful !\nNamaskaram %s\n Session Id:%d\n",userid,sessionid);
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
                            snprintf(buffer,sizeof(buffer),"Bhai Paise nahi hai tere paas itne!.");
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
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==6){
                    //Change password
                    char new_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s", new_password);
                    int result=change_password_custm(userid, new_password);
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
    				char new_customer_id[100], new_customer_password[10],name[50],mobile[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s %s %s %s", new_customer_id, new_customer_password,name,mobile);
                    add_new_customer(new_customer_id, new_customer_password,name,mobile);

                    strcpy(buffer,"New Customer Added.");
                    send(client_sock, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
    			}
                else if(choice==2){
                    //Modify Customer Details
                    int decision;
                    char change[100],cust_id[100];
                    sscanf(buffer,"%s",cust_id);
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));

                    view_customer(cust_id,buffer);
                    send(client_sock,buffer,sizeof(buffer),0);

                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock,buffer,sizeof(buffer),0);
                    sscanf(buffer,"%d %s",&decision,change);
                    modify_custm(cust_id,change,decision);
                    memset(buffer, 0, sizeof(buffer));
                    snprintf(buffer,sizeof(buffer),"Customer Details Modified!");
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==3){
                    //Process Loan Application
                    char loanid[110];
                    int decision;
                    
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock,buffer,sizeof(buffer),0);

                    memset(buffer, 0, sizeof(buffer));
                    sscanf(buffer,"%s",loanid);
                    search_loan(loanid,buffer);
                    send(client_sock,buffer,sizeof(buffer),0);

                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock,buffer,sizeof(buffer),0);
                    sscanf(buffer,"%d",&decision);

                    process_loan(loanid,decision);
                    memset(buffer, 0, sizeof(buffer));

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
                    char new_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s", new_password);
                    change_password_emp(userid, new_password);
                    memset(buffer, 0, sizeof(buffer));
                    snprintf(buffer,sizeof(buffer),"\nPassword changed successfully.");
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
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
            while(1){
                memset(buffer, 0, sizeof(buffer));
                recv(client_sock,buffer,sizeof(buffer),0);
                int choice;
                sscanf(buffer,"%d",&choice);


                if(choice==1){
                    //Activate/Deactivate Customer accounts
                    char customer_id[100];
                    int decision;
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s %d", customer_id, &decision);
                    memset(buffer, 0, sizeof(buffer));
                    int result=activate_d_user(userid, decision);
                    if(result>0){
                        if(decision==2){
                            memset(buffer, 0, sizeof(buffer));
                            snprintf(buffer,sizeof(buffer),"\nCustomer Account successfully deactivated.");
                            
                            send(client_sock,buffer,sizeof(buffer),0);
                            memset(buffer, 0, sizeof(buffer));
                        }
                        else if(decision==1){
                            memset(buffer, 0, sizeof(buffer));
                            snprintf(buffer,sizeof(buffer),"\nCustomer Account successfully activated.");
                            
                            send(client_sock,buffer,sizeof(buffer),0);
                            memset(buffer, 0, sizeof(buffer));
                        }
                    }
                    else if(result<0){
                        if(decision==2){
                            memset(buffer, 0, sizeof(buffer));
                            snprintf(buffer,sizeof(buffer),"\nCustomer Account already deactivated.");
                            
                            send(client_sock,buffer,sizeof(buffer),0);
                            memset(buffer, 0, sizeof(buffer));
                        }
                        else if(decision==1){
                            memset(buffer, 0, sizeof(buffer));
                            snprintf(buffer,sizeof(buffer),"\nCustomer Account already activated.");
                            
                            send(client_sock,buffer,sizeof(buffer),0);
                            memset(buffer, 0, sizeof(buffer));
                        }
                    }
                    else{
                        
                        snprintf(buffer,sizeof(buffer),"\nNo such Account found. Please check the User Id.");
                        send(client_sock,buffer,sizeof(buffer),0);
                    }
                }
                else if(choice==2){
                    //Assign Loan Applications
                    char employeeid[100];
                    char loanid[110];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s %s", employeeid, loanid);
                    memset(buffer, 0, sizeof(buffer));
                    int result=assign_loan_appl(loanid,employeeid);

                    if(result){
                        memset(buffer, 0, sizeof(buffer));
                        snprintf(buffer,sizeof(buffer),"\nLoan application:%s has been assigned to EmployeeId:%s",loanid,employeeid);
                        send(client_sock,buffer,sizeof(buffer),0);
                        memset(buffer, 0, sizeof(buffer));
                    }
                    else{
                        memset(buffer, 0, sizeof(buffer));
                        snprintf(buffer,sizeof(buffer),"\nLoan Application not found.");
                        send(client_sock,buffer,sizeof(buffer),0);
                        memset(buffer, 0, sizeof(buffer));
                    }
                }
                else if(choice==3){
                    //Review Customer feedback
                    memset(buffer, 0, sizeof(buffer));
                    char temp[1500];

                    view_feedback(temp);
                    send(client_sock,temp,sizeof(temp),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==4){
                    //Change Password
                    char new_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s", new_password);
                    change_password_man(userid, new_password);
                    snprintf(buffer,sizeof(buffer),"\nPassword changed successfully.");
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==5){
                    //Logout
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

                } 
                else if(choice==4){
                    //Manage user roles
                    char employee_id[100];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s", employee_id);

                    manage_roles(employee_id);
                    snprintf(buffer,sizeof(buffer),"The Employee has been Promoted to Manager!");
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==5){
                    //Change Password
                    char new_password[10];
                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock, buffer, sizeof(buffer), 0);
                    sscanf(buffer, "%s", new_password);
                    change_password_admin(userid, new_password);
                    memset(buffer, 0, sizeof(buffer));
                    snprintf(buffer,sizeof(buffer),"\nPassword changed successfully.");
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==6){
                    //Modify Customer Details
                    int decision;
                    char change[100],cust_id[100];

                    memset(buffer, 0, sizeof(buffer));
                    recv(client_sock,buffer,sizeof(buffer),0);
                    sscanf(buffer,"%d %s %s",&decision,cust_id,change);
                    modify_custm(cust_id,change,decision);
                    memset(buffer, 0, sizeof(buffer));
                    snprintf(buffer,sizeof(buffer),"Customer Details Modified!");
                    send(client_sock,buffer,sizeof(buffer),0);
                    memset(buffer, 0, sizeof(buffer));
                }
                else if(choice==7){
                    //Add new admin

                }
                else {
                    // Invalid choice
                    
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


    //declaring a shared memory
    int shm_id = shmget(IPC_PRIVATE, sizeof(struct Session) * 100 + sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    //Attach shared memory 
    activeSessions = shmat(shm_id, NULL, 0);
    sessionCount = (int *)(activeSessions + 100); 
    *sessionCount = 0;

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
            close(server_sock);
            handle_client(new_sock);
            exit(0); 
        } else { 
            
            close(new_sock); // Close the child's socket
        }

        printf("New Client connected\n");
        
    }
    close(server_sock);
    shmdt(activeSessions);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);


}