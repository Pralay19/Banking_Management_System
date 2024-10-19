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

// void manage_roles(){
    
// }


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