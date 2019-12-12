#pragma once

#include "UIWidget.h"

namespace UIKit
{
	namespace UI
	{
		class Rectangle : public Widget
		{
		private:
			virtual void update();
			virtual void render();

			virtual void onAttach();
			virtual void onDetach();

		private:
			ID2D1SolidColorBrush* pBrush{};
			D2D1_COLOR_F rectColor{ D2D1::ColorF(D2D1::ColorF::Black) };
			float radiusX{}, radiusY{}, strokeSize{ 1.0f };
			bool framed{}, roundValue{ true };

		public:
			Rectangle(const std::wstring&& rectID, const float&& width = 0.0f, const float&& height = 0.0f, const float&& x = 0.0f, const float&& y = 0.0f);
			Rectangle(const std::wstring& rectID, const float& width = 0.0f, const float& height = 0.0f, const float& x = 0.0f, const float& y = 0.0f);
			~Rectangle();

			void setFramed(bool framed);

			void setRadius(const float&& radiusX, const float&& radiusY);
			void setRadius(const float& radiusX, const float& radiusY);

			void setRectColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a = 255);
			void setRectColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a = 255);

			void setStrokeSize(const float&& strokeSize);
			void setStrokeSize(const float& strokeSize);

			void roundRenderValue(const bool&& flag);

			void draw();
			void setRT(ID2D1DeviceContext* pRT);

		};
	}
}