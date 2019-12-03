#include "UIWindow.h"
#include "UIConst.h"
#include "UIGraphicsHelper.hpp"
#include <algorithm>

namespace UIKit::UI
{
	bool Window::createWindow()
	{
		WNDCLASSEX wc{ sizeof(wc) };
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
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

		this->pRenderer = new Graphics::Renderer{ this->windowHandle };
		this->pRenderer->init();

		UpdateWindow(this->windowHandle);

		return true;
	}

	WidgetPoints Window::getCorrectedWindowSize()
	{
		RECT rc{ 0, 0, static_cast<LONG>(this->width), static_cast<LONG>(this->height) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

		return WidgetPoints { static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top) };
	}

	void Window::update()
	{
	}

	void Window::render()
	{
		this->pRenderer->getContext()->BeginDraw();
		this->pRenderer->getContext()->Clear(D2D1::ColorF(D2D1::ColorF::White));

		for (const auto& widget : this->windowWidgets)
		{
			if (widget->isVisible())
			{
				widget->update();
				widget->render();
			}
		}

		this->pRenderer->getContext()->EndDraw();
		this->pRenderer->getSwapChain()->Present(1, 0);
	}

	void Window::onMouseUp(const int& xPos, const int& yPos)
	{
		for (const auto& widget : this->windowWidgets)
		{
			if (widget->isVisible() && widget->isHandleMouse())
				widget->onMouseUp(xPos, yPos);
		}
	}

	void Window::onMouseDown(const int& xPos, const int& yPos)
	{
		for (const auto& widget : this->windowWidgets)
		{
			if (widget->isVisible() && widget->isHandleMouse())
				widget->onMouseDown(xPos, yPos);
		}
	}

	LRESULT Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_DISPLAYCHANGE:
			case WM_PAINT:
			{
				PAINTSTRUCT ps{};
				BeginPaint(hWnd, &ps);

				this->updateWindow();

				EndPaint(hWnd, &ps);
			}
			return 0;

			case WM_LBUTTONUP:
			{
				this->onMouseUp(static_cast<const int&>(Graphics::dipToPixelX(static_cast<float>(LOWORD(lParam)))), static_cast<const int&>(Graphics::dipToPixelY(static_cast<float>(HIWORD(lParam)))));
			}
			return 0;

			case WM_LBUTTONDOWN:
			{
				this->onMouseDown(static_cast<const int&>(Graphics::dipToPixelX(static_cast<float>(LOWORD(lParam)))), static_cast<const int&>(Graphics::dipToPixelY(static_cast<float>(HIWORD(lParam)))));
			}
			return 0;

			case WM_SIZE:
			{
				if (this->pRenderer != nullptr)
					this->pRenderer->resize();
			}
			return 0;

			case WM_DESTROY:
			{
				if (this->onCloseCallback != nullptr)
					this->onCloseCallback(this);
			}
			return 0;

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

	Window::Window(const std::wstring&& windowClass, const std::wstring&& windowTitle, const unsigned short&& width, const unsigned short&& height, const unsigned short&& x, const unsigned short&& y)
		: windowClass(windowClass), windowTitle(windowTitle), Widget(windowClass, width, height, x, y)
	{
		this->windowClass = this->widgetID;
		this->createWindow();
	}

	Window::Window(const std::wstring& windowClass, const std::wstring& windowTitle, const unsigned short& width, const unsigned short& height, const unsigned short& x, const unsigned short& y)
		: windowClass(windowClass), windowTitle(windowTitle), Widget(windowClass, width, height, x, y)
	{
		this->windowClass = this->widgetID;
		this->createWindow();
	}

	Window::~Window()
	{
		if (this->pRenderer != nullptr)
		{
			delete this->pRenderer;
			this->pRenderer = nullptr;
		}
	}

	const std::wstring& Window::getTitle() const
	{
		return this->windowTitle;
	}

	void Window::setTitle(const std::wstring&& windowTitle)
	{
		this->windowTitle = windowTitle;
		SetWindowText(this->windowHandle, this->windowTitle.c_str());
	}

	void Window::setTitle(const std::wstring& windowTitle)
	{
		this->windowTitle = windowTitle;
		SetWindowText(this->windowHandle, this->windowTitle.c_str());
	}

