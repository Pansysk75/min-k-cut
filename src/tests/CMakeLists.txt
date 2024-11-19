set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)

function (_add_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PRIVATE lemon)
  target_include_directories(${name} PRIVATE ${INCLUDE_DIR})
endfunction()


_add_test(test_mtx_reader)