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

int view_account_balance(long position) {
	FILE *file = fopen("customers.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int fd = fileno(file);
    struct Customer who;
    struct flock lock;
    fseek(file, position, SEEK_SET);// Setting the head to where user block is
    if (fread(&who, sizeof(struct Customer), 1, file) != 1){
        perror("Error reading user data");
        return 1;
    }
    // Locking the user block part
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Customer);   
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);

    lock.l_type = F_UNLCK; // Unlock the user part
    if (fcntl(fd, F_SETLCK, &lock) == -1) {
        perror("fcntl");
        return 1;
    }
    fclose(file);

    return who.balance;
}

int transfer_funds(long position,char* receiver_id,int amount){
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

    // Locking the sender block part
    locks.l_type = F_WRLCK;
    locks.l_whence = SEEK_SET;
    locks.l_start = position;
    locks.l_len = sizeof(struct Customer);
    locks.l_pid = getpid();
    

    fseek(file, position, SEEK_SET);// Setting the head to where user block is
    if (fread(&sender, sizeof(struct Customer), 1, file) != 1){
    	perror("Error reading user data");
    	return 0;
	}
	// Checking if sender has suffecient balance or not
	if(amount>sender.balance){
		return -1; // Fail
	}
	long positionRec=0;
	// searching receiver block in the file
	while (fread(&recvr, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(recvr.userid, receiver_id) == 0) {
        	positionRec=ftell(file);	          
    		lockr.l_type = F_WRLCK;
			lockr.l_whence = SEEK_SET;
			lockr.l_start = positionRec;
			lockr.l_len = sizeof(struct Customer);
			lockr.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lockr); // locking receiver part and if lock not acquired then wait
			fcntl(fd, F_SETLKW, &locks);
			recvr.balance+=amount;
			sender.balance-=amount;
            // Updating the changes of receiver to file
            fseek(file, -sizeof(struct Customer), SEEK_CUR);
            fwrite(&recvr, sizeof(struct Customer), 1, file);// Writing the changes to file
            lockr.l_type = F_UNLCK; // Unlock the receiver part
    		if (fcntl(fd, F_SETLCK, &lockr) == -1) {
        		perror("fcntl");
        		return 0;
    		}

            // Updating changes of sender to file
            fseek(file, position, SEEK_SET);
            fwrite(&sender, sizeof(struct Customer), 1, file);// Writing the changes to file
            fflush(file);// Ensuring data is written to disk
            locks.l_type = F_UNLCK; // Unlock the sender part
    		if (fcntl(fd, F_SETLCK, &locks) == -1) {
        		perror("fcntl");
        		return 0;
    		}
            fclose(file);
            return sender.balance; // Success
        }
    }

    return 0; // Fail
}

int withdraw_money(long position){
	int amount;
    printf("Enter amount to Withdraw: ");
    scanf("%d", &amount);

    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int fd = fileno(file);
    struct Customer who;
    struct flock lock;
    fseek(file, position, SEEK_SET);// Setting the head to where user block is
    if (fread(&who, sizeof(struct Customer), 1, file) != 1){
    	perror("Error reading user data");
    	return 0;
	}
	if(amount>who.balance){
		return -1;
	}
	// Locking the user block part
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
	who.balance-=amount;
	// Updating the changes to file
    fseek(file, -sizeof(struct Customer), SEEK_CUR);
    fwrite(&who, sizeof(struct Customer), 1, file);// Writing the changes to file
    fflush(file);// Ensuring data is written to disk
    lock.l_type = F_UNLCK; // Unlock the user part
    if (fcntl(fd, F_SETLCK, &lock) == -1) {
        perror("fcntl");
        return 0;
    }
    fclose(file);
    return who.balance;
}

int deposit_money(long position,int amount) {

    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int fd = fileno(file);
    struct Customer who;
    struct flock lock;

    fseek(file, position, SEEK_SET);// Setting the head to where user block is
    if (fread(&who, sizeof(struct Customer), 1, file) != 1){
    	perror("Error reading user data");
    	return 0;
	}

	// Locking the user block part
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
	who.balance+=amount;
	// Updating the changes to file
    fseek(file, -sizeof(struct Customer), SEEK_CUR);
    fwrite(&who, sizeof(struct Customer), 1, file);// Writing the changes to file

    lock.l_type = F_UNLCK; // Unlock the user part
	if (fcntl(fd, F_SETLCK, &lock) == -1) {
		perror("fcntl");
		return 0;
	}
	fflush(file);// Ensuring data is written to disk
    fclose(file);

    return who.balance;
}

