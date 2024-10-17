/* 
====================================
* PROGRAM: customer.h
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

#include "allStructures.h"

int view_account_balance(char* user_id) {

	FILE *file = fopen("customers.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
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
                    int balance=who.balance;
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return 1;
                    }
                    fclose(file);
                    return balance;  // Success
            }
       }
    fclose(file);

    return 0;
}

int deposit_money(char* user_id,int amount){

	FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int fd = fileno(file);
    struct Customer who;
    struct flock lock;
    long position;

    // Searching in the file
        while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
            if (strcmp(who.userid, user_id) == 0) {
                    position = ftell(file) - sizeof(struct Customer);
                    lock.l_type = F_WRLCK;// F_WRLCK for exclusive lock
                    lock.l_whence = SEEK_CUR;
                    lock.l_start = 0;
                    lock.l_len = -sizeof(struct Customer);
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLKW, &lock);// LOCKING the part where user block is present          
                    int balance=who.balance+amount;
                    who.balance+=amount;

                    fseek(file, position, SEEK_SET);
                	fwrite(&who, sizeof(struct Customer), 1, file);
                	fflush(file);
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return 1;
                    }
                    fclose(file);
                    return balance;  // Success
            }
       }
    fclose(file);

    return 0;
}

#endif