cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_shmq LANGUAGES C)

include_directories("../qc_base")


add_library(qc_shmq
            OBJECT
            shmq_config.c
            shm_mem.c 
            shm_queue.c 
            shm_message.h
            shm_message.c
            shmque_api.h
            shmque_api.c
            )


######################################################################################


add_executable(shmq_daemon 
               shmq_daemon.c 
               $<TARGET_OBJECTS:qc_shmq>
               $<TARGET_OBJECTS:qc_base>
              )

target_link_libraries(shmq_daemon rt pthread)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(shmq_daemon ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")    
    target_link_libraries(shmq_daemon pthread)
else()
    target_link_libraries(shmq_daemon rt pthread)
endif()

target_compile_definitions(shmq_daemon PUBLIC "_CRT_SECURE_NO_WARNINGS")

list(APPEND shmq_res
            shmq.cfg
            )

set_target_properties(shmq_daemon
PROPERTIES
RESOURCE "${shmq_res}"
)


install(
    TARGETS
    shmq_daemon
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
    RESOURCE
    DESTINATION  ${INSTALL_ETCDIR}
    COMPONENT etc    
)