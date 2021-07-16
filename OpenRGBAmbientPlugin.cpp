//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <algorithm>
#include <chrono>

#include <QCoreApplication>
#include <QImage>

#include "LedUpdateEvent.h"
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

bool OpenRGBAmbientPlugin::event(QEvent *event)
{
    if (static_cast<int>(event->type()) == LedUpdateEvent::eventType)
    {
        processUpdate(*static_cast<LedUpdateEvent *>(event)); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        return true;
    }
    
    return QObject::event(event);
}

OpenRGBPluginInfo OpenRGBAmbientPlugin::Initialize(bool dark_theme, ResourceManager *resource_manager_ptr) {
    resourceManager = resource_manager_ptr;

    settings = new Settings{QString::fromStdString(resourceManager->GetConfigurationDirectory() + "/OpenRGBAmbientPlugin.ini"), this};
    connect(settings, &Settings::settingsChanged, this, &OpenRGBAmbientPlugin::updateProcessors);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &OpenRGBAmbientPlugin::turnOffLeds);

    startCapture();

    return {
            "OpenRGBAmbientPlugin",
            "Desktop ambient light support",
            "TopTabBar",
            false,
            new QLabel{"Ambient"}
    };
}

QWidget *OpenRGBAmbientPlugin::CreateGUI(QWidget *parent) {
    const auto ui = new SettingsTab{resourceManager, *settings, parent};
    connect(this, &OpenRGBAmbientPlugin::previewUpdated, ui, &SettingsTab::updatePreview);
    connect(ui, &SettingsTab::previewChanged, this, &OpenRGBAmbientPlugin::setPreview);
    connect(ui, &SettingsTab::settingsVisibilityChanged, this, &OpenRGBAmbientPlugin::setPauseCapture);

    resourceManager->RegisterDeviceListChangeCallback([](auto ui) {
        const auto list = static_cast<SettingsTab *>(ui);
        QMetaObject::invokeMethod(list, &SettingsTab::controllerListChanged, Qt::QueuedConnection);
    }, ui);
    resourceManager->RegisterDeviceListChangeCallback([](auto widget) {
        const auto realSettings = static_cast<Settings *>(widget);
        QMetaObject::invokeMethod(realSettings, &Settings::settingsChanged, Qt::QueuedConnection);
    }, settings);

    return ui;
}

void OpenRGBAmbientPlugin::setPreview(bool enabled)
{
    preview = enabled;
}

void OpenRGBAmbientPlugin::setPauseCapture(bool enabled)
{
    pauseCapture = enabled;
}

void OpenRGBAmbientPlugin::updateProcessors()
{
    std::lock_guard lock{processorMutex};

    processors.clear();

    const auto &controllers = resourceManager->GetRGBControllers();
    for (const auto controller : controllers)
    {
        if (!settings->isControllerSelected(controller->location))
            continue;

        processors.emplace_back(
                controller->location,
                settings->getTopRegion(controller->location),
                settings->getBottomRegion(controller->location),
                settings->getRightRegion(controller->location),
                settings->getLeftRegion(controller->location),
                this
        );
    }
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
            processImage(image);
        }
    });
}

void OpenRGBAmbientPlugin::turnOffLeds()
{
    pauseCapture = true;

    const auto &controllers = resourceManager->GetRGBControllers();
    for (const auto controller : controllers)
    {
        if (settings->isControllerSelected(controller->location))
        {
            controller->SetAllLEDs(0);
            controller->UpdateLEDs();
        }
    }
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

    if (!pauseCapture)
    {
        std::lock_guard lock{processorMutex};
        for (auto &processor : processors)
            processor.processImage(static_cast<const uchar *>(mapped.pData), static_cast<int>(desc.Width), static_cast<int>(desc.Height));
    }

    if (preview)
    {
        QImage previewImg{static_cast<const uchar *>(mapped.pData), static_cast<int>(desc.Width), static_cast<int>(desc.Height), QImage::Format_RGB32};
        emit previewUpdated(previewImg.copy());
    }
}

void OpenRGBAmbientPlugin::processUpdate(const LedUpdateEvent &event)
{
    const auto &location = event.getControllerLocation();

    const auto &controllers = resourceManager->GetRGBControllers();
    const auto controller = std::find_if(std::begin(controllers), std::end(controllers), [&](auto controller) {
        return controller->location == location;
    });

    if (controller != std::end(controllers))
    {
        const auto &colors = event.getColors();

        const auto numColors = colors.size();
        for (auto i = 0; i < numColors; ++i)
            (*controller)->SetLED(i, colors[i]);

        (*controller)->UpdateLEDs();
    }
}
