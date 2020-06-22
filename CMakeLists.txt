cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(queue-c LANGUAGES C)
#SET(CMAKE_BUILD_TYPE "Debug")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")


include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for header")
if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKEDIR CMake)
else()
    set(DEF_INSTALL_CMAKEDIR share/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKEDIR ${DEF_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")

# Report to user
foreach(p LIB BIN INCLUDE CMAKE)
    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path )
    message(STATUS "Installing ${p} components to ${_path}")
    unset(_path)
endforeach()


add_subdirectory(src/qc_base)
add_subdirectory(src/qc_mq)
add_subdirectory(src/qc_psist)
add_subdirectory(src/qc_qsys)
add_subdirectory(src/qc_sock)
add_subdirectory(demo)
add_subdirectory(test)

add_library(qc
            STATIC
            $<TARGET_OBJECTS:qc_base>
            $<TARGET_OBJECTS:qc_mq>
            $<TARGET_OBJECTS:qc_psist>
            $<TARGET_OBJECTS:qc_qsys>
            $<TARGET_OBJECTS:qc_sock>
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


list(APPEND qc_API_HEADERS
            src/qc_base/qc_error.h
            src/qc_mq/qc_message.h
            src/qc_mq/qc_queue.h
            src/qc_qsys/qc_qsystem.h
            src/qc_sock/qc_service.h
            src/qc_sock/qc_client.h
            )

set_target_properties(qc
PROPERTIES
PUBLIC_HEADER "${qc_API_HEADERS}"
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
