cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_base LANGUAGES C)

#message(STATUS ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_subdirectory(qc_base)
add_subdirectory(qc_mq)
add_subdirectory(qc_qdb)
add_subdirectory(qc_qsys)
add_subdirectory(qc_net)