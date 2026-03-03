cmake_minimum_required(VERSION 3.10)
project(image_processing_pipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------
# OpenCV
# --------------------------
set(OpenCV_DIR "D:/Downloads/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}                                           # OpenCV headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include                              # Your include folder
    ${CMAKE_CURRENT_SOURCE_DIR}/include/asio-asio-1-30-2/asio/include  # Asio headers
)

set(SOURCES
    src/main.cpp
    src/noise.cpp
    src/filters.cpp
    src/edge_detection.cpp
    src/histograms.cpp
    src/histogram_draw.cpp
    src/enhance.cpp
    src/frequency_filters.cpp
    src/base64.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ws2_32)
target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601)