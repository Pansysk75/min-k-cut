# Sets up or fetches Lemon graph library

# If we're not using the system Lemon, we need to fetch it
if(FETCH_LEMON AND NOT TARGET lemon)
  message(STATUS "Fetching Lemon")
  include(FetchContent)
  FetchContent_Declare(
    Lemon
    GIT_REPOSITORY
    https://github.com/The-OpenROAD-Project/lemon-graph.git
    GIT_TAG
    master
    )
    FetchContent_MakeAvailable(Lemon)

elseif(NOT TARGET lemon)
    find_package(Lemon REQUIRED)
endif()