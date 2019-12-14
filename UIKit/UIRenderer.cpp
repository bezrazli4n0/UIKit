#include "UIRenderer.h"
#include "UIGraphics.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::Graphics
{
	bool Renderer::init()
	{
		HRESULT hr{};
		if (hr = Core::getDevice()->QueryInterface(&this->pDXGIDevice); !SUCCEEDED(hr))
			return false;

		if (hr = this->pDXGIDevice->GetAdapter(&this->pDXGIAdapter); !SUCCEEDED(hr))
			return false;

		if (hr = this->pDXGIAdapter->GetParent(IID_PPV_ARGS(&this->pDXGIFactory)); !SUCCEEDED(hr))
			return false;

		if (hr = Core::getFactory()->CreateDevice(this->pDXGIDevice, &this->pD2D1Device); !SUCCEEDED(hr))
			return false;

		if (hr = this->pD2D1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &this->pD2D1DeviceContext); !SUCCEEDED(hr))
			return false;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = 0;                           // use automatic sizing
		swapChainDesc.Height = 0;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;                     // use double buffering to enable flip
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
		swapChainDesc.Flags = 0;

		if (!SUCCEEDED(this->pDXGIFactory->CreateSwapChainForHwnd(Core::getDevice(), this->hWnd, &swapChainDesc, nullptr, nullptr, &this->pDXGISwapChain)))
			return false;

		this->pDXGIDevice->SetMaximumFrameLatency(1);

		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
				DPI::dpiX,
				DPI::dpiY
			);

		this->pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&this->pDXGISurface));
		
		ID2D1Bitmap1* pBitmap{};
		this->pD2D1DeviceContext->CreateBitmapFromDxgiSurface(this->pDXGISurface, &bitmapProperties, &pBitmap);
		this->pD2D1DeviceContext->SetTarget(pBitmap);
		SafeRelease(&pBitmap);
		SafeRelease(&this->pDXGISurface);

		return true;
	}

	void Renderer::free()
	{
		SafeRelease(&this->pDXGIFactory);
		SafeRelease(&this->pDXGIDevice);
		SafeRelease(&this->pDXGIAdapter);
		SafeRelease(&this->pDXGISwapChain);
		SafeRelease(&this->pDXGISurface);
		SafeRelease(&this->pD2D1Device);
		SafeRelease(&this->pD2D1DeviceContext);
	}

	void Renderer::resize()
	{
		this->pD2D1DeviceContext->SetTarget(nullptr);
		this->pDXGISwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
				DPI::dpiX,
				DPI::dpiY
			);

		this->pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&this->pDXGISurface));

		ID2D1Bitmap1* pBitmap{};
		this->pD2D1DeviceContext->CreateBitmapFromDxgiSurface(this->pDXGISurface, &bitmapProperties, &pBitmap);
		this->pD2D1DeviceContext->SetTarget(pBitmap);
		SafeRelease(&pBitmap);
		SafeRelease(&this->pDXGISurface);
	}

	Renderer::Renderer(HWND hWnd)
		: hWnd(hWnd)
	{
	}

	Renderer::~Renderer()
	{
		this->free();
	}

	ID2D1DeviceContext* Renderer::getContext() const
	{
		return this->pD2D1DeviceContext;
	}

	IDXGISwapChain1* Renderer::getSwapChain() const
	{
		return this->pDXGISwapChain;
	}
}