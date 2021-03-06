#pragma once
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "dxguid")

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include <wincodec.h>

namespace UIKit
{
	namespace Graphics
	{
		template<typename T>
		inline void SafeRelease(T** ComObj)
		{
			if ((*ComObj) != nullptr)
			{
				(*ComObj)->Release();
				(*ComObj) = nullptr;
			}
		}

		class Core
		{
		private:
			static inline ID3D11Device* pD3D11Device{};
			static inline ID2D1Factory1* pD2D1Factory{};
			static inline IDWriteFactory* pDWriteFactory{};
			static inline IWICImagingFactory* pIWICFactory{};

		public:
			static ID3D11Device* getDevice();
			static ID2D1Factory1* getFactory();
			static IDWriteFactory* getDWriteFactory();
			static IWICImagingFactory* getWICFactory();
			static bool initCore(const bool customDPI = false, const float customDpiX = 96.0f, const float customDpiY = 96.0f);
			static void freeCore();

		};
	}
}