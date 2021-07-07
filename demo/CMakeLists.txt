cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qcdemo LANGUAGES C)

add_executable(qcdemo 
               demo_mq.c 
               demo_qsys.c 
               demo_sock.c
               main.c 
              )

include_directories("../src/qc_rt")
include_directories("../src/qc_mq")
include_directories("../src/qc_mqsys")

#message(STATUS ${CMAKE_BINARY_DIR}/lib)
link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(qcdemo qc)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(qcdemo ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")    
    target_link_libraries(qcdemo pthread)
else()
    target_link_libraries(qcdemo rt pthread)
endif()

target_compile_definitions(qcdemo PUBLIC "_CRT_SECURE_NO_WARNINGS")

install(
    TARGETS
    qcdemo
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)