#pragma once
#pragma comment(lib, "Winmm")

#include "UIWidget.h"
#include <dwrite.h>

namespace UIKit
{
	namespace UI
	{
		class TextBox : public Widget
		{
		private:
			enum class SelectMode
			{
				head, tile,
				lastChar, nextChar,
				lastWord, nextWord,
				absoluteLeading, absoluteTrailing,
				lastLine, nextLine,
				all,
				up, down,
			};

		private:
			virtual void update();
			virtual void render();

			virtual void onAttach();
			virtual void onDetach();

			DWRITE_TEXT_RANGE getSelectionRange();
			void copyToClipboard();
			void deleteSelection();
			void pasteFromClipboard();
			void setSelectionFromPoint(float x, float y, bool moveAnchor);
			void select(SelectMode mode, bool moveAnchor = true);
			bool isUnicodeUnit(wchar_t char1, wchar_t char2);
			bool IsHighSurrogate(UINT32 ch);
			bool IsLowSurrogate(UINT32 ch);
			void createTextLayout();
			bool checkMouse(const float& xPos, const float& yPos);

			void fillSelectedRange();
			void drawCaret();
			void drawText();

		private:
			std::wstring text{ L"" }, passwordText{ L"" };
			const std::wstring passwordChar{ L"•" };
			UINT32 caretAnchor{}, caretPosition{}, lastSelectLength{};
			IDWriteTextLayout* pTextLayout{};
			float maxScrollY{}, scrollY{}, lastInputTime{ -1.0f }, lastClickTime{ -1.0f }, fontSize{}, scrollX{}, expandedMouseAreaX{};
			ID2D1SolidColorBrush* pBrush{};
			bool isOnScroll{ }, needUpdate{ true }, multiline{ }, readOnly{ }, mouseHover{ }, mousePressed{ }, password{ };
			ID2D1RoundedRectangleGeometry* pRoundRectGeometry{};
			D2D1_COLOR_F textColor{ D2D1::ColorF(D2D1::ColorF::Black) };
			D2D1_COLOR_F caretColor{ D2D1::ColorF(D2D1::ColorF::Black) };
			static inline HCURSOR hBeamCursor{};

		public:
			TextBox(const std::wstring&& textBoxID, const std::wstring&& text = L"TextBox", const bool&& isMultiline = false, const bool&& isReadOnly = false, const bool&& isPassword = false, const float&& fontSize = 12.0f, const float&& width = 100.0f, const float&& height = 100.0f, const float&& x = 10.0f, const float&& y = 10.0f);
			TextBox(const std::wstring& textBoxID, const std::wstring& text = L"TextBox", const bool& isMultiline = false, const bool& isReadOnly = false, const bool& isPassword = false, const float& fontSize = 12.0f, const float& width = 100.0f, const float& height = 100.0f, const float& x = 10.0f, const float& y = 10.0f);
			~TextBox();

			void setTextColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a = 255);
			void setTextColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a = 255);

			void setCaretColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a = 255);
			void setCaretColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a = 255);

			virtual void onChar(UINT32 c);
			virtual void onKey(UINT32 vk);

			virtual void onMouseUp(const int& xPos, const int& yPos);
			virtual void onMouseDown(const int& xPos, const int& yPos);
			virtual void onMouseMove(const int& xPos, const int& yPos);
			virtual void onMouseScroll(const int& xPos, const int& yPos, const short& delta);

			virtual bool updateCursor();

			std::wstring getText() const;

			void setText(const std::wstring&& text);
			void setText(const std::wstring& text);

			void draw();
			void setRT(ID2D1DeviceContext* pRT);

			void expandMouseArea(const float&& areaX);
			void expandMouseArea(const float& areaX);

		};
	}
}