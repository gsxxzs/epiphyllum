
set_source_files_properties(test_cuda.cpp PROPERTIES LANGUAGE CUDA)
add_executable(test_cuda test_cuda.cpp)

target_link_libraries(test_cuda ${LIB_NAME})

add_executable(test_opencl test_opencl.cpp)
target_link_libraries(test_opencl ${LIB_NAME} OpenCL ${OpenCV_LIBS} fmt)


add_executable(test_kdtree test_kdtree.cpp ../opencl_kernel/ikdtree.cpp)
target_link_libraries(test_kdtree ${LIB_NAME} OpenCL ${OpenCV_LIBS} fmt)


install(TARGETS
  test_cuda test_opencl   
  RUNTIME DESTINATION bin
)
