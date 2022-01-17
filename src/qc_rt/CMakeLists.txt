cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
#project(qc_rt LANGUAGES C)

list(APPEND qc_rt_source
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
			qc_sem.h
			qc_shm.h
            )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND qc_rt_source
                qc_thread_win.c
                qc_socket_win.c
                qc_file_win.c
				qc_sem_win.c
				qc_shm_win.c
                )
else()
    list(APPEND qc_rt_source
                qc_thread.c
                qc_socket.c
                qc_file.c
				qc_sem.c
				qc_shm.c
                )
endif()


add_library(qc_rt
            OBJECT
            ${qc_rt_source}
            )


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
target_compile_definitions(qc_rt PUBLIC "IS_LINUX")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
target_compile_definitions(qc_rt PUBLIC "IS_DARWIN")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(qc_rt PUBLIC "IS_WINDOWS")
target_compile_definitions(qc_rt PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()
