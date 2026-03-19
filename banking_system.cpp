#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>

//color codes

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"
using namespace std;

class Customer
{

private:
    string name, customerId, email, phone;

public:
    Customer(string n, string id, string e, string p): name(n), customerId(id), email(e), phone(p) {}

    string getName() const { return name; }
    string getId() const { return customerId; }
    string getDetails() const //concatination
    {
        return "Name: " + name + ", ID: " + customerId + ", Email: " + email;
    }
};

class Transaction
{
public:
    string type, date;
    double amount;
    Transaction(string t, double a) : type(t), amount(a)
    {
        time_t now = time(0);
        char *dt = ctime(&now);
        date = string(dt).substr(0, 24);
    }

    string toString() const
    {
        return type + " | $" + to_string(amount) + " | " + date;
    }
};

class Account
{
private:
    string accountNo;
    double balance;
    vector<Transaction *> history;
    Customer *owner;

public:
    Account(string no, Customer *c) : accountNo(no), balance(0.0), owner(c) {}

    // Getter for owner name
    string getOwnerName() const { return owner->getName(); }

    bool deposit(double amount)
    {
        if (amount > 0)
        {
            balance += amount;
            history.push_back(new Transaction("DEPOSIT", amount));
            return true;
        }
        return false;
    }

    bool withdraw(double amount)
    {
        if (amount > 0 && amount <= balance)
        {
            balance -= amount;
            history.push_back(new Transaction("WITHDRAWAL", amount));
            return true;
        }
        return false;
    }

    bool transfer(Account *toAccount, double amount)
    {
        if (withdraw(amount))
        {
            toAccount->deposit(amount);
            history.push_back(new Transaction("TRANSFER OUT", amount));
            return true;
        }
        return false;
    }

    void displayInfo() const
    {
        cout << MAGENTA << BOLD << "\n ACCOUNT INFO " << RESET << endl;
        cout << BLUE << "Account: " << CYAN << accountNo << RESET << endl;
        cout << GREEN << "Owner: " << WHITE << owner->getName() << RESET << endl;
        cout << YELLOW << BOLD << "Balance: $" << GREEN
             << fixed << setprecision(2) << balance << RESET << endl;
    }

    double getBalance() const { return balance; }
    string getAccountNo() const { return accountNo; }
    vector<Transaction *> &getHistory() { return history; }
};

class Bank
{
private:
    vector<Customer *> customers;
    vector<Account *> accounts;
    Account *currentAccount;

    Customer *findCustomer(string id)
    {
        for (auto *cust : customers)
        {
            if (cust->getId() == id)
                return cust;
        }
        return nullptr;
    }

    Account *findAccount(string accNo)
    {
        for (auto *acc : accounts)
        {
            if (acc->getAccountNo() == accNo)
                return acc;
        }
        return nullptr;
    }

public:
    Bank() : currentAccount(nullptr) {}

    void createCustomer()
    {
        string name, id, email, phone;
        cout << "\n--- CREATE CUSTOMER ---" << endl;
        cout << "Name: ";
        cin >> ws;
        getline(cin, name);
        cout << "Customer ID: ";
        cin >> id;
        cout << "Email: ";
        cin >> email;
        cout << "Phone: ";
        cin >> phone;

        customers.push_back(new Customer(name, id, email, phone));
        cout << "Customer created successfully! ID: " << id << endl;
    }

    void createAccount()
    {
        string custId;
        cout << "\n--- CREATE ACCOUNT ---" << endl;
        cout << "Enter Customer ID: ";
        cin >> custId;

        Customer *cust = findCustomer(custId);
        if (!cust)
        {
            cout << "Customer not found!" << endl;
            return;
        }

        string accNo;
        cout << "Enter Account Number: ";
        cin >> accNo;

        if (findAccount(accNo))
        {
            cout << "Account already exists!" << endl;
            return;
        }

        accounts.push_back(new Account(accNo, cust));
        cout << "Account created! Number: " << accNo << endl;
    }

