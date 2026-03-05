#include<iostream>
#include <vector>
#include <bitset>
using namespace std;
const int TOTAL_BLOCKS = 16;
class DiskManager {
private:
    bitset<TOTAL_BLOCKS> freeBlocks;
public:
    DiskManager() { freeBlocks.set(); } // all free
    
    int allocate() {
        for (int i = 0; i < TOTAL_BLOCKS; i++) {
            if (freeBlocks[i]) { 
                freeBlocks[i] = 0; 
                return i; 
            }
        }
        // Better error handling: explicit message and error code
        cerr << "ERROR: Disk full! No free blocks available for allocation." << endl;
        return -1;  // Negative value indicates failure
    }
    
    bool release(int block) {
        if (block < 0 || block >= TOTAL_BLOCKS) {
            cerr << "ERROR: Invalid block number " << block << endl;
            return false;
        }
        if (!freeBlocks[block]) {
            cerr << "WARNING: Block " << block << " is already free (double-free attempt)" << endl;
            return false;
        }
        freeBlocks[block] = 1;
        return true;
    }
    
    void status() {
        cout << "Free blocks: " << freeBlocks.count() << "/" << TOTAL_BLOCKS << endl;
    }
    
    // Allocate N contiguous blocks
    // Returns starting block index, or -1 if allocation fails
    int allocateContiguous(int numBlocks) {
        if (numBlocks <= 0 || numBlocks > TOTAL_BLOCKS) {
            cerr << "ERROR: Invalid number of blocks " << numBlocks << endl;
            return -1;
        }
        
        if ((int)freeBlocks.count() < numBlocks) {
            cerr << "ERROR: Not enough free blocks. Requested: " << numBlocks 
                 << ", Available: " << freeBlocks.count() << endl;
            return -1;
        }
        
        // Find contiguous free blocks
        int consecutive = 0;
        for (int i = 0; i < TOTAL_BLOCKS; i++) {
            if (freeBlocks[i]) {
                consecutive++;
                if (consecutive == numBlocks) {
                    // Found numBlocks contiguous free blocks
                    int startBlock = i - numBlocks + 1;
                    // Mark all blocks as allocated
                    for (int j = startBlock; j < startBlock + numBlocks; j++) {
                        freeBlocks[j] = 0;
                    }
                    cout << "Successfully allocated " << numBlocks << " contiguous blocks starting at block " 
                         << startBlock << endl;
                    return startBlock;
                }
            } else {
                consecutive = 0;  // Reset if we hit an allocated block
            }
        }
        
        cerr << "ERROR: Cannot find " << numBlocks << " contiguous free blocks" << endl;
        return -1;
    }
};
int main() {
    DiskManager dm;
    dm.status();
    
    // Test single block allocation
    int b1 = dm.allocate();
    cout << "Allocated single block: " << b1 << endl;
    
    int b2 = dm.allocate();
    cout << "Allocated single block: " << b2 << endl;
    dm.status();
    
    // Test contiguous block allocation
    int contiguous = dm.allocateContiguous(4);
    if (contiguous != -1) {
        cout << "Contiguous blocks allocated at: " << contiguous << endl;
    }
    dm.status();
    
    // Test release with error handling
    dm.release(b1);
    dm.status();
    
    // Test releasing invalid block
    dm.release(-1);
    
    // Test allocating more contiguous blocks
    int cont2 = dm.allocateContiguous(3);
    if (cont2 != -1) {
        cout << "Contiguous blocks allocated at: " << cont2 << endl;
    }
    dm.status();
    
    return 0;
}