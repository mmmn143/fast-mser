include_directories(include ../basiclog/include src/stdafx)

file(GLOB lib_source_files "src/*.cpp")
file(GLOB test_source_files "test/*.cpp")


add_library(basicsys ${lib_source_files})

add_executable(basicsys_test ${test_source_files} ${CMAKE_CXX_COMPLIER})

target_link_libraries(basicsys_test basicsys basiclog)