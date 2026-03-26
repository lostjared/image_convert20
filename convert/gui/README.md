# ImageConvertQt

Qt6/OpenCV desktop version of the image conversion tool.

## Features

- Convert one image, a text file containing image paths, or both
- Resize to a specific width and height
- Set JPEG quality from 0 to 100
- Choose output format from the UI
- View a live conversion log

## Build with CMake

```bash
cmake -S . -B build
cmake --build build
./build/ImageConvertQt
```

## Build with the included Makefile

```bash
make
make run
```

## Dependencies

- Qt6 Widgets
- OpenCV

On many Linux systems, the package names are similar to:

- `qt6-base-dev` or `qt6-base`
- `opencv` or `opencv-devel`
