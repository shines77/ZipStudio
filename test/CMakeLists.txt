# Add test executable file
add_executable(ZipLab_test)

# Add test source file
target_sources(ZipLab_test
    PRIVATE
        ${PROJECT_SOURCE_DIR}/test/ZipLab_test/ZipLab_test.cpp
)

# Link to the main project library
target_link_libraries(ZipLab_test
    PRIVATE
        zipstd
        ziplad
)
