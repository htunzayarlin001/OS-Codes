#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int main() {
    fs::path dirPath = "."; // Current directory

    std::cout << "Directory listing for: " << fs::absolute(dirPath) << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::cout << "[FILE] " << entry.path().filename().string()
                      << " (" << entry.file_size() << " bytes)" << std::endl;
        } else if (entry.is_directory()) {
            std::cout << "[DIR] " << entry.path().filename().string() << "/" << std::endl;
        } else {
            std::cout << "[OTHER] " << entry.path().filename().string() << std::endl;
        }
    }
    return 0;
}
