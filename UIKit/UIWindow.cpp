#include "UIWindow.h"
#include "UIConst.h"

namespace UIKit::UI
{
	bool Window::createWindow()
	{
		WNDCLASSEX wc{ sizeof(wc) };
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hInstance = nullptr;
		wc.lpfnWndProc = WindowProc_;
		wc.lpszClassName = this->windowClass.c_str();
		wc.lpszMenuName = nullptr;
		wc.style = CS_VREDRAW | CS_HREDRAW;

		if (!RegisterClassEx(&wc))
			return false;

		auto [correctedWidth, correctedHeight] = this->getCorrectedWindowSize();

		if (this->windowHandle = CreateWindowEx(0, this->windowClass.c_str(), this->windowTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			static_cast<int>(this->x),
			static_cast<int>(this->y),
			static_cast<int>(correctedWidth),
			static_cast<int>(correctedHeight),
			nullptr, nullptr, wc.hInstance, this
		);
			!this->windowHandle)
			return false;

		UpdateWindow(this->windowHandle);

		return true;
	}

	WidgetPoints Window::getCorrectedWindowSize()
	{
		RECT rc{ 0, 0, static_cast<LONG>(this->width), static_cast<LONG>(this->height) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

		return WidgetPoints { static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top) };
	}

	LRESULT Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_UPDATE_AND_RENDER:
			{
				this->updateWindow();
			}
			return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT Window::WindowProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window* pWindow{};

		if (uMsg == WM_NCCREATE)
		{
			pWindow = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

			SetLastError(0);
			if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow)))
				if (GetLastError() != 0)
					return false;
		}
		else
			pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (pWindow)
		{
			pWindow->windowHandle = hWnd;
			return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	Window::Window()
		: Widget(L"Window", 640.0f, 480.0f)
	{
		this->windowClass = this->widgetID;
		this->createWindow();
	}

	Window::~Window()
	{
	}

	void Window::show(bool flag)
	{
		flag ? ShowWindow(this->windowHandle, SW_SHOWDEFAULT) : ShowWindow(this->windowHandle, SW_HIDE);
	}

	HWND Window::getHandle() const
	{
		return this->windowHandle;
	}

	void Window::updateWindow()
	{
		this->update();
		this->render();
	}
}
