#pragma once

namespace UIKit
{
	namespace Graphics
	{
		struct DPI
		{
			static inline float dpiX{ 0.0f }, dpiY{ 0.0f };
		};

		static inline float pixelToDipX(const float x, bool round = false)
		{
			return round ? (static_cast<int>((x * DPI::dpiX) / 96.0f)) : (x * DPI::dpiX) / 96.0f;
		}

		static inline float pixelToDipY(const float y, bool round = false)
		{
			return round ? (static_cast<int>((y * DPI::dpiY) / 96.0f)) : (y * DPI::dpiY) / 96.0f;
		}

		static inline float dipToPixelX(const float x)
		{
			return (x * 96.0f) / DPI::dpiX;
		}

		static inline float dipToPixelY(const float y)
		{
			return (y * 96.0f) / DPI::dpiY;
		}
	}
}