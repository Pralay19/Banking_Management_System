/* 
====================================
* PROGRAM: Customer.h
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/
#ifndef Customer_H
#define Customer_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include "allStructures.h"

void add_new_customer(const char *customer_id, const char *password,int number_of_custm) {
    
    struct Customer new_customer;
    strcpy(new_customer.userid, customer_id);
    strcpy(new_customer.password, password);

    FILE *file = fopen("customers.txt", "a+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    int offset = sizeof(struct Customer) * number_of_custm;
    struct flock lock;
    int fd = fileno(file);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = offset;
    lock.l_len = sizeof(struct Customer);  
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    fseek(file, offset, SEEK_SET);
    if (fwrite(&new_customer, sizeof(struct Customer), 1, file) != 1) {
    printf("\nError writing new Customer to file\n");
    }
    fflush(file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);
}



#endif