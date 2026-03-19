//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_SCREENCAPTURE_H
#define OPENRGB_AMBIENT_SCREENCAPTURE_H

#include <vector>
#include <memory>
#include <string>

#include <windows.h>
#include <dxgi1_5.h>
#include <d3d11.h>
#include <dxgi.h>

class ScreenCapture final
{
public:
    struct MonitorInfo
    {
        std::string displayName; // e.g. "\\.\DISPLAY1 (1920x1080)"
        UINT        adapterIndex;
        int         outputIndex;
    };

    static std::vector<MonitorInfo> enumerateMonitors();

    explicit ScreenCapture(UINT adapterIndex = 0, int outputIndex = 0);

    bool grabContent();

    [[nodiscard]] std::shared_ptr<ID3D11Texture2D> getScreen() const;

private:
    UINT adapterIndex;
    int  outputIndex;

    std::shared_ptr<IDXGIFactory1> factory;
    std::shared_ptr<IDXGIAdapter1> adapter;
    std::shared_ptr<IDXGIOutput> adapterOutput;
    std::shared_ptr<ID3D11Device> device;
    std::shared_ptr<ID3D11DeviceContext> deviceContext;
    std::shared_ptr<IDXGIOutputDuplication> duplicator;
    std::shared_ptr<IDXGIOutput5> duplicatorOutput;
    std::shared_ptr<ID3D11Texture2D> image;

    void initAdapter();
    void initOutput();
    void initDevice();
    bool initDuplicator();

    std::vector<std::shared_ptr<IDXGIOutput>> enumerateVideoOutputs();
};

#endif //OPENRGB_AMBIENT_SCREENCAPTURE_H
