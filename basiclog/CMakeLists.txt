include_directories(include ../basicsys/include src/stdafx)

file(GLOB lib_source_files "src/*.cpp")
file(GLOB test_source_files "test/*.cpp")


add_library(basiclog ${lib_source_files})

add_executable(basiclog_test ${test_source_files} ${CMAKE_CXX_COMPLIER})

target_link_libraries(basiclog_test basicsys basiclog)