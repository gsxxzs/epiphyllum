add_executable(hello hello_word.cpp)

# binary
install(TARGETS
  hello
  RUNTIME DESTINATION bin
)