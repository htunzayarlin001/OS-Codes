#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

struct MemoryBlock {
	int startAddress;
	int size;
	bool isFree;
	int processID;

	MemoryBlock(int start, int s, bool free = true, int pid = -1)
		: startAddress(start), size(s), isFree(free), processID(pid) {}
};

class MemoryManager {
private:
	vector<MemoryBlock> blocks;
	const int TOTAL_MEMORY = 1048576; // 1MB

public:
	MemoryManager() {
		// Initialize with one large free block
		blocks.push_back(MemoryBlock(0, TOTAL_MEMORY, true));
	}

	// First-Fit allocation
	bool allocateFirstFit(int processID, int size) {
		// Find first block that fits
		for (int i = 0; i < blocks.size(); i++) {
			if (blocks[i].isFree && blocks[i].size >= size) {
				// Found a suitable block
				if (blocks[i].size > size) {
					// Split block
					MemoryBlock newBlock(blocks[i].startAddress + size,
										blocks[i].size - size, true);
					blocks.insert(blocks.begin() + i + 1, newBlock);
				}
				// Allocate
				blocks[i].isFree = false;
				blocks[i].size = size;
				blocks[i].processID = processID;
				return true;
			}
		}
		return false; // No suitable block found
	}

	// Best-Fit allocation
	bool allocateBestFit(int processID, int size) {
		// Find smallest block that fits
		int bestIdx = -1;
		int bestSize = TOTAL_MEMORY + 1;

		for (int i = 0; i < blocks.size(); i++) {
			if (blocks[i].isFree && blocks[i].size >= size) {
				if (blocks[i].size < bestSize) {
					bestSize = blocks[i].size;
					bestIdx = i;
				}
			}
		}

		if (bestIdx == -1) {
			return false; // No suitable block found
		}

		// Split if necessary
		if (blocks[bestIdx].size > size) {
			MemoryBlock newBlock(blocks[bestIdx].startAddress + size,
								blocks[bestIdx].size - size, true);
			blocks.insert(blocks.begin() + bestIdx + 1, newBlock);
		}

		// Allocate
		blocks[bestIdx].isFree = false;
		blocks[bestIdx].size = size;
		blocks[bestIdx].processID = processID;
		return true;
	}

	// Worst-Fit allocation
	bool allocateWorstFit(int processID, int size) {
		// Find largest block
		int worstIdx = -1;
		int worstSize = -1;

		for (int i = 0; i < blocks.size(); i++) {
			if (blocks[i].isFree && blocks[i].size >= size) {
				if (blocks[i].size > worstSize) {
					worstSize = blocks[i].size;
					worstIdx = i;
				}
			}
		}

		if (worstIdx == -1) {
			return false; // No suitable block found
		}

		// Split if necessary
		if (blocks[worstIdx].size > size) {
			MemoryBlock newBlock(blocks[worstIdx].startAddress + size,
								blocks[worstIdx].size - size, true);
			blocks.insert(blocks.begin() + worstIdx + 1, newBlock);
		}

		// Allocate
		blocks[worstIdx].isFree = false;
		blocks[worstIdx].size = size;
		blocks[worstIdx].processID = processID;
		return true;
	}

	// Deallocation
	void deallocate(int processID) {
		// Find and mark block as free
		for (int i = 0; i < blocks.size(); i++) {
			if (!blocks[i].isFree && blocks[i].processID == processID) {
				blocks[i].isFree = true;
				blocks[i].processID = -1;
			}
		}

		// Merge adjacent free blocks
		mergeAdjacentFreeBlocks();
	}

	// Merge adjacent free blocks
	void mergeAdjacentFreeBlocks() {
		for (int i = 0; i < blocks.size() - 1; i++) {
			if (blocks[i].isFree && blocks[i + 1].isFree) {
				// Merge blocks
				blocks[i].size += blocks[i + 1].size;
				blocks.erase(blocks.begin() + i + 1);
				i--; // Recheck current position
			}
		}
	}

