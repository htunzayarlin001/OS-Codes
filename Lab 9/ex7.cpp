#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <algorithm>
using namespace std;

const int TOTAL_MEMORY = 1024; // KB
const int MAX_K = 10; // 2^10 = 1024

class BuddyAllocator {
private:
    vector<list<int>> freeLists; // index k: list of starting addresses for size 2^k
    map<int, pair<int, int>> allocated; // addr -> (block_size, request_size)
    int totalAllocated = 0;

    int getK(int size) {
        int k = 0;
        while ((1 << k) < size) k++;
        return k;
    }

    int getBuddy(int addr, int k) {
        return addr ^ (1 << k);
    }

    void split(int k) {
        if (freeLists[k].empty()) {
            split(k + 1);
        }
        int addr = freeLists[k].front();
        freeLists[k].pop_front();
        int half = 1 << (k - 1);
        freeLists[k - 1].push_back(addr);
        freeLists[k - 1].push_back(addr + half);
    }

    void merge(int addr, int k) {
        int buddy = getBuddy(addr, k);
        // Check if buddy is free
        auto it = find(freeLists[k].begin(), freeLists[k].end(), buddy);
        if (it != freeLists[k].end()) {
            freeLists[k].erase(it);
            int mergedAddr = min(addr, buddy);
            freeLists[k + 1].push_back(mergedAddr);
            merge(mergedAddr, k + 1);
        } else {
            freeLists[k].push_back(addr);
        }
    }

public:
    BuddyAllocator() {
        freeLists.resize(MAX_K + 1);
        freeLists[MAX_K].push_back(0);
    }

    int allocate(int size) {
        int k = getK(size);
        if (k > MAX_K) return -1; // Too big

        if (freeLists[k].empty()) {
            if (k + 1 > MAX_K) return -1;
            split(k + 1);
        }

        if (freeLists[k].empty()) return -1;

        int addr = freeLists[k].front();
        freeLists[k].pop_front();
        allocated[addr] = {1 << k, size};
        totalAllocated += 1 << k;
        return addr;
    }

    void deallocate(int addr) {
        if (allocated.find(addr) == allocated.end()) return;

        auto [blockSize, reqSize] = allocated[addr];
        allocated.erase(addr);
        totalAllocated -= blockSize;

        int k = getK(blockSize);
        merge(addr, k);
    }

    void display() {
        cout << "Allocated Blocks:" << endl;
        for (auto& p : allocated) {
            cout << "Address: " << p.first << ", Block Size: " << p.second.first << " KB, Requested: " << p.second.second << " KB" << endl;
        }

        cout << "Free Blocks:" << endl;
        for (int k = 0; k <= MAX_K; ++k) {
            if (!freeLists[k].empty()) {
                cout << "Size " << (1 << k) << " KB: ";
                for (int addr : freeLists[k]) {
                    cout << addr << " ";
                }
                cout << endl;
            }
        }
    }

    void calculateFragmentation() {
        int internalFrag = 0;
        for (auto& p : allocated) {
            internalFrag += p.second.first - p.second.second;
        }

        int totalFree = TOTAL_MEMORY - totalAllocated;
        int maxFreeBlock = 0;
        for (int k = MAX_K; k >= 0; --k) {
            if (!freeLists[k].empty()) {
                maxFreeBlock = 1 << k;
                break;
            }
        }
        int externalFrag = totalFree - maxFreeBlock;

        cout << "Fragmentation:" << endl;
        cout << "Internal Fragmentation: " << internalFrag << " KB" << endl;
        cout << "External Fragmentation: " << externalFrag << " KB" << endl;
        cout << "Total Free Memory: " << totalFree << " KB" << endl;
    }
};

int main() {
    BuddyAllocator allocator;

    cout << "Initial State:" << endl;
    allocator.display();
    allocator.calculateFragmentation();
    cout << endl;

    int block1 = allocator.allocate(70);
    cout << "Allocated 70 KB at address " << block1 << endl;
    allocator.display();
    allocator.calculateFragmentation();
    cout << endl;

    int block2 = allocator.allocate(35);
    cout << "Allocated 35 KB at address " << block2 << endl;
    allocator.display();
    allocator.calculateFragmentation();
    cout << endl;

    int block3 = allocator.allocate(80);
    cout << "Allocated 80 KB at address " << block3 << endl;
    allocator.display();
    allocator.calculateFragmentation();
    cout << endl;

    allocator.deallocate(block2);
    cout << "Deallocated block at " << block2 << endl;
    allocator.display();
    allocator.calculateFragmentation();
    cout << endl;

    return 0;
}
