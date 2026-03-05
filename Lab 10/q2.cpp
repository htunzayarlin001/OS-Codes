#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;
int sstf(int head, vector<int> requests) {
    /*
    STARVATION RISK IN SSTF:
    SSTF can cause starvation of requests at distant locations.
    If new requests continuously arrive near the current head position,
    distant requests may wait indefinitely before being serviced.
    Example: If head is at 50, request at 14 may starve if requests at
    40, 45, 55, 60 keep arriving, as they are always "closest".
    */
int totalMovement = 0;
while (!requests.empty()) {
int closest = 0;
for (int i = 1; i < requests.size(); i++)
if (abs(requests[i]-head) < abs(requests[closest]-head))
closest = i;
totalMovement += abs(requests[closest] - head);
head = requests[closest];
requests.erase(requests.begin() + closest);
}
return totalMovement;
}
// FCFS Disk Scheduling Algorithm
int fcfs(int head, vector<int> requests) {
    int totalMovement = 0;
    // Service requests in the order they arrive
    for (int request : requests) {
        totalMovement += abs(request - head);
        head = request;
    }
    return totalMovement;
}

int main() {
vector<int> req = {98, 183, 37, 122, 14, 124, 65, 67};
cout << "SSTF Total Head Movement: " << sstf(53, req) << endl;
cout << "FCFS Total Head Movement: " << fcfs(53, req) << endl;
return 0;
}