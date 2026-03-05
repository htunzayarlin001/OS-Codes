#include <iostream>
#include <vector>
#include <string>
using namespace std;
class RAID {
int numDisks;
vector<vector<string>> disks;
public:
RAID(int n) : numDisks(n), disks(n) {}
// RAID-0: stripe data across disks
void writeRAID0(string data) {
for (int i = 0; i < data.size(); i++)
disks[i % numDisks].push_back(string(1, data[i]));
}
// RAID-1: mirror data to all disks
void writeRAID1(string data) {
for (auto& disk : disks)
disk.push_back(data);
}
// RAID-5: striped with distributed parity
// Parity calculation: XOR of all data blocks
void writeRAID5(string data) {
    if (numDisks < 3) {
        cout << "RAID-5 requires at least 3 disks" << endl;
        return;
    }
    
    // For simplicity, store data and parity separately
    // In real RAID-5, parity is rotated across disks
    vector<int> parities;  // Store parity calculations
    
    // Write data to striped disks (first n-1 disks)
    for (int i = 0; i < (int)data.size(); i++) {
        int diskIdx = i % (numDisks - 1);  // Use first n-1 disks for data
        disks[diskIdx].push_back(string(1, data[i]));
    }
    
    // Calculate and store parity on last disk
    // Simple parity: XOR of all characters (treating as integers)
    int parity = 0;
    for (char c : data) {
        parity ^= (int)c;
    }
    
    disks[numDisks - 1].push_back(to_string(parity));  // Store parity
    cout << "Data written with parity XOR = " << parity << endl;
}
void writeRAID5Alt(string data) {
    /*
    Advanced RAID-5 with distributed parity per stripe:
    - Divide data into stripes of size (numDisks - 1)
    - Each stripe: (n-1) data blocks + 1 parity block (rotated)
    - Parity[i] = XOR(Data[0..n-2])
    - Recovery: if any block lost, XOR remaining = lost block
    */
    if (numDisks < 3) {
        cout << "RAID-5 requires at least 3 disks" << endl;
        return;
    }
    
    int stripeSize = numDisks - 1;  // data blocks per stripe
    
    for (int stripe = 0; stripe < (int)((data.size() + stripeSize - 1) / stripeSize); stripe++) {
        int parity = 0;
        
        // Write data blocks for this stripe
        for (int i = 0; i < stripeSize && stripe * stripeSize + i < (int)data.size(); i++) {
            char dataChar = data[stripe * stripeSize + i];
            int diskIdx = (stripe + i) % numDisks;  // Rotate disk position
            disks[diskIdx].push_back(string(1, dataChar));
            parity ^= (int)dataChar;
        }
        
        // Write parity block (rotated position)
        int parityDiskIdx = (stripe + stripeSize) % numDisks;
        disks[parityDiskIdx].push_back("P");  // P represents parity
    }
}
void status() {
for (int i = 0; i < numDisks; i++) {
cout << "Disk " << i << ": ";
for (auto& b : disks[i]) cout << b << " ";
cout << endl;
}
}
};
int main() {
RAID r0(3);
r0.writeRAID0("ABCDEF");
cout << "=== RAID-0 ===" << endl; r0.status();

RAID r1(3);
r1.writeRAID1("BACKUP");
cout << "=== RAID-1 ===" << endl; r1.status();

RAID r5(3);
r5.writeRAID5("DATA");
cout << "=== RAID-5 (Simple Parity) ===" << endl; r5.status();

RAID r5alt(3);
r5alt.writeRAID5Alt("XYZ");
cout << "=== RAID-5 (Distributed Parity) ===" << endl; r5alt.status();

return 0;
}