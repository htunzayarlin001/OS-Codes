#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_set>
#include <climits>
using namespace std;

class PageReplacement {
private:
    int numFrames;
    vector<int> referenceString;

public:
    PageReplacement(int frames, vector<int> refs) : numFrames(frames), referenceString(refs) {}

    virtual int simulate() = 0;
    virtual void displayState(const vector<int>& frames) {
        cout << "Frames: ";
        for (int f : frames) {
            if (f == -1) cout << "- ";
            else cout << f << " ";
        }
        cout << endl;
    }
};

class FIFO : public PageReplacement {
private:
    int numFrames;
    vector<int> referenceString;

public:
    FIFO(int frames, vector<int> refs) : PageReplacement(frames, refs), numFrames(frames), referenceString(refs) {}

    int simulate() override {
        vector<int> frames(numFrames, -1);
        queue<int> order;
        int pageFaults = 0;

        cout << "FIFO Simulation:" << endl;
        for (size_t i = 0; i < referenceString.size(); ++i) {
            int page = referenceString[i];
            bool found = false;
            for (int f : frames) {
                if (f == page) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                pageFaults++;
                if (order.size() < numFrames) {
                    // find empty slot
                    int idx = -1;
                    for (int j = 0; j < numFrames; ++j) {
                        if (frames[j] == -1) {
                            idx = j;
                            break;
                        }
                    }
                    frames[idx] = page;
                    order.push(idx);
                } else {
                    int idx = order.front();
                    order.pop();
                    frames[idx] = page;
                    order.push(idx);
                }
            }
            cout << "Reference: " << page << " -> ";
            displayState(frames);
        }
        return pageFaults;
    }
};

class LRU : public PageReplacement {
private:
    int numFrames;
    vector<int> referenceString;

public:
    LRU(int frames, vector<int> refs) : PageReplacement(frames, refs), numFrames(frames), referenceString(refs) {}

    int simulate() override {
        vector<int> frames(numFrames, -1);
        vector<int> timestamps(numFrames, 0);
        map<int, int> pageToIndex;
        int pageFaults = 0;
        int time = 0;

        cout << "LRU Simulation:" << endl;
        for (size_t i = 0; i < referenceString.size(); ++i) {
            int page = referenceString[i];
            time++;
            if (pageToIndex.find(page) != pageToIndex.end()) {
                // hit, update timestamp
                int idx = pageToIndex[page];
                timestamps[idx] = time;
            } else {
                // fault
                pageFaults++;
                if (pageToIndex.size() < numFrames) {
                    // find empty slot
                    int idx = -1;
                    for (int j = 0; j < numFrames; ++j) {
                        if (frames[j] == -1) {
                            idx = j;
                            break;
                        }
                    }
                    frames[idx] = page;
                    timestamps[idx] = time;
                    pageToIndex[page] = idx;
                } else {
                    // find least recently used
                    int minTime = INT_MAX;
                    int replaceIdx = -1;
                    for (int j = 0; j < numFrames; ++j) {
                        if (timestamps[j] < minTime) {
                            minTime = timestamps[j];
                            replaceIdx = j;
                        }
                    }
                    // remove old page
                    pageToIndex.erase(frames[replaceIdx]);
                    // add new
                    frames[replaceIdx] = page;
                    timestamps[replaceIdx] = time;
                    pageToIndex[page] = replaceIdx;
                }
            }
            cout << "Reference: " << page << " -> ";
            displayState(frames);
        }
        return pageFaults;
    }
};

int main() {
    vector<int> refString = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int numFrames = 3;

    FIFO fifo(numFrames, refString);
    int fifoFaults = fifo.simulate();
    double fifoRate = (double)fifoFaults / refString.size() * 100;

    cout << "\nFIFO Total Page Faults: " << fifoFaults << endl;
    cout << "FIFO Page Fault Rate: " << fifoRate << "%" << endl;

    cout << "\n";

    LRU lru(numFrames, refString);
    int lruFaults = lru.simulate();
    double lruRate = (double)lruFaults / refString.size() * 100;

    cout << "\nLRU Total Page Faults: " << lruFaults << endl;
    cout << "LRU Page Fault Rate: " << lruRate << "%" << endl;

    cout << "\nComparison Table:" << endl;
    cout << "Algorithm\tPage Faults\tFault Rate" << endl;
    cout << "FIFO\t\t" << fifoFaults << "\t\t" << fifoRate << "%" << endl;
    cout << "LRU\t\t" << lruFaults << "\t\t" << lruRate << "%" << endl;

    return 0;
}
