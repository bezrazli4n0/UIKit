#pragma once

#include "UIWidget.h"
#include "UIRenderer.h"
#include <Windows.h>

namespace UIKit
{
	namespace UI
	{
		class Window : public Widget
		{
		private:
			HWND windowHandle{};
			std::wstring windowClass{}, windowTitle{};
			WidgetCallback onCloseCallback{};
			Graphics::Renderer* pRenderer{};

		private:
			bool createWindow();
			WidgetPoints getCorrectedWindowSize();
			virtual void update();
			virtual void render();

			LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK WindowProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		public:
			Window(const std::wstring&& windowClass, const std::wstring&& windowTitle = { L"UIKit" }, const unsigned short&& width = 640, const unsigned short&& height = 480, const unsigned short&& x = 0, const unsigned short&& y = 0);
			Window(const std::wstring& windowClass, const std::wstring& windowTitle = { L"UIKit" }, const unsigned short& width = 640, const unsigned short& height = 480, const unsigned short& x = 0, const unsigned short& y = 0);
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

			virtual void show(bool flag = true);
			HWND getHandle() const;

			void onClose(WidgetCallback callback);

			void updateWindow();

		};
	}
}