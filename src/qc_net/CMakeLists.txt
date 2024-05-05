cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_net LANGUAGES C)

include_directories("../qc_rt")
include_directories("../qc_mq")
include_directories("../qc_qsys")

add_library(qc_net
            OBJECT
            qc_protocol.h
            qc_protocol.c 
            qc_server.h
            qc_server.c 
            qc_client.h
            qc_client.c 
            )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(qc_qsys PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()
