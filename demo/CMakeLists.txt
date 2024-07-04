if(ENABLE_TEST)
  add_subdirectory(test)
endif()

add_executable(hello hello_word.cpp)
target_link_libraries(hello ${LIB_NAME})
# binary
install(TARGETS
  hello
  RUNTIME DESTINATION bin
)