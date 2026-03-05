#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <chrono>
#include <string>
using namespace std;

const int PAGE_SIZE = 4096; // 4KB

class MemoryMappedFile {
private:
    string filename;
    set<int> loadedPages;
    ifstream file;
    long long fileSize;

public:
    MemoryMappedFile(const string& fname) : filename(fname) {
        file.open(filename, ios::binary | ios::ate);
        if (!file) {
            cerr << "Error opening file" << endl;
            return;
        }
        fileSize = file.tellg();
        file.seekg(0);
    }

    ~MemoryMappedFile() {
        if (file.is_open()) file.close();
    }

    char readChar(long long offset) {
        if (offset >= fileSize) return '\0';

        int page = offset / PAGE_SIZE;
        if (loadedPages.find(page) == loadedPages.end()) {
            // Simulate loading page
            loadedPages.insert(page);
            cout << "Loading page " << page << " into memory" << endl;
        }

        // Read from file
        file.seekg(offset);
        char ch;
        file.get(ch);
        return ch;
    }

    string readString(long long offset, int length) {
        string result;
        for (int i = 0; i < length && offset + i < fileSize; ++i) {
            result += readChar(offset + i);
        }
        return result;
    }

    int getLoadedPagesCount() { return loadedPages.size(); }
};

void traditionalRead(const string& filename, vector<long long> offsets, vector<int> lengths) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file" << endl;
        return;
    }

    for (size_t i = 0; i < offsets.size(); ++i) {
        file.seekg(offsets[i]);
        string data(lengths[i], '\0');
        file.read(&data[0], lengths[i]);
        // Simulate use
    }
    file.close();
}

int main() {
    // Create a sample file
    ofstream sample("sample.txt");
    string content = "This is a sample text file for memory-mapped I/O simulation. ";
    content += "It contains some text to demonstrate paging and on-demand loading. ";
    content += "Memory-mapped files allow efficient access to file data as if it were in memory. ";
    for (int i = 0; i < 100; ++i) content += "Additional content to make the file larger. ";
    sample << content;
    sample.close();

    MemoryMappedFile mmf("sample.txt");

    vector<long long> offsets = {0, 100, 2000, 5000, 10000, 15000};
    vector<int> lengths = {10, 20, 50, 30, 40, 25};

    // Simulate memory-mapped reads
    auto start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < offsets.size(); ++i) {
        string data = mmf.readString(offsets[i], lengths[i]);
        cout << "Read at offset " << offsets[i] << ": " << data.substr(0, 10) << "..." << endl;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> mmapTime = end - start;

    cout << "Memory-Mapped I/O:" << endl;
    cout << "Loaded pages: " << mmf.getLoadedPagesCount() << endl;
    cout << "Time taken: " << mmapTime.count() << " ms" << endl;

    // Traditional I/O
    start = chrono::high_resolution_clock::now();
    traditionalRead("sample.txt", offsets, lengths);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> tradTime = end - start;

    cout << "Traditional I/O:" << endl;
    cout << "Time taken: " << tradTime.count() << " ms" << endl;

    cout << "Performance Comparison:" << endl;
    cout << "Memory-Mapped I/O is " << (tradTime.count() / mmapTime.count()) << " times faster in this simulation." << endl;

    return 0;
}
