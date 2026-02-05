#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

const int NUM_PAGES = 64;
const int NUM_FRAMES = 32;
const int PAGE_SIZE = 1024;

class PageTable {
private:
	int pageTable[NUM_PAGES];
	bool valid[NUM_PAGES];

public:
	// Constructor: Initialize page table with random frame numbers
	PageTable() {
		srand(time(0));
		// Initialize page table with random frame numbers
		// Mark some pages as valid, others as invalid
		for (int i = 0; i < NUM_PAGES; i++) {
			pageTable[i] = rand() % NUM_FRAMES;  // Random frame number
			valid[i] = (rand() % 2 == 0);        // 50% chance of being valid
		}
	}

	// Address translation function
	int translateAddress(int logicalAddress) {
		// Extract page number (upper bits)
		int pageNumber = logicalAddress / PAGE_SIZE;
		
		// Extract offset (lower bits)
		int offset = logicalAddress % PAGE_SIZE;
		
		// Validate page number
		if (pageNumber < 0 || pageNumber >= NUM_PAGES) {
			return -1;  // Invalid page number
		}
		
		// Check if page is valid
		if (!valid[pageNumber]) {
			return -1;  // Page not in memory (page fault)
		}
		
		// Get frame number from page table
		int frameNumber = pageTable[pageNumber];
		
		// Calculate physical address
		int physicalAddress = frameNumber * PAGE_SIZE + offset;
		
		return physicalAddress;
	}

	// Display page table
	void displayPageTable() {
		cout << "\n========== PAGE TABLE ==========" << endl;
		cout << "Page#\tFrame#\tValid" << endl;
		cout << "-----\t------\t-----" << endl;
		
		for (int i = 0; i < NUM_PAGES; i++) {
			cout << i << "\t" << pageTable[i] << "\t" 
				 << (valid[i] ? "YES" : "NO") << endl;
		}
		cout << "================================\n" << endl;
	}
};

int main() {
	// Create PageTable object
	PageTable pageTable;
	
	// Display the page table
	pageTable.displayPageTable();
	
	// Test with various logical addresses
	cout << "\n========== ADDRESS TRANSLATION TESTS ==========" << endl;
	cout << "Logical Address\tPage#\tOffset\tPhysical Address\tStatus" << endl;
	cout << "---------------\t-----\t------\t----------------\t------" << endl;
	
	int testAddresses[] = {
		0,           // Page 0, Offset 0
		1023,        // Page 0, Offset 1023 (last byte of page 0)
		1024,        // Page 1, Offset 0
		5000,        // Page 4, Offset 856
		10240,       // Page 10, Offset 0
		65535,       // Page 63, Offset 1023
		100000       // Out of range
	};
	
	for (int addr : testAddresses) {
		int pageNum = addr / PAGE_SIZE;
		int offset = addr % PAGE_SIZE;
		int physicalAddr = pageTable.translateAddress(addr);
		
		cout << addr << "\t" << pageNum << "\t" << offset << "\t";
		
		if (physicalAddr != -1) {
			cout << physicalAddr << "\t\tSUCCESS" << endl;
		} else {
			cout << "N/A\t\tFAIL (";
			if (pageNum >= NUM_PAGES) {
				cout << "Invalid page)";
			} else {
				cout << "Page fault)";
			}
			cout << endl;
		}
	}
	
	cout << "\n==============================================\n" << endl;

	return 0;
}
