
cmake_minimum_required(VERSION 3.10)
project(spotify_tui LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Define source files
set(SOURCES
    src/main.cpp
    src/spotify_auth.cpp
    src/utils.cpp
    src/base64.cpp
    src/spotify_operations/PlaylistOperations.cpp
    src/spotify_operations/PlaybackOperations.cpp
    src/spotify_operations/RecommendationsOperations.cpp
    src/spotify_operations/SearchOperations.cpp
    src/spotify_operations/LibraryOperations.cpp
)

# Create the executable
add_executable(spotify_tui ${SOURCES})

# Include directories
target_include_directories(spotify_tui PRIVATE include)

# Find CURL library
find_package(CURL REQUIRED)
if(CURL_FOUND)
    target_include_directories(spotify_tui PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(spotify_tui PRIVATE ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "CURL library not found")
endif()

# Find RapidJSON
find_path(RAPIDJSON_INCLUDE_DIR rapidjson/document.h)
if(RAPIDJSON_INCLUDE_DIR)
    add_library(RapidJSON INTERFACE)
    target_include_directories(RapidJSON INTERFACE ${RAPIDJSON_INCLUDE_DIR})
    target_link_libraries(spotify_tui PRIVATE RapidJSON)
else()
    message(FATAL_ERROR "RapidJSON library not found")
endif()

