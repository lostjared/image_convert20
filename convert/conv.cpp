/*
    Image File Conversion/Resize Tool
    written by Jared Bruni
*/

#include "argz.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

static std::string trim(std::string s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || std::isspace(static_cast<unsigned char>(s.back()))))
        s.pop_back();

    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
        ++start;

    if (start > 0)
        s.erase(0, start);

    return s;
}

static std::string toLower(std::string_view text) {
    std::string out;
    out.reserve(text.size());
    for (unsigned char ch : text)
        out += static_cast<char>(std::tolower(ch));
    return out;
}

bool convertFiles(const std::vector<fs::path> &list_files, std::string_view output, int width, int height, int quality);

int main(int argc, char **argv) {
    Argz<std::string> argz;
    argz.initArgs(argc, argv)
        .addOptionSingleValue('i', "input file")
        .addOptionSingleValue('f', "image file input")
        .addOptionSingleValue('o', "output file format")
        .addOptionSingleValue('s', "scale resolution format: WidthxHeight")
        .addOptionSingleValue('q', "JPEG quality")
        .addOptionDoubleValue('I', "input", "input file")
        .addOptionDoubleValue('O', "output", "output format")
        .addOptionDoubleValue('S', "size", "scale resolution")
        .addOptionDoubleValue('F', "file", "input image file")
        .addOptionDoubleValue('Q', "quality", "JPEG quality");

    if (argc == 1) {
        argz.help(std::cout);
        return 0;
    }

    fs::path input_file;
    fs::path image_file;
    std::string output_format;
    std::string image_size;
    int quality = 100;

    try {
        int value{};
        Argument<std::string> arg;

        while ((value = argz.proc(arg)) != -1) {
            switch (value) {
            case 'i':
            case 'I':
                input_file = fs::path(arg.arg_value);
                break;
            case 'o':
            case 'O':
                output_format = arg.arg_value;
                break;
            case 's':
            case 'S':
                image_size = arg.arg_value;
                break;
            case 'f':
            case 'F':
                image_file = fs::path(arg.arg_value);
                break;
            case 'q':
            case 'Q':
                quality = static_cast<int>(std::strtol(arg.arg_value.c_str(), nullptr, 10));
                break;
            default:
                break;
            }
        }

        if (output_format.empty()) {
            std::cerr << "Error missing output format argument.\n";
            argz.help(std::cout);
            return 1;
        }

        if (quality < 0 || quality > 100) {
            std::cerr << "Error quality out of range, must be between 0-100.\n";
            return 1;
        }

        if (input_file.empty() && image_file.empty()) {
            std::cerr << "Error missing input file/list.\n";
            argz.help(std::cout);
            return 1;
        }

        std::vector<fs::path> list_files;

        if (!input_file.empty()) {
            std::ifstream file(input_file);
            if (!file) {
                std::cerr << "Error could not open input list: " << input_file << "\n";
                return 1;
            }

            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (!line.empty())
                    list_files.emplace_back(line);
            }
        }

        if (!image_file.empty())
            list_files.push_back(image_file);

        if (list_files.empty()) {
            std::cerr << "Error no input files were found.\n";
            return 1;
        }

        int width = -1;
        int height = -1;

        if (!image_size.empty()) {
            std::size_t pos = image_size.find('x');
            if (pos == std::string::npos)
                pos = image_size.find('X');

            if (pos == std::string::npos) {
                std::cerr << "Error use format WidthxHeight ex: 640x360\n";
                return 1;
            }

            const std::string left = trim(image_size.substr(0, pos));
            const std::string right = trim(image_size.substr(pos + 1));

            width = std::atoi(left.c_str());
            height = std::atoi(right.c_str());

            if (width <= 0 || height <= 0) {
                std::cerr << "Error invalid size, width and height must be greater than zero.\n";
                return 1;
            }
        }

        if (convertFiles(list_files, output_format, width, height, quality))
            std::cout << "image_convert: success.\n";
        else
            std::cout << "image_convert: failed.\n";
    } catch (const ArgException<std::string> &e) {
        std::cerr << "Syntax Error: " << e.text() << "\n";
        return 1;
    }

    return 0;
}

bool convertFiles(const std::vector<fs::path> &list_files, std::string_view output, int width, int height, int quality) {
    if (width == 0 || height == 0) {
        std::cerr << "Error invalid width/height.\n";
        return false;
    }

    const std::string output_type = toLower(output);
    if (output_type.empty()) {
        std::cerr << "Error empty output type.\n";
        return false;
    }

    int converted = 0;
    int failed = 0;

    for (const auto &input_path : list_files) {
        if (!input_path.has_filename()) {
            ++failed;
            continue;
        }

        cv::Mat inputf = cv::imread(input_path.string(), cv::IMREAD_UNCHANGED);
        if (inputf.empty()) {
            std::cerr << "Error could not load file: " << input_path << "\n";
            ++failed;
            continue;
        }

        fs::path out_path;
        const std::string ext = "." + output_type;

        if (width != -1 && height != -1) {
            std::string name = input_path.filename().string();
            std::size_t pos = name.rfind('.');
            if (pos != std::string::npos)
                name = name.substr(0, pos);

            std::ostringstream outname;
            outname << name << "." << width << "x" << height << ext;
            out_path = input_path.parent_path() / outname.str();
        } else {
            out_path = input_path;
            out_path.replace_extension(ext);
        }

        std::error_code ec1;
        std::error_code ec2;
        fs::path abs_in = fs::absolute(input_path, ec1);
        fs::path abs_out = fs::absolute(out_path, ec2);

        if (!ec1 && !ec2 && abs_in == abs_out) {
            std::cerr << "Skipping same input/output path: " << input_path << "\n";
            ++failed;
            continue;
        }

        if (output_type == "jpg" || output_type == "jpeg")
            std::cout << input_path << " -> " << out_path << " quality: " << quality << "\n";
        else
            std::cout << input_path << " -> " << out_path << "\n";

        std::vector<int> compression_params;
        if (output_type == "jpg" || output_type == "jpeg")
            compression_params = {cv::IMWRITE_JPEG_QUALITY, quality};

        try {
            bool ok = false;

            if (width != -1 && height != -1) {
                cv::Mat resized;
                cv::resize(inputf, resized, cv::Size(width, height));

                if (!compression_params.empty())
                    ok = cv::imwrite(out_path.string(), resized, compression_params);
                else
                    ok = cv::imwrite(out_path.string(), resized);
            } else {
                if (!compression_params.empty())
                    ok = cv::imwrite(out_path.string(), inputf, compression_params);
                else
                    ok = cv::imwrite(out_path.string(), inputf);
            }

            if (ok) {
                ++converted;
            } else {
                std::cerr << "Error writing file: " << out_path << "\n";
                ++failed;
            }
        } catch (const cv::Exception &e) {
            std::cerr << input_path << " could not convert to format " << output_type << ": " << e.what() << "\n";
            ++failed;
        } catch (const std::exception &e) {
            std::cerr << input_path << " failed: " << e.what() << "\n";
            ++failed;
        }
    }
    std::cout << "image(s) converted: " << converted << "\n";
    if (failed > 0)
        std::cout << "image(s) failed/skipped: " << failed << "\n";
    return converted > 0;
}
