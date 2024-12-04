#pragma once

#include <iostream>
#include <vector>

namespace FileManager {
    std::vector<std::string> Read(const std::string& filename);
    void Write(const std::string& filename, const std::vector<std::string>& data);
}

