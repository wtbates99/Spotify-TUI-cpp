cmake_minimum_required(VERSION 3.10)
project(spotify_tui LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/spotify_auth.cpp
    src/spotify_operations.cpp
    src/utils.cpp
)

# Create the executable
add_executable(spotify_tui ${SOURCES})

# Find CURL library
find_package(CURL REQUIRED)
if(CURL_FOUND)
    message(STATUS "Found CURL: ${CURL_INCLUDE_DIRS}")
    target_include_directories(spotify_tui PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(spotify_tui PRIVATE ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "CURL library not found")
endif()

# Find RapidJSON
find_path(RAPIDJSON_INCLUDE_DIR rapidjson/document.h)
if(RAPIDJSON_INCLUDE_DIR)
    message(STATUS "Found RapidJSON: ${RAPIDJSON_INCLUDE_DIR}")
    target_include_directories(spotify_tui PRIVATE ${RAPIDJSON_INCLUDE_DIR})
else()
    message(FATAL_ERROR "RapidJSON library not found")
endif()

