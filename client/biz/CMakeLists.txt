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
    set(OPENCV_ROOT /data/src/opencv/_install)
    set(BOOST_ROOT /data/src/boost/_install)
    set(SQLITE_CPP_ROOT /data/src/sqlitecpp/_install)
    set(QT_ROOT /data/src/Qt5.12.2/_install/lib/cmake/Qt5)
    set(BREAKPAD_ROOT /data/src/breakpad/_install)
    set(RK_ROOT /data/src/rockchip/_install)
    set(INSPIRE_ROOT /data/src/InspireFace/InspireFace-master/build/inspireface-linux-armv7-armhf/InspireFace)
    set(CMAKE_PREFIX_PATH
            "${INSPIRE_ROOT}"
            "${BOOST_ROOT}"
            "${SQLITE_CPP_ROOT}"
            "${QT_ROOT}"
            "${BREAKPAD_ROOT}"
            "${OPENCV_ROOT}"
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

set(SDK_PATH 3rdparty)
set(AIRSTRIP_PREFIX_PATH ../airstrip)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options json filesystem process)
find_package(SQLiteCpp REQUIRED)

if (UNIX)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)
    find_package(Breakpad REQUIRED)
else ()
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets MultimediaWidgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets MultimediaWidgets)
endif ()


if (WIN32)
    include_directories(
            ${SDK_PATH}/include/common/inspire
            ${SDK_PATH}/include/common/rockchip
            ${AIRSTRIP_PREFIX_PATH}/include
            ${SQLITE_CPP_ROOT}/include
            ${SDK_PATH}/include/windows/face
    )
    link_directories(
            ${AIRSTRIP_PREFIX_PATH}/lib
            ${SDK_PATH}/lib/windows/face
    )
elseif (UNIX)
    include_directories(
            ${SDK_PATH}/include/common/inspire
            ${SDK_PATH}/include/common/rockchip
            ${SDK_PATH}/include/linux/rockchip
            ${AIRSTRIP_PREFIX_PATH}/include
            ${SQLITE_CPP_ROOT}/include
            ${SDK_PATH}/include/linux/face
    )
    link_directories(
            ${INSPIRE_ROOT}/lib
            ${AIRSTRIP_PREFIX_PATH}/lib
            ${RK_ROOT}/lib
            ${OPENCV_ROOT}/lib/opencv4/3rdparty
            ${SDK_PATH}/lib/linux/face
            ${SDK_PATH}/lib/linux/nfc
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()


message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "Breakpad include path: ${BREAKPAD_INCLUDE_DIRS}")
message(STATUS "Breakpad libraries path: ${BREAKPAD_LIBRARY_DIR}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

set(PROJECT_HEADER_FOR_QT_MOC
        include/utils/global_data_manager.h
)

set(PROJECT_SOURCES
        src/main.cpp
        src/nfc/nfc_tool.cpp
        src/api/api.cpp
        src/config/config.cpp
        src/db/card_db.cpp
        src/db/face_db.cpp
        src/db/open_record_db.cpp
        src/camera/common/sample_common_isp.c
        src/camera/common/sample_fake_isp.c
        src/camera/common/display.c
        src/camera/common/draw_rect.c
        src/camera/common/rkdrm_display.c
        src/camera/camera_rk.cpp
        src/camera/camera_frame.cpp
        src/ui/main_router.cpp
        src/ui/components/main_component_header.cpp
        src/ui/components/virtual_keyboard.cpp
        src/ui/components/virtual_keyboard_number.cpp
        src/ui/components/notification.cpp
        src/ui/components/common_components.cpp
        src/ui/components/face_register.cpp
        src/ui/components/setting_header.cpp
        src/ui/pages/main_page_init.cpp
        src/ui/pages/main_page_home.cpp
        src/ui/pages/main_page_setting_login.cpp
        src/ui/pages/main_page_setting_main.cpp
        src/ui/pages/main_page_setting_server.cpp
        src/ui/pages/main_page_setting_face.cpp
        src/ui/pages/main_page_setting_door.cpp
        src/ui/pages/main_page_setting_maintain.cpp
        src/ui/pages/main_page_setting_info.cpp
        src/ui/pages/main_page_setting_network.cpp
        src/ui/pages/main_page_setting_tmp.cpp
        src/utils/face_recognition.cpp
        src/utils/card_recognition.cpp
        src/utils/general_utils.cpp
        src/utils/scheduled_task.cpp
        src/utils/global_data_manager.cpp
)

add_executable(FacialRecognitionDoor
        ${PROJECT_HEADER_FOR_QT_MOC}
        ${PROJECT_SOURCES}
)

if (WIN32)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor PRIVATE
            ${OpenCV_LIBS}
            airstrip
            SQLiteCpp sqlite3
            Boost::program_options Boost::json Boost::filesystem Boost::process
            Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::MultimediaWidgets
            # Face
            -lfacedetection -fopenmp
    )
elseif (UNIX)
    target_include_directories(FacialRecognitionDoor PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(FacialRecognitionDoor PRIVATE
            airstrip
            # Open CV
            ${OpenCV_LIBS}
            -llibprotobuf -lade -littnotify -llibjpeg-turbo -llibwebp -llibpng
            -llibtiff -llibopenjp2 -lIlmImf -lzlib -ltegra_hal -ldl -lm -lpthread -lrt
            # SQL
            SQLiteCpp sqlite3
            Boost::program_options Boost::json Boost::filesystem
            Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
            # InspireFace
            InspireFace
            # Crash
            Breakpad::Client
            # Rock chip
            -lrkaiq -leasymedia -ldrm -lrockchip_mpp -lavformat -lavcodec -lswresample -lavutil -lasound
            -lRKAP_3A -lRKAP_ANR -lRKAP_Common -lv4l2 -lrga -lmd_share -lod_share -lrkaiq -lz -lv4lconvert -ljpeg
            # Face
            -lfacedetection -fopenmp
            # Nfc
            -lserial
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

