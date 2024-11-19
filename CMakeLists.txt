cmake_minimum_required(VERSION 3.11) # cmake 11+ has fetch content

# This is your project statement. You should always list languages; Listing the
# version is nice here since it sets lots of useful variables
project(
  min-k-cut-project
  VERSION 1.0
  LANGUAGES CXX)

# We need to enable C++ 11 and ahead but for the sake of uniform behaviour we stick
# with the latest adoption of C++ 17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This is needed because lemon uses `register` storage
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

# helps your code editor and extensions
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# If you set any CMAKE_ variables, that can go here. (But usually don't do this,
# except maybe for C++ standard)
option(FETCH_LEMON "Fetch Lemon graph library" ON)

# Find packages go here.
include(SetupLemon)

add_subdirectory(src)
