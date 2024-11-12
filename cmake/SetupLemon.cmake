# Sets up or fetches Lemon graph library

# If we're not using the system Lemon, we need to fetch it
if(FETCH_LEMON AND NOT TARGET lemon)
  message(STATUS "Fetching Lemon")
  include(FetchContent)
  FetchContent_Declare(
    Lemon
    GIT_REPOSITORY https://github.com/The-OpenROAD-Project/lemon-graph.git
    GIT_TAG master)
  FetchContent_MakeAvailable(Lemon)

  # Include directories are not included in the Lemon target, so we need to add
  # them manually
  target_include_directories(lemon INTERFACE ${lemon_SOURCE_DIR}/)
  target_include_directories(lemon INTERFACE ${lemon_BINARY_DIR}/)

elseif(NOT TARGET lemon)
  # I haven't tested this
  find_package(Lemon REQUIRED)
endif()
