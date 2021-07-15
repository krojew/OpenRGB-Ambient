//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <chrono>

#include "ScreenCapture.h"
#include "DeviceList.h"
#include "Limiter.h"

#include "OpenRGBAmbientPlugin.h"

using namespace std::chrono_literals;

OpenRGBAmbientPlugin::~OpenRGBAmbientPlugin()
{
    if (captureThread.joinable())
    {
        stopFlag = true;
        captureThread.join();
    }
}

OpenRGBPluginInfo OpenRGBAmbientPlugin::Initialize(bool dark_theme, ResourceManager *resource_manager_ptr) {
    resourceManager = resource_manager_ptr;
    settings = std::make_unique<Settings>(QString::fromStdString(resourceManager->GetConfigurationDirectory() + "/OpenRGBAmbientPlugin.ini"));

    startCapture();

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

void OpenRGBAmbientPlugin::startCapture()
{
    captureThread = std::thread([=] {
        ScreenCapture capture;
        Limiter limiter{60};

        while (!stopFlag.load())
        {
            limiter.sleep();

            if (!capture.grabContent())
            {
                std::this_thread::sleep_for(1ms);
                continue;
            }

            const auto image = capture.getScreen();
        }
    });
}
