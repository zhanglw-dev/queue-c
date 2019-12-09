cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_base LANGUAGES C)

list(APPEND qc_base_source
            qc_prelude.h
            qc_list.h
            qc_list.c
            qc_hash.h
            qc_hash.c
            qc_utils.h  
            qc_utils.c 
            qc_error.h 
            qc_error.c 
            qc_log.h 
            qc_log.c 
            qc_thread.h
            qc_socket.h
            qc_file.h
            )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND qc_base_source
                qc_thread_win.c
                qc_socket_win.c
                qc_file_win.c
                )
else()
    list(APPEND qc_base_source
                qc_thread.c
                qc_socket.c
                qc_file.c
                )
endif()


add_library(qc_base 
            STATIC
            ${qc_base_source}
            )

target_compile_definitions(qc_base PUBLIC "_CRT_SECURE_NO_WARNINGS")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
target_compile_definitions(qc_base PUBLIC "IS_LINUX")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
target_compile_definitions(qc_base PUBLIC "IS_MACOS")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(qc_base PUBLIC "IS_WINDOWS")
endif()
