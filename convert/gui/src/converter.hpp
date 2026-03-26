#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

struct ConvertOptions {
    std::vector<std::filesystem::path> files;
    std::string outputFormat;
    int width = -1;
    int height = -1;
    int quality = 100;
};

struct ConvertResult {
    bool success = false;
    int converted = 0;
    std::vector<std::string> log;
};

ConvertResult convertFiles(const ConvertOptions &options);
std::vector<std::filesystem::path> loadInputList(const std::filesystem::path &inputFile, std::string &error);
