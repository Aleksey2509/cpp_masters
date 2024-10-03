cmake_minimum_required(VERSION 3.22)

project(cow_string VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)

# Find Google test
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    message(WARNING "Google tests are not found on system. Testing is disabled")
else()
    enable_testing()
endif ()

add_library(cow_string INTERFACE)

target_include_directories(cow_string INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
