#include <iostream>
#include <vector>
#include <set>
#include <iomanip>
using namespace std;

int main() {
    vector<int> referenceString = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int delta = 3;

    cout << "Working Set Simulator" << endl;
    cout << "Reference String: ";
    for (int p : referenceString) cout << p << " ";
    cout << endl;
    cout << "Window Size (Delta): " << delta << endl << endl;

    cout << left << setw(5) << "Time" << "|"
         << setw(5) << "Page" << "|"
         << setw(15) << "Working Set" << "|"
         << setw(8) << "WS Size" << endl;
    cout << "-----|-----|---------------|--------" << endl;

    vector<int> workingSetSizes;
    for (size_t i = 0; i < referenceString.size(); ++i) {
        set<int> workingSet;
        int start = (i >= delta - 1) ? i - delta + 1 : 0;
        for (size_t j = start; j <= i; ++j) {
            workingSet.insert(referenceString[j]);
        }

        cout << left << setw(5) << (i + 1) << "|"
             << setw(5) << referenceString[i] << "|"
             << "{";
        bool first = true;
        for (int p : workingSet) {
            if (!first) cout << ",";
            cout << p;
            first = false;
        }
        cout << "}" << setw(15 - workingSet.size() * 2) << "" << "|"
             << setw(8) << workingSet.size() << endl;

        workingSetSizes.push_back(workingSet.size());
    }

    double totalSize = 0;
    for (int s : workingSetSizes) totalSize += s;
    double averageSize = totalSize / referenceString.size();

    cout << endl << "Average Working Set Size: " << fixed << setprecision(2) << averageSize << endl;

    return 0;
}
