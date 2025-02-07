cmake_minimum_required(VERSION 3.10)

project(Airstrip)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
    set(BOOST_ROOT P:/projects/cplusplus/boost_1_87_0/_install)
    set(SQLITE_CPP_ROOT P:/projects/cplusplus/sqlitecpp/SQLiteCpp-3.3.2/_install)
    set(CMAKE_PREFIX_PATH
            "${BOOST_ROOT}"
            "${SQLITE_CPP_ROOT}"
    )
elseif (UNIX)
    #todo
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS program_options json filesystem)
find_package(SQLiteCpp REQUIRED)

message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost library dirs: ${Boost_LIBRARY_DIRS}")
message(STATUS "Cmake prefix path: ${CMAKE_PREFIX_PATH}")

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${SQLITE_CPP_ROOT}/include
)

set(SOURCES
        #        For test
        #                src/main.cpp
        src/airstrip_log.cpp
        src/airstrip_configs.cpp
        src/airstrip_thread_pool.cpp
        src/airstrip_program_options.cpp
        src/airstrip_db.cpp
)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # static lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # dynamic lib

add_library(airstrip STATIC ${SOURCES})
target_include_directories(airstrip PUBLIC
        ${Boost_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
        ${SQLITE_CPP_ROOT}/include
)


#For test
#add_library(airstrip STATIC ${SOURCES})
#target_include_directories(airstrip PUBLIC
#        ${Boost_INCLUDE_DIRS}
#        ${PROJECT_SOURCE_DIR}/include
#        ${SQLITE_CPP_ROOT}/include
#)
#target_link_libraries(airstrip PRIVATE
#        SQLiteCpp sqlite3
#        Boost::program_options Boost::json Boost::filesystem
#)

