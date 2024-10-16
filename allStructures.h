#ifndef ALLSTRUCTURES_H
#define ALLSTRUCTURES_H

#include<stdbool.h>
#include<string.h>
#include<fcntl.h>

struct Customer {
    char userid[100];
    char password[10];
    int balance;
};

// struct Transaction{
//     int unique_id;
//     char userid[100];
//     char transaction_type[50];
//     char receiver_id[100];
//     int amount;
//     int balance_after;

// };

struct Employee{
    char userid[100];
    char password[10];
    int loanAppl[100];


};

struct Manager{
    char userid[100];
    char password[10];

};

struct Admin{
    char userid[100];
    char password[10];

};

#endif