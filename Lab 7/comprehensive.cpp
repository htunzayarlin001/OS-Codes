#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>

class BankAccount {
private:
    double balance;
    std::mutex mtx;
    int accountId;
    
public:
    BankAccount(int id, double initial) 
        : accountId(id), balance(initial) {}
    
    // TODO: Implement safe transfer method
    // Use lock ordering or std::lock to prevent deadlock
    
    static bool transfer(BankAccount& from, BankAccount& to, double amount) {
    if (&from == &to) return false;
    
    // Use address comparison for consistent ordering
    BankAccount* first = (&from < &to) ? &from : &to;
    BankAccount* second = (&from < &to) ? &to : &from;
    
    std::lock(first->mtx, second->mtx);
    std::lock_guard<std::mutex> lock1(first->mtx, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(second->mtx, std::adopt_lock);
    
    if (from.balance >= amount) {
        from.balance -= amount;
        to.balance += amount;
        std::cout << "Transfer: $" << amount 
                  << " from Account " << from.accountId 
                  << " to Account " << to.accountId << "\n";
        return true;
    }
    return false;
}
    
    double getBalance() {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }
};

int main() {
    BankAccount acc1(1, 1000);
    BankAccount acc2(2, 500);
    
    std::thread t1([&acc1, &acc2]() {
        for (int i = 0; i < 3; ++i) {
            BankAccount::transfer(acc1, acc2, 100);
        }
    });
    
    std::thread t2([&acc1, &acc2]() {
        for (int i = 0; i < 3; ++i) {
            BankAccount::transfer(acc2, acc1, 50);
        }
    });
    
    t1.join();
    t2.join();
    
    std::cout << "Final Account 1 balance: $" << acc1.getBalance() << std::endl;
    std::cout << "Final Account 2 balance: $" << acc2.getBalance() << std::endl;
    return 0;
}
