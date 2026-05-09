#include <stdio.h>
#include <stdlib.h>

// structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

// ⭐ ONLY FEATURE ADDED
void miniStatement();
void logTransaction(unsigned int acc, double amount, double balance);

//////////////////////////////////////////////////////////

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }

    while ((choice = enterChoice()) != 6)   // ⭐ changed exit
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            miniStatement();   // ⭐ NEW FEATURE
            break;
        default:
            printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
}

//////////////////////////////////////////////////////////

// ⭐ LOG TRANSACTION
void logTransaction(unsigned int acc, double amount, double balance)
{
    FILE *fp = fopen("transactions.txt", "a");

    if (fp != NULL)
    {
        fprintf(fp, "Acc:%d | Amount: %.2f | Balance: %.2f\n",
                acc, amount, balance);
        fclose(fp);
    }
}

//////////////////////////////////////////////////////////

// ⭐ MINI STATEMENT
void miniStatement()
{
    FILE *fp = fopen("transactions.txt", "r");
    char ch;

    if (fp == NULL)
    {
        printf("No transactions found.\n");
        return;
    }

    printf("\n--- MINI STATEMENT ---\n");

    while ((ch = fgetc(fp)) != EOF)
        putchar(ch);

    fclose(fp);
}

//////////////////////////////////////////////////////////

void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        printf("File could not be opened.\n");
    }
    else
    {
        rewind(readPtr);

        fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
                "Acct", "Last Name", "First Name", "Balance");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                        client.acctNum, client.lastName,
                        client.firstName, client.balance);
            }
        }

        fclose(writePtr);
    }
}

//////////////////////////////////////////////////////////

void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0};

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("Enter amount (+credit / -debit): ");
        scanf("%lf", &transaction);

        client.balance += transaction;

        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        // ⭐ LOGGING ADDED
        logTransaction(account, transaction, client.balance);

        printf("Updated Balance: %.2f\n", client.balance);
    }
}

//////////////////////////////////////////////////////////

void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter account to delete: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
    }
    else
    {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

        printf("Account deleted.\n");
    }
}

//////////////////////////////////////////////////////////

void newRecord(FILE *fPtr)
{
    struct clientData client = {0};
    unsigned int accountNum;

    printf("Enter account number: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account exists.\n");
    }
    else
    {
        printf("Enter lastname firstname balance:\n");
        scanf("%14s%9s%lf",
              client.lastName,
              client.firstName,
              &client.balance);

        client.acctNum = accountNum;

        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        printf("Account created.\n");
    }
}

//////////////////////////////////////////////////////////

unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\nMENU\n"
           "1 - Export to text file\n"
           "2 - Update account\n"
           "3 - Add account\n"
           "4 - Delete account\n"
           "5 - Mini Statement\n"   // ⭐ ONLY NEW FEATURE
           "6 - Exit\n"
           "Enter choice: ");

    scanf("%u", &choice);
    return choice;
}