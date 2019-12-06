#include "UITextBox.h"
#include "UIGraphics.h"

namespace UIKit::UI
{
	void TextBox::update()
	{
		if (needUpdate)
		{
			IDWriteTextLayout* temp = createTextLayout(text);
			if (temp) {
				needUpdate = false;
				Graphics::SafeRelease(&textLayout);//释放上一个文本布局
				textLayout = temp;
			}
			//获取文本区域的宽高
			DWRITE_TEXT_METRICS metrics;
			textLayout->GetMetrics(&metrics);

			//修改更新后的滚动值
			maxScrollY = max(metrics.height - height, 0);
			if (scrollY > maxScrollY)
				scrollY = maxScrollY;
		}
	}

	void TextBox::render()
	{
		//获取文本区域的宽高
		DWRITE_TEXT_METRICS metrics;
		textLayout->GetMetrics(&metrics);

		//修改更新后的滚动值
		maxScrollY = max(metrics.height - height, 0);
		if (scrollY > maxScrollY)
			scrollY = maxScrollY;

		if (this->textLayout)
		{
			brush->SetColor(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
			this->pRT->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(this->x - 5.0f + 0.5f, this->y - 5.0f + 0.5f, this->x + this->width + 5.0f + 0.5f, this->y + this->height + 5.0f + 0.5f), 6.0f, 6.0f), this->brush);

			ID2D1Layer* pLayer{};
			this->pRT->CreateLayer(D2D1::SizeF(this->width, this->height), &pLayer);
			this->pRT->PushLayer(D2D1::LayerParameters(D2D1::RectF(this->x, this->y, this->x + this->width, this->y + this->height)), pLayer);

			this->fillSelectedRange();
			this->drawCaret();
			this->drawText();

			this->pRT->PopLayer();
			Graphics::SafeRelease(&pLayer);
		}

	}

	void TextBox::onAttach()
	{
		this->pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &this->brush);
	}

	void TextBox::onDetach()
	{
	}

	void TextBox::onChar(UINT32 c)
	{
		if (c >= 0x20 || c == 9)
		{
			deleteSelection();

			UINT32 charsLength = 1;
			wchar_t chars[2] = { static_cast<wchar_t>(c), 0 };

			if (c > 0xFFFF)
			{
				chars[0] = wchar_t(0xD800 + (c >> 10) - (0x10000 >> 10));
				chars[1] = wchar_t(0xDC00 + (c & 0x3FF));
				charsLength++;
			}

			text.insert(caretPosition, chars, charsLength);

			caretPosition += charsLength;
			caretAnchor = caretPosition;

			needUpdate = true;
			isOnScroll = false;

			lastInputTime = timeGetTime() / 1000.f;
		}
	}

	void TextBox::onKey(UINT32 vk)
	{
		bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
		bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
		bool heldAlt = (GetKeyState(VK_MENU) & 0x80) != 0;

		switch (vk)
		{
		case VK_RETURN:
			deleteSelection();
			wchar_t chars[3];
			chars[0] = '\n';
			chars[1] = 0;
			text.insert(caretPosition, chars, 1);

			caretPosition += 1;
			caretAnchor = caretPosition;

			needUpdate = true;
			break;
		case VK_BACK:

			if (caretPosition != caretAnchor)
			{
				deleteSelection();
			}
			else if (caretPosition > 0)
			{
				UINT32 count = 1;
				if (caretPosition >= 2
					&& caretPosition <= text.length())
				{
					wchar_t charBackOne = text[caretPosition - 1];
					wchar_t charBackTwo = text[caretPosition - 2];
					if ((IsLowSurrogate(charBackOne) && IsHighSurrogate(charBackTwo))
						|| (charBackOne == '\n' && charBackTwo == '\r'))
					{
						count = 2;
					}
				}

				caretPosition -= count;
				caretAnchor = caretPosition;

				text.erase(caretPosition, count);
				needUpdate = true;

			}
			break;
		case VK_DELETE:
			if (caretPosition != caretAnchor) {
				deleteSelection();
			}
			else {
				DWRITE_HIT_TEST_METRICS hitTestMetrics;
				float caretX, caretY;

				textLayout->HitTestTextPosition(
					caretPosition,
					false,
					&caretX,
					&caretY,
					&hitTestMetrics
				);

				text.erase(hitTestMetrics.textPosition, hitTestMetrics.length);
				needUpdate = true;

			}
			break;
		case VK_TAB:
			break;
		case VK_LEFT:
			if (!heldControl)
				select(SelectMode::lastChar, !heldShift);
			else
				select(SelectMode::lastWord, !heldShift);
			break;

		case VK_RIGHT:
			if (!heldControl)
				select(SelectMode::nextChar, !heldShift);
			else
				select(SelectMode::nextWord, !heldShift);
			break;
		case VK_UP:
			select(SelectMode::up);
			break;
		case VK_DOWN:
			select(SelectMode::down);
			break;
		case VK_HOME:
			select(SelectMode::head);
			break;
		case VK_END:
			select(SelectMode::tile);
			break;
		case 'C':
			if (heldControl)
				copyToClipboard();
			break;
		case VK_INSERT:
			if (heldControl)
				copyToClipboard();
			else if (heldShift) {
				pasteFromClipboard();
			}
			break;
		case 'V':
			if (heldControl) {
				pasteFromClipboard();
			}
			break;
		case 'X':
			//剪切文本，先复制再删除
			if (heldControl) {
				copyToClipboard();
				deleteSelection();
			}
			break;
		case 'A':
			if (heldControl) {
				select(SelectMode::all);
			}
			break;
		default:
			return;
		}
		isOnScroll = false;
		lastInputTime = timeGetTime() / 1000.f;
	}

	void TextBox::onMouseUp(const int& xPos, const int& yPos)
	{
		float time = timeGetTime() / 1000.f;
		const float doubleClickInterval = 0.3f;

		if (time - lastClickTime < doubleClickInterval) {
			if (lastSelectLength == 0)
				select(SelectMode::all);
		}
		lastClickTime = time;
	}

	void TextBox::onMouseDown(const int& xPos, const int& yPos)
	{
		//float xP{ xPos }, yPos{ yPos };
		isOnScroll = false;
		lastSelectLength = getSelectionRange().length;
		setSelectionFromPoint(static_cast<float>(xPos), static_cast<float>(yPos + scrollY), (GetKeyState(VK_SHIFT) & 0x80) == 0);
	}

	void TextBox::onMouseMove(const int& xPos, const int& yPos)
	{
		if ((GetKeyState(MK_LBUTTON) & 0x100) != 0)
			setSelectionFromPoint(static_cast<float>(xPos), static_cast<float>(yPos + scrollY), false);
	}

	void TextBox::onMouseScroll(const short& delta)
	{
		isOnScroll = true;

		//滚动事件发生不意味着滚动值改变

		float nextScroll = scrollY - delta;

		if (nextScroll < 0)
			nextScroll = 0;
		else if (nextScroll > maxScrollY)
			nextScroll = maxScrollY;

		if (nextScroll != scrollY) {
			scrollY = nextScroll;
			needUpdate = true;
		}
	}

	DWRITE_TEXT_RANGE TextBox::getSelectionRange()
	{
		UINT32 caretBegin = this->caretAnchor;
		UINT32 caretEnd = this->caretPosition;
		if (caretBegin > caretEnd)
			std::swap(caretBegin, caretEnd);

		UINT32 textLength = static_cast<UINT32>(this->text.length());

		if (caretBegin > textLength)
			caretBegin = textLength;

		if (caretEnd > textLength)
			caretEnd = textLength;

		return { caretBegin,caretEnd - caretBegin };
	}

	void TextBox::copyToClipboard()
	{
		DWRITE_TEXT_RANGE selectionRange = getSelectionRange();
		if (selectionRange.length <= 0)
			return;

		if (OpenClipboard(0)) {
			if (EmptyClipboard()) {

				size_t byteSize = sizeof(wchar_t) * (selectionRange.length + 1);
				HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, byteSize);

				if (hClipboardData != NULL) {
					void* memory = GlobalLock(hClipboardData);

					if (memory != NULL) {
						const wchar_t* ctext = this->text.c_str();
						memcpy(memory, &ctext[selectionRange.startPosition], byteSize);
						GlobalUnlock(hClipboardData);

						if (SetClipboardData(CF_UNICODETEXT, hClipboardData) != NULL) {
							hClipboardData = NULL;
						}
					}
					GlobalFree(hClipboardData);
				}
			}
			CloseClipboard();
		}
	}

	void TextBox::deleteSelection()
	{
		DWRITE_TEXT_RANGE range = getSelectionRange();

		if (range.length <= 0)
			return;

		this->text.erase(range.startPosition, range.length);

		this->caretPosition = range.startPosition;
		this->caretAnchor = caretPosition;

		needUpdate = true;
	}

	void TextBox::pasteFromClipboard()
	{
		this->deleteSelection();

		UINT32 characterCount = 0;

		if (OpenClipboard(0)) {
			HGLOBAL hClipboardData = GetClipboardData(CF_UNICODETEXT);

			if (hClipboardData != NULL)
			{
				// Get text and size of text.
				size_t byteSize = GlobalSize(hClipboardData);
				void* memory = GlobalLock(hClipboardData); // [byteSize] in bytes
				const wchar_t* ctext = reinterpret_cast<const wchar_t*>(memory);
				characterCount = static_cast<UINT32>(wcsnlen(ctext, byteSize / sizeof(wchar_t)));

				if (memory != NULL)
				{
					// Insert the text at the current position.
					this->text.insert(
						caretPosition,
						ctext,
						characterCount
					);

					GlobalUnlock(hClipboardData);

				}
			}
			CloseClipboard();
		}

		this->caretPosition += characterCount;
		this->caretAnchor = caretPosition;

		needUpdate = true;
	}

	void TextBox::setSelectionFromPoint(float x, float y, bool moveAnchor)
	{
		BOOL isTrailingHit;
		BOOL isInside;
		DWRITE_HIT_TEST_METRICS caretMetrics;

		x -= this->x;
		y -= this->y;

		this->textLayout->HitTestPoint(
			x, y,
			&isTrailingHit,
			&isInside,
			&caretMetrics
		);

		if (isTrailingHit)
			this->caretPosition = caretMetrics.textPosition + caretMetrics.length;

		else
			this->caretPosition = caretMetrics.textPosition;

		if (moveAnchor)
			this->caretAnchor = caretPosition;
	}

	void TextBox::select(SelectMode mode, bool moveAnchor)
	{
		switch (mode)
		{
		case SelectMode::up:
		case SelectMode::down:
		{
			std::vector<DWRITE_LINE_METRICS> lineMetrics;
			DWRITE_TEXT_METRICS textMetrics;
			textLayout->GetMetrics(&textMetrics);

			lineMetrics.resize(textMetrics.lineCount);
			textLayout->GetLineMetrics(&lineMetrics.front(), textMetrics.lineCount, &textMetrics.lineCount);

			UINT32 line = 0;
			UINT32 linePosition = 0;
			UINT32 nextLinePosition = 0;
			UINT32 lineCount = static_cast<UINT32>(lineMetrics.size());
			for (; line < lineCount; ++line)
			{
				linePosition = nextLinePosition;
				nextLinePosition = linePosition + lineMetrics[line].length;
				if (nextLinePosition > caretPosition) {
					break;
				}
			}

			if (line > lineCount - 1) {
				line = lineCount - 1;
			}

			if (mode == SelectMode::up)
			{
				if (line <= 0)
					break;
				line--;
				linePosition -= lineMetrics[line].length;
			}
			else
			{
				linePosition += lineMetrics[line].length;
				line++;
				if (line >= lineMetrics.size())
					break;
			}

			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			float caretX, caretY, dummyX;

			textLayout->HitTestTextPosition(
				caretPosition,
				false,
				&caretX,
				&caretY,
				&hitTestMetrics
			);

			textLayout->HitTestTextPosition(
				linePosition,
				false,
				&dummyX,
				&caretY,
				&hitTestMetrics
			);

			BOOL isInside, isTrailingHit;
			textLayout->HitTestPoint(
				caretX,
				caretY,
				&isTrailingHit,
				&isInside,
				&hitTestMetrics
			);

			caretPosition = hitTestMetrics.textPosition;

			if (isTrailingHit) {
				caretPosition += hitTestMetrics.length;
			}
			break;
		}
		case SelectMode::head:
			caretPosition = 0;
			break;
		case SelectMode::tile:
			caretPosition = text.length();
			break;
		case SelectMode::lastChar:
			if (caretPosition > 0) {
				UINT32 moveCount = 1;

				if (caretPosition >= 2
					&& caretPosition <= text.length())
				{
					if (isUnicodeUnit(text[caretPosition - 1], text[caretPosition - 2]))
					{
						moveCount = 2;
					}
				}
				if (caretPosition < (UINT32)moveCount)
					caretPosition = 0;
				else caretPosition -= moveCount;
			}
			break;
		case SelectMode::nextChar:
			if (caretPosition < text.length()) {
				UINT32 moveCount = 1;
				if (caretPosition >= 0
					&& caretPosition <= text.length() - 2)
				{
					wchar_t charBackOne = text[caretPosition];
					wchar_t charBackTwo = text[caretPosition + 1];
					if (isUnicodeUnit(text[caretPosition], text[caretPosition + 1]))
					{
						moveCount = 2;
					}
				}
				if (caretPosition > text.length())
					caretPosition = text.length();
				else caretPosition += moveCount;
			}
			break;
		case SelectMode::lastWord:
		case SelectMode::nextWord: {
			std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics;
			UINT32 clusterCount;
			textLayout->GetClusterMetrics(NULL, 0, &clusterCount);
			if (clusterCount == 0)
				break;

			clusterMetrics.resize(clusterCount);
			textLayout->GetClusterMetrics(&clusterMetrics.front(), clusterCount, &clusterCount);

			UINT32 clusterPosition = 0;
			UINT32 oldCaretPosition = caretPosition;

			if (mode == SelectMode::lastWord) {

				caretPosition = 0;
				for (UINT32 cluster = 0; cluster < clusterCount; ++cluster) {

					clusterPosition += clusterMetrics[cluster].length;
					if (clusterMetrics[cluster].canWrapLineAfter) {
						if (clusterPosition >= oldCaretPosition)
							break;

						caretPosition = clusterPosition;
					}

				}

			}
			else {
				for (UINT32 cluster = 0; cluster < clusterCount; ++cluster) {
					UINT32 clusterLength = clusterMetrics[cluster].length;

					if (clusterPosition + clusterMetrics[cluster].length > oldCaretPosition&& clusterMetrics[cluster].canWrapLineAfter) {
						caretPosition = clusterPosition + clusterMetrics[cluster].length;
						break;

					}
					clusterPosition += clusterLength;
					caretPosition = clusterPosition;
				}
			}
			break;
		}
		case SelectMode::absoluteLeading: {
			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			float caretX, caretY;

			textLayout->HitTestTextPosition(
				caretPosition,
				false,
				&caretX,
				&caretY,
				&hitTestMetrics
			);

			caretPosition = hitTestMetrics.textPosition;

			break;
		}
		case SelectMode::absoluteTrailing: {
			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			float caretX, caretY;

			textLayout->HitTestTextPosition(
				caretPosition,
				true,
				&caretX,
				&caretY,
				&hitTestMetrics
			);

			caretPosition = hitTestMetrics.textPosition + hitTestMetrics.length;
			break;
		}
		case SelectMode::all:
			caretAnchor = 0;
			caretPosition = text.length();
			return;
		default:
			break;
		}

		if (moveAnchor)
			this->caretAnchor = caretPosition;
	}

	bool TextBox::isUnicodeUnit(wchar_t char1, wchar_t char2)
	{
		return (IsLowSurrogate(char1) && IsHighSurrogate(char2))
			|| (char1 == '\n' && char2 == '\r');
	}

	bool TextBox::IsHighSurrogate(UINT32 ch)
	{
		return (ch & 0xFC00) == 0xD800;
	}

	bool TextBox::IsLowSurrogate(UINT32 ch)
	{
		return (ch & 0xFC00) == 0xDC00;
	}

	IDWriteTextLayout* TextBox::createTextLayout(const std::wstring& text)
	{
		IDWriteTextLayout* textLayout = nullptr;
		IDWriteTextFormat* textFormat = nullptr;

		Graphics::Core::getDWriteFactory()->CreateTextFormat(L"", 0,DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"", &textFormat);

		Graphics::Core::getDWriteFactory()->CreateTextLayout(
			text.c_str(),
			static_cast<UINT32>(text.length()),
			textFormat,
			width, height,
			&textLayout
		);

		Graphics::SafeRelease(&textFormat);

		return textLayout;
	}

	void TextBox::fillSelectedRange()
	{
		UINT32 actualHitTestCount = 0;
		auto selectedRange = getSelectionRange();
		if (selectedRange.length > 0) {
			textLayout->HitTestTextRange(selectedRange.startPosition, selectedRange.length, 0, 0, 0, 0, &actualHitTestCount);
			std::vector<DWRITE_HIT_TEST_METRICS>hitTestMetrics(actualHitTestCount);
			textLayout->HitTestTextRange(selectedRange.startPosition, selectedRange.length, this->x, this->y - scrollY, &hitTestMetrics[0], static_cast<UINT32>(hitTestMetrics.size()), &actualHitTestCount);

			//改变画刷为天蓝色
			brush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));

			//遍历选中区域并进行填充
			for (UINT32 i = 0; i < actualHitTestCount; i++) {
				const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
				D2D1_RECT_F highlightRect = {
					htm.left,
					htm.top,
					(htm.left + htm.width),
					(htm.top + htm.height)
				};

				this->pRT->FillRectangle(highlightRect, brush);
			}
		}
	}

	void TextBox::drawCaret()
	{
		DWRITE_HIT_TEST_METRICS caretMetrics;
		float caretX, caretY;
		textLayout->HitTestTextPosition(caretPosition, false, &caretX, &caretY, &caretMetrics);

		caretX = std::ceilf(caretX) + 0.5f;
		caretY = std::ceilf(caretY) + 0.5f;

		//若不处于滚动状态，则对光标位置进行判断修改，使其处于显示区域
		if (!isOnScroll) {
			if (caretY - scrollY + caretMetrics.height > height) {//光标超出窗口底部
				scrollY = caretY - height + caretMetrics.height;
			}
			else if (caretY - scrollY < 0) {//光标在窗口上方
				scrollY = caretY;
			}
		}

		//使用sin函数决定是否绘制caret
		if (sin((timeGetTime() / 1000.f - lastInputTime) * 6.28f) > -0.1) {

			//caret颜色为黑色
			brush->SetColor(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

			this->pRT->DrawLine(D2D1::Point2F(this->x + caretX, this->y + caretY - scrollY), D2D1::Point2F(this->x + caretX, this->y + caretY + caretMetrics.height - scrollY), brush);
		}
	}

	void TextBox::drawText()
	{
		brush->SetColor(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

		//文本为黑色
		this->pRT->DrawTextLayout(D2D1::Point2F(this->x, this->y - scrollY), textLayout, brush);
	}

	TextBox::TextBox()
		: Widget(L"DebugTextBox", 200.0f, 200.0f, 50.0f, 50.0f)
	{
		this->handleKeyboard = true;
		this->handleMouse = true;
	}

	TextBox::~TextBox()
	{
	}
}
