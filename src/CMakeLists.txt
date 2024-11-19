set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE lemon)
target_include_directories(main PRIVATE ${INCLUDE_DIR})