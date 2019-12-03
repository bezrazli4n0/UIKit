#pragma once

#include "UIGraphics.h"

namespace UIKit
{
	namespace Graphics
	{
		class Renderer
		{
		private:
			HWND hWnd{};
			IDXGIFactory2* pDXGIFactory{};
			IDXGIDevice1* pDXGIDevice{};
			IDXGIAdapter* pDXGIAdapter{};
			IDXGISwapChain1* pDXGISwapChain{};
			IDXGISurface* pDXGISurface{};
			ID2D1Device* pD2D1Device{};
			ID2D1DeviceContext* pD2D1DeviceContext{};

		private:
			bool createSwapChain();

		public:
			Renderer(HWND hWnd);
			~Renderer();
			ID2D1DeviceContext* getContext() const;
			IDXGISwapChain1* getSwapChain() const;

			bool init();
			void free();

		};
	}
}