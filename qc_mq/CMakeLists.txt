cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_mq LANGUAGES C)

include_directories("../qc_base")

add_library(qc_mq
            STATIC
            qc_message.h
            qc_message.c 
            qc_getters.h
            qc_getters.c 
            qc_putters.h
            qc_putters.c 
            qc_msgchain.h
            qc_msgchain.c 
            qc_queue.h
            qc_queue.c 
            )

