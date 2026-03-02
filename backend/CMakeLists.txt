cmake_minimum_required(VERSION 3.10)
project(image_processing_pipeline)

# --------------------------
# C++ standard
# --------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --------------------------
# OpenCV
# --------------------------
set(OpenCV_DIR "D:/Downloads/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

# --------------------------
# Include directories
# --------------------------
include_directories(
    ${OpenCV_INCLUDE_DIRS}                                           # OpenCV headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include                              # Your include folder
    ${CMAKE_CURRENT_SOURCE_DIR}/include/asio-asio-1-30-2/asio/include  # Asio headers
)

# --------------------------
# Source files
# --------------------------
set(SOURCES
    src/main.cpp
    src/noise.cpp
    src/filters.cpp
    src/edge_detection.cpp
    src/histograms.cpp
    src/frequency_filters.cpp
    src/base64.cpp   # <-- Add this line explicitly
)

# --------------------------
# Create executable
# --------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

# --------------------------
# Link libraries
# --------------------------
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# --------------------------
# Preprocessor definitions
# --------------------------
# Crow settings (disable SSL if not needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601)

# --------------------------
# Optional: Compiler warnings (Windows/MSVC)
# --------------------------
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
endif()

# --------------------------
# Set output directory
# --------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Release"
)

# --------------------------
# Optional: Link Windows libraries if needed
# --------------------------
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()