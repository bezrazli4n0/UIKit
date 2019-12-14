#pragma once

namespace UIKit
{
	class App
	{
	private:
		bool appRunning{ true };

	public:
		App(const bool customDPI = false, const float customDpiX = 96.0f, const float customDpiY = 96.0f);
		~App();

		int runApp();
		void exitApp();

	};
}