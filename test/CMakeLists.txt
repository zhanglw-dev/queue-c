cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qctest LANGUAGES C)


include_directories("../src/qc_base")
include_directories("../src/qc_mq")
include_directories("../src/qc_psist")
include_directories("../src/qc_qsys")
include_directories("../src/qc_sock")
include_directories("../src/qc_shmq")

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


add_executable(test_shmqget 
               test_shmqget.c
              )

add_executable(test_shmqput 
               test_shmqput.c
              )

#message(STATUS ${CMAKE_BINARY_DIR}/lib)
link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(qctest qc)

target_link_libraries(test_shmqget qc)
target_link_libraries(test_shmqput qc)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(qctest ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(qctest pthread)
    target_link_libraries(test_shmqget pthread)
    target_link_libraries(test_shmqput pthread)
else()
    target_link_libraries(qctest rt pthread)
    target_link_libraries(test_shmqget rt pthread)
    target_link_libraries(test_shmqput rt pthread)
endif()

target_compile_definitions(qctest PUBLIC "_CRT_SECURE_NO_WARNINGS")
target_compile_definitions(test_shmqget PUBLIC "_CRT_SECURE_NO_WARNINGS")
target_compile_definitions(test_shmqput PUBLIC "_CRT_SECURE_NO_WARNINGS")


install(
    TARGETS
    qctest test_shmqget test_shmqput
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)