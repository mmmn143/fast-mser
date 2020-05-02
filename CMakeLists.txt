CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(MATEL)



include_directories(/usr/local/Cellar/llvm/10.0.0/include)
link_directories(/usr/local/Cellar/llvm/10.0.0/lib)

set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -O0 -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
link_libraries(libomp.a)

ADD_SUBDIRECTORY(basiclog)
ADD_SUBDIRECTORY(basicsys)
ADD_SUBDIRECTORY(basicmath)
ADD_SUBDIRECTORY(basicimg)