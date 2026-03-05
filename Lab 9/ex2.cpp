#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

int main() {
    vector<int> referenceString = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int numFrames = 3;

    vector<int> frames;
    queue<int> fifoQueue;
    int pageFaults = 0;

    cout << "\nSimulation:\n";
    for (size_t i = 0; i < referenceString.size(); ++i) {
        int currentPage = referenceString[i];
        bool fault = false;

        // Check if page is in frames
        if (find(frames.begin(), frames.end(), currentPage) == frames.end()) {
            fault = true;
            pageFaults++;

            if (frames.size() < numFrames) {
                frames.push_back(currentPage);
            } else {
                // Replace the oldest
                int oldest = fifoQueue.front();
                fifoQueue.pop();
                // Find and replace
                auto it = find(frames.begin(), frames.end(), oldest);
                if (it != frames.end()) {
                    *it = currentPage;
                }
            }
            fifoQueue.push(currentPage);
        }

        // Display
        cout << "Reference: " << currentPage;
        if (fault) {
            cout << " -> Page Fault";
        }
        cout << " | Frames: ";
        for (int f : frames) {
            cout << f << " ";
        }
        cout << endl;
    }

    cout << "\nTotal Page Faults: " << pageFaults << endl;
    cout << "Page Fault Rate: " << (double)pageFaults / referenceString.size() * 100 << "%" << endl;

    return 0;
}
