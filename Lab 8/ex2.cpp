#include <iostream>
#include <list>
#include <unordered_map>
#include <iomanip>
using namespace std;

const int TLB_SIZE = 8;

class TLB {
private:
	struct TLBEntry {
		int pageNumber;
		int frameNumber;
	};

	list<TLBEntry> tlbCache;
	unordered_map<int, list<TLBEntry>::iterator> tlbMap;
	int hits;
	int misses;

public:
	TLB() : hits(0), misses(0) {}

	// TLB lookup function
	int lookup(int pageNumber) {
		// Search for page in TLB
		if (tlbMap.find(pageNumber) != tlbMap.end()) {
			// Found: increment hits
			hits++;
			
			// Move to front (most recently used)
			auto it = tlbMap[pageNumber];
			tlbCache.splice(tlbCache.begin(), tlbCache, it);
			
			return it->frameNumber;
		} else {
			// Not found: increment misses
			misses++;
			return -1;
		}
	}

	// TLB insert function
	void insert(int pageNumber, int frameNumber) {
		// Check if already in TLB
		if (tlbMap.find(pageNumber) != tlbMap.end()) {
			// Update existing entry and move to front
			auto it = tlbMap[pageNumber];
			it->frameNumber = frameNumber;
			tlbCache.splice(tlbCache.begin(), tlbCache, it);
			return;
		}
		
		// If TLB is full, remove LRU entry (at the back)
		if (tlbCache.size() == TLB_SIZE) {
			int lruPage = tlbCache.back().pageNumber;
			tlbMap.erase(lruPage);
			tlbCache.pop_back();
		}
		
		// Add new entry to front (most recently used)
		tlbCache.push_front({pageNumber, frameNumber});
		tlbMap[pageNumber] = tlbCache.begin();
	}

	// Display statistics
	void displayStats() {
		int total = hits + misses;
		double hitRatio = (total > 0) ? (double)hits / total * 100 : 0;
		
		cout << "\n========== TLB STATISTICS ==========" << endl;
		cout << "Hits:           " << hits << endl;
		cout << "Misses:         " << misses << endl;
		cout << "Total:          " << total << endl;
		cout << fixed << setprecision(2);
		cout << "Hit Ratio:      " << hitRatio << "%" << endl;
		cout << "====================================\n" << endl;
	}

	// Display current TLB contents
	void displayTLB() {
		cout << "\n========== TLB CONTENTS ==========" << endl;
		cout << "Page#\tFrame#" << endl;
		cout << "-----\t------" << endl;
		
		if (tlbCache.empty()) {
			cout << "(empty)" << endl;
		} else {
			for (const auto& entry : tlbCache) {
				cout << entry.pageNumber << "\t" << entry.frameNumber << endl;
			}
		}
		cout << "=================================\n" << endl;
	}

	// Reset statistics
	void resetStats() {
		hits = 0;
		misses = 0;
	}
};

int main() {
	TLB tlb;
	
	cout << "========== TLB SIMULATION ==========" << endl;
	
	// Test case 1: Insert entries
	cout << "\n--- Inserting entries into TLB ---" << endl;
	int testData[][2] = {
		{5, 15},   // Page 5 -> Frame 15
		{10, 20},  // Page 10 -> Frame 20
		{15, 25},  // Page 15 -> Frame 25
		{3, 8},    // Page 3 -> Frame 8
		{7, 12},   // Page 7 -> Frame 12
		{2, 5},    // Page 2 -> Frame 5
		{8, 18},   // Page 8 -> Frame 18
		{11, 22}   // Page 11 -> Frame 22
	};
	
	for (const auto& data : testData) {
		tlb.insert(data[0], data[1]);
		cout << "Inserted: Page " << data[0] << " -> Frame " << data[1] << endl;
	}
	
	tlb.displayTLB();
	
	// Test case 2: Lookups (mix of hits and misses)
	cout << "\n--- Testing TLB lookups ---" << endl;
	int lookupTests[] = {5, 10, 99, 3, 7, 99, 15, 100, 2, 5, 8, 99};
	
	for (int page : lookupTests) {
		int result = tlb.lookup(page);
		cout << "Lookup Page " << page << ": ";
		if (result != -1) {
			cout << "HIT (Frame " << result << ")" << endl;
		} else {
			cout << "MISS" << endl;
		}
	}
	
	tlb.displayTLB();
	
	// Test case 3: Insert new entry (should evict LRU)
	cout << "\n--- Inserting new entry (TLB full) ---" << endl;
	tlb.insert(50, 50);
	cout << "Inserted: Page 50 -> Frame 50 (should evict LRU)" << endl;
	tlb.displayTLB();
	
	// Display final statistics
	tlb.displayStats();
	
	return 0;
}
