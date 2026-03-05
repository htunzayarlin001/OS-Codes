#include <iostream>
#include <vector>
using namespace std;
int computeParity(vector<int>& bits) {
int parity = 0;
for (int b : bits) parity ^= b;
return parity;
}
bool checkParity(vector<int>& data, int storedParity) {
    return computeParity(data) == storedParity;
}

// Calculate Hamming Distance: number of positions where two bit vectors differ
int hammingDistance(vector<int>& a, vector<int>& b) {
    if (a.size() != b.size()) {
        cout << "Error: Vectors must have equal length" << endl;
        return -1;
    }
    int distance = 0;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            distance++;
        }
    }
    return distance;
}
int main() {
    vector<int> data = {1, 0, 1, 1, 0, 0, 1, 0};
    int parity = computeParity(data);
    cout << "Parity bit: " << parity << endl;
    // Simulate a 1-bit error
    data[3] = 1 - data[3];
    cout << "Error detected: " << !checkParity(data, parity) << endl;
    // Demonstrate Hamming Distance
    vector<int> original = {1, 0, 1, 1, 0, 0, 1, 0};
    vector<int> corrupted = {1, 0, 1, 0, 0, 0, 1, 0};  // Bit 3 flipped
    int distance = hammingDistance(original, corrupted);
    cout << "Hamming Distance: " << distance << " (number of bit flips)" << endl;
    return 0;
}