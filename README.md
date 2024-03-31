
# Image File Conversion/Resize Tool

This tool is designed for converting and resizing image files. It leverages the power of OpenCV to handle various image operations efficiently. This utility allows for batch processing of images, resizing based on specified dimensions, and supports multiple input and output formats.

## Features

- Convert image files between different formats.
- Resize images to specified dimensions.
- Batch processing of multiple images.
- Command-line interface for easy scripting and automation.

## Requirements

- C++ compiler with C++20 support.
- OpenCV library installed.

## Installation

To use this tool, you must have OpenCV installed on your system. After ensuring OpenCV is set up, follow these steps:

1. Clone this repository or download the source code.
2. Compile the source code using a C++ compiler, ensuring the OpenCV library is correctly linked. For example:

```bash
make
sudo make install
```

## Usage

The tool is used through the command line, with several options available for specifying input and output parameters.

```
Usage: image_convert [options]

Options:
  -i, --input file       Specify an input file list.
  -f, --image file input Specify a single image file for input.
  -o, --output file format Specify the output file format (e.g., jpg, png).
  -s, --scale resolution Specify the scale resolution in the format WidthxHeight (e.g., 800x600).
  -I, --input            Alternative way to specify an input file.
  -O, --output           Alternative way to specify output format.
  -S, --size             Alternative way to specify scale resolution.
  -F, --file             Alternative way to specify a single image file input.
```

### Examples

- Convert a single image to JPG format:

```bash
./image_convert -f image.png -o jpg
```

- Resize and convert an image to PNG format:

```bash
./image_convert -f image.jpg -s 1024x768 -o png
```

- Process multiple images listed in a file:

```bash
./image_convert -i input_list.txt -o png
```

Each line in `input_list.txt` should contain the path to an individual image file.

## How It Works

The tool reads the command-line arguments to determine the operation mode, input files, output format, and dimensions for resizing. It then processes each input image according to the specified options, performing conversion and/or resizing as needed, and saves the result to the specified output format.

Note: Ensure that the dimensions for resizing are positive integers, and the output format is supported by OpenCV.
