cmake_minimum_required(VERSION 3.19)
project(openrgb_ambient)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)

add_library(${PROJECT_NAME} SHARED OpenRGBAmbientPlugin.h OpenRGBAmbientPlugin.cpp DeviceList.cpp DeviceList.h Settings.cpp Settings.h ScreenCapture.cpp ScreenCapture.h Limiter.cpp Limiter.h ReleaseWrapper.h SettingsTab.cpp SettingsTab.h RegionsWidget.cpp RegionsWidget.h RegionWidget.cpp RegionWidget.h)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
target_include_directories(${PROJECT_NAME} PRIVATE OpenRGB OpenRGB/i2c_smbus OpenRGB/net_port OpenRGB/RGBController OpenRGB/dependencies/json OpenRGB/qt)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _MBCS WIN32 _CRT_SECURE_NO_WARNINGS _WINSOCKAPI_ _WINSOCK_DEPRECATED_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX)

    find_library(D3D11_LIBRARY d3d11.lib PATHS "$ENV{DXSDK_DIR}/Lib/x86" "C:/Program Files (x86)/Microsoft DirectX SDK/Lib/x86")
    target_link_libraries(${PROJECT_NAME} ${D3D11_LIBRARY})

    find_library(DXGI_LIBRARY dxgi.lib PATHS "$ENV{DXSDK_DIR}/Lib/x86" "C:/Program Files (x86)/Microsoft DirectX SDK/Lib/x86")
    target_link_libraries(${PROJECT_NAME} ${DXGI_LIBRARY})

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
