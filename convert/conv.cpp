/*
	Image fIle conversion/resizse Tool
	written by Jared Bruni
*/

#include "argz.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <vector>

bool convertFiles(const std::vector<std::string> &list_files, std::string_view output, const int &width, const int &height);

int main(int argc, char **argv) {
	Argz<std::string> argz;
	argz.initArgs(argc, argv).addOptionSingleValue('i', "input file").addOptionSingleValue('f', "image file input").addOptionSingleValue('o', "output file format").addOptionSingleValue('s', "scale resolution formst: WithxHeight").addOptionDoubleValue('I', "input", "input file").addOptionDoubleValue('O', "output", "output format").addOptionDoubleValue('S', "size", "scale resolution").addOptionDoubleValue('F', "file", "input image file");

	if(argc == 1) {
		argz.help(std::cout);
		return 0;
	}

	std::string input_file, output_format, image_size, image_file;

	try {
		int value{};
		Argument<std::string> arg;
		while((value = argz.proc(arg)) != -1) {
			switch(value) {
			case 'i':
			case 'I':
				input_file = arg.arg_value;
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
				image_file = arg.arg_value;
				break;
			}
		}
		if(output_format.length() == 0) {
			std::cerr << "Error missing output format arguments...\n";
			argz.help(std::cout);
			return 0;
		}

		if(image_file.length() == 0 && input_file.length() == 0) {
			std::cerr << "Error missing input file/list...\n";
			argz.help(std::cout);
			return 0;
		}
		std::vector<std::string> list_files;
		if(input_file.length() > 0) {
			std::fstream file;
			file.open(input_file, std::ios::in);
			while(!file.eof()) {
				std::string line;
				std::getline(file, line);
				if(file) {
					list_files.push_back(line);
				}
			}
		}
		if(image_file.length() > 0) {
			list_files.push_back(image_file);
		}
		if(image_size.length() > 0) {
			const auto pos{image_size.find("x")};
			if(pos == std::string::npos) {
				std::cerr << "Error use format WidthxHeight ex: 640x360\n";
				return 0;
			}
			const std::string left{image_size.substr(0, pos)};
			const std::string right{image_size.substr(pos + 1, image_size.length() - pos)};
			if(convertFiles(list_files, output_format, atoi(left.c_str()), atoi(right.c_str()))) {
				std::cout << "image_convert: success.\n";
			} else {
				std::cout << "image_convert: failed.\n";
			}
		} else {
			if(convertFiles(list_files, output_format, -1, -1)) {
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

bool convertFiles(const std::vector<std::string> &list_files, std::string_view output, const int &width, const int &height) {

	if(width == 0 || height == 0) {
		std::cerr << "invalid with/height\n";
		return false;
	}
	int converted{0};

	if(list_files.size() > 0) {
		for(const auto &i : list_files) {
			std::ostringstream stream;
			auto pos{i.rfind(".")};

			if(pos == std::string::npos)
				continue;

			std::string fname{i.substr(0, pos)};
			if(fname.length() > 0) {
				cv::Mat inputf{cv::imread(i)};
				if(width != -1 && height != -1) {
					stream << fname << "." << width << "x" << height << "." << output;
				} else {
					stream << fname << "." << output;
				}
				if(!inputf.empty()) {
					const std::string o_file{stream.str()};
					if(o_file == i)
						continue;
					std::cout << i << " -> " << o_file << "\n";
					try {
						if(width != -1 && height != -1) {
							cv::Mat resized;
							cv::resize(inputf, resized, cv::Size(width, height));
							cv::imwrite(o_file, resized);
							converted++;
						} else {
							cv::imwrite(o_file, inputf);
							converted++;
						}
					} catch(...) {
						std::cerr << i << " could not convert to format " << output << "...\n";
					}
				}
			}
		}
	}
	std::cout << "image(s) converted: " << converted << "\n";
	return true;
}
