#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <iomanip>
using namespace std;

class PageReplacementFIFO {
private:
	int numFrames;
	queue<int> frameQueue;
	unordered_set<int> frameSet;
	int pageFaults;
	int pageReferences;

public:
	PageReplacementFIFO(int frames) : numFrames(frames), pageFaults(0), pageReferences(0) {}

	// Page reference function
	void referencePage(int page) {
		pageReferences++;

		// Check if page is already in memory
		if (frameSet.find(page) != frameSet.end()) {
			// Page hit - no page fault
			return;
		}

		// Page fault occurred
		pageFaults++;

		// If frames are full, remove oldest (FIFO)
		if (frameQueue.size() == numFrames) {
			int oldestPage = frameQueue.front();
			frameQueue.pop();
			frameSet.erase(oldestPage);
		}

		// Add new page
		frameQueue.push(page);
		frameSet.insert(page);
	}

	// Simulate reference string with detailed display
	void simulate(vector<int> referenceString) {
		cout << "\n========== FIFO PAGE REPLACEMENT SIMULATION ==========" << endl;
		cout << "Reference String: ";
		for (int page : referenceString) {
			cout << page << " ";
		}
		cout << "\n\nNumber of Frames: " << numFrames << endl;
		cout << "\n" << left << setw(12) << "Reference" 
			 << setw(15) << "Page Fault?" << "Frame Contents" << endl;
		cout << "----------\t---------\t--------------" << endl;

		for (int page : referenceString) {
			int faultsBefore = pageFaults;
			referencePage(page);
			bool isFault = (pageFaults > faultsBefore);

			// Display reference and fault status
			cout << left << setw(12) << page 
				 << setw(15) << (isFault ? "YES" : "NO");

			// Display frame contents
			queue<int> tempQueue = frameQueue;
			while (!tempQueue.empty()) {
				cout << tempQueue.front() << " ";
				tempQueue.pop();
			}
			cout << endl;
		}

		cout << "\n=====================================================\n" << endl;
	}

	// Display results and statistics
	void displayResults() {
		double faultRate = (pageReferences > 0) ? 
			(double)pageFaults / pageReferences * 100 : 0;

		cout << "========== RESULTS ==========" << endl;
		cout << "Total Page References:   " << pageReferences << endl;
		cout << "Total Page Faults:       " << pageFaults << endl;
		cout << fixed << setprecision(2);
		cout << "Page Fault Rate:         " << faultRate << "%" << endl;
		cout << "Page Hit Rate:           " << (100 - faultRate) << "%" << endl;
		cout << "=============================\n" << endl;
	}

	// Reset simulator
	void reset() {
		while (!frameQueue.empty()) {
			frameQueue.pop();
		}
		frameSet.clear();
		pageFaults = 0;
		pageReferences = 0;
	}

	// Get page faults
	int getPageFaults() const {
		return pageFaults;
	}
};

int main() {
	cout << "========== PAGE REPLACEMENT ALGORITHM SIMULATION ==========" << endl;

	// Test Case 1: Standard reference string
	cout << "\n--- TEST CASE 1: Standard Reference String ---" << endl;
	PageReplacementFIFO simulator1(3);
	vector<int> referenceString1 = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
	simulator1.simulate(referenceString1);
	simulator1.displayResults();

	// Test Case 2: Belady's Anomaly example (FIFO with different page numbers)
	cout << "\n--- TEST CASE 2: Belady's Anomaly Example ---" << endl;
	PageReplacementFIFO simulator2(3);
	vector<int> referenceString2 = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
	simulator2.simulate(referenceString2);
	simulator2.displayResults();

	// Test Case 3: Repeated pattern
	cout << "\n--- TEST CASE 3: Simple Repeating Pattern ---" << endl;
	PageReplacementFIFO simulator3(3);
	vector<int> referenceString3 = {1, 2, 3, 1, 2, 3, 1, 2, 3};
	simulator3.simulate(referenceString3);
	simulator3.displayResults();

	// Test Case 4: Sequential references (worst case for FIFO)
	cout << "\n--- TEST CASE 4: Sequential References ---" << endl;
	PageReplacementFIFO simulator4(3);
	vector<int> referenceString4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	simulator4.simulate(referenceString4);
	simulator4.displayResults();

	// Test Case 5: Single page repeated (best case)
	cout << "\n--- TEST CASE 5: Single Page Repeated (Best Case) ---" << endl;
	PageReplacementFIFO simulator5(3);
	vector<int> referenceString5 = {1, 1, 1, 1, 1, 1, 1, 1};
	simulator5.simulate(referenceString5);
	simulator5.displayResults();

	// Test Case 6: Compare frame sizes
	cout << "\n--- TEST CASE 6: Comparison with Different Frame Counts ---" << endl;
	vector<int> testReference = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
	
	cout << "\nWith 3 Frames:" << endl;
	PageReplacementFIFO sim3frames(3);
	sim3frames.simulate(testReference);
	cout << "Page Faults: " << sim3frames.getPageFaults() << endl;

	cout << "\nWith 4 Frames:" << endl;
	PageReplacementFIFO sim4frames(4);
	sim4frames.simulate(testReference);
	cout << "Page Faults: " << sim4frames.getPageFaults() << endl;

	return 0;
}
