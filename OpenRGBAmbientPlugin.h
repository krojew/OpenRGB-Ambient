#ifndef OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
#define OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H

#include <atomic>
#include <thread>
#include <vector>
#include <array>

#include <d3d11.h>

#include <QObject>
#include <QString>
#include <QTimer>

#include <OpenRGBPluginInterface.h>

#include "ImageProcessor.h"
#include "Settings.h"

class LedUpdateEvent;

class OpenRGBAmbientPlugin
        : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID FILE "OpenRGBAmbientPlugin.json")
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    OpenRGBAmbientPlugin() = default;
    ~OpenRGBAmbientPlugin() override;

    bool event(QEvent *event) override;

    OpenRGBPluginInfo GetPluginInfo() override;

    unsigned int GetPluginAPIVersion() override;

    void Load(OpenRGBPluginAPIInterface* plugin_api_ptr) override;

    QWidget *GetWidget() override;

    QMenu *GetTrayMenu() override;

    void Unload() override;

    void ResourceManagerUpdated(unsigned update_reason) override;

    void OnProfileAboutToLoad() override;

    void OnProfileLoad(nlohmann::json profile_data) override;

    nlohmann::json OnProfileSave() override;

    unsigned char* OnSDKCommand(unsigned pkt_id, unsigned char* pkt_data, unsigned* pkt_size) override;

    void ProfileManagerUpdated(unsigned update_reason) override;

    void SettingsManagerUpdated(unsigned update_reason) override;

    void turnOffLeds();

public slots:
    void setPreview(bool enabled);
    void setPauseCapture(bool enabled);
    void updateProcessors();

signals:
    void previewUpdated(const QImage &image);
    void ledColorsUpdated(const QString &location, const std::vector<RGBColor> &colors);

private:
    static const TCHAR *END_SESSION_WND_CLASS;

    OpenRGBPluginAPIInterface *pluginApiPtr = nullptr;
    Settings *settings = nullptr;

    std::atomic_bool stopFlag{false};
    std::atomic_bool preview{false};
    std::atomic_bool pauseCapture{false};

    QTimer *debounceTimer = nullptr;

    std::vector<std::unique_ptr<ImageProcessorBase>> processors;

    std::thread captureThread;

    void startCapture();
    void stopCapture();

    void processImage(const std::shared_ptr<ID3D11Texture2D> &image);
    void processUpdate(const LedUpdateEvent &event);

    template<ColorPostProcessor CPP>
    std::unique_ptr<ImageProcessorBase> createProcessor(RGBControllerInterface* controller,
                                                        std::array<float, 3> colorFactors, CPP colorPostProcessor);
};

#endif //OPENRGB_AMBIENT_OPENRGBAMBIENTPLUGIN_H
