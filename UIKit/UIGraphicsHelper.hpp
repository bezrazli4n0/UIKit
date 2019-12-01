#pragma once

namespace UIKit
{
	namespace Graphics
	{
		static float dpiX{ 96.0f }, dpiY{ 96.0f };

		static inline float pixelToDipX(const float x, bool round = false)
		{
			return round ? (static_cast<int>((x * dpiX) / 96.0f)) : (x * dpiX) / 96.0f;
		}

		static inline float pixelToDipY(const float y, bool round = false)
		{
			return round ? (static_cast<int>((y * dpiY) / 96.0f)) : (y * dpiY) / 96.0f;
		}

		static inline float dipToPixelX(const float x)
		{
			return (x * 96.0f) / dpiX;
		}

		static inline float dipToPixelY(const float y)
		{
			return (y * 96.0f) / dpiY;
		}
	}
}