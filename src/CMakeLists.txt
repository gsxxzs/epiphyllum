if(ENABLE_CUDA)
  FILE(GLOB CUDA_SRCS ./kernel/*.cu)
  if(ENABLE_TEST)
    add_subdirectory(./kernel/test/)
  endif()
endif(ENABLE_CUDA)

FILE(GLOB SRCS ./*.cc ./*.cpp ./*/*.cc ./*/*.cpp)
add_library(${LIB_NAME} SHARED ${SRCS} ${CUDA_SRCS})

if(ENABLE_CUDA)
  set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
  find_library(CUDART_LIBRARY cudart ${CUDA_TOOLKIT_ROOT_DIR}/lib64)

  target_link_libraries(${LIB_NAME} ${CUDART_LIBRARY})
endif(ENABLE_CUDA)

install(TARGETS
  ${LIB_NAME}
  LIBRARY DESTINATION lib
)