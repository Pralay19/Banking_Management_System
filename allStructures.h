#include<string.h>

struct Customer {
public:
    string userid[100];
    string password[10];
    int balance;
    bool active;


};

struct Employee{
public:
    string userid[100];
    string password[10];
    vector<int>loanAppl;
    bool active;

    vector<int> viewLoanAppl(){
        return loanAppl;
    }

};

struct Manager{
public:
    string userid[100];
    string password[10];
    bool active;

};

struct Admin{
public:
    string userid[100];
    string password[10];
    bool active;

};