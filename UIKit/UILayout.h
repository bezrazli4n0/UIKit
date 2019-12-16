#pragma once

#include <vector>
#include <initializer_list>

namespace UIKit
{
	namespace UI
	{
		class Widget;
		class Window;

		class Layout
		{
		private:
			struct WidgetInfo
			{
				Widget* pWidget{};
				unsigned int row{}, rowSpan{}, col{}, colSpan{};
			};

			std::vector<float> rows{}, cols{};
			std::vector<WidgetInfo> widgetCollection{};
			float width{}, height{}, xPadding{}, yPadding{};

		private:
			void calculateLayout();

		public:
			Layout(std::initializer_list<float> Rows, std::initializer_list<float> Cols);
			~Layout();

			void setPadding(const float&& x, const float&& y);
			void setPadding(const float& x, const float& y);

			void addWidget(Widget* pWidget, unsigned int row, unsigned int col, unsigned int rowSpan = 0, unsigned int colSpan = 0);

			friend class Window;

		};
	}
}