#include "UILine.h"
#include "UIGraphics.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::UI
{
	void Line::update()
	{
	}

	void Line::render()
	{
		this->pBrush->SetColor(this->lineColor);
		this->pRT->DrawLine(D2D1::Point2F(Graphics::pixelToDipX(this->pt1.x), Graphics::pixelToDipY(this->pt1.y)), D2D1::Point2F(Graphics::pixelToDipX(this->pt2.x), Graphics::pixelToDipY(this->pt2.y)), this->pBrush, this->strokeSize);
	}

	void Line::onAttach()
	{
		this->pRT->CreateSolidColorBrush(this->lineColor, &this->pBrush);
	}

	void Line::onDetach()
	{
		Graphics::SafeRelease(&this->pBrush);
	}

	Line::Line(const std::wstring&& lineID, WidgetPoints&& pt1, WidgetPoints&& pt2, const float&& strokeSize)
		: strokeSize(strokeSize), pt1(pt1), pt2(pt2), Widget(lineID, pt1.x + pt2.x, pt1.y + pt2.y, pt1.x, pt1.y)
	{
	}

	Line::Line(const std::wstring& lineID, WidgetPoints& pt1, WidgetPoints& pt2, const float& strokeSize)
		: strokeSize(strokeSize), pt1(pt1), pt2(pt2), Widget(lineID, pt1.x + pt2.x, pt1.y + pt2.y, pt1.x, pt1.y)
	{
	}

	Line::~Line()
	{
		Graphics::SafeRelease(&this->pBrush);
	}

	void Line::setLineColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a)
	{
		this->lineColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void Line::setLineColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		this->lineColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void Line::setStrokeSize(const float&& strokeSize)
	{
		this->strokeSize = strokeSize;
	}

	void Line::setStrokeSize(const float& strokeSize)
	{
		this->strokeSize = strokeSize;
	}

	void Line::draw()
	{
		if (this->isVisible() && this->pRT != nullptr)
		{
			this->update();
			this->render();
		}
	}

	void Line::setRT(ID2D1DeviceContext* pRT)
	{
		this->pRT = pRT;
		this->onAttach();
	}
}
