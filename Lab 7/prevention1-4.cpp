#include <iostream>
#include <thread>
#include <mutex>

std::mutex resourceA, resourceB, resourceC;

// Solution: Lock all resources in order A -> B -> C

void process1() {
    resourceA.lock();
    resourceB.lock();
    // Work...
    resourceB.unlock();
    resourceA.unlock();
}

void process2() {
    resourceB.lock();
    resourceC.lock();
    // Work...
    resourceC.unlock();
    resourceB.unlock();
}

void process3() {
    resourceA.lock();  // Changed from resourceC
    resourceC.lock();
    // Work...
    resourceC.unlock();
    resourceA.unlock();
}

int main() {
    std::thread t1(process1);
    std::thread t2(process2);
    std::thread t3(process3);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "All processes completed\n";
    return 0;
}