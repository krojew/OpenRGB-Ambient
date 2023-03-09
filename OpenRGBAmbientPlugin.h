//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
#define OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H

#include <atomic>
#include <thread>
#include <vector>
#include <array>

#include <d3d11.h>

#include <QObject>

#include <OpenRGBPluginInterface.h>

#include "ImageProcessor.h"
#include "Settings.h"

class LedUpdateEvent;

class OpenRGBAmbientPlugin
        : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    OpenRGBAmbientPlugin() = default;
    ~OpenRGBAmbientPlugin() override;

    bool event(QEvent *event) override;

    OpenRGBPluginInfo GetPluginInfo() override;

    unsigned int GetPluginAPIVersion() override;

    void Load(bool dark_theme, ResourceManager *resource_manager_ptr) override;

    QWidget *GetWidget() override;

    QMenu *GetTrayMenu() override;

    void Unload() override;

    void turnOffLeds();

public slots:
    void setPreview(bool enabled);
    void setPauseCapture(bool enabled);
    void updateProcessors();

signals:
    void previewUpdated(const QImage &image);

private:
    static const char *END_SESSION_WND_CLASS;

    ResourceManager *resourceManager = nullptr;
    Settings *settings = nullptr;

    std::atomic_bool stopFlag{false};
    std::atomic_bool preview{false};
    std::atomic_bool pauseCapture{false};

    std::vector<std::unique_ptr<ImageProcessorBase>> processors;

    std::thread captureThread;

    void startCapture();
    void stopCapture();

    void processImage(const std::shared_ptr<ID3D11Texture2D> &image);
    void processUpdate(const LedUpdateEvent &event);

    template<ColorPostProcessor CPP>
    std::unique_ptr<ImageProcessorBase> createProcessor(RGBController *controller, std::array<float, 3> colorFactors, CPP colorPostProcessor);
};

#endif //OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
