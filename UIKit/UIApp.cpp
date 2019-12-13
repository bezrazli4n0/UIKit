#include "UIApp.h"
#include "UIWindow.h"
#include "UIGraphics.h"

#include <Windows.h>

namespace UIKit
{
	App::App()
	{
		Graphics::Core::initCore();
	}

	App::~App()
	{
		Graphics::Core::freeCore();
	}

	int App::runApp()
	{
		MSG msg{};

		DWORD uiNextCall{};
		const DWORD uiFpsLock{ 1000 / 60 };

		while (this->appRunning)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// Main-loop
				DWORD ms = GetTickCount();
				DWORD msNext = uiNextCall;
				LONG  lWait{};
				DWORD dwRet{ WAIT_TIMEOUT };

				if (ms < msNext)
					lWait = min(static_cast<LONG>(uiFpsLock), static_cast<LONG>(msNext - ms));

				if (lWait <= 1)
				{
					uiNextCall = ms + uiFpsLock;
					for (auto& window : *UI::Window::getWindows())
						window->updateWindow();
				}
				else
				{
					if (MsgWaitForMultipleObjects(0, NULL, FALSE, lWait, QS_ALLEVENTS) == WAIT_TIMEOUT)
					{
						uiNextCall = GetTickCount() + uiFpsLock;
						for (auto& window : *UI::Window::getWindows())
							window->updateWindow();
					}
				}
			}
		}

		return static_cast<int>(msg.wParam);
	}

	void App::exitApp()
	{
		this->appRunning = false;
	}
}

