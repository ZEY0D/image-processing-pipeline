cmake_minimum_required(VERSION 3.10)

# Name your project
project(ImageProcessingPipeline)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 🚨 THIS IS THE CRITICAL LINK TO YOUR NEW DOWNLOAD 🚨
set(OpenCV_DIR "C:/msys64/ucrt64/lib/cmake/opencv4")

# Find the OpenCV library
find_package(OpenCV REQUIRED)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

# Include our own header files from the "include" folder
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Gather all .cpp files in the src folder
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link OpenCV libraries to the executable
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})