#ifndef ALLSTRUCTURES_H
#define ALLSTRUCTURES_H

#include<stdbool.h>
#include<string.h>
#include<fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <stdio.h>

struct Customer {
    char userid[100];
    char password[10];
    char name[50];
    char mobile[12];
    int balance;
    int active;
};

struct Transaction{
    int transac_id;
    char userid[100];
    char transaction_type[20];
    char receiver_id[100];
    char debitCredit[10];
    int amount;
    int balance_after;

};

struct TransactionFile{
    char userid[100];
    struct Transaction transactions[100];
};


struct Employee{
    char userid[100];
    char password[10];

};

struct Manager{
    char userid[100];
    char password[10];

};

struct Admin{
    char userid[100];
    char password[10];

};

struct Feedback{
    char userid[100];
    char feedback[300];
};



struct Loan{
    char loanid[110];
    char userid[100];
    char employeeid[100];
    int amount;
    int status;
};

#endif