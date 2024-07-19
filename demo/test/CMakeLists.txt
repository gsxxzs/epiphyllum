add_executable(test_array test_array.cpp)
# target_link_libraries(test_array ${LIB_NAME})

# binary
install(TARGETS
  test_array
  RUNTIME DESTINATION bin
)

add_executable(test_ring_queue test_ring_queue.cpp)
target_link_libraries(test_ring_queue ${LIB_NAME})

# binary
install(TARGETS
  test_ring_queue
  RUNTIME DESTINATION bin
)