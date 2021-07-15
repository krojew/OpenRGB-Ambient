//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
#define OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H

#include <atomic>
#include <memory>
#include <thread>

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

private:
    ResourceManager *resourceManager = nullptr;
    std::unique_ptr<Settings> settings;

    std::atomic_bool stopFlag{false};
    std::thread captureThread;

    void startCapture();
};

#endif //OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
