add_executable(test_array test_array.cpp)
# target_link_libraries(test_array ${LIB_NAME})

# binary
install(TARGETS
  test_array
  RUNTIME DESTINATION bin
)