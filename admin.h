/* 
====================================
* PROGRAM: employee.h
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
#include "allstructures.h"

// Add a new employee to the system
void add_new_employee(const char *employee_id, const char *password) {
    struct Employee new_employee;
    strcpy(new_employee.userid, employee_id);
    strcpy(new_employee.password, password);
    new_employee.active = true;

    FILE *file = fopen("employees.txt", "a");
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
    fwrite(&new_employee, sizeof(struct Employee), 1, file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);
}

#endif