

# Adding something we can run - Output name matches target name
add_executable(main main.cpp)
target_link_libraries(main PRIVATE lemon)
