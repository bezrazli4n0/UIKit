#pragma comment(lib, "dwmapi")

#include "UIWindow.h"
#include "UIGraphicsHelper.hpp"
#include "UILayout.h"
#include <algorithm>
#include <dwmapi.h>

namespace UIKit::UI
{
	bool Window::createWindow()
	{
		WNDCLASSEX wc{ sizeof(wc) };
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = nullptr;
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
			this->windowStyle,
			this->windowCenter ? (GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(correctedWidth)) / 2 : static_cast<int>(this->x),
			this->windowCenter ? (GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(correctedHeight)) / 2 : static_cast<int>(this->y),
			static_cast<int>(correctedWidth),
			static_cast<int>(correctedHeight),
			nullptr, nullptr, wc.hInstance, this
		);
			!this->windowHandle)
			return false;

		MARGINS margins{ 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(this->windowHandle, &margins);
		SetWindowPos(this->windowHandle, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		this->pRenderer = new Graphics::Renderer{ this->windowHandle };
		this->pRenderer->init();

		if (this->roundedWindow.roundedWindow)
		{
			this->hRoundedRgn = CreateRoundRectRgn(0, 0, static_cast<int>(correctedWidth), static_cast<int>(correctedHeight), this->roundedWindow.windowRadiusX, this->roundedWindow.windowRadiusY);
			SetWindowRgn(this->windowHandle, this->hRoundedRgn, false);
		}

		UpdateWindow(this->windowHandle);

		return true;
	}

	WidgetPoints Window::getCorrectedWindowSize()
	{
		if (this->windowFrameless)
			return this->getSizeInDIP();

		RECT rc{ 0, 0, static_cast<LONG>(this->width), static_cast<LONG>(this->height) };
		AdjustWindowRect(&rc, this->windowStyle, false);

		return WidgetPoints { static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top) };
	}

	void Window::sortWidgets()
	{
		if (this->windowLayout != nullptr)
			std::sort(this->windowLayout->widgetCollection.begin(), this->windowLayout->widgetCollection.end(), [](Layout::WidgetInfo lhs, Layout::WidgetInfo rhs) { return lhs.pWidget->getZIndex() < lhs.pWidget->getZIndex(); });
		else
			this->windowWidgets.sort([](Widget* lhs, Widget* rhs) { return lhs->getZIndex() < rhs->getZIndex(); });
	}

	bool Window::processTabKey()
	{
		bool testFlag{};
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
			{
				testFlag = widgetInfo.pWidget->isTabStop();
				if (testFlag)
					break;
			}
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
			{
				testFlag = widget->isTabStop();
				if (testFlag)
					break;
			}
		}
		if (!testFlag)
			return false;

		if (this->pTabWidget == nullptr)
		{
			if (this->windowLayout != nullptr)
			{
				for (auto it = this->windowLayout->widgetCollection.begin(); it != this->windowLayout->widgetCollection.end(); ++it)
				{
					auto widget = (*it).pWidget;

					if (widget->isTabStop() && !widget->isActive())
					{
						for (const auto& tabbedWidgets : this->windowLayout->widgetCollection)
							if (tabbedWidgets.pWidget->isTabStop())
								tabbedWidgets.pWidget->onTabStop(false);

						widget->onTabStop(true);
						this->pTabWidget = widget;
						return true;
					}
				}
			}
			else
			{
				bool activeFlag{};
				std::wstring activeId{};
				for (const auto& widget : this->windowWidgets)
				{
					if (widget->isActive())
					{
						activeFlag = true;
						activeId = widget->getWidgetID();
						break;
					}
				}

				if (!activeFlag)
				{
					for (auto it = this->windowWidgets.rbegin(); it != this->windowWidgets.rend(); ++it)
					{
						auto widget = (*it);

						if (widget->isTabStop() && !widget->isActive())
						{
							for (const auto& tabbedWidgets : this->windowWidgets)
								if (tabbedWidgets->isTabStop())
									tabbedWidgets->onTabStop(false);

							widget->onTabStop(true);
							this->pTabWidget = widget;
							return true;
						}
					}
				}
				else
				{
					auto idx{ -1 };
					auto it = std::find_if(this->windowWidgets.rbegin(), this->windowWidgets.rend(), [&](Widget* pWidget) { return pWidget->getWidgetID() == activeId; });
					for (const auto& tabbedWidgets : this->windowWidgets)
						if (tabbedWidgets->isTabStop())
							tabbedWidgets->onTabStop(false);

					while (true)
					{
						it = std::next(it);
						if (*it == nullptr)
							it = std::rbegin(this->windowWidgets);
						idx++;

						if (idx >= static_cast<int>(this->windowWidgets.size()))
						{
							idx = -1;
							it = std::rbegin(this->windowWidgets);
							if ((*it)->isTabStop())
								break;

							continue;
						}

						if ((*it)->isTabStop())
							break;
					}
					
					(*it)->onTabStop(true);
					this->pTabWidget = *it;
				}
			}
		}
		else
		{
			if (this->windowLayout != nullptr)
			{
				for (const auto& tabbedWidgets : this->windowLayout->widgetCollection)
					if (tabbedWidgets.pWidget->isTabStop())
						tabbedWidgets.pWidget->onTabStop(false);
			}
			else
			{
				for (const auto& tabbedWidgets : this->windowWidgets)
					if (tabbedWidgets->isTabStop())
						tabbedWidgets->onTabStop(false);
			}

			auto idx{ -1 };
			if (this->windowLayout != nullptr)
			{
				auto currentTabIt = std::find_if(std::begin(this->windowLayout->widgetCollection), std::end(this->windowLayout->widgetCollection), [&](Layout::WidgetInfo pWidgetInfo) {
					idx++;
					return pWidgetInfo.pWidget->getWidgetID() == this->pTabWidget->getWidgetID();
				});

				while (true)
				{
					currentTabIt = std::next(currentTabIt);
					idx++;

					if (idx >= static_cast<int>(this->windowLayout->widgetCollection.size()))
					{
						idx = -1;
						currentTabIt = std::begin(this->windowLayout->widgetCollection);
						if ((*currentTabIt).pWidget->isTabStop())
							break;

						continue;
					}

					if ((*currentTabIt).pWidget->isTabStop())
						break;
				}

				(*currentTabIt).pWidget->onTabStop(true);
				this->pTabWidget = (*currentTabIt).pWidget;
			}
			else
			{
				auto currentTabIt = std::find_if(std::rbegin(this->windowWidgets), std::rend(this->windowWidgets), [&](Widget* pWidget) {
					idx++;
					return pWidget->getWidgetID() == this->pTabWidget->getWidgetID();
					});

				while (true)
				{
					currentTabIt = std::next(currentTabIt);
					idx++;

					if (idx >= static_cast<int>(this->windowWidgets.size()))
					{
						idx = -1;
						currentTabIt = std::rbegin(this->windowWidgets);
						if ((*currentTabIt)->isTabStop())
							break;

						continue;
					}

					if ((*currentTabIt)->isTabStop())
						break;
				}

				(*currentTabIt)->onTabStop(true);
				this->pTabWidget = *currentTabIt;
			}
			
			return true;
		}

		return false;
	}

	void Window::update()
	{
	}

	void Window::render()
	{
		this->pRenderer->getContext()->BeginDraw();
		this->pRenderer->getContext()->Clear(this->windowBackgroundColor);

		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
			{
				if (widgetInfo.pWidget->isVisible())
				{
					widgetInfo.pWidget->update();
					widgetInfo.pWidget->render();
				}
			}
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
			{
				if (widget->isVisible())
				{
					widget->update();
					widget->render();
				}
			}
		}

		this->pRenderer->getContext()->EndDraw();
		this->pRenderer->getSwapChain()->Present(1, 0);
	}

	void Window::onMouseUp(const int& xPos, const int& yPos)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
				if (widgetInfo.pWidget->isVisible() && widgetInfo.pWidget->isHandleMouse())
					widgetInfo.pWidget->onMouseUp(xPos, yPos);
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
				if (widget->isVisible() && widget->isHandleMouse())
					widget->onMouseUp(xPos, yPos);
		}
	}

	void Window::onMouseDown(const int& xPos, const int& yPos)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
			{
				if (this->pTabWidget != nullptr)
					if (widgetInfo.pWidget->isTabStop())
						widgetInfo.pWidget->onTabStop(false);

				if (widgetInfo.pWidget->isVisible() && widgetInfo.pWidget->isHandleMouse())
					widgetInfo.pWidget->onMouseDown(xPos, yPos);
			}
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
			{
				if (this->pTabWidget != nullptr)
					if (widget->isTabStop())
						widget->onTabStop(false);

				if (widget->isVisible() && widget->isHandleMouse())
					widget->onMouseDown(xPos, yPos);
			}
		}

		this->pTabWidget = nullptr;
	}

	void Window::onMouseMove(const int& xPos, const int& yPos)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
				if (widgetInfo.pWidget->isVisible() && widgetInfo.pWidget->isHandleMouse())
					widgetInfo.pWidget->onMouseMove(xPos, yPos);
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
				if (widget->isVisible() && widget->isHandleMouse())
					widget->onMouseMove(xPos, yPos);
		}
	}

	void Window::onMouseScroll(const int& xPos, const int& yPos, const short& delta)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
				if (widgetInfo.pWidget->isVisible() && widgetInfo.pWidget->isHandleMouse())
					widgetInfo.pWidget->onMouseScroll(xPos, yPos, delta);
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
				if (widget->isVisible() && widget->isHandleMouse())
					widget->onMouseScroll(xPos, yPos, delta);
		}
	}

	void Window::onChar(UINT32 c)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
				if (widgetInfo.pWidget->isVisible() && widgetInfo.pWidget->isHandleKeyboard() && !(widgetInfo.pWidget->isTabStop() && c == VK_TAB))
					widgetInfo.pWidget->onChar(c);
		}
		else
		{
			for (auto& widget : this->windowWidgets)
				if (widget->isVisible() && widget->isHandleKeyboard() && !(widget->isTabStop() && c == VK_TAB))
					widget->onChar(c);
		}
	}

	void Window::onKey(UINT32 vk)
	{
		if (vk == VK_TAB)
		{
			if (this->processTabKey())
				return;
		}

		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
				if (widgetInfo.pWidget->isVisible() && widgetInfo.pWidget->isHandleKeyboard())
					widgetInfo.pWidget->onKey(vk);
		}
		else
		{
			for (auto& widget : this->windowWidgets)
				if (widget->isVisible() && widget->isHandleKeyboard())
					widget->onKey(vk);
		}
	}

	bool Window::updateCursor()
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
				if (widgetInfo.pWidget->isVisible() && widgetInfo.pWidget->isHandleMouse())
					if (widgetInfo.pWidget->updateCursor())
						return true;
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
				if (widget->isVisible() && widget->isHandleMouse())
					if (widget->updateCursor())
						return true;
		}

		return false;
	}

	LRESULT Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_DISPLAYCHANGE:
			case WM_PAINT:
			{
				PAINTSTRUCT ps{};
				BeginPaint(this->windowHandle, &ps);

				this->updateWindow();

				EndPaint(this->windowHandle, &ps);
			}
			return 0;

			case WM_LBUTTONUP:
			{
				this->onMouseUp(static_cast<const int&>(LOWORD(lParam)), static_cast<const int&>(HIWORD(lParam)));
			}
			return 0;

			case WM_MOUSEWHEEL:
			{
				this->onMouseScroll(static_cast<const int&>(LOWORD(lParam)), static_cast<const int&>(HIWORD(lParam)), static_cast<short>(HIWORD(wParam)));
			}
			return 0;

			case WM_CHAR:
			{
				this->onChar(wParam);
			}
			return 0;

			case WM_KEYDOWN:
			{
				this->onKey(wParam);
			}
			return 0;

			case WM_SETCURSOR:
			{
				auto ret = this->updateCursor();
				if (!ret)
					break;
				return ret;
			}
			return 0;

			case WM_NCLBUTTONDBLCLK:
			case WM_NCLBUTTONDOWN:
			{
				POINT pt{ LOWORD(lParam), HIWORD(lParam) };
				ScreenToClient(hWnd, &pt);
				this->onMouseDown(static_cast<const int&>(pt.x), static_cast<const int&>(pt.y));
			}
			break;

			case WM_NCLBUTTONUP:
			{
				POINT pt{ LOWORD(lParam), HIWORD(lParam) };
				ScreenToClient(hWnd, &pt);
				this->onMouseUp(static_cast<const int&>(pt.x), static_cast<const int&>(pt.y));
			}
			break;

			case WM_NCHITTEST:
			{
				if (this->windowFrameless)
				{
					// Returning HTCAPTION allows the user to move the window around by clicking 
					// anywhere.
					// Depending on the mouse coordinates passed in LPARAM, you may 
					// return other values to enable resizing.
					//SetWindowLong(hWnd, DWL_MSGRESULT, HTCAPTION);

					POINT pt{ LOWORD(lParam), HIWORD(lParam) };
					ScreenToClient(hWnd, &pt);
					if (pt.y <= this->titleBarArea)
						return HTCAPTION;
				}
			}
			break;

			case WM_NCCALCSIZE:
			{
				if (this->windowFrameless)
				{
					// Returning 0 from the message when wParam is TRUE removes the standard
					// frame, but keeps the window shadow.
					if (wParam == TRUE)
					{
						SetWindowLong(hWnd, 0, 0);
						return TRUE;
					}
					return FALSE;
				}
			}
			break;

			case WM_MOUSEMOVE:
			{
				this->onMouseMove(static_cast<const int&>(LOWORD(lParam)), static_cast<const int&>(HIWORD(lParam)));
			}
			return 0;

			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
			{
				this->onMouseDown(static_cast<const int&>(LOWORD(lParam)), static_cast<const int&>(HIWORD(lParam)));
			}
			return 0;

			case WM_MOVE:
			{
				this->x = LOWORD(lParam);
				this->y = HIWORD(lParam);
			}
			return 0;

			case WM_SIZE:
			{
				this->width = LOWORD(lParam);
				this->height = HIWORD(lParam);

				if (this->pRenderer != nullptr)
					this->pRenderer->resize();

				if (this->windowLayout != nullptr)
				{
					this->windowLayout->width = this->width;
					this->windowLayout->height = this->height;
					this->windowLayout->calculateLayout();
				}
			}
			return 0;

			case WM_DESTROY:
			{
				if (this->onCloseCallback != nullptr)
					this->onCloseCallback(this);
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

	Window::Window(const std::wstring&& windowClass, const std::wstring&& windowTitle, windowOptions* pWindowOptions, const unsigned short&& width, const unsigned short&& height, const unsigned short&& x, const unsigned short&& y)
		: windowClass(windowClass), windowTitle(windowTitle), Widget(windowClass, width, height, x, y)
	{
		if (pWindowOptions != nullptr)
		{
			this->windowCenter = pWindowOptions->centeredWindow;
			this->windowFrameless = pWindowOptions->framelessWindow;
			this->roundedWindow.roundedWindow = pWindowOptions->roundedWindow.roundedWindow;
			this->roundedWindow.windowRadiusX = pWindowOptions->roundedWindow.windowRadiusX;
			this->roundedWindow.windowRadiusY = pWindowOptions->roundedWindow.windowRadiusY;
		}

		if (this->windowFrameless)
			this->windowStyle = WS_POPUP | WS_CAPTION | DS_CENTER;

		this->windowClass = this->widgetID;
		this->createWindow();
		uiWindows.push_back(this);
	}

	Window::Window(const std::wstring& windowClass, const std::wstring& windowTitle, windowOptions* pWindowOptions, const unsigned short& width, const unsigned short& height, const unsigned short& x, const unsigned short& y)
		: windowClass(windowClass), windowTitle(windowTitle), Widget(windowClass, width, height, x, y)
	{
		if (pWindowOptions != nullptr)
		{
			this->windowCenter = pWindowOptions->centeredWindow;
			this->windowFrameless = pWindowOptions->framelessWindow;
			this->roundedWindow.roundedWindow = pWindowOptions->roundedWindow.roundedWindow;
			this->roundedWindow.windowRadiusX = pWindowOptions->roundedWindow.windowRadiusX;
			this->roundedWindow.windowRadiusY = pWindowOptions->roundedWindow.windowRadiusY;
		}

		if (this->windowFrameless)
			this->windowStyle = WS_POPUP | WS_CAPTION | DS_CENTER;

		this->windowClass = this->widgetID;
		this->createWindow();
		uiWindows.push_back(this);
	}

	Window::~Window()
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widget : this->windowLayout->widgetCollection)
				widget.pWidget->onDetach();

			this->windowLayout->widgetCollection.clear();
			this->windowLayout = nullptr;
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
				widget->onDetach();

			this->windowWidgets.clear();
		}

		UnregisterClass(this->windowClass.c_str(), nullptr);

		if (this->roundedWindow.roundedWindow)
			DeleteObject(this->hRoundedRgn);

		if (this->pRenderer != nullptr)
		{
			delete this->pRenderer;
			this->pRenderer = nullptr;
		}

		uiWindows.erase(std::remove_if(std::begin(uiWindows), std::end(uiWindows), [&](Window* pWindow) {
			return pWindow->getWidgetID() == this->getWidgetID();
		}), std::end(uiWindows));
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

	void Window::setBackgroundColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a)
	{
		this->windowBackgroundColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void Window::setBackgroundColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		this->windowBackgroundColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	std::vector<Window*>* Window::getWindows()
	{
		return &uiWindows;
	}

	void Window::addWidget(Widget* pWidget)
	{
		if (this->windowLayout == nullptr)
		{
			pWidget->pRT = this->pRenderer->getContext();
			this->windowWidgets.push_back(pWidget);
			pWidget->onAttach();
		}
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

	void Window::setZIndex(const std::wstring&& widgetID, const int&& z)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
			{
				if (widgetInfo.pWidget->getWidgetID() == widgetID)
				{
					widgetInfo.pWidget->zIndex = z;
					this->sortWidgets();
					break;
				}
			}
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
			{
				if (widget->getWidgetID() == widgetID)
				{
					widget->zIndex = z;
					this->sortWidgets();
					break;
				}
			}
		}
	}

	void Window::setZIndex(const std::wstring& widgetID, const int& z)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
			{
				if (widgetInfo.pWidget->getWidgetID() == widgetID)
				{
					widgetInfo.pWidget->zIndex = z;
					this->sortWidgets();
					break;
				}
			}
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
			{
				if (widget->getWidgetID() == widgetID)
				{
					widget->zIndex = z;
					this->sortWidgets();
					break;
				}
			}
		}
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

	void Window::setTitleBarArea(const int&& areaY)
	{
		this->titleBarArea = areaY;
	}

	void Window::setTitleBarArea(const int& areaY)
	{
		this->titleBarArea = areaY;
	}

	Widget* Window::getWidget(const std::wstring&& widgetID)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
			{
				if (widgetInfo.pWidget->getWidgetID() == widgetID)
					return widgetInfo.pWidget;
			}
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
			{
				if (widget->getWidgetID() == widgetID)
					return widget;
			}
		}

		return nullptr;
	}

	Widget* Window::getWidget(const std::wstring& widgetID)
	{
		if (this->windowLayout != nullptr)
		{
			for (const auto& widgetInfo : this->windowLayout->widgetCollection)
			{
				if (widgetInfo.pWidget->getWidgetID() == widgetID)
					return widgetInfo.pWidget;
			}
		}
		else
		{
			for (const auto& widget : this->windowWidgets)
			{
				if (widget->getWidgetID() == widgetID)
					return widget;
			}
		}

		return nullptr;
	}

	void Window::setLayout(Layout* pLayout)
	{
		if (pLayout != nullptr)
		{
			for (const auto& widgetInfo : pLayout->widgetCollection)
			{
				widgetInfo.pWidget->pRT = this->pRenderer->getContext();
				widgetInfo.pWidget->onAttach();
			}

			pLayout->width = this->width;
			pLayout->height = this->height;
			pLayout->calculateLayout();

			this->windowLayout = pLayout;
		}
	}

	void Window::minimize()
	{
		ShowWindow(this->windowHandle, SW_MINIMIZE);
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
