#include "UIView.h"
#include "UIGraphics.h"

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
			widget->render();

		this->pRT->SetTransform(D2D1::Matrix3x2F::Identity());
		this->pRT->PopLayer();
		Graphics::SafeRelease(&pLayer);
	}

	void View::onAttach()
	{
		for (const auto& widget : this->viewWidgets)
		{
			widget->pRT = this->pRT;
			widget->onAttach();
		}
	}

	void View::onDetach()
	{
		for (const auto& widget : this->viewWidgets)
			widget->onDetach();
	}

	View::View(const std::wstring&& viewID, const float&& width, const float&& height, const float&& x, const float&& y)
		: Widget(viewID, width, height, x, y)
	{
	}

	View::View(const std::wstring& viewID, const float& width, const float& height, const float& x, const float& y)
		: Widget(viewID, width, height, x, y)
	{
	}

	View::~View()
	{
	}

	void View::addWidget(Widget* pWidget)
	{
		if (pWidget != nullptr)
			this->viewWidgets.push_back(pWidget);
	}
}