	void Window::setSizeInDIP(const WidgetPoints&& widgetSizeDIP)
	{
		auto [width, height] = widgetSizeDIP;
		this->width = width;
		this->height = height;

		auto [correctedWidth, correctedHeight] = this->getCorrectedWindowSize();
		SetWindowPos(this->windowHandle, nullptr, 0, 0, static_cast<int>(correctedWidth), static_cast<int>(correctedHeight), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);

	}

	void Window::setSizeInDIP(const WidgetPoints& widgetSizeDIP)
	{
		auto [width, height] = widgetSizeDIP;
		this->width = width;
		this->height = height;

		auto [correctedWidth, correctedHeight] = this->getCorrectedWindowSize();
		SetWindowPos(this->windowHandle, nullptr, 0, 0, static_cast<int>(correctedWidth), static_cast<int>(correctedHeight), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);
	}

	void Window::setSizeInPixel(const WidgetPoints&& widgetSize)
	{
		auto [width, height] = widgetSize;
		this->width = Graphics::pixelToDipX(width);
		this->height = Graphics::pixelToDipY(height);

		auto [correctedWidth, correctedHeight] = this->getCorrectedWindowSize();
		SetWindowPos(this->windowHandle, nullptr, 0, 0, static_cast<int>(correctedWidth), static_cast<int>(correctedHeight), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);
	}

	void Window::setSizeInPixel(const WidgetPoints& widgetSize)
	{
		auto [width, height] = widgetSize;
		this->width = Graphics::pixelToDipX(width);
		this->height = Graphics::pixelToDipY(height);

		auto [correctedWidth, correctedHeight] = this->getCorrectedWindowSize();
		SetWindowPos(this->windowHandle, nullptr, 0, 0, static_cast<int>(correctedWidth), static_cast<int>(correctedHeight), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);
	}

	void Window::setPosInDIP(const WidgetPoints&& widgetPosDIP)
	{
		auto [x, y] = widgetPosDIP;
		this->x = x;
		this->y = y;

		SetWindowPos(this->windowHandle, nullptr, static_cast<int>(this->x), static_cast<int>(this->y), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	void Window::setPosInDIP(const WidgetPoints& widgetPosDIP)
	{
		auto [x, y] = widgetPosDIP;
		this->x = x;
		this->y = y;

		SetWindowPos(this->windowHandle, nullptr, static_cast<int>(this->x), static_cast<int>(this->y), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	void Window::setPosInPixel(const WidgetPoints&& widgetPos)
	{
		auto [x, y] = widgetPos;
		this->x = Graphics::pixelToDipX(x);
		this->y = Graphics::pixelToDipY(y);

		SetWindowPos(this->windowHandle, nullptr, static_cast<int>(this->x), static_cast<int>(this->y), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	void Window::setPosInPixel(const WidgetPoints& widgetPos)
	{
		auto [x, y] = widgetPos;
		this->x = Graphics::pixelToDipX(x);
		this->y = Graphics::pixelToDipY(y);

		SetWindowPos(this->windowHandle, nullptr, static_cast<int>(this->x), static_cast<int>(this->y), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	void Window::addWidget(Widget* pWidget)
	{
		pWidget->pRT = this->pRenderer->getContext();
		this->windowWidgets.push_back(pWidget);
		pWidget->onAttach();
	}

	void Window::removeWidget(const std::wstring&& widgetID)
	{
		this->windowWidgets.erase(std::remove_if(std::begin(this->windowWidgets), std::end(this->windowWidgets), [&widgetID](Widget* pWidget) {
			if (pWidget->getWidgetID() == widgetID)
			{
				pWidget->onDetach();
				return true;
			}
			return false;
		}));
	}

	void Window::removeWidget(const std::wstring& widgetID)
	{
		this->windowWidgets.erase(std::remove_if(std::begin(this->windowWidgets), std::end(this->windowWidgets), [&widgetID](Widget* pWidget) {
			if (pWidget->getWidgetID() == widgetID)
			{
				pWidget->onDetach();
				return true;
			}
			return false;
		}));
	}

	void Window::show(bool flag)
	{
		this->visible = flag;
		flag ? ShowWindow(this->windowHandle, SW_SHOWDEFAULT) : ShowWindow(this->windowHandle, SW_HIDE);
	}

	HWND Window::getHandle() const
	{
		return this->windowHandle;
	}

	void Window::onClose(WidgetCallback callback)
	{
		this->onCloseCallback = callback;
	}

	void Window::updateWindow()
	{
		this->update();
		this->render();
	}
}
