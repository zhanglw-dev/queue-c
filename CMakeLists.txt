cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(queue-c LANGUAGES C)


#message(STATUS ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

file(COPY "src/qc_base/qc_error.h" DESTINATION ${CMAKE_BINARY_DIR}/include)
file(COPY "src/qc_mq/qc_message.h" DESTINATION ${CMAKE_BINARY_DIR}/include)
file(COPY "src/qc_mq/qc_queue.h" DESTINATION ${CMAKE_BINARY_DIR}/include)
file(COPY "src/qc_qsys/qc_qsystem.h" DESTINATION ${CMAKE_BINARY_DIR}/include)
file(COPY "src/qc_sock/qc_service.h" DESTINATION ${CMAKE_BINARY_DIR}/include)
file(COPY "src/qc_sock/qc_client.h" DESTINATION ${CMAKE_BINARY_DIR}/include)
