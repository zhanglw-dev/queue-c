cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(queue-c LANGUAGES C)


#message(STATUS ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_subdirectory(src/qc_base)
add_subdirectory(src/qc_mq)
add_subdirectory(src/qc_psist)
add_subdirectory(src/qc_qsys)
add_subdirectory(src/qc_net)
add_subdirectory(test)
