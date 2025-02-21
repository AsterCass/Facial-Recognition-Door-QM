cmake_minimum_required(VERSION 3.10)

project(facial-recognition-door)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
    set(OPENCV_ROOT P:/projects/cplusplus/OpenCV-MinGW-Build-OpenCV-4.5.5-x64)
    set(BOOST_ROOT P:/projects/cplusplus/boost_1_87_0/_install)
    set(SQLITE_CPP_ROOT P:/projects/cplusplus/sqlitecpp/SQLiteCpp-3.3.2/_install)
    set(CMAKE_PREFIX_PATH
            "${BOOST_ROOT}"
            "${SQLITE_CPP_ROOT}"
            "${OPENCV_ROOT}"
    )
    #set(CMAKE_PREFIX_PATH P:/tools/opencv/OpenCV-MinGW-Build-OpenCV-4.5.5-x64)
elseif (UNIX)
    set(OPENCV_ROOT /home/astercasc/Qt5/5.15.2/gcc_64/lib/cmake/Qt5;/usr/local/lib/cmake/opencv4)
    set(BOOST_ROOT /home/astercasc/biz/boost/boost_1_82_0/_install)
    set(SQLITE_CPP_ROOT /home/astercasc/biz/SQLiteCpp/_install)
    set(BREAKPAD_ROOT /data/src/breakpad/_install)
    set(CMAKE_PREFIX_PATH
            "${BOOST_ROOT}"
            "${SQLITE_CPP_ROOT}"
            "${OPENCV_ROOT}"
            "${BREAKPAD_ROOT}"
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

set(SDK_PATH 3rdparty)
set(AIRSTRIP_PREFIX_PATH ../airstrip)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options json filesystem)
find_package(SQLiteCpp REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Bluetooth Widgets)
if (UNIX)
    find_package(Breakpad REQUIRED)
endif ()


if (WIN32)
    include_directories(
            ${SDK_PATH}/include/windows/inspire
            ${AIRSTRIP_PREFIX_PATH}/include
            ${SQLITE_CPP_ROOT}/include
    )
    link_directories(
            ${AIRSTRIP_PREFIX_PATH}/lib
    )
elseif (UNIX)
    include_directories(
            ${SDK_PATH}/include/linux/inspire
            ${AIRSTRIP_PREFIX_PATH}/include
            ${SQLITE_CPP_ROOT}/include
    )
    link_directories(
            ${SDK_PATH}/lib/linux/inspire
            ${AIRSTRIP_PREFIX_PATH}/lib
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()


message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV libraries path: ${OpenCV_LIB_PATH}")
message(STATUS "Breakpad include path: ${BREAKPAD_INCLUDE_DIRS}")
message(STATUS "Breakpad libraries path: ${BREAKPAD_LIBRARY_DIR}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")


add_executable(FacialRecognitionDoor src/main.cpp
        src/config/config.cpp
        src/utils/general_utils.cpp
        src/ui/main_router.cpp
        src/ui/pages/main_page_init.cpp
        src/ui/pages/main_page_home.cpp
        src/utils/scheduled_task.cpp
)

if (WIN32)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor PRIVATE
            ${OpenCV_LIBS}
            airstrip
            SQLiteCpp sqlite3
            Boost::program_options Boost::json Boost::filesystem
            Qt5::Core Qt5::Bluetooth Qt5::Widgets
    )
elseif (UNIX)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor PRIVATE
            ${OpenCV_LIBS}
            airstrip
            SQLiteCpp sqlite3
            Boost::program_options Boost::json Boost::filesystem
            Qt5::Core Qt5::Bluetooth Qt5::Widgets
            InspireFace Breakpad::Client
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

