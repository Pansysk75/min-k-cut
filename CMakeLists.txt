cmake_minimum_required(VERSION 3.10)

# This is your project statement. You should always list languages; Listing the
# version is nice here since it sets lots of useful variables
project(
  min-k-cut-project
  VERSION 1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# If you set any CMAKE_ variables, that can go here. (But usually don't do this,
# except maybe for C++ standard)
option(FETCH_LEMON "Fetch Lemon graph library" ON)

# Find packages go here.
include(SetupLemon)

add_subdirectory(src)
