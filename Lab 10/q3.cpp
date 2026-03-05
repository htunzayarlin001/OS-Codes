#include <iostream>
#include <queue>
using namespace std;
struct NVMRequest {
int lba; // Logical Block Address
string type; // "read" or "write"
};
void processNVM(queue<NVMRequest>& q) {
    queue<NVMRequest> reads, writes;
    
    // Separate read and write requests
    while (!q.empty()) {
        NVMRequest r = q.front();
        q.pop();
        if (r.type == "read") {
            reads.push(r);
        } else {
            writes.push(r);
        }
    }
    
    // Process all read requests first
    cout << "Processing READ requests:" << endl;
    while (!reads.empty()) {
        NVMRequest r = reads.front();
        reads.pop();
        cout << r.type << " LBA " << r.lba
            << " -> latency: ~0.1ms" << endl;
    }
    
    // Then process all write requests
    cout << "Processing WRITE requests:" << endl;
    while (!writes.empty()) {
        NVMRequest r = writes.front();
        writes.pop();
        cout << r.type << " LBA " << r.lba
            << " -> latency: ~0.1ms" << endl;
    }
}
int main() {
queue<NVMRequest> q;
q.push({100, "read"});
q.push({5, "write"});
q.push({999, "read"});
processNVM(q);
return 0;
}