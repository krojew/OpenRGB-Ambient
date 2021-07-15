//
// Created by Kamil Rojewski on 15.07.2021.
//

#include "OpenRGBAmbientPlugin.h"

OpenRGBPluginInfo OpenRGBAmbientPlugin::Initialize(bool dark_theme, ResourceManager *resource_manager_ptr) {
    return {
            "OpenRGBAmbientPlugin",
            "Desktop ambient light support",
            "TopTabBar",
            false,
            new QLabel("Ambient")
    };
}

QWidget *OpenRGBAmbientPlugin::CreateGUI(QWidget *parent) {
    return new QWidget{parent};
}
