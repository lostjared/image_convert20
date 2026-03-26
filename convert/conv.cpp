/*
	Image fIle conversion/resizse Tool
	written by Jared Bruni
*/

#include "argz.hpp"
#include <algorithm>
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

bool convertFiles(const std::vector<fs::path> &list_files, std::string_view output, int width, int height, int quality);

int main(int argc, char **argv) {
	Argz<std::string> argz;
	argz.initArgs(argc, argv)
		.addOptionSingleValue('i', "input file")
		.addOptionSingleValue('f', "image file input")
		.addOptionSingleValue('o', "output file format")
		.addOptionSingleValue('s', "scale resolution formst: WithxHeight")
		.addOptionSingleValue('q', "JPEG quality.")
		.addOptionDoubleValue('I', "input", "input file")
		.addOptionDoubleValue('O', "output", "output format")
		.addOptionDoubleValue('S', "size", "scale resolution")
		.addOptionDoubleValue('F', "file", "input image file")
		.addOptionDoubleValue('Q', "quality", "JPEG quality");

	if(argc == 1) {
		argz.help(std::cout);
		return 0;
	}

	fs::path input_file;
	fs::path image_file;
	std::string output_format, image_size;
	int quality = 100;
	try {
		int value{};
		Argument<std::string> arg;
		while((value = argz.proc(arg)) != -1) {
			switch(value) {
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
			case 'Q':
			case 'q':
				quality = strtol(arg.arg_value.c_str(), nullptr, 0);
				break;
			}
		}

		if(output_format.empty()) {
			std::cerr << "Error missing output format arguments...\n";
			argz.help(std::cout);
			return 0;
		}

		if(input_file.empty() && image_file.empty()) {
			std::cerr << "Error missing input file/list...\n";
			argz.help(std::cout);
			return 0;
		}

		std::vector<fs::path> list_files;

		if(!input_file.empty()) {
			std::ifstream file(input_file);
			if(!file) {
				std::cerr << "Error could not open input list: " << input_file << "\n";
				return 1;
			}

			std::string line;
			while(std::getline(file, line)) {
				if(!line.empty()) {
					list_files.emplace_back(line);
				}
			}
		}

		if(!image_file.empty()) {
			list_files.push_back(image_file);
		}

		if(!image_size.empty()) {
			const auto pos = image_size.find('x');
			if(pos == std::string::npos) {
				std::cerr << "Error use format WidthxHeight ex: 640x360\n";
				return 0;
			}

			const std::string left = image_size.substr(0, pos);
			const std::string right = image_size.substr(pos + 1);

			if(convertFiles(list_files, output_format, std::atoi(left.c_str()), std::atoi(right.c_str()), quality))	{

				std::cout << "image_convert: success.\n";
			} else {
				std::cout << "image_convert: failed.\n";
			}
		} else {
			if(convertFiles(list_files, output_format, -1, -1, quality)) {
				std::cout << "image_convert: success.\n";
			} else {
				std::cout << "image_convert: failed.\n";
			}
		}
	} catch(const ArgException<std::string> &e) {
		std::cerr << "Syntax Error: " << e.text() << "\n";
	}

	return 0;
}

bool convertFiles(const std::vector<fs::path> &list_files, std::string_view output, int width, int height, int quality) {
	if(width == 0 || height == 0) {
		std::cerr << "invalid width/height\n";
		return false;
	}
	int converted{0};
	std::string lower;
	for(auto &i : output) {
		lower += tolower(i);
	}
	std::string output_type = lower;
	for(const auto &input_path : list_files) {
		if(!input_path.has_filename())
			continue;

		cv::Mat inputf = cv::imread(input_path.string());
		if(inputf.empty())
			continue;

		fs::path out_path;
		std::string ext = ".";
		ext += output;

		if(width != -1 && height != -1) {
			std::ostringstream name;
			name << input_path.stem().string() << "." << width << "x" << height;
			out_path = input_path.parent_path() / name.str();
			out_path.replace_extension(ext);
		} else {
			out_path = input_path.parent_path() / input_path.stem();
			out_path.replace_extension(ext);
		}

		if(fs::absolute(input_path) == fs::absolute(out_path))
			continue;

		if(output_type == "jpg")
			std::cout << input_path << " -> " << out_path << " quality: " << quality << " \n";
		else
			std::cout << input_path << " -> " << out_path << "\n";

		try {
			if(width != -1 && height != -1) {
				cv::Mat resized;
				cv::resize(inputf, resized, cv::Size(width, height));
				if(output_type == "jpg") {
					std::vector<int> compression_params = {
      					  cv::IMWRITE_JPEG_QUALITY,
					  quality
					};
					cv::imwrite(out_path.string(), resized, compression_params);
				} else {
					cv::imwrite(out_path.string(), resized);
				}
				++converted;
			} else {
				if(output_type == "jpg") {
					std::vector<int> compression_params = {
					        cv::IMWRITE_JPEG_QUALITY, 
				        	quality
					};
					cv::imwrite(out_path.string(), inputf, compression_params);
				} else cv::imwrite(out_path.string(), inputf);
				++converted;
			}
		} catch(...) {
			std::cerr << input_path << " could not convert to format " << output << "...\n";
		}
	}

	std::cout << "image(s) converted: " << converted << "\n";
	return true;
}
