FILE(GLOB SRCS ./*.cc ./*.cpp ./*/*.cc ./*/*.cpp)
add_library(${LIB_NAME} SHARED ${SRCS})