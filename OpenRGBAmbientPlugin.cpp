//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <chrono>

#include <QCoreApplication>
#include <QImage>

#include <windows.h>

#include "ColorConversion.h"
#include "LedUpdateEvent.h"
#include "ZoneMapping.h"
#include "ReleaseWrapper.h"
#include "ScreenCapture.h"
#include "SettingsTab.h"
#include "Limiter.h"

#include "OpenRGBAmbientPlugin.h"

using namespace std::chrono_literals;

const TCHAR *OpenRGBAmbientPlugin::END_SESSION_WND_CLASS = TEXT("OpenRGBAmbientPlugin");

LRESULT CALLBACK EndSessionWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_QUERYENDSESSION)
    {
        const auto plugin = reinterpret_cast<OpenRGBAmbientPlugin *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (plugin != nullptr)
            plugin->turnOffLeds();
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

OpenRGBAmbientPlugin::~OpenRGBAmbientPlugin()
{
    stopCapture();
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

OpenRGBPluginInfo OpenRGBAmbientPlugin::GetPluginInfo()
{
    return {
            "OpenRGBAmbientPlugin",
            "Desktop ambient light support",
            "3.0.3",
            "",
            "https://github.com/krojew/OpenRGB-Ambient",
            {},
            OPENRGB_PLUGIN_LOCATION_TOP,
            "Ambient",
            "",
            {}
    };
}

unsigned int OpenRGBAmbientPlugin::GetPluginAPIVersion()
{
    return OPENRGB_PLUGIN_API_VERSION;
}

void OpenRGBAmbientPlugin::Load(ResourceManagerInterface *resource_manager_ptr)
{
    resourceManager = resource_manager_ptr;

    settings = new Settings{QString::fromStdString((resourceManager->GetConfigurationDirectory() / "OpenRGBAmbientPlugin.ini").string()), this};
    debounceTimer = new QTimer{this};
    debounceTimer->setSingleShot(true);
    debounceTimer->setInterval(150);
    connect(debounceTimer, &QTimer::timeout, this, &OpenRGBAmbientPlugin::updateProcessors);
    connect(settings, &Settings::settingsChanged, debounceTimer, qOverload<>(&QTimer::start));

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &OpenRGBAmbientPlugin::turnOffLeds, Qt::DirectConnection);

    // create a dummy window to capture WM_QUERYENDSESSION and turn off leds (it's not captured by Qt)

    WNDCLASSEX wx = {};
    wx.cbSize = sizeof(WNDCLASSEX);
    wx.lpfnWndProc = EndSessionWindowProc;
    wx.hInstance = GetModuleHandle(nullptr);
    wx.lpszClassName = END_SESSION_WND_CLASS;
    RegisterClassEx(&wx);

    const auto hwnd = CreateWindowEx(0, END_SESSION_WND_CLASS, TEXT(""), 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    resourceManager->RegisterDeviceListChangeCallback([](auto widget) {
        const auto realSettings = static_cast<Settings *>(widget);
        QMetaObject::invokeMethod(realSettings, &Settings::settingsChanged, Qt::QueuedConnection);
    }, settings);

    updateProcessors();
}

QWidget *OpenRGBAmbientPlugin::GetWidget()
{
    const auto ui = new SettingsTab{resourceManager, *settings};
    connect(this, &OpenRGBAmbientPlugin::previewUpdated, ui, &SettingsTab::updatePreview);
    connect(this, &OpenRGBAmbientPlugin::ledColorsUpdated, ui, &SettingsTab::updateLedColors);
    connect(ui, &SettingsTab::previewChanged, this, &OpenRGBAmbientPlugin::setPreview);
    connect(ui, &SettingsTab::settingsVisibilityChanged, this, &OpenRGBAmbientPlugin::setPauseCapture);

    emit ui->controllerListChanged();

    const auto refreshList = [](auto ui) {
        const auto list = static_cast<SettingsTab *>(ui);
        QMetaObject::invokeMethod(list, &SettingsTab::controllerListChanged, Qt::QueuedConnection);
    };

    resourceManager->RegisterDeviceListChangeCallback(refreshList, ui);
    resourceManager->RegisterDetectionEndCallback(refreshList, ui);

    return ui;
}

QMenu *OpenRGBAmbientPlugin::GetTrayMenu()
{
    return nullptr;
}

void OpenRGBAmbientPlugin::Unload()
{
    turnOffLeds();
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
    stopCapture();

    processors.clear();

    const auto blueCompensation = settings->compensateCoolWhite() ? coolWhiteBlueScale : 1.f;
    const auto smoothTransitions = settings->smoothTransitions();
    const auto smoothTransitionsWeight = settings->smoothTransitionsWeight();

    auto colorFactors = colorTemperatureFactors[settings->colorTemperatureFactorIndex()];
    colorFactors[2] *= blueCompensation;

    const auto &controllers = resourceManager->GetRGBControllers();
    for (const auto controller : controllers)
    {
        if (!settings->isControllerSelected(controller->location))
            continue;

        processors.emplace_back(smoothTransitions ? createProcessor(controller, colorFactors, SmoothingColorPostProcessor{smoothTransitionsWeight}) : createProcessor(controller, colorFactors, IdentityColorPostProcessor{}));
    }

    stopFlag = false;
    startCapture();
}

void OpenRGBAmbientPlugin::startCapture()
{
    const auto adapterIdx = static_cast<UINT>(settings->monitorAdapter());
    const auto outputIdx  = settings->monitorOutput();
    captureThread = std::thread([=] {
        ScreenCapture capture{adapterIdx, outputIdx};
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

void OpenRGBAmbientPlugin::stopCapture()
{
    if (captureThread.joinable())
    {
        stopFlag = true;
        captureThread.join();
    }
}

void OpenRGBAmbientPlugin::turnOffLeds()
{
    // since this method can be called from various places and resourceManager might be gone, there's a need to guard
    // from nullptr access
    if (stopFlag.load(std::memory_order::relaxed))
        return;

    stopCapture();

    const auto &controllers = resourceManager->GetRGBControllers();
    for (const auto controller : controllers)
    {
        if (settings->isControllerSelected(controller->location))
        {
            controller->SetAllLEDs(0);
            controller->UpdateLEDs();
        }
    }

    // give some time for async update
    std::this_thread::sleep_for(300ms);
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

    // If width = 1920 and bytesPerPixel = 4, a tightly packed buffer would have RowPitch = 7680 bytes.
    // But a GPU might align rows to 8192 bytes, so RowPitch = 8192. Then stridePixels = 8192 / 4 = 2048.
    // To index pixel (x, y), you must use data[y * stridePixels + x], not data[y * width + x].
    const int stridePixels = static_cast<int>(mapped.RowPitch) / 4;

    // When preview is enabled, process frames in real-time even if settings tab is visible (pauseCapture set).
    if (!pauseCapture || preview)
    {
        if (desc.Format == DXGI_FORMAT_R10G10B10A2_UNORM)
        {
            for (const auto &processor : processors)
            {
                processor->processHdrImage(static_cast<const std::uint32_t *>(mapped.pData), static_cast<int>(desc.Width), static_cast<int>(desc.Height), stridePixels);
            }
        }
        else
        {
            for (const auto &processor : processors)
            {
                processor->processSdrImage(static_cast<const uchar *>(mapped.pData), static_cast<int>(desc.Width), static_cast<int>(desc.Height), stridePixels);
            }
        }
    }

    if (preview)
    {
        const int srcW = static_cast<int>(desc.Width);
        const int srcH = static_cast<int>(desc.Height);
        constexpr int dstW = 320;
        const int dstH = srcW > 0 ? dstW * srcH / srcW : 180;

        if (desc.Format == DXGI_FORMAT_R10G10B10A2_UNORM)
        {
            const auto *hdrData = static_cast<const std::uint32_t *>(mapped.pData);
            QImage previewImg{dstW, dstH, QImage::Format_RGB32};
            for (int y = 0; y < dstH; ++y)
            {
                const int srcY = y * srcH / dstH;
                auto *line = reinterpret_cast<QRgb *>(previewImg.scanLine(y));
                for (int x = 0; x < dstW; ++x)
                {
                    const int srcX = x * srcW / dstW;
                    const std::uint32_t pixel = hdrData[srcY * stridePixels + srcX];
                    line[x] = qRgb((pixel & 0x3ff) >> 2,
                                   ((pixel >> 10) & 0x3ff) >> 2,
                                   ((pixel >> 20) & 0x3ff) >> 2);
                }
            }
            emit previewUpdated(previewImg);
        }
        else
        {
            QImage previewImg{static_cast<const uchar *>(mapped.pData), srcW, srcH, static_cast<int>(mapped.RowPitch), QImage::Format_RGB32};
            emit previewUpdated(previewImg.copy());
        }
    }

    context->Unmap(image.get(), 0);
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

        if (settings->getMappingMode(location) == MappingMode::Standard)
        {
            auto writeRange = [&](const LedRange &range) {
                const int lo = std::min(range.from, range.to);
                const int hi = std::max(range.from, range.to);
                for (int i = lo; i < hi && i < static_cast<int>(colors.size()); ++i)
                    (*controller)->SetLED(i, colors[i]);
            };
            writeRange(settings->getTopRegion(location));
            writeRange(settings->getBottomRegion(location));
            writeRange(settings->getLeftRegion(location));
            writeRange(settings->getRightRegion(location));
        }
        else
        {
            for (const auto &zone : (*controller)->zones)
            {
                if (!settings->isZoneEnabled(location, zone.name))
                    continue;

                for (const auto &part : settings->getZoneParts(location, zone.name))
                {
                    if (part.region == ScreenRegion::None)
                        continue;

                    const int absFrom = static_cast<int>(zone.start_idx) + part.from;
                    const int absTo   = static_cast<int>(zone.start_idx) + part.to;
                    const int lo      = std::min(absFrom, absTo);
                    const int hi      = std::max(absFrom, absTo);
                    for (int i = lo; i < hi && i < static_cast<int>(colors.size()); ++i)
                        (*controller)->SetLED(i, colors[i]);
                }
            }
        }

        (*controller)->UpdateLEDs();

        emit ledColorsUpdated(QString::fromStdString(location), colors);
    }
}

template<ColorPostProcessor CPP>
std::unique_ptr<ImageProcessorBase> OpenRGBAmbientPlugin::createProcessor(RGBController *controller, std::array<float, 3> colorFactors, CPP colorPostProcessor)
{
    std::vector<ZoneLedRange> zoneMappings;
    for (const auto &zone : controller->zones)
    {
        if (!settings->isZoneEnabled(controller->location, zone.name))
            continue;

        const auto parts = settings->getZoneParts(controller->location, zone.name);
        for (const auto &part : parts)
        {
            if (part.region == ScreenRegion::None)
                continue;
            const auto absFrom = static_cast<int>(zone.start_idx) + part.from;
            const auto absTo   = static_cast<int>(zone.start_idx) + part.to;
            zoneMappings.push_back({{absFrom, absTo}, part.region, part.reversed});
        }
    }

    return std::make_unique<ImageProcessor<CPP>>(
            controller->location,
            static_cast<int>(controller->leds.size()),
            settings->getTopRegion(controller->location),
            settings->getBottomRegion(controller->location),
            settings->getRightRegion(controller->location),
            settings->getLeftRegion(controller->location),
            std::move(zoneMappings),
            colorFactors,
            colorPostProcessor,
            settings->getMappingMode(controller->location) == MappingMode::Zone,
            this
    );
}
