cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_net LANGUAGES C)

include_directories("../qc_base")
include_directories("../qc_mq")
include_directories("../qc_qsys")

add_library(qc_net
            STATIC
            qc_protocol.h
            qc_protocol.c
            qc_service.h
            qc_service.c
            qc_client.h
            qc_client.c  
            )
