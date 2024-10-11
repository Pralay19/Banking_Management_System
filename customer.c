/* 
====================================
* PROGRAM: customer.c
* AUTHOR: PRALAY D. SAW
* ROLLNO: MT2024119
* DESCRIPTION:

====================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

#include "allStructures.h"

void view_account_balance(struct Customer custm) {
	int result=custm.balance;
    printf("Account Balance: %d\n",result);
}

void transfer_funds(long position,string receiver_id,int amount){
	struct Customer recvr;
	struct Customer sender;
	struct flock locks;
	struct flock lockr;

	FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
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
    	return 1;
	}
	// Checking if sender has suffecient balance or not
	if(amount>sender.balance){
		printf("You have insuffecient balance! \nBalance: $%d",sender.balance);
		return;
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
        		return 1;
    		}

            // Updating changes of sender to file
            fseek(file, position, SEEK_SET);
            fwrite(&sender, sizeof(struct Customer), 1, file);// Writing the changes to file
            fflush(file);// Ensuring data is written to disk
            locks.l_type = F_UNLCK; // Unlock the sender part
    		if (fcntl(fd, F_SETLCK, &locks) == -1) {
        		perror("fcntl");
        		return 1;
    		}
            fclose(file);
            printf("\nTransfer Successful! Available Balance: $%d",sender.balance);
            return;
        }
    }

    printf{"\nNo such Account found. Please check the User Id of the receiver and try again later."}


}

void withdraw_money(long position){
	int amount;
    printf("Enter amount to Withdraw: ");
    scanf("%d", &amount);

    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int fd = fileno(file);
    struct Customer who;
    struct floack lock;
    fseek(file, position, SEEK_SET);// Setting the head to where user block is
    if (fread(&who, sizeof(struct Customer), 1, file) != 1){
    	perror("Error reading user data");
    	return 1;
	}
	if(amount>who.balance){
		printf("You have insuffecient balance.\nAvailable Balance: $%d",who.balance);
		return;
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
        return 1;
    }
    fclose(file);

    printf("Withdrawal successful. Please collect your cash. \nNew balance: $%d\n", who.balance);
}

void deposit_money(long position) {
    int amount;
    printf("Enter amount to deposit: ");
    scanf("%d", &amount);

    FILE *file = fopen("customers.txt", "r+");
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
    fflush(file);// Ensuring data is written to disk

    // Searching in the file
    // while (fread(&who, sizeof(struct Customer), 1, file) == 1) {
    //     if (strcmp(who.userid, user_id) == 0 && strcmp(who.password, password) == 0) {          
    //             who.balance+=amount;
    //             // Updating the changes to file
    //             fseek(file, -sizeof(struct Customer), SEEK_CUR);
    //             fwrite(&who, sizeof(struct Customer), 1, file);// Writing the changes to file
    //             fflush(file);// Ensuring data is written to disk
    //             
    //             return position;  // Success
            
    //     }
    // }

    lock.l_type = F_UNLCK; // Unlock the user part
	if (fcntl(fd, F_SETLCK, &lock) == -1) {
		perror("fcntl");
		return 1;
	}
    fclose(file);

    printf("Deposit successful! New balance: $%d\n", who.balance);
}

int main(int argc,char *argv[]){
    int choice;
    string user_id=argv[1];
    string password=argv[2];
    char *position_str = argv[3];
    long position = strtol(position_str, NULL, 10);// Position of the user block in file

    //first we will have to set the head to the user block 
    FILE *file = fopen("customers.txt", "r+");
    if (file == NULL){
        perror("Error opening file");
        return 1;
    }
    // int fd = fileno(file);
    // flock(fd, LOCK_EX);
    fseek(file, position, SEEK_SET);
    struct Customer custm;
    if (fread(&custm, sizeof(struct Customer), 1, file) != 1){
    	perror("Error reading user data");
    	return 1;
	}

    while (1){

        printf("\n1. View Account Balance\n2. Deposit Money\n3. Withdraw Money\n4. Transfer Funds\n5. APPLY FOR LOAN\n6. Change Password\n7. Adding FeedBack\n8. View Transaction History\n9. Logout\nChoose an option: ");
        scanf("%d", &choice);

        switch (choice){
            case 1:
                view_account_balance(custm)
                break;
            case 2:
                deposit_money(position);
                break;
            case 3:
                withdraw_money(position);
                break;
            case 4:
            	string reciever_id;
            	int amount;
            	printf("\nPlease enter the Receiver's ID: ");
            	scanf("%s",&receiver_id);
            	printf("\nPlease enter the amount to be Transferred: ");
            	scanf("%d",&amount);
                transfer_funds(position,receiver_id,amount);
                break;
            case 5:
                apply_for_loan();
                break;
            case 6:
                change_password();
                break;
            case 7:
                add_feedback();
                break;
            case 8:
                view_transaction_history();
                break;
            case 9:
                printf("Logging out...\n");
                // Here we have to make the customer offline

                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
