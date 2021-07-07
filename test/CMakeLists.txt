cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qctest LANGUAGES C)


include_directories("../src/qc_rt")
include_directories("../src/qc_mq")
include_directories("../src/qc_mqsys")
include_directories("../src/qc_psist")
include_directories("../src/qc_shq")

add_executable(qctest 
               test_mq.h 
               test_mq.c 
               test_qsys.h 
               test_qsys.c 
               test_sock.h 
               test_sock.c 
			   test_sem.h
			   test_sem.c 
			   test_shm.h
			   test_shm.c
			   test_log.h
               test_log.c
			   test_psist.h
			   test_psist.c
               main.c 
              )


add_executable(qctest_shqget 
               test_shqget.c
              )

add_executable(qctest_shqput 
               test_shqput.c
              )

#message(STATUS ${CMAKE_BINARY_DIR}/lib)
link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(qctest qc)

target_link_libraries(qctest_shqget qc)
target_link_libraries(qctest_shqput qc)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(qctest ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(qctest pthread)
    target_link_libraries(qctest_shqget pthread)
    target_link_libraries(qctest_shqput pthread)
else()
    target_link_libraries(qctest rt pthread)
    target_link_libraries(qctest_shqget rt pthread)
    target_link_libraries(qctest_shqput rt pthread)
endif()

target_compile_definitions(qctest PUBLIC "_CRT_SECURE_NO_WARNINGS")
target_compile_definitions(qctest_shqget PUBLIC "_CRT_SECURE_NO_WARNINGS")
target_compile_definitions(qctest_shqput PUBLIC "_CRT_SECURE_NO_WARNINGS")


install(
    TARGETS qctest qctest_shqget qctest_shqput
    RUNTIME DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)
