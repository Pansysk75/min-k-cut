set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE lemon)
target_include_directories(main PRIVATE ${INCLUDE_DIR})

add_subdirectory(tests)