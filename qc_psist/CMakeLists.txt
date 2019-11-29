cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_psist LANGUAGES C)

add_library(qc_psist-objs
            OBJECT
            qc_psist_file.h
            qc_psist_file.c 
            qc_psist.h
            qc_psist.c 
            )

include_directories("../qc_base")
include_directories("../qc_mq")

add_library(qc_psist
            STATIC
            $<TARGET_OBJECTS:qc_psist-objs>
           )
