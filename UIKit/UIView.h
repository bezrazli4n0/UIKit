#pragma once

#include "UIWidget.h"
#include <vector>

namespace UIKit
{
	namespace UI
	{
		class View : public Widget
		{
		private:
			std::vector<Widget*> viewWidgets{};
			float offsetX{}, offsetY{}, widgetsHeight{}, lastCursorY{};
			bool displayScrollBar{ true }, thumbPressed{};
			ID2D1RoundedRectangleGeometry* pThumRect{};
			ID2D1SolidColorBrush* pBrush{};
			D2D1_MATRIX_3X2_F thumbMatrix{ D2D1::Matrix3x2F::Identity() };

		private:
			virtual void update();
			virtual void render();
			
			virtual void onAttach();
			virtual void onDetach();

			virtual void onMouseDown(const int& xPos, const int& yPos);
			virtual void onMouseUp(const int& xPos, const int& yPos);
			virtual void onMouseMove(const int& xPos, const int& yPos);
			virtual void onMouseScroll(const int& xPos, const int& yPos, const short& delta);

			void calculateWidgetsHeight();
			bool cursorInThumb(const int& xPos, const int& yPos);
			void rereateThumbGeometry();
			void drawScrollbar();

		public:
			View(const std::wstring&& viewID, const bool&& scrollbar = true, const float&& width = 0.0f, const float&& height = 0.0f, const float&& x = 0.0f, const float&& y = 0.0f);
			View(const std::wstring& viewID, const bool& scrollbar = true, const float& width = 0.0f, const float& height = 0.0f, const float& x = 0.0f, const float& y = 0.0f);
			~View();

			void setOffset(const float&& x = 0.0f, const float&& y = 0.0f);
			void setOffset(const float& x = 0.0f, const float& y = 0.0f);

			void setOffsetX(const float&& x = 0.0f);
			void setOffsetX(const float& x = 0.0f);

			void setOffsetY(const float&& y = 0.0f);
			void setOffsetY(const float& y = 0.0f);

			void addWidget(Widget* pWidget);

		};
	}
}