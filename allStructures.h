#include<string.h>
#include<fcntl.h>
struct Customer {
public:
    string userid[100];
    string password[10];
    int balance;
    bool active;


};

struct Transaction{
public:
    int unique_id;
    string userid[100];
    string transaction_type[50];
    string receiver_id[100];
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
    string userid[100];
    string password[10];
    int loanAppl[1024];
    bool active;

    int *viewLoanAppl(){
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