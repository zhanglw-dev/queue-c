cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(demo LANGUAGES C)

add_executable(demo 
               demo_mq.h
               demo_mq.c 
               main.c 
              )

include_directories("../src/qc_base")
include_directories("../src/qc_mq")
include_directories("../src/qc_psist")
include_directories("../src/qc_qsys")
include_directories("../src/qc_sock")

#message(STATUS ${CMAKE_BINARY_DIR}/lib)
link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(demo qc)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(demo ws2_32)
else()
    target_link_libraries(demo pthread)
endif()

target_compile_definitions(demo PUBLIC "_CRT_SECURE_NO_WARNINGS")