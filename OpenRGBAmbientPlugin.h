//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
#define OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H

#include <atomic>
#include <thread>
#include <vector>
#include <mutex>

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

    OpenRGBPluginInfo Initialize(bool dark_theme, ResourceManager *resource_manager_ptr) override;

    QWidget *CreateGUI(QWidget *parent) override;

public slots:
    void setPreview(bool enabled);
    void setPauseCapture(bool enabled);
    void updateProcessors();

signals:
    void previewUpdated(const QImage &image);

private:
    ResourceManager *resourceManager = nullptr;
    Settings *settings = nullptr;

    std::atomic_bool stopFlag{false};
    std::atomic_bool preview{false};
    std::atomic_bool pauseCapture{false};

    std::mutex processorMutex;
    std::vector<ImageProcessor> processors;

    std::thread captureThread;

    void startCapture();
    void turnOffLeds();

    void processImage(const std::shared_ptr<ID3D11Texture2D> &image);
    void processUpdate(const LedUpdateEvent &event);
};

#endif //OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
