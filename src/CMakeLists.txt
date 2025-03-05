## zipstd library
add_library(zipstd STATIC
    ${PROJECT_SOURCE_DIR}/src/zipstd/huffman/huffman.cpp
)

target_include_directories(zipstd PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(zipstd PUBLIC cxx_std_17)

## ziplab library
add_library(ziplab STATIC
    ${PROJECT_SOURCE_DIR}/src/ziplab/huffman/huffman.cpp
)

target_include_directories(ziplab PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(ziplab PUBLIC cxx_std_17)
