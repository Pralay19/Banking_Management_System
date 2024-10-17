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


int transfer_funds(char* user_id,char* receiver_id,int amount){

	struct Customer recvr;
	struct Customer sender;
	struct flock locks;
	struct flock lockr;

	FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }
    int fd = fileno(file);

    long position;
    // Searching in the file
    fseek(file,0,SEEK_SET);
    while (fread(&sender, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(sender.userid, user_id) == 0) {
        		position=ftell(file)-sizeof(struct Customer);
                locks.l_type = F_WRLCK;
                locks.l_whence = SEEK_CUR;
                locks.l_start = sizeof(struct Customer)*(-1);
                locks.l_len = sizeof(struct Customer);
                locks.l_pid = getpid();
                
        }
   }
	fseek(file,0,SEEK_SET);
	// searching receiver block in the file
	while (fread(&recvr, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(recvr.userid, receiver_id) == 0) {	          
    		lockr.l_type = F_WRLCK;
			lockr.l_whence = SEEK_CUR;
			lockr.l_start = sizeof(struct Customer)*(-1);
			lockr.l_len = sizeof(struct Customer);
			lockr.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lockr); 
			fcntl(fd, F_SETLKW, &locks);
            // Checking if sender has suffecient balance or not
            if(amount>sender.balance){
                return -1; // Fail
            }
			recvr.balance+=amount;
			sender.balance-=amount;
            // Updating the changes of receiver to file
            fseek(file, (-1)*sizeof(struct Customer), SEEK_CUR);
            fwrite(&recvr, sizeof(struct Customer), 1, file);// Writing the changes to file
            lockr.l_type = F_UNLCK; // Unlock the receiver part
    		if (fcntl(fd, F_SETLK, &lockr) == -1) {
        		perror("fcntl");
        		return 0;
    		}

            // Updating changes of sender to file
            fseek(file, position, SEEK_SET);
            fwrite(&sender, sizeof(struct Customer), 1, file);// Writing the changes to file
            fflush(file);// Ensuring data is written to disk
            locks.l_type = F_UNLCK; // Unlock the sender part
    		if (fcntl(fd, F_SETLK, &locks) == -1) {
        		perror("fcntl");
        		return 0;
    		}
            fclose(file);
            return sender.balance; // Success
        }
    }
    fclose(file);
    return 0; // Fail
}

#endif