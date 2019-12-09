cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_qsys LANGUAGES C)

include_directories("../qc_base")
include_directories("../qc_mq")

add_library(qc_qsys
            STATIC
            qc_qmanager.h
            qc_qmanager.c 
            qc_qsystem.h
            qc_qsystem.c 
            )
