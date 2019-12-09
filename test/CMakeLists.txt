cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(test LANGUAGES C)

add_executable(test 
               test_mq.h 
               test_mq.c 
               test_psist.h 
               test_psist.c 
               test_qsys.h 
               test_qsys.c 
               test_net.h 
               test_net.c 
               main.c 
              )

include_directories("../qc_base")
include_directories("../qc_mq")
include_directories("../qc_psist")
include_directories("../qc_qsys")
include_directories("../qc_net")

#message(STATUS ${CMAKE_BINARY_DIR}/Lib)
link_directories(${CMAKE_BINARY_DIR}/Lib)
target_link_libraries(test qc_base qc_mq qc_psist qc_qsys qc_net)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(test ws2_32)
else()
    target_link_libraries(test pthread)
endif()
