/* 
====================================
* PROGRAM: employee.h
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/
#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include "allstructures.h"

// Add new customer to the system
void add_new_customer(const char *userid, const char *password) {
    struct Customer new_customer;
    strcpy(new_customer.userid, userid);
    strcpy(new_customer.password, password);
    new_customer.balance = 0;

    FILE *file = fopen("customers.txt", "a");
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
    fwrite(&new_customer, sizeof(struct Customer), 1, file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    fclose(file);
}

// Modify customer details
void modify_customer_details(const char *userid, const char *new_password) {
    struct Customer customer;
    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    struct flock lock;
    int fd = fileno(file);
    // Locking the user part
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fd, F_SETLKW, &lock);
    while (fread(&customer, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(customer.userid, userid) == 0) {
            strcpy(customer.password, new_password);
            fseek(file, -sizeof(struct Customer), SEEK_CUR);
            fwrite(&customer, sizeof(struct Customer), 1, file);
            
            break;
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    fclose(file);
}

// Process loan application: either accept or reject
void process_loan_appl(const char *userid, bool accept) {
    struct Customer customer;
    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    // Need another file for the loan applications or new variable in struct Customer
    

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    fclose(file);
}

// View assigned loan applications (dummy function for now)
void view_assigned_loan_appl(const char *employee_id) {
    // Dummy implementation - In reality, you'd fetch assigned loan applications from a file.
    printf("Loan applications assigned to employee %s:\n", employee_id);
    printf("Loan Application 1: Pending\nLoan Application 2: Approved\n");
}

// Change employee password
void change_password_employee(long position, const char *new_password) {
    struct Employee employee;
    FILE *file = fopen("employees.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fseek(file, position, SEEK_SET);

    struct flock lock;
    int fd = fileno(file);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Employee);

    fcntl(fd, F_SETLKW, &lock);
    while (fread(&employee, sizeof(struct Employee), 1, file) == 1) {
        if (strcmp(employee.userid, userid) == 0) {
            strcpy(employee.password, new_password);
            fseek(file, -sizeof(struct Employee), SEEK_CUR);
            fwrite(&employee, sizeof(struct Employee), 1, file);
            printf("Employee password updated successfully.\n");
            break;
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    fclose(file);
}

// Logout function (dummy)
void logout() {
    printf("Logging out...\n");
}

#endif