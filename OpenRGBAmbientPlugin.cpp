//
// Created by Kamil Rojewski on 15.07.2021.
//

#include "DeviceList.h"

#include "OpenRGBAmbientPlugin.h"

OpenRGBPluginInfo OpenRGBAmbientPlugin::Initialize(bool dark_theme, ResourceManager *resource_manager_ptr) {
    resourceManager = resource_manager_ptr;
    settings = std::make_unique<Settings>(QString::fromStdString(resourceManager->GetConfigurationDirectory() + "/OpenRGBAmbientPlugin.ini"));

    return {
            "OpenRGBAmbientPlugin",
            "Desktop ambient light support",
            "TopTabBar",
            false,
            new QLabel("Ambient")
    };
}

QWidget *OpenRGBAmbientPlugin::CreateGUI(QWidget *parent) {
    const auto ui = new DeviceList{resourceManager, *settings, parent};
    resourceManager->RegisterDeviceListChangeCallback([](const auto ui) {
        const auto list = static_cast<DeviceList *>(ui);
        QMetaObject::invokeMethod(list, &DeviceList::fillControllerList, Qt::QueuedConnection);
    }, ui);

    return ui;
}
