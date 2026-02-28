cmake_minimum_required(VERSION 3.10)
project(image_processing_pipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- ENSURE THIS PATH IS CORRECT ---
#set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib")
set(OpenCV_DIR "C:/Amira/applications/opencv/build/x64/vc16/lib")

find_package(OpenCV REQUIRED)

# This tells the compiler where 'opencv2/opencv.hpp' lives
include_directories(${OpenCV_INCLUDE_DIRS})
# This tells the compiler where your own '.h' files live
include_directories(include)

set(SOURCES
        src/main.cpp
        src/filters.cpp
        src/noise.cpp
        src/edge_detection.cpp
        src/thresholding.cpp
)

add_executable(image_processing_pipeline ${SOURCES})

# Link the actual optimized OpenCV code
target_link_libraries(image_processing_pipeline ${OpenCV_LIBS})