cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(queue-c LANGUAGES C)


#message(STATUS ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/qc_base)
add_subdirectory(src/qc_mq)
add_subdirectory(src/qc_psist)
add_subdirectory(src/qc_qsys)
add_subdirectory(src/qc_sock)
add_subdirectory(test)
