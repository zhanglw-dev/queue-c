cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_base LANGUAGES C)

add_library(qc_net-objs
            OBJECT
            qc_netconf.h
            qc_netconf.c 
            qc_protocol.h
            qc_protocol.c
            qc_service.h
            qc_service.c
            qc_client.h
            qc_client.c  
            )

include_directories("../qc_base")
include_directories("../qc_mq")
include_directories("../qc_qsys")

add_library(qc_net
            STATIC
            $<TARGET_OBJECTS:qc_net-objs>
           )
