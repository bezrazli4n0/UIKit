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
			float offsetX{}, offsetY{};

		private:
			virtual void update();
			virtual void render();
			
			virtual void onAttach();
			virtual void onDetach();

		public:
			View(const std::wstring&& viewID, const float&& width, const float&& height, const float&& x = 0.0f, const float&& y = 0.0f);
			View(const std::wstring& viewID, const float& width, const float& height, const float& x = 0.0f, const float& y = 0.0f);
			~View();

			void addWidget(Widget* pWidget);

		};
	}
}