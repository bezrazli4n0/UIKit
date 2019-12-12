#pragma once

#include "UIWidget.h"

namespace UIKit
{
	namespace UI
	{
		class Line : public Widget
		{
		private:
			virtual void update();
			virtual void render();

			virtual void onAttach();
			virtual void onDetach();

		private:
			ID2D1SolidColorBrush* pBrush{};
			D2D1_COLOR_F lineColor{ D2D1::ColorF(D2D1::ColorF::Black) };
			float strokeSize{};
			WidgetPoints pt1{}, pt2{};

		public:
			Line(const std::wstring&& lineID, WidgetPoints&& pt1 = { 0.0f, 0.0f }, WidgetPoints&& pt2 = { 0.0f, 0.0f }, const float&& strokeSize = 1.0f);
			Line(const std::wstring& lineID, WidgetPoints& pt1, WidgetPoints& pt2, const float& strokeSize = 1.0f);
			~Line();

			void setLineColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a = 255);
			void setLineColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a = 255);

			void setStrokeSize(const float&& strokeSize);
			void setStrokeSize(const float& strokeSize);

			void draw();
			void setRT(ID2D1DeviceContext* pRT);

		};
	}
}