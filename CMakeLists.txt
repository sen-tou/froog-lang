cmake_minimum_required(VERSION 3.16)
project(Froog_Lang C)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Keep build artifacts out of source tree
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE PROJECT_SOURCES
    src/*.c
)

add_executable(froogc
    main.c
    ${PROJECT_SOURCES}
)

target_include_directories(froogc PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_compile_options(froogc PRIVATE -Wall -g)
