#pragma once

#include <string>
#include <functional>
#include <d2d1_1.h>

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
			bool visible{ true }, handleMouse{ false }, handleKeyboard{ false }, tabStop{ false }, active{ false };
			std::wstring widgetID{};
			ID2D1DeviceContext* pRT{};
			int zIndex{};

		protected:
			virtual void update();
			virtual void render();

			virtual void onAttach();
			virtual void onDetach();

			virtual void onMouseUp(const int& xPos, const int& yPos);
			virtual void onMouseDown(const int& xPos, const int& yPos);
			virtual void onMouseMove(const int& xPos, const int& yPos);
			virtual void onMouseScroll(const int& xPos, const int& yPos, const short& delta);
			virtual void onChar(UINT32 c);
			virtual void onKey(UINT32 vk);
			virtual void onTabStop(bool flag);

			virtual bool updateCursor();

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
			
			int getZIndex() const;

			bool isVisible() const;
			bool isHandleMouse() const;
			bool isHandleKeyboard() const;
			bool isTabStop() const;
			bool isActive() const;

			friend class Window;

		};

		using WidgetCallback = std::function<void(Widget* pWidget)>;
	}
}