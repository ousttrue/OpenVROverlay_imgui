#include "overlay.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <iostream>

auto CLASS_NAME = L"MinOverlayClass";
auto WINDOW_NAME = L"MinOverlay";

static Microsoft::WRL::ComPtr<IDXGIOutput1> GetPrimaryOutput(const Microsoft::WRL::ComPtr<IDXGIDevice> &dxgi)
{
    // Microsoft::WRL::ComPtr<IDXGIFactory> factory;
    // if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
    // {
    //     return nullptr;
    // }

    // get first adaapter
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    // if (FAILED(factory->EnumAdapters(0, &adapter)))
    // {
    //     return nullptr;
    // }
    if (FAILED(dxgi->GetAdapter(&adapter)))
    {
        return nullptr;
    }

    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);
        std::wcout << desc.Description << std::endl;
    }

    // get output
    Microsoft::WRL::ComPtr<IDXGIOutput> output;
    if (FAILED(adapter->EnumOutputs(0, &output)))
    {
        return nullptr;
    }

    Microsoft::WRL::ComPtr<IDXGIOutput1> output1;
    if (FAILED(output.As(&output1)))
    {
        return nullptr;
    }

    return output1;
}

void loop(const Microsoft::WRL::ComPtr<ID3D11Device> &device,
          const Microsoft::WRL::ComPtr<IDXGIOutput1> &output)
{
    Overlay overlay;
    if (!overlay.Initialize("desktop_dupl", "desktop dupl"))
    {
        return;
    }

    // dupl
    Microsoft::WRL::ComPtr<IDXGIOutputDuplication> dupl;
    if (FAILED(output->DuplicateOutput(device.Get(), &dupl)))
    {
        return;
    }

    // get dup
    for (int i = 0; true; ++i)
    {
        {
            dupl->ReleaseFrame();

            DXGI_OUTDUPL_FRAME_INFO info;
            Microsoft::WRL::ComPtr<IDXGIResource> resource;
            auto hr = dupl->AcquireNextFrame(INFINITE, &info, &resource);
            switch (hr)
            {
            case S_OK:
            {
                // update overlay
                overlay.Render(resource.Get());
            }
            break;

            case DXGI_ERROR_WAIT_TIMEOUT:
                break;

            case DXGI_ERROR_ACCESS_LOST:
                return;

            case DXGI_ERROR_INVALID_CALL:
                // not released previous frame
                return;

            default:
                return;
            }
        }
    }
}

int main(int argc, char **argv)
{
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_0,
    };
    D3D_FEATURE_LEVEL level;
    if (FAILED(D3D11CreateDevice(nullptr,
                                 D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                 D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, _countof(levels), D3D11_SDK_VERSION,
                                 &device, &level, nullptr)))
    {
        return 2;
    }

    Microsoft::WRL::ComPtr<IDXGIDevice> dxgi;
    if (FAILED(device.As(&dxgi)))
    {
        return 2;
    }

    auto output = GetPrimaryOutput(dxgi);
    if (!output)
    {
        return 2;
    }

    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);
        std::wcout << desc.DeviceName << std::endl;
    }

    loop(device, output);

    return 0;
}
