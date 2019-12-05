#include "UIRectangle.h"
#include "UIGraphics.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::UI
{
	void Rectangle::update()
	{
	}

	void Rectangle::render()
	{
		this->pBrush->SetColor(this->rectColor);
		if (this->framed)
			this->pRT->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(std::ceilf(this->x) + Graphics::pixelToDipX(0.5f), std::ceilf(this->y) + Graphics::pixelToDipY(0.5f), std::ceilf(this->x + this->width) + Graphics::pixelToDipX(0.5f), std::ceilf(this->y + this->height) + Graphics::pixelToDipY(0.5f)), this->radiusX, this->radiusY), this->pBrush);
		else
			this->pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(std::ceilf(this->x), std::ceilf(this->y), std::ceilf(this->x + this->width), std::ceilf(this->y + this->height)), this->radiusX, this->radiusY), this->pBrush);
	}

	void Rectangle::onAttach()
	{
		Graphics::SafeRelease(&this->pBrush);
		this->pRT->CreateSolidColorBrush(this->rectColor, &this->pBrush);
	}

	void Rectangle::onDetach()
	{
		Graphics::SafeRelease(&this->pBrush);
	}

	Rectangle::Rectangle(const std::wstring&& rectID, const float&& width, const float&& height, const float&& x, const float&& y)
		: Widget(rectID, width, height, x, y)
	{
	}

	Rectangle::Rectangle(const std::wstring& rectID, const float& width, const float& height, const float& x, const float& y)
		: Widget(rectID, width, height, x, y)
	{
	}

	Rectangle::~Rectangle()
	{
		Graphics::SafeRelease(&this->pBrush);
	}

	void Rectangle::setFramed(bool framed)
	{
		this->framed = framed;
	}

	void Rectangle::setRadius(const float&& radiusX, const float&& radiusY)
	{
		this->radiusX = radiusX;
		this->radiusY = radiusY;
	}

	void Rectangle::setRadius(const float& radiusX, const float& radiusY)
	{
		this->radiusX = radiusX;
		this->radiusY = radiusY;
	}

	void Rectangle::setRectColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a)
	{
		this->rectColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void Rectangle::setRectColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		this->rectColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void Rectangle::draw()
	{
		if (this->isVisible() && this->pRT != nullptr)
		{
			this->update();
			this->render();
		}
	}

	void Rectangle::setRT(ID2D1DeviceContext* pRT)
	{
		this->pRT = pRT;
		this->onAttach();
	}
}
