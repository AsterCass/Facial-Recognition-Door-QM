cmake_minimum_required(VERSION 3.10)

project(Airstrip)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
    set(BOOST_PREFIX_PATH P:/projects/cplusplus/boost_1_87_0)
elseif (UNIX)
    #todo
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

include_directories(
        ${BOOST_PREFIX_PATH}
)
link_directories(
        ${BOOST_PREFIX_PATH}/build
)

message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

include_directories(${PROJECT_SOURCE_DIR}/include)
set(SOURCES
        src/airstrip_log.cpp
        src/airstrip_configs.cpp
        src/airstrip_thread_pool.cpp
        src/airstrip_program_options.cpp
)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # static lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # dynamic lib
add_library(airstrip STATIC ${SOURCES} Boost::Boost Boost::program_options)




