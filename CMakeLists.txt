cmake_minimum_required(VERSION 3.5.1)

project(epiphyllum)

option(ENABLE_TEST "Build test" ON)
option(ENABLE_CUDA "Build cuda" ON)
if (ENABLE_CUDA)
  add_definitions(-DENABLE_CUDA)
endif ()

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/output)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CUDA_FLAGS_DEBUG "-g -G") 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fPIC  -Wall -Werror=return-type -Wno-error=shadow")
include_directories(include)

set(LIB_NAME epiphyllum)

include(extern_dep.cmake)

#设置可执行文件保存的目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#设置库文件保存的目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if(ENABLE_CUDA)
  find_package(OpenCV REQUIRED)
  find_package(CUDA REQUIRED)
  enable_language(CUDA)
  include_directories(${OpenCV_INCLUDE_DIRS} ${CUDA_INCLUDE_DIRS})
  link_directories(${CUDA_LIBRARY_DIRS})
endif()

add_subdirectory(src)
add_subdirectory(demo)