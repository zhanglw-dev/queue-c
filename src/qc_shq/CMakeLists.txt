cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_shq LANGUAGES C)

include_directories("../qc_rt")


add_library(qc_shq
            OBJECT
			qc_shq_def.h
			qc_shq_mem.h
            qc_shq_mem.c 
			qc_shque.h
            qc_shque.c 
            qc_shq_que.h
            qc_shq_que.c
            qc_shq_conf.h
            qc_shq_conf.c
            )


######################################################################################


add_executable(qc_shqd_demo
               qc_shqd_demo.c 
               $<TARGET_OBJECTS:qc_shq>
               $<TARGET_OBJECTS:qc_rt>
              )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(qc_shqd_demo ws2_32)
	target_compile_definitions(qc_shq PUBLIC "_CRT_SECURE_NO_WARNINGS")
	target_compile_definitions(qc_shqd_demo PUBLIC "_CRT_SECURE_NO_WARNINGS")
	target_compile_definitions(qc_shqd_demo PUBLIC "IS_WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")    
    target_link_libraries(qc_shqd_demo pthread)
else()
    target_link_libraries(qc_shqd_demo rt pthread)
endif()


install(
    TARGETS qc_shqd_demo
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)

#install(
#   FILES shqd_demo.cfg
    #TYPE SYSCONF
#   DESTINATION etc
#)
