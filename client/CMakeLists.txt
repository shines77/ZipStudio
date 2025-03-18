# Add client executable file
add_executable(ZipStudio)

# Add client include directories
target_include_directories(benchmark
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ZipStudio"
        "${CMAKE_CURRENT_LIST_DIR}/../src"
        ${EXTRA_INCLUDES}
)

# Add client source file
target_sources(ZipStudio
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ZipStudio/ZipStudio.cpp"
)

# Link to the main project library
target_link_libraries(ZipStudio
    PRIVATE
        zipstd
        ziplab
        ${EXTRA_LIBS}
)

if (NOT MSVC)
    # For gcc or clang warning setting
    target_compile_options(ZipStudio
        PUBLIC
            -Wall -Wno-unused-function -Wno-deprecated-declarations -Wno-unused-variable -Wno-deprecated
    )
else()
    # Warning level 3 and all warnings as errors
    target_compile_options(ZipStudio PUBLIC /W3 /WX)
endif()
