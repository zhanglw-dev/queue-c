cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(qc_shq LANGUAGES C)

include_directories("../qc_rt")


add_library(qc_shq
            OBJECT
			qc_shq_def.h
			qc_shq_config.h
            qc_shq_config.c
			qc_shq_mem.h
            qc_shq_mem.c 
			qc_shque.h
            qc_shque.c 
            qc_shq_que.h
            qc_shq_que.c
            cJSON.h
            cJSON.c
            )


######################################################################################


add_executable(qc_shqd 
               qc_shqd.c 
               $<TARGET_OBJECTS:qc_shq>
               $<TARGET_OBJECTS:qc_rt>
              )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(qc_shqd ws2_32)
	target_compile_definitions(qc_shq PUBLIC "_CRT_SECURE_NO_WARNINGS")
	target_compile_definitions(qc_shqd PUBLIC "_CRT_SECURE_NO_WARNINGS")
	target_compile_definitions(qc_shqd PUBLIC "IS_WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")    
    target_link_libraries(qc_shqd pthread)
else()
    target_link_libraries(qc_shqd rt pthread)
endif()


install(
    TARGETS qc_shqd
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)

install(
    FILES shqd_demo.cfg
    #TYPE SYSCONF
    DESTINATION etc
)
