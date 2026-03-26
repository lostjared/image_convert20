cmake_minimum_required(VERSION 3.16)
project(ImageConvertQt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

qt_standard_project_setup()

add_executable(ImageConvertQt
    src/main.cpp
    src/mainwindow.cpp
    src/converter.cpp
)

target_include_directories(ImageConvertQt PRIVATE
    src
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ImageConvertQt PRIVATE
    Qt6::Widgets
    ${OpenCV_LIBS}
)

install(TARGETS ImageConvertQt RUNTIME DESTINATION bin)
