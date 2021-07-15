//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <chrono>

#include <QImage>

#include "ReleaseWrapper.h"
#include "ScreenCapture.h"
#include "SettingsTab.h"
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
    const auto ui = new SettingsTab{resourceManager, *settings, parent};
    connect(this, &OpenRGBAmbientPlugin::previewUpdated, ui, &SettingsTab::updatePreview);
    connect(ui, &SettingsTab::previewChanged, this, &OpenRGBAmbientPlugin::setPreview);

    resourceManager->RegisterDeviceListChangeCallback([](const auto ui) {
        const auto list = static_cast<SettingsTab *>(ui);
        QMetaObject::invokeMethod(list, &SettingsTab::controllerListChanged, Qt::QueuedConnection);
    }, ui);

    return ui;
}

void OpenRGBAmbientPlugin::setPreview(bool enabled)
{
    preview = enabled;
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

            processImage(capture.getScreen());
        }
    });
}

void OpenRGBAmbientPlugin::processImage(const std::shared_ptr<ID3D11Texture2D> &image)
{
    D3D11_TEXTURE2D_DESC desc;
    image->GetDesc(&desc);

    ID3D11Device *pDevice = nullptr;
    image->GetDevice(&pDevice);

    const auto device = releasing(pDevice);

    ID3D11DeviceContext *pContext = nullptr;
    device->GetImmediateContext(&pContext);

    const auto context = releasing(pContext);

    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(image.get(), 0, D3D11_MAP_READ, 0, &mapped);

    if (preview)
    {
        QImage previewImg{static_cast<const uchar *>(mapped.pData), static_cast<int>(desc.Width), static_cast<int>(desc.Height), QImage::Format_RGB32};
        emit previewUpdated(previewImg.copy());
    }
}
