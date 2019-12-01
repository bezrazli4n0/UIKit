#pragma once

#include <vector>
#include <string>

namespace UIKit
{
	namespace UI
	{
		class Window;
	}

	static std::vector<UI::Window*> appWindows{};
	void removeWindow(const std::wstring& windowID);

	class App
	{
	private:
		bool isRunning{ true };

	public:
		int runApp();
		void exitApp();

	};
}