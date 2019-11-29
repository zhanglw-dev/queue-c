cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_base LANGUAGES C)

add_library(qc_qdb-objs
            OBJECT
            qc_qtbl_file.h
            qc_qtbl_file.c 
            qc_qtbl.h
            qc_qtbl.c 
            )

include_directories("../qc_base")
include_directories("../qc_mq")

add_library(qc_qdb
            STATIC
            $<TARGET_OBJECTS:qc_qdb-objs>
           )
