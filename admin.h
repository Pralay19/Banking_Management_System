/* 
====================================
* PROGRAM: admin.h
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/

#ifndef ADMIN_H
#define ADMIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include "allStructures.h"


// Add a new employee to the system
void add_new_employee(const char *employee_id, const char *password) {
    
    struct Employee new_employee;
    strcpy(new_employee.userid, employee_id);
    strcpy(new_employee.password, password);

    FILE *file = fopen("employees.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    struct flock lock;
    int fd = fileno(file);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = 0;
    lock.l_len = sizeof(struct Employee);  
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    fseek(file,0,SEEK_END);
    if (fwrite(&new_employee, sizeof(struct Employee), 1, file) != 1) {
    printf("\nError writing new employee to file\n");
    }
    fflush(file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);
}

void add_new_manager(const char *manager_id, const char *password) {
    
    struct Manager man;
    strcpy(man.userid, manager_id);
    strcpy(man.password, password);

    FILE *file = fopen("managers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    struct flock lock;
    int fd = fileno(file);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = 0;
    lock.l_len = sizeof(struct Manager);  
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    fseek(file,0,SEEK_END);
    if (fwrite(&man, sizeof(struct Manager), 1, file) != 1) {
    printf("\nError writing new manger to file\n");
    }
    fflush(file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);
}

void manage_roles(char*employeeid){
    
    //Removing the Loans assigned
    struct Loan loan;
    struct flock lock;
    FILE *file = fopen("loans.txt", "r");
    if (file == NULL) {
        perror("Error opening transactions file");
        return ; 
    }
    
    int fd = fileno(file);
    fseek(file, 0, SEEK_SET);
    long position=0;

    while (fread(&loan, sizeof(struct Loan), 1, file) == 1) {
        if (strcmp(loan.employeeid, employeeid) == 0 && loan.status==2) {
            position=ftell(file)-sizeof(struct Loan);
            lock.l_type = F_WRLCK; 
            lock.l_whence = SEEK_SET;
            lock.l_start = position;
            lock.l_len = sizeof(struct Loan); 
            lock.l_pid = getpid();

            if (fcntl(fd, F_SETLKW, &lock) == -1) {
                perror("Error locking file");
                fclose(file);
                return ;  // Error while locking
            }

            strcpy(loan.employeeid,"None");

            fseek(file,position,SEEK_SET);
            if (fwrite(&loan, sizeof(struct Loan), 1, file) != 1) {
            printf("\nError writing new loan to file\n");
            }
            fflush(file);
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &lock);

        }
    }
    fclose(file);

    //removing from emplyoees
    char password[10];
    FILE*file2;
    file2 = fopen("employees.txt", "r+");
    if (file2 == NULL) {
        printf("\nError opening role file\n");
        return;
    }
    fd=fileno(file2);
    struct Employee who;

    fseek(file2, 0, SEEK_SET);
    position=0;
    while (fread(&who, sizeof(struct Employee), 1, file2) == 1) {
        if (strcmp(who.userid, employeeid) == 0) {
            position=ftell(file2)-sizeof(struct Employee);
            break;
        }
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Admin);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking sender");
        fclose(file2);
        return;
    }
    strcpy(who.userid,"#");
    strcpy(password,who.password);

    fseek(file, position, SEEK_SET);
    fwrite(&who, sizeof(struct Customer), 1, file);
    fflush(file);

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);


    //Editing the manager file

    add_new_manager(employeeid,password);

}


void change_password_admin(char*user_id,char*password){
    FILE *file = fopen("admins.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return ;
    }
    int fd = fileno(file);
    struct Admin who;
    struct flock lock;

    long position=0;
    // Find sender in the file
    fseek(file, 0, SEEK_SET); 
    while (fread(&who, sizeof(struct Admin), 1, file) == 1) {
        if (strcmp(who.userid, user_id) == 0) {
            position = ftell(file) - sizeof(struct Admin);
            break;
        }
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Admin);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking sender");
        fclose(file);
        return ;
    }
    strcpy(who.password, password);

    fseek(file, position, SEEK_SET);
    fwrite(&who, sizeof(struct Admin), 1, file);
        
    lock.l_start = position;  
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
}

#endif