int change_password(long position,char*password){

    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }
    int fd = fileno(file);
    struct Customer who;
    struct flock lock;

    fseek(file, position, SEEK_SET);// Setting the head to where user block is
    if (fread(&who, sizeof(struct Customer), 1, file) != 1){
    	perror("Error reading user data");
    	return 0;
	}

	// Locking the user block part
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
	who.password=password;
	// Updating the changes to file
    fseek(file, -sizeof(struct Customer), SEEK_CUR);
    fwrite(&who, sizeof(struct Customer), 1, file);// Writing the changes to file

    lock.l_type = F_UNLCK; // Unlock the user part
	if (fcntl(fd, F_SETLCK, &lock) == -1) {
		perror("fcntl");
		return 0;
	}
	fflush(file);// Ensuring data is written to disk
    fclose(file);
    return 1;
}

void add_feedback(char*feedback){

	struct flock lock;
	FILE *file = fopen("feedbacks.txt", "r+");
    if (file == NULL){
        perror("Error opening file");
        return;
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    fseek(file, 0, SEEK_END);
    fwrite(feedback, sizeof(feedback), 1, file);
    lock.l_type = F_UNLCK; // Unlock the user part
	if (fcntl(fd, F_SETLCK, &lock) == -1) {
		perror("fcntl");
		return;
	}
	fflush(file);
    fclose(file);
}

void view_transaction_history(){
	FILE *file = fopen("transactions.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    int fd = fileno(file);
    struct Transaction trans;

    fseek(file, 0, SEEK_SET);// Setting the head to where user block is
    if (fread(&trans, sizeof(struct Customer), 1, file) != 1){
    	perror("Error reading user data");
    	return 1;
	}
	
}

#endif
// int main(int argc,char *argv[]){
//     int choice;
//     string user_id=argv[1];
//     string password=argv[2];
//     char *position_str = argv[3];
//     long position = strtol(position_str, NULL, 10);// Position of the user block in file

//     //first we will have to set the head to the user block 
//     FILE *file = fopen("customers.txt", "r+");
//     if (file == NULL){
//         perror("Error opening file");
//         return 1;
//     }
//     // int fd = fileno(file);
//     // flock(fd, LOCK_EX);
//     fseek(file, position, SEEK_SET);
//     struct Customer custm;
//     if (fread(&custm, sizeof(struct Customer), 1, file) != 1){
//     	perror("Error reading user data");
//     	return 1;
// 	}

//     while (1){

//         printf("\n1. View Account Balance\n2. Deposit Money\n3. Withdraw Money\n4. Transfer Funds\n5. APPLY FOR LOAN\n6. Change Password\n7. Adding FeedBack\n8. View Transaction History\n9. Logout\nChoose an option: ");
//         scanf("%d", &choice);

//         switch (choice){
//             case 1:
//                 view_account_balance(custm)
//                 break;
//             case 2:
//                 deposit_money(position);
//                 break;
//             case 3:
//                 withdraw_money(position);
//                 break;
//             case 4:
//             	string reciever_id;
//             	int amount;
//             	printf("\nPlease enter the Receiver's ID: ");
//             	scanf("%s",&receiver_id);
//             	printf("\nPlease enter the amount to be Transferred: ");
//             	scanf("%d",&amount);
//                 transfer_funds(position,receiver_id,amount);
//                 break;
//             case 5:
//                 apply_for_loan();
//                 break;
//             case 6:
//                 change_password(position);
//                 break;
//             case 7:
//                 add_feedback();
//                 break;
//             case 8:
//                 view_transaction_history();
//                 break;
//             case 9:
//                 printf("Logging out...\n");
//                 // Here we have to make the customer offline

//                 exit(0);
//             default:
//                 printf("Invalid choice. Try again.\n");
//         }
//     }
//     return 0;
// }
