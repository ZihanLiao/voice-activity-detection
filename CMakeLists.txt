cmake_minimum_required(VERSION 3.0.0)
project(vad VERSION 0.1.0)

message(NOTICE "c++ : " ${CMAKE_CXX_COMPILER})

set(CMAKE_CXX_FLAGS "-std=c++14 -fpermissive")
list(APPEND CMAKE_PREFIX_PATH /Users/marlowe/workspace/libtorch-1.12.1/libtorch)
find_package(Torch REQUIRED PATHS /Users/marlowe/workspace/libtorch-1.12.1/libtorch NO_DEFAULT_PATH)
find_package(GLOG REQUIRED PATHS /Users/marlowe/workspace/glog)
# find_library(TORCH_LIBRARIES libc10.dylib libkineto.a PATHS /Users/marlowe/workspace/libtorch-1.12.1/libtorch/lib)
# set(TORCH_LIBRARIES /Users/marlowe/workspace/libtorch-1.12.1/libtorch/lib)
message(STATUS "Torch libraries ${TORCH_LIBRARIES}")
message(STATUS "Torch include dir ${TORCH_INCLUDE_DIRS}")
set(INCLUDE_DIR /usr/local/include)
# set(JSON_DIR /Users/gaoxinglong/my_home/lib/json/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIR}
    ${JSON_DIR})


set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

include_directories(
    "/usr/local/include"
)

link_directories(
 "/usr/local/include/lib"
)

add_subdirectory(utils)
add_subdirectory(frontend)
add_subdirectory(model)
add_subdirectory(bin)