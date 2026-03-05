#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <climits>
using namespace std;

class PageReplacement {
protected:
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
public:
    FIFO(int frames, vector<int> refs) : PageReplacement(frames, refs) {}

    int simulate() override {
        vector<int> frames(numFrames, -1);
        queue<int> order;
        int pageFaults = 0;

        cout << "FIFO Simulation:" << endl;
        for (size_t i = 0; i < referenceString.size(); ++i) {
            int page = referenceString[i];
            bool found = find(frames.begin(), frames.end(), page) != frames.end();
            if (!found) {
                pageFaults++;
                if (order.size() < numFrames) {
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
public:
    LRU(int frames, vector<int> refs) : PageReplacement(frames, refs) {}

    int simulate() override {
        vector<int> frames(numFrames, -1);
        vector<int> timestamps(numFrames, 0);
        unordered_map<int, int> pageToIndex;
        int pageFaults = 0;
        int time = 0;

        cout << "LRU Simulation:" << endl;
        for (size_t i = 0; i < referenceString.size(); ++i) {
            int page = referenceString[i];
            time++;
            if (pageToIndex.find(page) != pageToIndex.end()) {
                int idx = pageToIndex[page];
                timestamps[idx] = time;
            } else {
                pageFaults++;
                if (pageToIndex.size() < numFrames) {
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
                    int minTime = INT_MAX;
                    int replaceIdx = -1;
                    for (int j = 0; j < numFrames; ++j) {
                        if (timestamps[j] < minTime) {
                            minTime = timestamps[j];
                            replaceIdx = j;
                        }
                    }
                    pageToIndex.erase(frames[replaceIdx]);
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

class Optimal : public PageReplacement {
public:
    Optimal(int frames, vector<int> refs) : PageReplacement(frames, refs) {}

    int simulate() override {
        vector<int> frames(numFrames, -1);
        unordered_map<int, int> pageToIndex;
        int pageFaults = 0;

        cout << "Optimal Simulation:" << endl;
        for (size_t i = 0; i < referenceString.size(); ++i) {
            int page = referenceString[i];
            if (pageToIndex.find(page) != pageToIndex.end()) {
                // hit
            } else {
                pageFaults++;
                if (pageToIndex.size() < numFrames) {
                    int idx = -1;
                    for (int j = 0; j < numFrames; ++j) {
                        if (frames[j] == -1) {
                            idx = j;
                            break;
                        }
                    }
                    frames[idx] = page;
                    pageToIndex[page] = idx;
                } else {
                    // find the one with farthest next use
                    int farthest = -1;
                    int replaceIdx = -1;
                    for (int j = 0; j < numFrames; ++j) {
                        int nextUse = -1;
                        for (size_t k = i + 1; k < referenceString.size(); ++k) {
                            if (referenceString[k] == frames[j]) {
                                nextUse = k;
                                break;
                            }
                        }
                        if (nextUse == -1) {
                            // no future use, replace this
                            replaceIdx = j;
                            break;
                        } else if (nextUse > farthest) {
                            farthest = nextUse;
                            replaceIdx = j;
                        }
                    }
                    pageToIndex.erase(frames[replaceIdx]);
                    frames[replaceIdx] = page;
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
    cout << "FIFO Page Fault Rate: " << fifoRate << "%" << endl << endl;

    LRU lru(numFrames, refString);
    int lruFaults = lru.simulate();
    double lruRate = (double)lruFaults / refString.size() * 100;

    cout << "\nLRU Total Page Faults: " << lruFaults << endl;
    cout << "LRU Page Fault Rate: " << lruRate << "%" << endl << endl;

    Optimal optimal(numFrames, refString);
    int optFaults = optimal.simulate();
    double optRate = (double)optFaults / refString.size() * 100;

    cout << "\nOptimal Total Page Faults: " << optFaults << endl;
    cout << "Optimal Page Fault Rate: " << optRate << "%" << endl << endl;

    cout << "Comparison Report:" << endl;
    cout << "Algorithm\tPage Faults\tFault Rate" << endl;
    cout << "FIFO\t\t" << fifoFaults << "\t\t" << fifoRate << "%" << endl;
    cout << "LRU\t\t" << lruFaults << "\t\t" << lruRate << "%" << endl;
    cout << "Optimal\t\t" << optFaults << "\t\t" << optRate << "%" << endl;

    return 0;
}
