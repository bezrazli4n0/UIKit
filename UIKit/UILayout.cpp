#include "UILayout.h"
#include "UIWidget.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::UI
{
	void Layout::calculateLayout()
	{
		const auto cellWidth = this->width / Graphics::pixelToDipX(static_cast<float>(this->cols.size()));
		const auto cellHeight = this->height / Graphics::pixelToDipY(static_cast<float>(this->rows.size()));

		if (this->widgetCollection.empty())
			return;

		for (int row = 0; row < static_cast<int>(this->rows.size()); row++)
		{
			for (int col = 0; col < static_cast<int>(this->cols.size()); col++)
			{
				for (auto&& widgetInfo : this->widgetCollection)
				{
					if (widgetInfo.row == row && widgetInfo.col == col)
					{
						auto newWidth = cellWidth * this->cols[col];
						auto newHeight = cellHeight * this->rows[row];

						auto xPos = (newWidth * (1 + widgetInfo.col) - newWidth + Graphics::pixelToDipX(1.0f));
						auto yPos = (newHeight * (1 + widgetInfo.row) - newHeight + Graphics::pixelToDipY(1.0f));

						auto ColSpan = widgetInfo.colSpan + 1;
						auto RowSpan = widgetInfo.rowSpan + 1;
						widgetInfo.pWidget->setPosInDIP({ xPos + this->xPadding, yPos + this->yPadding });
						widgetInfo.pWidget->setSizeInDIP({ newWidth * ColSpan - this->xPadding * 2, newHeight * RowSpan - this->yPadding * 2 });
					}
				}
			}
		}
	}

	Layout::Layout(std::initializer_list<float> Rows, std::initializer_list<float> Cols)
		: rows(Rows), cols(Cols)
	{
	}

	Layout::~Layout()
	{
	}

	void Layout::setPadding(const float&& x, const float&& y)
	{
		this->xPadding = Graphics::pixelToDipX(x);
		this->yPadding = Graphics::pixelToDipY(y);
	}

	void Layout::setPadding(const float& x, const float& y)
	{
		this->xPadding = Graphics::pixelToDipX(x);
		this->yPadding = Graphics::pixelToDipY(y);
	}

	void Layout::addWidget(Widget* pWidget, unsigned int row, unsigned int col, unsigned int rowSpan, unsigned int colSpan)
	{
		if (pWidget != nullptr)
			this->widgetCollection.push_back({ pWidget, row, rowSpan, col, colSpan });
	}
}
