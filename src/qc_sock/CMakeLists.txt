cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_sock LANGUAGES C)

include_directories("../qc_base")
include_directories("../qc_mq")
include_directories("../qc_qsys")

add_library(qc_sock
            OBJECT
            qc_protocol.h
            qc_protocol.c
            qc_service.h
            qc_service.c
            qc_client.h
            qc_client.c  
            )

target_compile_definitions(qc_sock PUBLIC "_CRT_SECURE_NO_WARNINGS")