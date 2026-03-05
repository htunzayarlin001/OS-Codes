#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
using namespace std;

const int PAGE_SIZE = 512;
const int NUM_PAGES = 256;
const int TLB_SIZE = 8;
const int TLB_ACCESS_TIME = 20; // ns
const int MEMORY_ACCESS_TIME = 100; // ns

class PageTable {
private:
    vector<int> frames;
    vector<bool> valid;
public:
    PageTable() {
        frames.assign(NUM_PAGES, -1);
        valid.assign(NUM_PAGES, false);
        // Initialize some pages
        frames[0] = 5;
        valid[0] = true;
        frames[2] = 10;
        valid[2] = true;
        frames[1] = 3;
        valid[1] = true;
    }
    bool isValid(int page) { return valid[page]; }
    int getFrame(int page) { return frames[page]; }
};

class TLB {
private:
    unordered_map<int, int> pageToFrame;
    list<int> lruOrder;
    int maxSize;
public:
    TLB(int size) : maxSize(size) {}
    bool lookup(int page, int& frame) {
        if (pageToFrame.find(page) != pageToFrame.end()) {
            frame = pageToFrame[page];
            // Move to front
            lruOrder.remove(page);
            lruOrder.push_front(page);
            return true;
        }
        return false;
    }
    void insert(int page, int frame) {
        if (pageToFrame.find(page) != pageToFrame.end()) {
            // Already in, just update
            lruOrder.remove(page);
        } else {
            if (lruOrder.size() >= maxSize) {
                int evict = lruOrder.back();
                lruOrder.pop_back();
                pageToFrame.erase(evict);
            }
        }
        pageToFrame[page] = frame;
        lruOrder.push_front(page);
    }
};

int main() {
    PageTable pt;
    TLB tlb(TLB_SIZE);

    vector<int> virtualAddresses = {1024, 512, 0, 1024, 1536, 512, 2048, 1024, 256, 512};

    int hits = 0, misses = 0;
    long long totalAccessTime = 0;

    cout << "TLB Simulation:" << endl;
    cout << "Virtual Address | Hit/Miss | Physical Address" << endl;
    cout << "----------------|----------|----------------" << endl;

    for (int addr : virtualAddresses) {
        int page = addr / PAGE_SIZE;
        int offset = addr % PAGE_SIZE;
        int frame;
        bool hit = tlb.lookup(page, frame);
        if (hit) {
            hits++;
            totalAccessTime += TLB_ACCESS_TIME;
            cout << addr << " | Hit      | " << (frame * PAGE_SIZE + offset) << endl;
        } else {
            misses++;
            totalAccessTime += TLB_ACCESS_TIME + MEMORY_ACCESS_TIME;
            if (pt.isValid(page)) {
                frame = pt.getFrame(page);
                tlb.insert(page, frame);
                cout << addr << " | Miss     | " << (frame * PAGE_SIZE + offset) << endl;
            } else {
                cout << addr << " | Miss     | Page Fault" << endl;
            }
        }
    }

    double hitRatio = (double)hits / (hits + misses);
    double effectiveTime = (double)totalAccessTime / (hits + misses);

    cout << endl << "Statistics:" << endl;
    cout << "TLB Hits: " << hits << endl;
    cout << "TLB Misses: " << misses << endl;
    cout << "TLB Hit Ratio: " << hitRatio * 100 << "%" << endl;
    cout << "Effective Memory Access Time: " << effectiveTime << " ns" << endl;

    return 0;
}
