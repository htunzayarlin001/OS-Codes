#include <iostream>
using namespace std;

const int PAGE_SIZE = 512;
const int NUM_PAGES = 256;

class PageTableEntry {
private:
    int frameNumber;
    bool valid;
    bool dirty;
    bool referenced;
public:
    PageTableEntry() : frameNumber(-1), valid(false), dirty(false), referenced(false) {}

    int getFrameNumber() { return frameNumber; }
    void setFrameNumber(int fn) { frameNumber = fn; }

    bool isValid() { return valid; }
    void setValid(bool v) { valid = v; }

    bool isDirty() { return dirty; }
    void setDirty(bool d) { dirty = d; }

    bool isReferenced() { return referenced; }
    void setReferenced(bool r) { referenced = r; }
};

class PageTable {
private:
    PageTableEntry entries[NUM_PAGES];
public:
    PageTable() {
        // Initialize some page table entries for demonstration
        entries[0].setFrameNumber(5);
        entries[0].setValid(true);

        entries[2].setFrameNumber(10);
        entries[2].setValid(true);
    }

    int translate(int virtualAddr) {
        int page = virtualAddr / PAGE_SIZE;
        int offset = virtualAddr % PAGE_SIZE;

        if (page >= NUM_PAGES || page < 0) {
            cout << "Invalid page number: " << page << endl;
            return -1;
        }

        if (!entries[page].isValid()) {
            cout << "Page fault for page " << page << endl;
            return -1;
        }

        entries[page].setReferenced(true);

        int physical = entries[page].getFrameNumber() * PAGE_SIZE + offset;

        cout << "Virtual Address: " << virtualAddr << endl;
        cout << "Page Number: " << page << ", Offset: " << offset << endl;
        cout << "Physical Address: " << physical << endl;

        return physical;
    }

    void display() {
        cout << "Page Table State:" << endl;
        for (int i = 0; i < NUM_PAGES; i++) {
            if (entries[i].isValid()) {
                cout << "Page " << i << ": Frame " << entries[i].getFrameNumber()
                     << ", Valid: " << (entries[i].isValid() ? "true" : "false")
                     << ", Dirty: " << (entries[i].isDirty() ? "true" : "false")
                     << ", Referenced: " << (entries[i].isReferenced() ? "true" : "false") << endl;
            }
        }
    }
};

int main() {
    PageTable pt;

    // Test translation
    cout << "Translating virtual address 1024:" << endl;
    pt.translate(1024);

    cout << "\nTranslating virtual address 512 (page 1, should cause page fault):" << endl;
    pt.translate(512);

    cout << "\nCurrent Page Table State:" << endl;
    pt.display();

    return 0;
}
