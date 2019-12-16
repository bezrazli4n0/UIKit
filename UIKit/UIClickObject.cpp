#include "UIClickObject.h"
#include "UIGraphics.h"

namespace UIKit::UI
{
	void ClickObject::recreateGeometry()
	{
		Graphics::SafeRelease(&this->pRoundRectGeometry);
		Graphics::Core::getFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(this->x, this->y, this->x + this->width, this->y + this->height), this->radiusX, this->radiusY), &this->pRoundRectGeometry);
	}

	void ClickObject::onPosChanged()
	{
		this->recreateGeometry();
	}

	void ClickObject::onSizeChanged()
	{
		this->recreateGeometry();
	}

	ClickObject::ClickObject(const std::wstring&& objectID, const float&& width, const float&& height, const float&& x, const float&& y)
		: Widget(objectID, width, height, x, y)
	{
		this->recreateGeometry();
	}

	ClickObject::ClickObject(const std::wstring& objectID, const float& width, const float& height, const float& x, const float& y)
		: Widget(objectID, width, height, x, y)
	{
		this->recreateGeometry();
	}

	ClickObject::~ClickObject()
	{
		Graphics::SafeRelease(&this->pRoundRectGeometry);
	}

	void ClickObject::setRadius(const float&& x, const float&& y)
	{
		this->radiusX = x;
		this->radiusY = y;
		this->recreateGeometry();
	}

	void ClickObject::setRadius(const float& x, const float& y)
	{
		this->radiusX = x;
		this->radiusY = y;
		this->recreateGeometry();
	}
	
	bool ClickObject::checkMouse(const float& xPos, const float& yPos)
	{
		BOOL contains{};
		this->pRoundRectGeometry->FillContainsPoint(D2D1::Point2F(xPos, yPos), D2D1::Matrix3x2F::Identity(), &contains);
		return static_cast<bool>(contains);
	}
}

