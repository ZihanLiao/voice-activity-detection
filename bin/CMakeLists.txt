cmake_minimum_required(VERSION 3.14.5)
project(vad_test)

option(CXX11_ABI "whether to use CXX11_ABI libtorch" ON)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

message(NOTICE "c++ : " ${CMAKE_CXX_COMPILER})
message(STATUS "torch link_libraries : " ${TORCH_LIBRARIES})
# list(APPEND CMAKE_PREFIX_PATH "/Users/marlowe/libtorch")
# find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -g -rdynamic")
# set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

link_directories(start_vad_test PUBLIC /usr/local/include/lib /opt/intel/oneapi/mkl/latest/lib)
add_executable(start_vad_test main.cc)
target_link_libraries(start_vad_test PUBLIC utils frontend model boost_filesystem stdc++ ${TORCH_LIBRARIES} /opt/intel/oneapi/mkl/latest/lib glog::glog)