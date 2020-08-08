cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qctest LANGUAGES C)

add_executable(qctest 
               test_mq.h 
               test_mq.c 
               test_psist.h 
               test_psist.c 
               test_qsys.h 
               test_qsys.c 
               test_sock.h 
               test_sock.c 
			   test_sem.h
			   test_sem.c 
			   test_shm.h
			   test_shm.c
               test_log.c
               main.c 
              )

include_directories("../src/qc_base")
include_directories("../src/qc_mq")
include_directories("../src/qc_psist")
include_directories("../src/qc_qsys")
include_directories("../src/qc_sock")

#message(STATUS ${CMAKE_BINARY_DIR}/lib)
link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(qctest qc)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(qctest ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")    
    target_link_libraries(qctest pthread)
else()
    target_link_libraries(qctest rt pthread)
endif()

target_compile_definitions(qctest PUBLIC "_CRT_SECURE_NO_WARNINGS")

install(
    TARGETS
    qctest
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)