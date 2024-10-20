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



int view_transaction(char *userid, struct Transaction transArray[100]);

void update_transaction(char *userid, char *transaction_type, char *receiver_id, char *debitCredit, int amount,int current_balance); 

void initialize_transaction_file(char*userid);

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
                    char type[20];
                    strcpy(type,"deposit");
                    char recvr[100];
                    strcpy(recvr,"-");
                    char debitCredit[10];
                    strcpy(debitCredit,"credit");
                    update_transaction(user_id,type,recvr,debitCredit,amount,who.balance);
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
    long position=0;
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
                    if(who.balance<amount){
                    	return -1;//Insufficient Balance
                    }
                    int balance=who.balance-amount;
                    who.balance-=amount;

                    fseek(file, position, SEEK_SET);
                	fwrite(&who, sizeof(struct Customer), 1, file);
                	fflush(file);
                    lock.l_type = F_UNLCK; // Unlock
                    if (fcntl(fd, F_SETLK, &lock) == -1) {
                        perror("fcntl");
                        return 1;
                    }
                    fclose(file);
                    char type[20];
                    strcpy(type,"withdraw");
                    char recvr[100];
                    strcpy(recvr,"-");
                    char debitCredit[10];
                    strcpy(debitCredit,"debit");
                    update_transaction(user_id,type,recvr,debitCredit,amount,who.balance);
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
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    

    if(sender.userid<receiver.userid){
    	//lock the sender
    	lock.l_start = sender_position;
    	fcntl(fd, F_SETLKW, &lock);

    	// Lock the receiver
    	lock.l_start = receiver_position;
    	fcntl(fd, F_SETLKW, &lock);



    }
    else{
    	// Lock the receiver
    	lock.l_start = receiver_position;
    	fcntl(fd, F_SETLKW, &lock);
    	
    	//lock the sender
    	fcntl(fd, F_SETLKW, &lock);
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

    if(sender.userid<receiver.userid){
    	lock.l_type = F_UNLCK;
    	lock.l_start = sender_position;
    	fcntl(fd, F_SETLK, &lock);

    	lock.l_start = receiver_position;
    	fcntl(fd, F_SETLK, &lock);

    }
    else{
    	lock.l_type = F_UNLCK;
    	lock.l_start = receiver_position;
    	fcntl(fd, F_SETLK, &lock);

    	lock.l_start = sender_position;
    	fcntl(fd, F_SETLK, &lock);
    }


    
    fclose(file);
    char type[20];
    strcpy(type,"fund transfer");
    char debitCredit[10];
    strcpy(debitCredit,"debit");
    update_transaction(user_id,type,receiver_id,debitCredit,amount,sender.balance);
    strcpy(debitCredit,"credit");
    update_transaction(receiver_id,type,user_id,debitCredit,amount,receiver.balance);
    return sender.balance;  

}

int change_password_custm(char*user_id,char*password){

	FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }
    int fd = fileno(file);
    struct Customer who;
    struct flock lock;

    long position=0;
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

void add_feedback(char*user_id,char*feedback){

	FILE *file = fopen("feedbacks.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    struct Feedback feed;
    struct flock lock;
    int fd = fileno(file);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = 0;
    lock.l_len = sizeof(struct Feedback);  
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);

    strcpy(feed.userid,user_id);
    strcpy(feed.feedback,feedback);

    fseek(file,0,SEEK_END);
    if (fwrite(&feed, sizeof(struct Feedback), 1, file) != 1) {
    printf("\nError writing new employee to file\n");
    }
    fflush(file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);
}


void initialize_transaction_file(char*userid){

	struct TransactionFile transFile;
    struct flock lock;
    FILE *file = fopen("transactions.txt", "r+");
    
    if (file == NULL) {
        perror("Error opening transactions file");
        return;
    }

    int fd = fileno(file);

    // Initialize the TransactionFile for the user
    strcpy(transFile.userid, userid);
    for (int i = 0; i < 100; i++) {
        transFile.transactions[i].transac_id = 0; 
    }

    
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = 0;
    lock.l_len = sizeof(struct TransactionFile);  
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return;
    }

    // Check if the user already has a TransactionFile
    struct TransactionFile temp;
    fseek(file, 0, SEEK_SET);
    int found = 0;
    while (fread(&temp, sizeof(struct TransactionFile), 1, file) == 1) {
        if (strcmp(temp.userid, userid) == 0) {
            found = 1;
            break;
        }
    }
    
    if (found) {
        
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock); 
        fclose(file);
        return;
    }

    // Write the new TransactionFile to the file
    fseek(file, 0, SEEK_END); 
    if (fwrite(&transFile, sizeof(struct TransactionFile), 1, file) != 1) {
        perror("Error writing new transaction file");
    }

    fflush(file);

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
}

