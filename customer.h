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

int withdraw_money(char* user_id,int amount){

	FILE *file = fopen("customers.txt", "r+");
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
                    lock.l_type = F_WRLCK;// F_WRLCK for exclusive lock
                    lock.l_whence = SEEK_CUR;
                    lock.l_start = (-1)*sizeof(struct Customer);
                    lock.l_len = sizeof(struct Customer);
                    lock.l_pid = getpid();
                    fcntl(fd, F_SETLKW, &lock);// LOCKING the part where user block is present          
                    if(who.balance<amount){
                    	return -1;//Insufficient Balance
                    }
                    int balance=who.balance-amount;
                    who.balance-=amount;

                    fseek(file, (-1)*sizeof(struct Customer), SEEK_SET);
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

int transfer_funds(char* user_id, char* receiver_id, int amount) {

    struct Customer sender, receiver;
    struct flock lock;
    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }
    int fd = fileno(file);

    long sender_position = -1, receiver_position = -1;
    int sender_found = 0, receiver_found = 0;

    // Find sender in the file
    fseek(file, 0, SEEK_SET); 
    while (fread(&sender, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(sender.userid, user_id) == 0) {
            sender_position = ftell(file) - sizeof(struct Customer);
            sender_found = 1;
            break;
        }
    }

    if (!sender_found) {
        fclose(file);
        printf("Sender not found\n");
        return 0;
    }

    // Find receiver in the file
    fseek(file, 0, SEEK_SET); 
    while (fread(&receiver, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(receiver.userid, receiver_id) == 0) {
            receiver_position = ftell(file) - sizeof(struct Customer);
            receiver_found = 1;
            break;
        }
    }

    if (!receiver_found) {
        fclose(file);
        printf("Receiver not found\n");
        return 0;
    }

    //checking if suffiecient balance
    if (sender.balance < amount) {
        fclose(file);
        printf("Insufficient funds\n");
        return -1;
    }

    // Lock the sender
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = sender_position;
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking sender");
        fclose(file);
        return 0;
    }

    // Lock the receiver
    lock.l_start = receiver_position;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking receiver");
        lock.l_start = sender_position;  
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        fclose(file);
        return 0;
    }

    
    sender.balance -= amount;
    receiver.balance += amount;

    //Write updated sender back to the file
    fseek(file, sender_position, SEEK_SET);
    if (fwrite(&sender, sizeof(struct Customer), 1, file) != 1) {
        perror("Error updating sender");
        lock.l_start = sender_position;  
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        lock.l_start = receiver_position;  
        fcntl(fd, F_SETLK, &lock);
        fclose(file);
        return 0;
    }

    //Write updated receiver back to the file
    fseek(file, receiver_position, SEEK_SET);
    if (fwrite(&receiver, sizeof(struct Customer), 1, file) != 1) {
        perror("Error updating receiver");
        lock.l_start = sender_position;  
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        lock.l_start = receiver_position;  
        fcntl(fd, F_SETLK, &lock);
        fclose(file);
        return 0;
    }

    //Unlock both sender and receiver
    lock.l_type = F_UNLCK;
    lock.l_start = sender_position;
    fcntl(fd, F_SETLK, &lock);
    lock.l_start = receiver_position;
    fcntl(fd, F_SETLK, &lock);

    
    fclose(file);

    return sender.balance;  
}

int change_password(char*user_id,char*password){

	FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }
    int fd = fileno(file);
    struct Customer who;
    struct flock lock;

    long position=-1;
    // Find sender in the file
    fseek(file, 0, SEEK_SET); 
    while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(who.userid, user_id) == 0) {
            position = ftell(file) - sizeof(struct Customer);
            break;
        }
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking sender");
        fclose(file);
        return 0;
    }
    strcpy(who.password, password);

    fseek(file, position, SEEK_SET);
    fwrite(&who, sizeof(struct Customer), 1, file);
        
    lock.l_start = position;  
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
    return 1;

}

#endif