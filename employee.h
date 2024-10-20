/* 
====================================
* PROGRAM: Customer.h
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/
#ifndef EMPLOYEE_H
#define EMPLPYEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>


#include "allStructures.h"
#include "customer.h"


int auth_user(char *customer_id){

	FILE *file = fopen("customers.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    struct flock lock;
    int fd = fileno(file);

    struct Customer who;
        fseek(file, 0, SEEK_SET);
        while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
	        if (strcmp(who.userid, customer_id) == 0 ) {
	            
                fclose(file);
	            return 1; // Authentication successful
	        }
    	}
        fclose(file);
        return 0;//Fail
}

int add_new_customer( char *customer_id, char *password,char *name,char *mobile) {


    if(auth_user(customer_id)){

    	return 0;
    }
    struct Customer new_customer;
    strcpy(new_customer.userid, customer_id);
    strcpy(new_customer.password, password);
    strcpy(new_customer.name, name);
    strcpy(new_customer.mobile, mobile);
    new_customer.balance=0;
    new_customer.active=1;
    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    struct flock lock;
    int fd = fileno(file);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = 0;
    lock.l_len = sizeof(struct Customer);  
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    fseek(file,0,SEEK_END);
    if (fwrite(&new_customer, sizeof(struct Customer), 1, file) != 1) {
    printf("\nError writing new Customer to file\n");
    }
    fflush(file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);

    initialize_transaction_file(customer_id);
    return 1;
}

int view_assigned_loans(char *employeeid,char *temp){

	struct Loan loan;
	struct flock lock;
	FILE *file = fopen("loans.txt", "r");
    if (file == NULL) {
        perror("Error opening transactions file");
        return -1; 
    }
    strcpy(temp,"");
    int fd = fileno(file);
    fseek(file, 0, SEEK_SET);
    int found=0;
    size_t offset = 0;

    lock.l_type = F_RDLCK; 
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; 
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return -1;  // Error while locking
    }


    while (fread(&loan, sizeof(struct Loan), 1, file) == 1) {
        if (strcmp(loan.employeeid, employeeid) == 0) {
        	found=1;
            char loan_info[500];
            char type[20];
            if(loan.status==1){
            	strcpy(type,"Approved");
            }
            else if(loan.status==2){
            	strcpy(type,"Processing");
            }
            else if(loan.status==-1){
            	strcpy(type,"Rejected");
            }
            snprintf(loan_info, sizeof(loan_info), "\n*UserID: %s, Loan Amount: %d, Status: %s\n",
                     loan.userid, loan.amount, type);

            size_t loan_info_len = strlen(loan_info);
                
            strcat(temp + offset, loan_info);
            offset += loan_info_len;
           
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(file);
    return found;
}

void process_loan(char*loanid,int decision){

	struct Loan loan;
	struct flock lock;
	FILE *file = fopen("loans.txt", "r+");
    if (file == NULL) {
        perror("Error opening transactions file");
        return ; 
    }
    
    int fd = fileno(file);
    fseek(file, 0, SEEK_SET);
    long position=0;


    while (fread(&loan, sizeof(struct Loan), 1, file) == 1) {
        if (strcmp(loan.loanid, loanid) == 0) {
        	position=ftell(file)-sizeof(struct Loan);


        	lock.l_type = F_WRLCK;  
		    lock.l_whence = SEEK_SET;
		    lock.l_start = position;
		    lock.l_len = sizeof(struct TransactionFile);  
		    lock.l_pid = getpid();

		    if (fcntl(fd, F_SETLKW, &lock) == -1) {
		        perror("Error locking file");
		        fclose(file);
		        return ;  
		    }

		    if(decision==1){
		    	loan.status=1;
		    }
		    else{
		    	loan.status=-1;
		    }


		    fseek(file,position,SEEK_SET);
		    if (fwrite(&loan, sizeof(struct Loan), 1, file) != 1) {
		    printf("\nError writing new loan to file\n");
		    }
		    fflush(file);
		    lock.l_type = F_UNLCK;
		    fcntl(fd, F_SETLKW, &lock);
        	break;
        }
    }

    

    fclose(file);
}

void change_password_emp(char*user_id,char*password){
	FILE *file = fopen("employees.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return ;
    }
    int fd = fileno(file);
    struct Employee who;
    struct flock lock;

    long position=0;
    // Find sender in the file
    fseek(file, 0, SEEK_SET); 
    while (fread(&who, sizeof(struct Employee), 1, file) == 1) {
        if (strcmp(who.userid, user_id) == 0) {
            position = ftell(file) - sizeof(struct Employee);
            break;
        }
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Employee);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking sender");
        fclose(file);
        return ;
    }
    strcpy(who.password, password);

    fseek(file, position, SEEK_SET);
    fwrite(&who, sizeof(struct Employee), 1, file);
        
    lock.l_start = position;  
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
}

void modify_custm(char*user_id,char*change,int decision){

	FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return ;
    }

    int fd = fileno(file);
    struct Customer who;
    struct flock lock;
    long position;

    // Searching in the file
        while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
            if (strcmp(who.userid, user_id) == 0) {
                    position = ftell(file) - sizeof(struct Customer);
                    lock.l_type = F_WRLCK;
                    lock.l_whence = SEEK_CUR;
                    lock.l_start = 0;
                    lock.l_len = -sizeof(struct Customer);
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLKW, &lock);          
                    if(decision==1){
                    	strcpy(who.name,change);
                    }
                    else{
                    	strcpy(who.mobile,change);
                    }

                    fseek(file, position, SEEK_SET);
                	fwrite(&who, sizeof(struct Customer), 1, file);
                	fflush(file);
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return ;
                    }
                    break;      
            }
       }

    fclose(file);

}

void view_customer(char*user_id,char*buffer){

	FILE *file = fopen("customers.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return ;
    }

    int fd = fileno(file);
    struct Customer who;
    struct flock lock;
    

    // Searching in the file
        while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
            if (strcmp(who.userid, user_id) == 0) {
                    
                    lock.l_type = F_RDLCK;// F_WRLCK for exclusive lock
                    lock.l_whence = SEEK_CUR;
                    lock.l_start = 0;
                    lock.l_len = -sizeof(struct Customer);
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLKW, &lock);// LOCKING the part where user block is present          
                    char info[300];
                    snprintf(info,sizeof(info),"%s %s",who.name,who.mobile);
                    strcat(buffer,info);
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return ;
                    }
                    fclose(file);
                    return ;
            }
       }
    fclose(file);
}

#endif