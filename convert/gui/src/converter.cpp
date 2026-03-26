#include "converter.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <sstream>

namespace fs = std::filesystem;

std::vector<fs::path> loadInputList(const fs::path &inputFile, std::string &error) {
    std::vector<fs::path> list;
    std::ifstream file(inputFile);
    if (!file) {
        error = "Could not open input list: " + inputFile.string();
        return list;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            auto pos = inputFile.string().rfind("/");
            std::string path = inputFile.string().substr(0, pos);
            list.emplace_back(path + "/" + line);
        }
    }
    return list;
}

ConvertResult convertFiles(const ConvertOptions &options) {
    ConvertResult result;

    if (options.outputFormat.empty()) {
        result.log.push_back("Error: missing output format.");
        return result;
    }

    if (options.files.empty()) {
        result.log.push_back("Error: no input files supplied.");
        return result;
    }

    if (options.width == 0 || options.height == 0) {
        result.log.push_back("Error: invalid width/height.");
        return result;
    }

    if (options.quality < 0 || options.quality > 100) {
        result.log.push_back("Error: JPEG quality must be between 0 and 100.");
        return result;
    }

    std::string outputType;
    outputType.reserve(options.outputFormat.size());
    for (unsigned char ch : options.outputFormat) {
        outputType.push_back(static_cast<char>(std::tolower(ch)));
    }

    for (const auto &inputPath : options.files) {
        if (!inputPath.has_filename()) {
            result.log.push_back("Skipping path with no filename: " + inputPath.string());
            continue;
        }

        cv::Mat inputImage = cv::imread(inputPath.string());
        if (inputImage.empty()) {
            result.log.push_back("Could not load image: " + inputPath.string());
            continue;
        }

        fs::path outPath;
        std::string ext = "." + outputType;

        if (options.width != -1 && options.height != -1) {
            std::ostringstream name;
            name << inputPath.stem().string() << "." << options.width << "x" << options.height;
            outPath = inputPath.parent_path() / name.str();
            outPath.replace_extension(ext);
        } else {
            outPath = inputPath.parent_path() / inputPath.stem();
            outPath.replace_extension(ext);
        }

        std::error_code ec1;
        std::error_code ec2;
        fs::path absIn = fs::absolute(inputPath, ec1);
        fs::path absOut = fs::absolute(outPath, ec2);
        if (!ec1 && !ec2 && absIn == absOut) {
            result.log.push_back("Skipping same input/output path: " + inputPath.string());
            continue;
        }

        try {
            if (options.width != -1 && options.height != -1) {
                cv::Mat resized;
                cv::resize(inputImage, resized, cv::Size(options.width, options.height));
                if (outputType == "jpg" || outputType == "jpeg") {
                    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, options.quality};
                    cv::imwrite(outPath.string(), resized, params);
                } else {
                    cv::imwrite(outPath.string(), resized);
                }
            } else {
                if (outputType == "jpg" || outputType == "jpeg") {
                    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, options.quality};
                    cv::imwrite(outPath.string(), inputImage, params);
                } else {
                    cv::imwrite(outPath.string(), inputImage);
                }
            }

            std::ostringstream msg;
            msg << inputPath.string() << " -> " << outPath.string();
            if (outputType == "jpg" || outputType == "jpeg") {
                msg << " quality: " << options.quality;
            }
            result.log.push_back(msg.str());
            ++result.converted;
        } catch (...) {
            result.log.push_back("Conversion failed: " + inputPath.string());
        }
    }

    result.success = result.converted > 0;
    result.log.push_back("image(s) converted: " + std::to_string(result.converted));
    return result;
}