void update_transaction(char *userid, char *transaction_type, char *receiver_id, char *debitCredit, int amount,int current_balance) {
    
    struct TransactionFile transFile;
    struct Transaction newTransaction;
    struct flock lock;
    
    FILE *file = fopen("transactions.txt", "r+");
    if (file == NULL) {
        perror("Error opening transactions file");
        return;
    }

    int fd = fileno(file);

    // Initialize the new transaction
     
    strcpy(newTransaction.userid, userid);
    strcpy(newTransaction.transaction_type, transaction_type);
    strcpy(newTransaction.receiver_id, receiver_id);
    strcpy(newTransaction.debitCredit, debitCredit);
    newTransaction.amount = amount;

    // Get the balance after the transaction
    newTransaction.balance_after=current_balance;

    
    

    // Search for the user in the file
    fseek(file, 0, SEEK_SET);
    long position=0;
    while (fread(&transFile, sizeof(struct TransactionFile), 1, file) == 1) {
        if (strcmp(transFile.userid, userid) == 0) {
        	position=ftell(file)-sizeof(struct TransactionFile);
            break;
        }
    }


	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct TransactionFile);  
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return;
    }

    // Find the next available slot in the transaction array
    int i;
    for (i = 0; i < 100; i++) {
        if (transFile.transactions[i].transac_id == 0) {
        	newTransaction.transac_id = i+1;
            transFile.transactions[i] = newTransaction;
            break;
        }
    }

   

    // Write the updated TransactionFile back to the file
    fseek(file, position, SEEK_SET);
    if (fwrite(&transFile, sizeof(struct TransactionFile), 1, file) != 1) {
        perror("Error writing transaction to file");
    }

    fflush(file);

    // Unlock the file
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
    
}

int view_transaction(char *userid, struct Transaction transArray[100]) {
    
    struct TransactionFile transFile;
    struct flock lock;
    FILE *file = fopen("transactions.txt", "r");
    
    if (file == NULL) {
        perror("Error opening transactions file");
        return -1; 
    }

    int fd = fileno(file);
    fseek(file, 0, SEEK_SET); 
    long position = 0;
    int found = 0;
    while (fread(&transFile, sizeof(struct TransactionFile), 1, file) == 1) {
        if (strcmp(transFile.userid, userid) == 0) {
            found = 1;
            position = ftell(file) - sizeof(struct TransactionFile);  
            break;
        }
    }

    if (!found) {
        fclose(file);
        return 0; //Fail
    }

    
    lock.l_type = F_RDLCK;  
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct TransactionFile);  
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return 0;  
    }
    
    memcpy(transArray, transFile.transactions, sizeof(transFile.transactions));

    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error unlocking file");
    }

    fclose(file);
    return 1;  // Success
}

void apply_for_loan(char*userid,int amount){

	struct Loan new_loan;
	int uniq=rand();
	char loanid[110];
	snprintf(loanid,sizeof(loanid),"%d%s",uniq,userid);
	strcpy(new_loan.loanid,loanid);
    strcpy(new_loan.userid, userid);
    strcpy(new_loan.employeeid, "None");
    new_loan.amount=amount;
    new_loan.status=2;

    FILE *file = fopen("loans.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    struct flock lock;
    int fd = fileno(file);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = 0;
    lock.l_len = sizeof(struct Loan);  
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    fseek(file,0,SEEK_END);
    if (fwrite(&new_loan, sizeof(struct Loan), 1, file) != 1) {
    printf("\nError writing new loan to file\n");
    }
    fflush(file);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    fclose(file);
}


#endif