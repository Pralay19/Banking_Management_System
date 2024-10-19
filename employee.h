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

void add_new_customer( char *customer_id, char *password) {
    
    struct Customer new_customer;
    strcpy(new_customer.userid, customer_id);
    strcpy(new_customer.password, password);
    new_customer.balance=0;
    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
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
}

int view_assigned_loans(char *employeeid,char *buffer){

	struct Loan loan;
	struct flock lock;
	FILE *file = fopen("loans.txt", "r");
    if (file == NULL) {
        perror("Error opening transactions file");
        return -1; 
    }
    strcpy(buffer,"");
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
            char loan_info[280];
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
            snprintf(loan_info, sizeof(loan_info), "*UserID: %s, Loan Amount: %d, Status: %s\n",
                     loan.userid, loan.amount, type);

            size_t loan_info_len = strlen(loan_info);
                
            strcat(buffer + offset, loan_info);
            offset += loan_info_len;
           
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(file);
    return found;
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


#endif