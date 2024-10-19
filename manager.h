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



int activate_d_user(char*user_id, int ade){

	FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }
    int fd = fileno(file);
    struct Customer who;
    struct flock lock;

    long position=0;
    int found=0;
    // Find user in the file
    fseek(file, 0, SEEK_SET); 
    while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
        if (strcmp(who.userid, user_id) == 0) {
        	found=1;
            position = ftell(file) - sizeof(struct Customer);
            break;
        }
    }

    if(found==1 && ade==1){
    	return -1;//Already active
    }
    else if(found==0 && ade==2){
    	return -1;//Already deactivated
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
    //1->activate
    //2->deactivate
    if(ade==2){
    	char type[100];
    	strcpy(type,"#");
    	strcpy(who.userid,type);
    }
    else{
    	strcpy(who.userid,user_id);
    }

    fseek(file, position, SEEK_SET);
    fwrite(&who, sizeof(struct Customer), 1, file);
        
    lock.l_start = position;  
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(file);
    return 1;

}


int assign_loan_appl(char*loanid,char*employeeid){

	struct Loan loan;
	struct flock lock;
	FILE *file = fopen("loans.txt", "r");
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
        	break;
        }
    }

    lock.l_type = F_RDLCK; 
    lock.l_whence = SEEK_SET;
    lock.l_start = position;
    lock.l_len = sizeof(struct Loan); 
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        fclose(file);
        return -1;  // Error while locking
    }
    strcpy(loan.employeeid,employeeid);

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
        return -1; 
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
        return -1;  // Error while locking
    }


    while (fread(&feed, sizeof(struct feed), 1, file) == 1) {
        	
            char feed_info[410];
            
            snprintf(feed_info, sizeof(feed_info), "*UserID: %s, Feedback: %s\n",
                     feed.userid, feed.feedback);

            size_t feed_info_len = strlen(feed_info);
                
            strcat(buffer + offset, loan_info);
            offset += loan_info_len;
           
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(file);
}

#endif