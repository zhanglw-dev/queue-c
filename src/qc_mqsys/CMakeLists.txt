cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_mqsys LANGUAGES C)

include_directories("../qc_rt")
include_directories("../qc_mq")

add_library(qc_mqsys
            OBJECT
            qc_qmanager.h
            qc_qmanager.c 
            qc_qsystem.h
            qc_qsystem.c 
            qc_protocol.h
            qc_protocol.c 
            qc_server.h
            qc_server.c 
            qc_client.h
            qc_client.c 
            )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(qc_mqsys PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()