cmake_minimum_required(VERSION 3.10)

project(facial-recognition-door)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
    set(CMAKE_PREFIX_PATH P:/projects/cplusplus/OpenCV-MinGW-Build-OpenCV-4.5.5-x64)
    #set(CMAKE_PREFIX_PATH P:/tools/opencv/OpenCV-MinGW-Build-OpenCV-4.5.5-x64)
elseif (UNIX)
    set(CMAKE_PREFIX_PATH /home/astercasc/Qt5/5.15.2/gcc_64/lib/cmake/Qt5;/usr/local/lib/cmake/opencv4)
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

set(SDK_PATH 3rdparty)
find_package(OpenCV REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Bluetooth Widgets)


if (WIN32)
    include_directories(
            ${SDK_PATH}/include/windows/inspire
    )
    link_directories(
    )
elseif (UNIX)
    include_directories(
            ${SDK_PATH}/include/linux/inspire
    )
    link_directories(
            ${SDK_PATH}/lib/linux/inspire
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()


message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV libraries path: ${OpenCV_LIB_PATH}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")


add_executable(FacialRecognitionDoor src/main.cpp
        src/config/config.cpp
        src/utils/general_utils.cpp
)

if (WIN32)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor ${OpenCV_LIBS} Qt5::Core Qt5::Bluetooth Qt5::Widgets)
elseif (UNIX)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor ${OpenCV_LIBS} Qt5::Core Qt5::Bluetooth Qt5::Widgets InspireFace)
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

