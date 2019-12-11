cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(test LANGUAGES C)

add_executable(test 
               test_mq.h 
               test_mq.c 
               test_psist.h 
               test_psist.c 
               test_qsys.h 
               test_qsys.c 
               test_sock.h 
               test_sock.c 
               main.c 
              )

include_directories("../src/qc_base")
include_directories("../src/qc_mq")
include_directories("../src/qc_psist")
include_directories("../src/qc_qsys")
include_directories("../src/qc_sock")

#message(STATUS ${CMAKE_BINARY_DIR}/lib)
link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(test qc)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(test ws2_32)
else()
    target_link_libraries(test pthread)
endif()

target_compile_definitions(test PUBLIC "_CRT_SECURE_NO_WARNINGS")