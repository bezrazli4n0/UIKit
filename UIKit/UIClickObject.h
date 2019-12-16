#pragma once

#include "UIWidget.h"

namespace UIKit
{
	namespace UI
	{
		class ClickObject : public Widget
		{
		private:
			ID2D1RoundedRectangleGeometry* pRoundRectGeometry{};
			float radiusX{}, radiusY{};

		private:
			void recreateGeometry();
			virtual void onPosChanged();
			virtual void onSizeChanged();

		public:
			ClickObject(const std::wstring&& objectID, const float&& width = 100.0f, const float&& height = 100.0f, const float&& x = 0.0f, const float&& y = 0.0f);
			ClickObject(const std::wstring& objectID, const float& width = 100.0f, const float& height = 100.0f, const float& x = 0.0f, const float& y = 0.0f);
			~ClickObject();

			void setRadius(const float&& x, const float&& y);
			void setRadius(const float& x, const float& y);

			bool checkMouse(const float& xPos, const float& yPos);

		};
	}
}