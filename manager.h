/* 
====================================
* PROGRAM: admin.h
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/

#ifndef MANAGER_H
#define MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>


#include "allStructures.h"


int activate_d_user(char* user_id, int ade) {

    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int fd = fileno(file);
    struct Customer who;
    struct flock lock;

    long position = 0;
    int found = 0;

    // Find user in the file
     
    while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(who.userid, user_id) == 0) {
            found = 1;
            position = ftell(file) - sizeof(struct Customer);
            break;
        }
    }

    if (found == 0) {
        fclose(file);
        printf("\nFAILED");
        return 0;
    }


    // Lock the customer record for writing
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Customer);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking customer record");
        fclose(file);
        return 0;
    }

    // 1 -> activate, 2 -> deactivate
    if (ade == 2) {
        who.active = 0;  // Deactivate
    } else {
        who.active = 1;  // Activate
    }

    // Update the customer record in the file
    fseek(file, position, SEEK_SET);
    if (fwrite(&who, sizeof(struct Customer), 1, file) != 1) {
        perror("Error updating customer");
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        fclose(file);
        return 0;
    }

    // Unlock the customer record
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
    return 1;  // Success
}


int assign_loan_appl(char*loanid,char*employeeid){

	struct Loan loan;
	struct flock lock;
	FILE *file = fopen("loans.txt", "r+");
    if (file == NULL) {
        perror("Error opening transactions file");
        return -1; 
    }
    int fd = fileno(file);
    fseek(file, 0, SEEK_SET);
    long position=0;
    int found=0;
    while(fread(&loan, sizeof(struct Loan), 1, file) == 1) {
        if (strcmp(loan.loanid, loanid) == 0) {
        	position=ftell(file)-sizeof(struct Loan);
        	found=1;
        	lock.l_type = F_WRLCK; 
		    lock.l_whence = SEEK_SET;
		    lock.l_start = position;
		    lock.l_len = sizeof(struct Loan); 
		    lock.l_pid = getpid();
		    if (fcntl(fd, F_SETLKW, &lock) == -1) {
		        perror("Error locking file");
		        fclose(file);
		        return -1;  
		    }
		    strcpy(loan.employeeid,employeeid);
		    fseek(file,position,SEEK_SET);
		    if (fwrite(&loan, sizeof(struct Loan), 1, file) != 1) {
		    	printf("\nError writing new loan to file\n");
		    }
		    fflush(file);
        	break;
        }
    }
    

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(file);
    return found;
}

void view_feedback(char*buffer){
	struct Feedback feed;
	struct flock lock;
	FILE *file = fopen("feedbacks.txt", "r");
    if (file == NULL) {
        perror("Error opening transactions file");
        return ; 
    }
    strcpy(buffer,"");
    int fd = fileno(file);
    fseek(file, 0, SEEK_SET);
    int found=0;
    size_t offset = 0;

    lock.l_type = F_RDLCK; 
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; 
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return ;  // Error while locking
    }


    while (fread(&feed, sizeof(struct Feedback), 1, file) == 1) {
        	
            char feed_info[600];
            
            snprintf(feed_info, sizeof(feed_info), "*UserID: %s\n:- Feedback: %s\n",
                     feed.userid, feed.feedback);

            size_t feed_info_len = strlen(feed_info);
                
            strcat(buffer + offset, feed_info);
            offset += feed_info_len;
           
        
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(file);

}

void change_password_man(char*user_id,char*password){
	FILE *file = fopen("managers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return ;
    }
    int fd = fileno(file);
    struct Manager who;
    struct flock lock;

    long position=0;
    // Find sender in the file
    fseek(file, 0, SEEK_SET); 
    while (fread(&who, sizeof(struct Manager), 1, file) == 1) {
        if (strcmp(who.userid, user_id) == 0) {
            position = ftell(file) - sizeof(struct Manager);
            break;
        }
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Manager);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking sender");
        fclose(file);
        return ;
    }
    strcpy(who.password, password);

    fseek(file, position, SEEK_SET);
    fwrite(&who, sizeof(struct Manager), 1, file);
        
    lock.l_start = position;  
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
}





void view_all_loans(char *temp){

    struct Loan loan;
    struct flock lock;
    FILE *file = fopen("loans.txt", "r");
    if (file == NULL) {
        perror("Error opening transactions file");
        return ; 
    }
    strcpy(temp,"");
    int fd = fileno(file);
    fseek(file, 0, SEEK_SET);
    int found=0;
    size_t offset = 0;

    lock.l_type = F_RDLCK; 
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; 
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return ;  
    }


    while (fread(&loan, sizeof(struct Loan), 1, file) == 1) {

        char info[500];
        char type[20];
        if(loan.status==1){
            strcpy(type,"Approved");
        }
        else if(loan.status==2){
            strcpy(type,"Processing");
        }
        else if(loan.status==-1){
            strcpy(type,"Rejected");
        }
        snprintf(info,sizeof(info),"\nLoan ApplicationId: %s  UserId: %s  Loan Amount: $%d  Current Status: %s assigned to -> EmployeeId: %s\n",loan.loanid,loan.userid,loan.amount,type,loan.employeeid);

        size_t info_len = strlen(info);
                
        strcat(temp + offset, info);
        offset += info_len;
    }

    lock.l_type=F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return ;  
    }
    fclose(file);

}

#endif