	// Display memory map
	void displayMemory() {
		cout << "\n========== MEMORY MAP ==========" << endl;
		cout << left << setw(15) << "Start Address" << setw(12) << "Size (KB)"
			 << setw(10) << "Status" << "Process ID" << endl;
		cout << "-----\t\t-----\t\t------\t\t----------" << endl;

		for (const auto& block : blocks) {
			cout << left << setw(15) << block.startAddress 
				 << setw(12) << (block.size / 1024)
				 << setw(10) << (block.isFree ? "FREE" : "ALLOCATED");
			
			if (block.isFree) {
				cout << "-" << endl;
			} else {
				cout << block.processID << endl;
			}
		}
		cout << "================================\n" << endl;
	}

	// Calculate fragmentation
	void calculateFragmentation() {
		int externalFragmentation = 0;
		int freeBlocks = 0;

		// Calculate external fragmentation (number of free blocks > 1 indicates fragmentation)
		for (const auto& block : blocks) {
			if (block.isFree) {
				freeBlocks++;
				externalFragmentation += block.size;
			}
		}

		if (freeBlocks > 1) {
			externalFragmentation -= blocks[blocks.size() - 1].isFree 
										? blocks[blocks.size() - 1].size : 0;
		} else {
			externalFragmentation = 0;
		}

		// Calculate memory utilization
		int allocatedMemory = 0;
		for (const auto& block : blocks) {
			if (!block.isFree) {
				allocatedMemory += block.size;
			}
		}

		double utilization = (double)allocatedMemory / TOTAL_MEMORY * 100;
		double fragPercent = externalFragmentation > 0 
							? (double)externalFragmentation / TOTAL_MEMORY * 100 : 0;

		cout << "\n========== FRAGMENTATION STATS ==========" << endl;
		cout << "Total Memory:              " << TOTAL_MEMORY / 1024 << " KB" << endl;
		cout << "Allocated Memory:          " << allocatedMemory / 1024 << " KB" << endl;
		cout << "Free Memory:               " << (TOTAL_MEMORY - allocatedMemory) / 1024 << " KB" << endl;
		cout << "Number of Free Blocks:     " << freeBlocks << endl;
		cout << fixed << setprecision(2);
		cout << "Memory Utilization:        " << utilization << "%" << endl;
		cout << "External Fragmentation:    " << fragPercent << "%" << endl;
		cout << "=========================================\n" << endl;
	}

	// Get statistics
	void getStatistics() {
		calculateFragmentation();
	}
};

int main() {
	MemoryManager manager;

	cout << "========== MEMORY ALLOCATION SIMULATION ==========" << endl;

	// Test First-Fit Strategy
	cout << "\n--- FIRST-FIT STRATEGY ---" << endl;
	cout << "Allocating P1 (100 KB)... ";
	if (manager.allocateFirstFit(1, 100 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	cout << "Allocating P2 (150 KB)... ";
	if (manager.allocateFirstFit(2, 150 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	cout << "Allocating P3 (200 KB)... ";
	if (manager.allocateFirstFit(3, 200 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	manager.displayMemory();

	cout << "Deallocating P2... ";
	manager.deallocate(2);
	cout << "SUCCESS" << endl;
	manager.displayMemory();

	// Test Best-Fit Strategy
	cout << "\n--- BEST-FIT STRATEGY ---" << endl;
	MemoryManager manager2;

	cout << "Allocating P1 (100 KB)... ";
	if (manager2.allocateBestFit(1, 100 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	cout << "Allocating P2 (150 KB)... ";
	if (manager2.allocateBestFit(2, 150 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	cout << "Allocating P3 (200 KB)... ";
	if (manager2.allocateBestFit(3, 200 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	cout << "Allocating P4 (50 KB)... ";
	if (manager2.allocateBestFit(4, 50 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	manager2.displayMemory();
	manager2.getStatistics();

	// Test Worst-Fit Strategy
	cout << "\n--- WORST-FIT STRATEGY ---" << endl;
	MemoryManager manager3;

	cout << "Allocating P1 (100 KB)... ";
	if (manager3.allocateWorstFit(1, 100 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	cout << "Allocating P2 (150 KB)... ";
	if (manager3.allocateWorstFit(2, 150 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	cout << "Allocating P3 (200 KB)... ";
	if (manager3.allocateWorstFit(3, 200 * 1024)) cout << "SUCCESS" << endl;
	else cout << "FAILED" << endl;

	manager3.displayMemory();
	manager3.getStatistics();

	return 0;
}
