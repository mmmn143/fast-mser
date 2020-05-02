include_directories(include ../basicsys/include ../basiclog/include ../basicmath/include src/stdafx /usr/local/Cellar/opencv@3/3.4.9_2/lib)

link_directories(/usr/local/Cellar/opencv@3/3.4.9_2/lib)

link_libraries(libopencv_core.a)
link_libraries(libopencv_imgproc.a)
link_libraries(libopencv_imgcodecs.a)
link_libraries(libopencv_highgui.a)

file(GLOB lib_source_files "src/*.cpp")
file(GLOB test_source_files "test/*.cpp")


add_library(basicimg ${lib_source_files})

add_executable(basicimg_test ${test_source_files} ${CMAKE_CXX_COMPLIER})

target_link_libraries(basicimg_test basicsys basiclog basicmath basicmath)