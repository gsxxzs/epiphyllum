
set_source_files_properties(test_cuda.cpp PROPERTIES LANGUAGE CUDA)
add_executable(test_cuda test_cuda.cpp)

target_link_libraries(test_cuda ${LIB_NAME})


