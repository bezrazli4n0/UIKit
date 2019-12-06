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
			enum class SelectMode {
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

			virtual void onChar(UINT32 c);
			virtual void onKey(UINT32 vk);

			virtual void onMouseUp(const int& xPos, const int& yPos);
			virtual void onMouseDown(const int& xPos, const int& yPos);
			virtual void onMouseMove(const int& xPos, const int& yPos);
			virtual void onMouseScroll(const short& delta);

			DWRITE_TEXT_RANGE getSelectionRange();
			void copyToClipboard();
			void deleteSelection();
			void pasteFromClipboard();
			void setSelectionFromPoint(float x, float y, bool moveAnchor);
			void select(SelectMode mode, bool moveAnchor = true);
			bool isUnicodeUnit(wchar_t char1, wchar_t char2);
			bool IsHighSurrogate(UINT32 ch);
			bool IsLowSurrogate(UINT32 ch);
			IDWriteTextLayout* createTextLayout(const std::wstring& text);

			void fillSelectedRange();
			void drawCaret();
			void drawText();

		private:
			std::wstring text{ L"" };
			UINT32 caretAnchor{}, caretPosition{}, lastSelectLength{};
			IDWriteTextLayout* textLayout{};
			float maxScrollY{}, scrollY{}, lastInputTime{ -1.0f }, lastClickTime{ -1.0f };
			ID2D1SolidColorBrush* brush{};
			bool isOnScroll{}, needUpdate{ true };
			D2D1_MATRIX_3X2_F viewMatrix{};

		public:
			TextBox();
			~TextBox();

		};
	}
}