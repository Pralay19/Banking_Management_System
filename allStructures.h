#ifndef ALLSTRUCTURES_H
#define ALLSTRUCTURES_H

#include<stdbool.h>
#include<string.h>
#include<fcntl.h>

struct Customer {
public:
    char userid[100];
    char password[10];
    int balance;
};

struct Transaction{
public:
    int unique_id;
    char userid[100];
    char transaction_type[50];
    char receiver_id[100];
    int amount;
    int balance_after;

};
void update_history(struct Transaction trans){
    FILE *file = fopen("transactions.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    int fd = fileno(file);
    struct flock lock;

    fseek(file, 0, SEEK_SET);// Setting the head to where user block is
    if (fread(&trans, sizeof(struct Customer), 1, file) != 1){
        perror("Error reading user data");
        return 1;
    }
}

struct Employee{
public:
    char userid[100];
    char password[10];
    int loanAppl[1024];


};

struct Manager{
public:
    char userid[100];
    char password[10];

};

struct Admin{
public:
    char userid[100];
    char password[10];

};

#endif