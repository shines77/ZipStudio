cmake_minimum_required(VERSION 3.15)

project(ZipStudio VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##
## See: https://blog.csdn.net/u012487272/article/details/12882283
##

# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## Configuration value are DEBUG, RELEASE, MINSIZEREL and RELWITHDEBINFO.

set(CMAKE_RELESE_POSTFIX "")
set(CMAKE_DEBUG_POSTFIX  "d")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -D_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")
else()
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
endif()

if (NOT MSVC)
    ## For C_FLAGS
    ## -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4a -msse4.1 -msse4.2 -mavx -mavx2 -mavx512vl -mavx512f
    if (CMakeIPOEnable)
        set(CMAKE_C_FLAGS_DEFAULT       "${CMAKE_C_FLAGS} -std=c99 -march=native -mtune=native -finput-charset=utf-8 -flto -ffat-lto-objects -fPIC -fpic")
    else()
        set(CMAKE_C_FLAGS_DEFAULT       "${CMAKE_C_FLAGS} -std=c99 -march=native -mtune=native -finput-charset=utf-8 -fPIC -fpic")
    endif()
    set(CMAKE_C_FLAGS_RELEASE           "${CMAKE_C_FLAGS_DEFAULT} -O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG             "${CMAKE_C_FLAGS_DEFAULT} -g -pg -D_DEBUG")
    set(CMAKE_C_FLAGS_MINSIZEREL        "${CMAKE_C_FLAGS_DEFAULT} -Os -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO    "${CMAKE_C_FLAGS_DEFAULT} -O2 -g")

    ## For CXX_FLAGS
    ## -Wall -Werror -Wextra -Wno-format -Wno-unused-function
    ## -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4a -msse4.1 -msse4.2 -mavx -mavx2 -mavx512vl -mavx512f
    # -fexec-charset=gbk -finput-charset=gbk
    if (CMakeIPOEnable)
        set(CMAKE_CXX_FLAGS_DEFAULT     "${CMAKE_CXX_FLAGS} -std=c++17 -march=native -mtune=native -finput-charset=utf-8 -flto -ffat-lto-objects -fPIC -fpic")
    else()
        set(CMAKE_CXX_FLAGS_DEFAULT     "${CMAKE_CXX_FLAGS} -std=c++17 -march=native -mtune=native -finput-charset=utf-8 -fPIC -fpic")
    endif()
    set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_DEFAULT} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEFAULT} -g -pg -D_DEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL      "${CMAKE_CXX_FLAGS_DEFAULT} -Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_DEFAULT} -O2 -g")

    set(CMAKE_AR        "/usr/bin/gcc-ar")
    set(CMAKE_LINKER    "/usr/bin/ld")
    set(CMAKE_NM        "/usr/bin/gcc-nm")
    set(CMAKE_OBJDUMP   "/usr/bin/objdump")
    set(CMAKE_RANLIB    "/usr/bin/gcc-ranlib")

    ## -faligned-new: GCC >= 7.1, CLANG >= 4.0
    ## See: https://trac.wildfiregames.com/ticket/5139
    if (CMAKE_COMPILER_IS_CLANGCXX)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "4.0")
            message("")
            message(STATUS "Clang >= 4.0, Added CXX_FLAGS: -faligned-new")
            message("")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -faligned-new")
        endif()
    elseif (CMAKE_COMPILER_IS_GNUCXX)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7.1")
            message("")
            message(STATUS "GCC >= 7.1, Added CXX_FLAGS: -faligned-new")
            message("")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -faligned-new")
        endif()
    endif()
endif()

if (WIN32)
    ## add_compile_options(-D__SSE3__ -D__SSE4A__ -D__SSE4_1__ -D__SSE4_2__)
    add_compile_options("-D_WIN32_WINNT=0x0601 -D_CRT_SECURE_NO_WARNINGS")
    ## set(EXTRA_LIBS ${EXTRA_LIBS} ws2_32 mswsock)
endif()

if (UNIX)
    set(EXTRA_LIBS ${EXTRA_LIBS} pthread)
else()
    set(EXTRA_LIBS ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif()

if (UNIX)
    # Adhere to GNU filesystem layout conventions
    include(GNUInstallDirs)
endif()

# Android needs to explicitly link against libm
if (ANDROID)
    target_link_libraries(${JSTD_HASHMAP_LIBNAME} m)
endif()

add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Switch CMAKE_BUILD_TYPE to Release"
)

# Set MSVC compile options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

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


