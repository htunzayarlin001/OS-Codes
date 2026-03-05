#include<iostream>
#include <map>
using namespace std;
class SwapManager {
int swapSize; // in MB
int usedSwap;
map<int,int> swapTable; // pid -> MB used
public:
SwapManager(int size) : swapSize(size), usedSwap(0) {}
bool swapOut(int pid, int mb) {
if (usedSwap + mb > swapSize) return false;
swapTable[pid] = mb;
usedSwap += mb;
cout << "Process " << pid << " swapped out (" << mb << "MB)" << endl;
return true;
}
void swapIn(int pid) {
if (swapTable.count(pid)) {
usedSwap -= swapTable[pid];
swapTable.erase(pid);
cout << "Process " << pid << " swapped back in" << endl;
}
}

void printStatus() {
    cout << "\n========== SWAP STATUS ==========" << endl;
    cout << "Total Swap: " << swapSize << "MB" << endl;
    cout << "Used Swap: " << usedSwap << "MB" << endl;
    cout << "Free Swap: " << (swapSize - usedSwap) << "MB" << endl;
    cout << "Usage: " << (usedSwap * 100 / swapSize) << "%" << endl;
    
    if (swapTable.empty()) {
        cout << "\nNo processes in swap" << endl;
    } else {
        cout << "\nProcesses in swap:" << endl;
        for (auto& entry : swapTable) {
            int pid = entry.first;
            int mb = entry.second;
            cout << "  PID " << pid << ": " << mb << "MB" << endl;
        }
    }
    cout << "================================\n" << endl;
}
};
int main() {
SwapManager sm(512);
sm.swapOut(1, 128);
sm.printStatus();

sm.swapOut(2, 256);
sm.printStatus();

cout << "Attempting to swap out 200MB (should fail):" << endl;
sm.swapOut(3, 200); // should fail
sm.printStatus();

sm.swapIn(1);
sm.printStatus();

cout << "Attempting to swap out 200MB again (should succeed):" << endl;
sm.swapOut(3, 200); // now succeeds
sm.printStatus();

return 0;
}