cmake_minimum_required(VERSION 3.15)

project(ZipStudio VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Append include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/test)
include_directories(${PROJECT_SOURCE_DIR}/bench)
include_directories(${PROJECT_SOURCE_DIR}/client)

# Append source directory CMakeLists.txt
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

# Append client directory CMakeLists.txt
add_subdirectory(${PROJECT_SOURCE_DIR}/client)

# Append test directory CMakeLists.txt
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

# Append bench directory CMakeLists.txt
add_subdirectory(${PROJECT_SOURCE_DIR}/bench)

# Set MSVC compile options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
