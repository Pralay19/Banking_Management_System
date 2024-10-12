/* 
====================================
* PROGRAM: employee.c
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

void add_new_customer() {
    char customer_id[50], customer_name[50];
    printf("Enter New Customer ID: ");
    scanf("%s", customer_id);
    printf("Enter Customer Name: ");
    scanf("%s", customer_name);

    FILE *file = fopen("customers.txt", "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int fd = fileno(file);
    flock(fd, LOCK_EX);

    fprintf(file, "%s %s\n", customer_id, customer_name);

    flock(fd, LOCK_UN);
    fclose(file);

    printf("New customer added successfully.\n");
}

void view_assigned_loans() {
    char employee_id[50];
    printf("Enter your Employee ID to view assigned loans: ");
    scanf("%s", employee_id);

    FILE *file = fopen("loan_assignments.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int fd = fileno(file);
    flock(fd, LOCK_SH);

    char line[256], loan_id[50], assigned_employee[50];
    printf("\nAssigned Loan Applications:\n");
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "Loan ID: %s -> Assigned to Employee: %s", loan_id, assigned_employee);
        if (strcmp(assigned_employee, employee_id) == 0) {
            printf("%s\n", loan_id);
        }
    }

    flock(fd, LOCK_UN);
    fclose(file);
}

void process_loan_application() {
    char loan_id[50];
    printf("Enter Loan Application ID to process: ");
    scanf("%s", loan_id);

    printf("Loan Application %s processed successfully.\n", loan_id);
}

int main() {
    int choice;
    while (1) {
        printf("\n1. Add New Customer\n2. View Assigned Loan Applications\n3. Process Loan Application\n4. Logout\nChoose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_new_customer();
                break;
            case 2:
                view_assigned_loans();
                break;
            case 3:
                process_loan_application();
                break;
            case 4:
                printf("Logging out...\n");
                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
