cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(queue-c LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")


include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for header")

# Report to user
foreach(p LIB BIN INCLUDE)
    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path )
    message(STATUS "Installing ${p} components to ${_path}")
    unset(_path)
endforeach()


add_subdirectory(src/qc_rt)
add_subdirectory(src/qc_mq)
add_subdirectory(src/qc_mqsys)
add_subdirectory(src/qc_shq)
add_subdirectory(src/qc_psist)
add_subdirectory(demo)
add_subdirectory(test)

add_library(qc
            STATIC
            $<TARGET_OBJECTS:qc_rt>
            $<TARGET_OBJECTS:qc_mq>
            $<TARGET_OBJECTS:qc_mqsys>
            $<TARGET_OBJECTS:qc_shq>
			$<TARGET_OBJECTS:qc_psist>
)


# Prepare RPATH
file(RELATIVE_PATH _rel ${CMAKE_INSTALL_PREFIX}/${INSTALL_BINDIR} ${CMAKE_INSTALL_PREFIX})
if(APPLE)
    set(_rpath "@loader_path/${_rel}")
else()
    set(_rpath "\$ORIGIN/${_rel}")
endif()
file(TO_NATIVE_PATH "${_rpath}/${INSTALL_LIBDIR}" qc_RPATH)

set_target_properties(qc
    PROPERTIES
    MACOSX_RPATH ON
    SKIP_BUILD_RPATH OFF
    BUILD_WITH_INSTALL_RPATH OFF
    INSTALL_RPATH "${qc_RPATH}"
    INSTALL_RPATH_USE_LINK_PATH ON
)


list(APPEND QC_API_HEADERS
            src/qc_rt/qc_prelude.h
            src/qc_rt/qc_error.h
            src/qc_rt/qc_file.h
            src/qc_rt/qc_hash.h
            src/qc_rt/qc_list.h
            src/qc_rt/qc_log.h
            src/qc_rt/qc_sem.h
            src/qc_rt/qc_shm.h
            src/qc_rt/qc_socket.h
            src/qc_rt/qc_thread.h
            src/qc_mq/qc_message.h
            src/qc_mq/qc_queue.h
            src/qc_mqsys/qc_qsystem.h
            src/qc_mqsys/qc_server.h
            src/qc_mqsys/qc_client.h
            src/qc_shq/qc_shq_mem.h
            src/qc_shq/qc_shq_que.h
            )

set_target_properties(qc
PROPERTIES
PUBLIC_HEADER "${QC_API_HEADERS}"
)


install(
    TARGETS
    qc
    ARCHIVE
    DESTINATION ${INSTALL_LIBDIR}
    COMPONENT lib
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
    LIBRARY
    DESTINATION ${INSTALL_LIBDIR}
    COMPONENT lib
    PUBLIC_HEADER
    DESTINATION ${INSTALL_INCLUDEDIR}/qc
    COMPONENT dev
)


enable_testing()

add_test(
    NAME qctest
    COMMAND $<TARGET_FILE:qctest>
)
