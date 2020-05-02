include_directories(include ../basicsys/include ../basiclog/include src/stdafx)

file(GLOB lib_source_files "src/*.cpp")
file(GLOB test_source_files "test/*.cpp")


add_library(basicmath ${lib_source_files})

add_executable(basicmath_test ${test_source_files} ${CMAKE_CXX_COMPLIER})

target_link_libraries(basicmath_test basicsys basiclog basicmath)