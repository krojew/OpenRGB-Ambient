//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
#define OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H

#include <atomic>
#include <memory>
#include <thread>

#include <d3d11.h>

#include <QObject>

#include <OpenRGBPluginInterface.h>

#include "Settings.h"

class OpenRGBAmbientPlugin
        : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    OpenRGBAmbientPlugin() = default;
    ~OpenRGBAmbientPlugin() override;

    OpenRGBPluginInfo Initialize(bool dark_theme, ResourceManager *resource_manager_ptr) override;

    QWidget *CreateGUI(QWidget *parent) override;

public slots:
    void setPreview(bool enabled);

signals:
    void previewUpdated(const QImage &image);

private:
    ResourceManager *resourceManager = nullptr;
    std::unique_ptr<Settings> settings;

    std::atomic_bool stopFlag{false};
    std::atomic_bool preview{false};
    std::thread captureThread;

    void startCapture();

    void processImage(const std::shared_ptr<ID3D11Texture2D> &image);
};

#endif //OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
