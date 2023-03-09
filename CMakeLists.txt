cmake_minimum_required(VERSION 3.19)
project(openrgb_ambient)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

if (WIN32)
    add_definitions(-D_MBCS -DWIN32 -D_CRT_SECURE_NO_WARNINGS -D_WINSOCKAPI_ -D_WINSOCK_DEPRECATED_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

add_library(${PROJECT_NAME} SHARED OpenRGBAmbientPlugin.h OpenRGBAmbientPlugin.cpp DeviceList.cpp DeviceList.h Settings.cpp Settings.h ScreenCapture.cpp ScreenCapture.h Limiter.cpp Limiter.h ReleaseWrapper.h SettingsTab.cpp SettingsTab.h RegionsWidget.cpp RegionsWidget.h RegionWidget.cpp RegionWidget.h LedUpdateEvent.h LedUpdateEvent.cpp ImageProcessor.h LedRange.h SdrHorizontalRegionProcessor.h SdrVerticalRegionProcessor.h ColorConversion.h HdrVerticalRegionProcessor.h HdrHorizontalRegionProcessor.h ColorPostProcessor.h)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
find_package(DirectX11)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
target_include_directories(${PROJECT_NAME} PRIVATE OpenRGB OpenRGB/i2c_smbus OpenRGB/net_port OpenRGB/RGBController OpenRGB/dependencies/json OpenRGB/qt)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${DirectX11_LIBRARY})

    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB ${REQUIRED_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
