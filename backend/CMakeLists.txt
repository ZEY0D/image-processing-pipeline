cmake_minimum_required(VERSION 3.10)
project(image_processing_pipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "G:/opencv/build")
find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/asio-1.36.0/asio-1.36.0/include
)

set(SOURCES
    src/main.cpp
    src/noise.cpp
    src/filters.cpp
    src/edge_detection.cpp
    src/histograms.cpp
    src/frequency_filters.cpp
    src/base64.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ws2_32)
target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601)