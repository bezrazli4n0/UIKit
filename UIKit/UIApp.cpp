#include "UIApp.h"
#include "UIWindow.h"
#include <Windows.h>
#include <algorithm>

namespace UIKit
{
	int App::runApp()
	{
		MSG msg{};
		while (this->isRunning)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// Update window state
			for (const auto& window : appWindows)
			{
				//if (msg.hwnd == window->getHandle())
					window->updateWindow();
			}
		}

		return static_cast<int>(msg.wParam);
	}

	void App::exitApp()
	{
		this->isRunning = false;
	}

	void removeWindow(const std::wstring& windowID)
	{
		using namespace std;
		remove_if(begin(appWindows), end(appWindows), [&](UI::Window* window) {
			return window->getWidgetID() == windowID;
		});
	}
}

