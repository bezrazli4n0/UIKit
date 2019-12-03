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

			LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK WindowProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		public:
			Window();
			~Window();

			virtual void show(bool flag = true);
			HWND getHandle() const;
			void onClose(WidgetCallback callback);

			void updateWindow();

		};
	}
}