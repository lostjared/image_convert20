cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(image_convert20 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQURIED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)


add_executable(image_conver20 conv.cpp)
include_directories({CMAKE_SOURCE_DIR $(OpenCV_INCLUDE_DIRS))
link_directories(/usr/local/lib /usr/lib /opt/homebrew/lib)
install(TARGETS image_convert20 DESTINATION bin)
