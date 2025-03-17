# Add benchmark executable file
add_executable(ZipStudio_bench)

# Add benchmark source file
target_sources(ZipStudio_bench
    PRIVATE
        ${PROJECT_SOURCE_DIR}/bench/ZipStudio_bench/ZipStudio_bench.cpp
)

# Link to the main project library
target_link_libraries(ZipStudio_bench
    PRIVATE
        zipstd
        ziplab
)
