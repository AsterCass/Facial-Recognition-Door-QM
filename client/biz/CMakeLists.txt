cmake_minimum_required(VERSION 3.10)

project(facial-recognition-door)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
add_definitions(-DRKAIQ)

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
    set(BOOST_ROOT /data/src/boost/_install)
    set(SQLITE_CPP_ROOT /data/src/sqlitecpp/_install)
    set(QT_ROOT /data/src/Qt5.12.2/_install/lib/cmake/Qt5)
    set(BREAKPAD_ROOT /data/src/breakpad/_install)
    set(RK_ROOT /data/src/rockchip/_install)
    set(CMAKE_PREFIX_PATH
            "${BOOST_ROOT}"
            "${SQLITE_CPP_ROOT}"
            "${QT_ROOT}"
            "${BREAKPAD_ROOT}"
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

set(SDK_PATH 3rdparty)
set(AIRSTRIP_PREFIX_PATH ../airstrip)
#find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options json filesystem)
find_package(SQLiteCpp REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)
if (UNIX)
    find_package(Breakpad REQUIRED)
endif ()


if (WIN32)
    include_directories(
            ${SDK_PATH}/include/windows/inspire
            ${SDK_PATH}/include/common/rockchip
            ${AIRSTRIP_PREFIX_PATH}/include
            ${SQLITE_CPP_ROOT}/include
    )
    link_directories(
            ${AIRSTRIP_PREFIX_PATH}/lib
    )
elseif (UNIX)
    include_directories(
            #${SDK_PATH}/include/linux/inspire
            ${SDK_PATH}/include/common/rockchip
            ${AIRSTRIP_PREFIX_PATH}/include
            ${SQLITE_CPP_ROOT}/include
    )
    link_directories(
            #${SDK_PATH}/lib/linux/inspire
            ${AIRSTRIP_PREFIX_PATH}/lib
            ${RK_ROOT}/lib
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

set(PROJECT_SOURCES
        src/main.cpp
        src/config/config.cpp
        src/camera/common/sample_common_isp.c
        src/camera/common/sample_fake_isp.c
        src/ui/main_router.cpp
        src/ui/components/main_component_header.cpp
        src/ui/components/virtual_keyboard.cpp
        src/ui/components/notification.cpp
        src/ui/components/common_components.cpp
        src/ui/pages/main_page_init.cpp
        src/ui/pages/main_page_home.cpp
        src/utils/general_utils.cpp
        src/utils/scheduled_task.cpp
)

add_executable(FacialRecognitionDoor src/main.cpp
        ${PROJECT_SOURCES}
)

if (WIN32)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor PRIVATE
            ${OpenCV_LIBS}
            airstrip
            SQLiteCpp sqlite3
            Boost::program_options Boost::json Boost::filesystem
            Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
    )
elseif (UNIX)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor PRIVATE
            #${OpenCV_LIBS}
            airstrip
            SQLiteCpp sqlite3
            Boost::program_options Boost::json Boost::filesystem
            Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
            # InspireFace
            Breakpad::Client
            # Rock chip
            -lrkaiq -leasymedia -ldrm -lrockchip_mpp -lavformat -lavcodec -lswresample -lavutil -lasound
            -lRKAP_3A -lRKAP_ANR -lRKAP_Common -lv4l2 -lrga -lmd_share -lod_share -lrkaiq -lz -lv4lconvert -ljpeg
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