    void loginAccount()
    {
        string accNo;
        cout << "\n--- LOGIN ---" << endl;
        cout << "Available Accounts: ";
        for (auto *acc : accounts)
        {
            cout << acc->getAccountNo() << " ";
        }
        cout << "\nAccount No: ";
        cin >> accNo;

        currentAccount = findAccount(accNo);
        if (currentAccount)
        {
            cout << "Logged in successfully!" << endl;
            currentAccount->displayInfo();
        }
        else
        {
            cout << "Account not found!" << endl;
        }
    }

    void deposit()
    {
        if (!currentAccount)
        {
            cout << "Please login first!" << endl;
            return;
        }
        double amount;
        cout << "Deposit Amount: $";
        cin >> amount;
        if (currentAccount->deposit(amount))
        {
            cout << "Deposited successfully! New Balance: $"
                 << fixed << setprecision(2) << currentAccount->getBalance() << endl;
        }
        else
        {
            cout << "Invalid amount!" << endl;
        }
    }

    void withdraw()
    {
        if (!currentAccount)
        {
            cout << "Please login first!" << endl;
            return;
        }
        double amount;
        cout << "Withdraw Amount: $";
        cin >> amount;
        if (currentAccount->withdraw(amount))
        {
            cout << "Withdrawn successfully! New Balance: $"
                 << fixed << setprecision(2) << currentAccount->getBalance() << endl;
        }
        else
        {
            cout << "Insufficient balance or invalid amount!" << endl;
        }
    }

    void transfer()
    {
        if (!currentAccount)
        {
            cout << "Please login first!" << endl;
            return;
        }
        string toAccNo;
        double amount;
        cout << "Transfer to Account: ";
        cin >> toAccNo;
        cout << "Amount: $";
        cin >> amount;

        Account *toAccount = findAccount(toAccNo);
        if (toAccount && toAccount != currentAccount)
        {
            if (currentAccount->transfer(toAccount, amount))
            {
                cout << "Transfer successful!" << endl;
                currentAccount->displayInfo();
            }
            else
            {
                cout << "Transfer failed!" << endl;
            }
        }
        else
        {
            cout << "Invalid destination account!" << endl;
        }
    }

    void showAccounts()
    {
        cout << "\n=== ALL ACCOUNTS ===" << endl;
        for (auto *acc : accounts)
        {
            cout << "Acc: " << acc->getAccountNo()
                 << " | Balance: $" << fixed << setprecision(2) << acc->getBalance()
                 << " | Owner: " << acc->getOwnerName() << endl;
        }
    }

    void run()
    {
        int choice;
        while (true)
        {
            cout << YELLOW << BOLD << "\n BANKING SYSTEM " << RESET << endl;
            cout << CYAN << "1.  Create Customer" << RESET << endl;
            cout << CYAN << "2.  Create Account" << RESET << endl;
            cout << CYAN << "3.  Login Account" << RESET << endl;
            cout << CYAN << "4.  Deposit" << RESET << endl;
            cout << CYAN << "5.  Withdraw" << RESET << endl;
            cout << CYAN << "6.  Transfer" << RESET << endl;
            cout << CYAN << "7.  Show All Accounts" << RESET << endl;
            cout << CYAN << BOLD << "8.  Exit" << RESET << endl;
            cout << CYAN << "Choice: ";
            cin >> choice;

            if (choice == 1)
                createCustomer();
            else if (choice == 2)
                createAccount();
            else if (choice == 3)
                loginAccount();
            else if (choice == 4)
                deposit();
            else if (choice == 5)
                withdraw();
            else if (choice == 6)
                transfer();
            else if (choice == 7)
                showAccounts();
            else if (choice == 8)
                break;
            else
                cout << "Invalid choice!" << endl;
        }
    }
};

int main()
{

    Bank bank;
    bank.run();
    return 0;
}
