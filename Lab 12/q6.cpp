#include<iostream>
#include<filesystem>
namespace fs = std::filesystem;
void traverseDirectory(const fs::path& dirPath, int depth = 0) {
std::string indent(depth * 2, ' ');
try {
for (const auto& entry : fs::directory_iterator(dirPath)) {
if (entry.is_directory()) {
std::cout << indent << "[DIR] " << entry.path().filename().string()
          << "/" << std::endl;
traverseDirectory(entry.path(), depth + 1); // Recurse
} else {
std::cout << indent << "[FILE] " << entry.path().filename().string()
          << " (" << entry.file_size() << " B)" << std::endl;
}
}
} catch (const fs::filesystem_error& e) {
std::cout << indent << " [Permission denied: " << e.what() << "]" << std::endl;
}
}
int main() {
traverseDirectory(".");
return 0;
}
// This mirrors the OS inode traversal used in hierarchical
// (tree-structured) directory systems described in OSC Ch.13