#pragma once

namespace UIKit
{
	class App
	{
	private:
		bool appRunning{ true };

	public:
		int runApp();
		void exitApp();

	};
}