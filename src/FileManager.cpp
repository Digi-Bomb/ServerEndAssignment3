#include "FileManager.h"

#include <fstream>

std::vector<std::string> FileManager::Read(const std::string& filename) {
    // Create and open file
    std::vector<std::string> data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return data;
    }

    // Read until the end of file
    std::string line;
    while (getline(file, line)) {
        data.push_back(line);
    }
    file.close();

    return data;
}

void FileManager::Write(const std::string& filename, const std::vector<std::string>& data) {
    // Create and open file
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    // Read until the end of file
    for (const std::string& datapoint : data) {
        file << datapoint << std::endl;
    }

    file.close();
}

