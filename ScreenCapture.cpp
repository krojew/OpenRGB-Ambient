//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <stdexcept>

#include "ReleaseWrapper.h"

#include "ScreenCapture.h"

ScreenCapture::ScreenCapture()
{
    initAdapter();
    initOutput();
    initDevice();
    initDuplicator();
}

void ScreenCapture::initAdapter(UINT index)
{
    IDXGIFactory1 *pFactory = nullptr;

    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&pFactory))))
        throw std::runtime_error{"Failed to create DXGIFactory"};

    IDXGIAdapter1 *pAdapter = nullptr;
    UINT i = 0;

    while (pFactory->EnumAdapters1(i, &pAdapter) == S_OK)
    {
        DXGI_ADAPTER_DESC desc;
        if (FAILED(pAdapter->GetDesc(&desc)))
            throw std::runtime_error{"Failed to retrieve description of adapter."};

        if (i == index)
        {
            adapter = releasing(pAdapter);
            factory = releasing(pFactory);
        }
        else
        {
            releasing(pAdapter);
        }

        ++i;
    }
}

void ScreenCapture::initOutput(size_t index)
{
    auto outputs = enumerateVideoOutputs();
    if (index < outputs.size())
        adapterOutput = std::move(outputs[index]);
}

std::vector<std::shared_ptr<IDXGIOutput>> ScreenCapture::enumerateVideoOutputs()
{
    auto i = 0;
    IDXGIOutput *output = nullptr;
    std::vector<std::shared_ptr<IDXGIOutput>> outputs;

    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        outputs.emplace_back(releasing(output));
        ++i;
    }

    return outputs;
}

void ScreenCapture::initDevice()
{
    const static D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
    };

    D3D_FEATURE_LEVEL levelUsed = D3D_FEATURE_LEVEL_9_3;

    ID3D11Device *pDevice = nullptr;
    ID3D11DeviceContext *pContext = nullptr;

    const auto hr = D3D11CreateDevice(adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                                   featureLevels, sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION,
                                   &pDevice, &levelUsed, &pContext);
    if (FAILED(hr))
        throw std::runtime_error("Failed to create device");

    device = releasing(pDevice);
    deviceContext = releasing(pContext);
}

bool ScreenCapture::initDuplicator()
{
    IDXGIOutput5 *output = nullptr;

    if (FAILED(adapterOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void **>(&output))))
        throw std::runtime_error("Failed to query IDXGIOutput1");

    duplicator.reset();
    duplicatorOutput.reset();

    IDXGIOutputDuplication *pDuplicator = nullptr;

    const DXGI_FORMAT captureFormats[] = {DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM};

    const auto hr = output->DuplicateOutput1(device.get(), 0, 2, captureFormats, &pDuplicator);
    if (hr == E_ACCESSDENIED || hr == DXGI_ERROR_SESSION_DISCONNECTED)
        return false;

    if (FAILED(hr))
        throw std::runtime_error("Failed to duplicate output");

    duplicator = releasing(pDuplicator);
    duplicatorOutput = releasing(output);

    return true;
}

bool ScreenCapture::grabContent()
{
    if (!duplicator)
    {
        if (!initDuplicator())
        {
            if (image)
                return true;
        }
    }

    DXGI_OUTDUPL_FRAME_INFO info;
    ID3D11Texture2D *frame = nullptr;
    IDXGIResource *res = nullptr;

    auto hr = duplicator->AcquireNextFrame(10, &info, &res);
    if (hr == DXGI_ERROR_ACCESS_LOST)
    {
        initDuplicator();
        return grabContent();
    }

    if (hr == DXGI_ERROR_WAIT_TIMEOUT)
    {
        duplicator->ReleaseFrame();
        return static_cast<bool>(image);
    }

    if (FAILED(hr))
    {
        initDuplicator();
        return false;
    }

    hr = res->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&frame));
    if (FAILED(hr))
    {
        duplicator->ReleaseFrame();
        return false;
    }

    D3D11_TEXTURE2D_DESC texDesc;
    frame->GetDesc(&texDesc);

    D3D11_TEXTURE2D_DESC imgDesc;
    if (image)
        image->GetDesc(&imgDesc);

    if (!image || imgDesc.Width != texDesc.Width || imgDesc.Height != texDesc.Height)
    {
        ID3D11Texture2D *img = nullptr;

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = texDesc.Width;
        desc.Height = texDesc.Height;
        desc.Format = texDesc.Format;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        device->CreateTexture2D(&desc, nullptr, &img);

        image = releasing(img);
    }

    deviceContext->CopyResource(image.get(), frame);
    duplicator->ReleaseFrame();

    return true;
}

std::shared_ptr<ID3D11Texture2D> ScreenCapture::getScreen() const
{
    return image;
}
