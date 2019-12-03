#pragma once

#include <string>
#include <functional>

namespace UIKit
{
	namespace UI
	{
		struct WidgetPoints
		{
			float x{}, y{};
		};

		class Widget
		{
		protected:
			float x{}, y{}, width{}, height{};
			bool visible{ true }, handleMouse{ false }, handleKeyboard{ false };
			std::wstring widgetID{};

		protected:
			virtual void update();
			virtual void render();

		public:
			Widget(const std::wstring&& widgetID, const float&& width = 0.0f, const float&& height = 0.0f, const float&& x = 0.0f, const float&& y = 0.0f);
			Widget(const std::wstring& widgetID, const float& width = 0.0f, const float& height = 0.0f, const float& x = 0.0f, const float& y = 0.0f);
			~Widget();

			virtual void setSizeInDIP(const WidgetPoints&& widgetSizeDIP);
			virtual void setSizeInDIP(const WidgetPoints& widgetSizeDIP);
			virtual void setSizeInPixel(const WidgetPoints&& widgetSize);
			virtual void setSizeInPixel(const WidgetPoints& widgetSize);

			virtual void setPosInDIP(const WidgetPoints&& widgetPosDIP);
			virtual void setPosInDIP(const WidgetPoints& widgetPosDIP);
			virtual void setPosInPixel(const WidgetPoints&& widgetPos);
			virtual void setPosInPixel(const WidgetPoints& widgetPos);

			WidgetPoints getSizeInDIP() const;
			WidgetPoints getSizeInPixel() const;
			WidgetPoints getPosInDIP() const;
			WidgetPoints getPosInPixel() const;

			std::wstring getWidgetID() const;

			virtual void show(bool flag = true);
			
			bool isVisible() const;
			bool isHandleMouse() const;
			bool isHandleKeyboard() const;

		};
		using WidgetCallback = std::function<void(Widget * pWidget)>;
	}
}