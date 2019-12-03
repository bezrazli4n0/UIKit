#pragma once

namespace UIKit
{
	class App
	{
	private:
		bool appRunning{ true };

	public:
		App();
		~App();

		int runApp();
		void exitApp();

	};
}