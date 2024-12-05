include(CPM.cmake)

# CPMAddPackage(
#   NAME yaml-cpp
#   GIT_TAG 0.8.0
#   GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
# )
# include_directories(${yaml-cpp_SOURCE_DIR}/include)
# link_directories(${yaml-cpp__SOURCE_DIR}/lib)

CPMAddPackage(
  NAME fmt
  GIT_TAG 9.1.0
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)
include_directories(${fmt_SOURCE_DIR}/include)
link_directories(${fmt__SOURCE_DIR}/lib)