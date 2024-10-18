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
    while (fread(&loan, sizeof(struct Loan), 1, file) == 1) {
        if (strcmp(loan.employeeid, employeeid) == 0) {
        	found=1;
            char loan_info[280];
            snprintf(loan_info, sizeof(loan_info), "UserID: %s, EmployeeID: %s, Amount: %d, Status: %d\n",
                     loan.userid, loan.employeeid, loan.amount, loan.status);

            size_t loan_info_len = strlen(loan_info);
                
            strcat(buffer + offset, loan_info);
            offset += loan_info_len;
           
        }
    }
    fclose(file);
    return found;
}


#endif