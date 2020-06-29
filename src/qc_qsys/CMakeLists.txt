cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_qsys LANGUAGES C)

include_directories("../qc_base")
include_directories("../qc_mq")

add_library(qc_qsys
            OBJECT
            qc_qmanager.h
            qc_qmanager.c 
            qc_qsystem.h
            qc_qsystem.c 
            )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(qc_qsys PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()