#include "UIView.h"
#include "UIGraphics.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::UI
{
	void View::update()
	{
		for (const auto& widget : this->viewWidgets)
			widget->update();
	}

	void View::render()
	{
		ID2D1Layer* pLayer{};
		this->pRT->CreateLayer(D2D1::SizeF(this->width, this->height), &pLayer);
		this->pRT->PushLayer(D2D1::LayerParameters1(D2D1::RectF(this->x, this->y, this->x + this->width, this->y + this->height)), pLayer);
		this->pRT->SetTransform(D2D1::Matrix3x2F::Translation(this->offsetX, this->offsetY));
		
		for (const auto& widget : this->viewWidgets)
			if (widget->isVisible())
				widget->render();

		this->pRT->SetTransform(D2D1::Matrix3x2F::Identity());
		this->pRT->PopLayer();
		
		if ((this->widgetsHeight > this->height) && this->displayScrollBar)
			// Draw scrollbar
			this->drawScrollbar();

		Graphics::SafeRelease(&pLayer);
	}

	void View::onAttach()
	{
		this->pRT->CreateSolidColorBrush(this->scrollBarColor, &this->pBrush);
		for (const auto& widget : this->viewWidgets)
		{
			widget->pRT = this->pRT;
			widget->onAttach();
		}
	}

	void View::onDetach()
	{
		Graphics::SafeRelease(&this->pBrush);

		for (const auto& widget : this->viewWidgets)
			widget->onDetach();
	}

	void View::onMouseDown(const int& xPos, const int& yPos)
	{
		for (const auto& widget : this->viewWidgets)
			if (widget->isHandleMouse() && widget->isVisible())
				widget->onMouseDown(xPos, yPos);

		if (this->cursorInThumb(xPos, yPos))
		{
			this->thumbPressed = true;
			this->lastCursorY = static_cast<float>(yPos);
		}
		else
			this->thumbPressed = false;
	}

	void View::onMouseUp(const int& xPos, const int& yPos)
	{
		for (const auto& widget : this->viewWidgets)
			if (widget->isHandleMouse() && widget->isVisible())
				widget->onMouseUp(xPos, yPos);

		this->thumbPressed = false;
		this->lastCursorY = 0.0f;
	}

	void View::onMouseMove(const int& xPos, const int& yPos)
	{
		for (const auto& widget : this->viewWidgets)
			if (widget->isHandleMouse() && widget->isVisible())
				widget->onMouseMove(xPos, yPos);

		if (this->thumbPressed)
		{
			// Drag loggic!
			auto curDelta = this->lastCursorY - yPos;
			D2D1_RECT_F bounds{};
			this->pThumRect->GetBounds(this->thumbMatrix, &bounds);
			if (this->y <= bounds.top - curDelta && this->y + this->height >= bounds.bottom - curDelta)
			{
				this->thumbMatrix = this->thumbMatrix * D2D1::Matrix3x2F::Translation(0.0f, -curDelta);

				const auto currentThumbValue = this->thumbMatrix.dy;
				const auto currentThumbPercentage = currentThumbValue * 100.0f / this->height;
				const auto currentHeight = (this->widgetsHeight - this->height) * currentThumbPercentage / 100.0f;

				this->offsetY = -currentHeight;
			}
			this->lastCursorY = static_cast<float>(yPos);
		}
	}

	void View::onMouseScroll(const int& xPos, const int& yPos, const short& delta)
	{
		for (const auto& widget : this->viewWidgets)
			if (widget->isHandleMouse() && widget->isVisible())
				widget->onMouseScroll(xPos, yPos, delta);
	}

	void View::calculateWidgetsHeight()
	{
		this->widgetsHeight = 0.0f;
		for (const auto& widget : this->viewWidgets)
			this->widgetsHeight += widget->height;
	}

	bool View::cursorInThumb(const int& xPos, const int& yPos)
	{
		BOOL contains{};
		if (this->pThumRect != nullptr)
			this->pThumRect->FillContainsPoint(D2D1::Point2F(static_cast<float>(xPos), static_cast<float>(yPos)), this->thumbMatrix, &contains);
		else
			return false;

		if (!this->displayScrollBar)
			return false;

		return static_cast<bool>(contains);
	}

	void View::rereateThumbGeometry()
	{
		Graphics::SafeRelease(&this->pThumRect);
		auto thumbHeight = this->height - (this->widgetsHeight - this->height);
		if (thumbHeight < Graphics::pixelToDipY(30.0f))
			thumbHeight = Graphics::pixelToDipY(30.0f);
		Graphics::Core::getFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(this->x + this->width + Graphics::pixelToDipX(2.0f), this->y, this->x + this->width + Graphics::pixelToDipX(this->scrollBarWidth) + Graphics::pixelToDipX(2.0f), this->y + thumbHeight), this->scrollBarRadiusX, this->scrollBarRadiusY), &this->pThumRect);
	}

	void View::drawScrollbar()
	{
		this->pRT->SetTransform(this->thumbMatrix);
		this->pBrush->SetColor(this->scrollBarColor);
		this->pRT->FillGeometry(this->pThumRect, this->pBrush);
		this->pRT->SetTransform(D2D1::Matrix3x2F::Identity());
	}

	View::View(const std::wstring&& viewID, const bool&& scrollbar, const float&& width, const float&& height, const float&& x, const float&& y)
		: displayScrollBar{scrollbar}, Widget(viewID, width, height, x, y)
	{
		this->handleMouse = true;
	}

	View::View(const std::wstring& viewID, const bool& scrollbar, const float& width, const float& height, const float& x, const float& y)
		: displayScrollBar{ scrollbar }, Widget(viewID, width, height, x, y)
	{
		this->handleMouse = true;
	}

	View::~View()
	{
		Graphics::SafeRelease(&this->pThumRect);
		Graphics::SafeRelease(&this->pBrush);
	}

	void View::setScrollBarRadius(const float&& radius)
	{
		this->scrollBarRadiusX = this->scrollBarRadiusY = radius;
		this->rereateThumbGeometry();
	}

	void View::setScrollBarRadius(const float& radius)
	{
		this->scrollBarRadiusX = this->scrollBarRadiusY = radius;
		this->rereateThumbGeometry();
	}

	void View::setScrollBarWidth(const float&& width)
	{
		this->scrollBarWidth = width;
		this->rereateThumbGeometry();
	}

	void View::setScrollBarWidth(const float& width)
	{
		this->scrollBarWidth = width;
		this->rereateThumbGeometry();
	}

	void View::setScrollBarColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a)
	{
		this->scrollBarColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void View::setScrollBarColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		this->scrollBarColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void View::setOffset(const float&& x, const float&& y)
	{
		this->offsetX = x;
		this->offsetY = y;
	}

	void View::setOffset(const float& x, const float& y)
	{
		this->offsetX = x;
		this->offsetY = y;
	}

	void View::setOffsetX(const float&& x)
	{
		this->offsetX = x;
	}

	void View::setOffsetX(const float& x)
	{
		this->offsetX = x;
	}

	void View::setOffsetY(const float&& y)
	{
		this->offsetY = y;
	}

	void View::setOffsetY(const float& y)
	{
		this->offsetY = y;
	}

	void View::addWidget(Widget* pWidget)
	{
		if (pWidget != nullptr)
		{
			this->viewWidgets.push_back(pWidget);
			this->calculateWidgetsHeight();
			this->rereateThumbGeometry();
		}
	}
}
