# Add client executable file
add_executable(ZipStudio)

# Add client source file
target_sources(ZipStudio
    PRIVATE
        ${PROJECT_SOURCE_DIR}/client/ZipStudio/ZipStudio-Client.cpp
)

# Link to the main project library
target_link_libraries(ZipStudio
    PRIVATE
        zipstd
        ziplad
)
