add_executable(test_array test_array.cpp)
# target_link_libraries(test_array ${LIB_NAME})

add_executable(test_ring_queue test_ring_queue.cpp)
target_link_libraries(test_ring_queue ${LIB_NAME})

add_executable(test_plugin test_plugin.cpp)
target_link_libraries(test_plugin ${LIB_NAME})
set_target_properties(test_plugin PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")

# binary
install(TARGETS
  test_array test_ring_queue test_plugin
  RUNTIME DESTINATION bin
)