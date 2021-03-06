#pragma warning(disable: 4996)

#include "UIGraphics.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::Graphics
{
	ID3D11Device* Core::getDevice()
	{
		return pD3D11Device;
	}

	ID2D1Factory1* Core::getFactory()
	{
		return pD2D1Factory;
	}

	IDWriteFactory* Core::getDWriteFactory()
	{
		return pDWriteFactory;
	}

	IWICImagingFactory* Core::getWICFactory()
	{
		return pIWICFactory;
	}

	bool Core::initCore(const bool customDPI, const float customDpiX, const float customDpiY)
	{
		CoInitialize(0);

		HRESULT hr1 = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pIWICFactory)
		);

		if (!SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2D1Factory)))
			return false;

		if (!SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory))))
			return false;

		if (customDPI)
		{
			DPI::dpiX = customDpiX;
			DPI::dpiY = customDpiY;
		}
		else
			pD2D1Factory->GetDesktopDpi(&DPI::dpiX, &DPI::dpiY);

		static const D3D_FEATURE_LEVEL levelAttempts[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		HRESULT hr{};
		for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++)
		{
			if (hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				D3D11_CREATE_DEVICE_BGRA_SUPPORT,
				&levelAttempts[level],
				ARRAYSIZE(levelAttempts),
				D3D11_SDK_VERSION,
				&pD3D11Device,
				nullptr,
				nullptr
			); SUCCEEDED(hr))
				break;
		}

		return true;
	}

	void Core::freeCore()
	{
		SafeRelease(&pIWICFactory);
		SafeRelease(&pDWriteFactory);
		SafeRelease(&pD2D1Factory);
		SafeRelease(&pD3D11Device);
		CoUninitialize();
	}
}
