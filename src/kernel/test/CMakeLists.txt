
set_source_files_properties(test_cuda.cpp PROPERTIES LANGUAGE CUDA)
add_executable(test_cuda test_cuda.cpp)

target_link_libraries(test_cuda ${LIB_NAME})

set(OpenCV_LIB opencv_core opencv_imgcodecs opencv_highgui)

add_executable(test_opencl test_opencl.cpp)
target_link_libraries(test_opencl ${LIB_NAME} OpenCL ${OpenCV_LIB} fmt)

install(TARGETS
  test_cuda test_opencl   
  RUNTIME DESTINATION bin
)
