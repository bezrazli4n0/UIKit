#include "UIWidget.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::UI
{
	void Widget::update()
	{
	}

	void Widget::render()
	{
	}

	void Widget::onAttach()
	{
	}

	void Widget::onDetach()
	{
	}

	void Widget::onMouseUp(const int& xPos, const int& yPos)
	{
	}

	void Widget::onMouseDown(const int& xPos, const int& yPos)
	{
	}

	Widget::Widget(const std::wstring&& widgetID, const float&& width, const float&& height, const float&& x, const float&& y)
		: widgetID(widgetID), width(Graphics::pixelToDipX(width)), height(Graphics::pixelToDipY(height)), x(Graphics::pixelToDipX(x)), y(Graphics::pixelToDipY(y))
	{
	}

	Widget::Widget(const std::wstring& widgetID, const float& width, const float& height, const float& x, const float& y)
		: widgetID(widgetID), width(Graphics::pixelToDipX(width)), height(Graphics::pixelToDipY(height)), x(Graphics::pixelToDipX(x)), y(Graphics::pixelToDipY(y))
	{
	}

	Widget::~Widget()
	{
	}

	void Widget::setSizeInDIP(const WidgetPoints&& widgetSizeDIP)
	{
		auto [width, height] = widgetSizeDIP;
		this->width = width;
		this->height = height;
	}

	void Widget::setSizeInDIP(const WidgetPoints& widgetSizeDIP)
	{
		auto [width, height] = widgetSizeDIP;
		this->width = width;
		this->height = height;
	}

	void Widget::setSizeInPixel(const WidgetPoints&& widgetSize)
	{
		auto [width, height] = widgetSize;
		this->width = Graphics::pixelToDipX(width);
		this->height = Graphics::pixelToDipY(height);
	}

	void Widget::setSizeInPixel(const WidgetPoints& widgetSize)
	{
		auto [width, height] = widgetSize;
		this->width = Graphics::pixelToDipX(width);
		this->height = Graphics::pixelToDipY(height);
	}
	
	void Widget::setPosInDIP(const WidgetPoints&& widgetPosDIP)
	{
		auto [x, y] = widgetPosDIP;
		this->x = x;
		this->y = y;
	}

	void Widget::setPosInDIP(const WidgetPoints& widgetPosDIP)
	{
		auto [x, y] = widgetPosDIP;
		this->x = x;
		this->y = y;
	}

	void Widget::setPosInPixel(const WidgetPoints&& widgetPos)
	{
		auto [x, y] = widgetPos;
		this->x = Graphics::pixelToDipX(x);
		this->y = Graphics::pixelToDipY(y);
	}

	void Widget::setPosInPixel(const WidgetPoints& widgetPos)
	{
		auto [x, y] = widgetPos;
		this->x = Graphics::pixelToDipX(x);
		this->y = Graphics::pixelToDipY(y);
	}

	WidgetPoints Widget::getSizeInDIP() const
	{
		return { this->width, this->height };
	}

	WidgetPoints Widget::getSizeInPixel() const
	{
		return { Graphics::dipToPixelX(this->width), Graphics::dipToPixelY(this->height) };
	}
	
	WidgetPoints Widget::getPosInDIP() const
	{
		return { this->x, this->y };
	}

	WidgetPoints Widget::getPosInPixel() const
	{
		return { Graphics::dipToPixelX(this->x), Graphics::dipToPixelY(this->y) };
	}

	std::wstring Widget::getWidgetID() const
	{
		return this->widgetID;
	}

	void Widget::show(bool flag)
	{
		this->visible = flag;
	}
	
	bool Widget::isVisible() const
	{
		return this->visible;
	}
	
	bool Widget::isHandleMouse() const
	{
		return this->handleMouse;
	}

	bool Widget::isHandleKeyboard() const
	{
		return this->handleKeyboard;
	}

	bool Widget::isHandleTab() const
	{
		return this->handleTab;
	}
}
