#pragma once

#include "UIWidget.h"
#include "UIRenderer.h"
#include <Windows.h>
#include <list>

namespace UIKit
{
	namespace UI
	{
		class Window : public Widget
		{
		public:
			struct windowOptions
			{
				bool centeredWindow{ false }, framelessWindow{ false };
				struct roundedOptions
				{
					int windowRadiusX{ 11 }, windowRadiusY{ 11 };
					bool roundedWindow{ false };
				} roundedWindow;
			};

		private:
			HWND windowHandle{};
			std::wstring windowClass{}, windowTitle{};
			WidgetCallback onCloseCallback{};
			Graphics::Renderer* pRenderer{};
			std::list<Widget*> windowWidgets{};
			DWORD windowStyle{ WS_OVERLAPPEDWINDOW };
			bool windowCenter{ false }, windowFrameless{ false };
			windowOptions::roundedOptions roundedWindow{};
			D2D1_COLOR_F windowBackgroundColor{ D2D1::ColorF(D2D1::ColorF::White) };
			HRGN hRoundedRgn{};

		private:
			bool createWindow();
			WidgetPoints getCorrectedWindowSize();

			virtual void update();
			virtual void render();
			virtual void onMouseUp(const int& xPos, const int& yPos);
			virtual void onMouseDown(const int& xPos, const int& yPos);
			virtual void onMouseMove(const int& xPos, const int& yPos);
			virtual void onMouseScroll(const short& delta);
			virtual void onChar(UINT32 c);
			virtual void onKey(UINT32 vk);

			LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK WindowProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		public:
			Window(const std::wstring&& windowClass, const std::wstring&& windowTitle = { L"UIKit" }, windowOptions* pWindowOptions = nullptr, const unsigned short&& width = 640, const unsigned short&& height = 480, const unsigned short&& x = 0, const unsigned short&& y = 0);
			Window(const std::wstring& windowClass, const std::wstring& windowTitle = { L"UIKit" }, windowOptions* pWindowOptions = nullptr, const unsigned short& width = 640, const unsigned short& height = 480, const unsigned short& x = 0, const unsigned short& y = 0);
			~Window();

			const std::wstring& getTitle() const;
			void setTitle(const std::wstring&& windowTitle);
			void setTitle(const std::wstring& windowTitle);

			virtual void setSizeInDIP(const WidgetPoints&& widgetSizeDIP);
			virtual void setSizeInDIP(const WidgetPoints& widgetSizeDIP);
			virtual void setSizeInPixel(const WidgetPoints&& widgetSize);
			virtual void setSizeInPixel(const WidgetPoints& widgetSize);

			virtual void setPosInDIP(const WidgetPoints&& widgetPosDIP);
			virtual void setPosInDIP(const WidgetPoints& widgetPosDIP);
			virtual void setPosInPixel(const WidgetPoints&& widgetPos);
			virtual void setPosInPixel(const WidgetPoints& widgetPos);

			void setBackgroundColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a = 255);
			void setBackgroundColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a = 255);

			void addWidget(Widget* pWidget);
			void removeWidget(const std::wstring&& widgetID);
			void removeWidget(const std::wstring& widgetID);

			virtual void show(bool flag = true);
			HWND getHandle() const;

			void onClose(WidgetCallback callback);

			void updateWindow();

		};
	}
}