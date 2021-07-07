cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_psist LANGUAGES C)

include_directories("../qc_rt")
include_directories("../qc_mq")


add_library(qc_psist
            OBJECT
            qc_psist_file.h
            qc_psist_file.c 
            qc_psist.h
            qc_psist.c 
            )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(qc_psist PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()