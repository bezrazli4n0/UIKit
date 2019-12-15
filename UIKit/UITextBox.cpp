#include "UITextBox.h"
#include "UIGraphics.h"
#include "UIGraphicsHelper.hpp"
#include <algorithm>

namespace UIKit::UI
{
	void TextBox::update()
	{
		if (this->needUpdate)
		{
			this->createTextLayout();
			if (this->pTextLayout)
				this->needUpdate = false;

			DWRITE_TEXT_METRICS metrics{};
			this->pTextLayout->GetMetrics(&metrics);

			this->maxScrollY = max(metrics.height - this->height, 0);
			if (this->scrollY > this->maxScrollY)
				this->scrollY = this->maxScrollY;
		}
	}

	void TextBox::render()
	{
		DWRITE_TEXT_METRICS metrics{};
		this->pTextLayout->GetMetrics(&metrics);

		this->maxScrollY = max(metrics.height - this->height, 0);
		if (this->scrollY > this->maxScrollY)
			this->scrollY = this->maxScrollY;

		if (this->pTextLayout)
		{
			ID2D1Layer* pLayer{};
			this->pRT->CreateLayer(D2D1::SizeF(this->width + 1.5f, this->height + 1.5f), &pLayer);
			this->pRT->PushLayer(D2D1::LayerParameters(D2D1::RectF(this->x - 1.5f, this->y - 1.5f, this->x + this->width + 1.5f, this->y + this->height + 1.5f)), pLayer);

			if (this->active)
			{
				this->fillSelectedRange();
				this->drawCaret();
			}

			this->drawText();

			this->pRT->PopLayer();
			Graphics::SafeRelease(&pLayer);
		}
	}

	void TextBox::onAttach()
	{
		this->pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &this->pBrush);
	}

	void TextBox::onDetach()
	{
		Graphics::SafeRelease(&this->pBrush);
		Graphics::SafeRelease(&this->pTextLayout);
	}

	void TextBox::onChar(UINT32 c)
	{
		if ((c >= 0x20 || c == 9) && this->active && !this->readOnly)
		{
			this->deleteSelection();

			UINT32 charsLength = 1;
			wchar_t chars[2] = { static_cast<wchar_t>(c), 0 };

			if (c > 0xFFFF)
			{
				chars[0] = wchar_t(0xD800 + (c >> 10) - (0x10000 >> 10));
				chars[1] = wchar_t(0xDC00 + (c & 0x3FF));
				charsLength++;
			}

			this->text.insert(this->caretPosition, chars, charsLength);
			if (this->password)
				this->passwordText.insert(this->caretPosition, charsLength, this->passwordChar[0]);

			this->caretPosition += charsLength;
			this->caretAnchor = this->caretPosition;

			this->needUpdate = true;
			this->isOnScroll = false;

			this->lastInputTime = timeGetTime() / 1000.f;
		}
	}

	void TextBox::onKey(UINT32 vk)
	{
		if (this->active)
		{
			bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
			bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
			bool heldAlt = (GetKeyState(VK_MENU) & 0x80) != 0;

			switch (vk)
			{
				case VK_RETURN:
				{
					if (this->multiline && !this->readOnly)
					{
						this->deleteSelection();
						wchar_t chars[3]{};
						chars[0] = '\n';
						chars[1] = 0;
						this->text.insert(this->caretPosition, chars, 1);

						this->caretPosition += 1;
						this->caretAnchor = this->caretPosition;

						this->needUpdate = true;
					}
				}
				break;

				case VK_BACK:
				{
					if (!this->readOnly)
					{
						if (this->caretPosition != this->caretAnchor)
							this->deleteSelection();
						else if (this->caretPosition > 0)
						{
							UINT32 count = 1;
							if (this->caretPosition >= 2 && this->caretPosition <= this->text.length())
							{
								wchar_t charBackOne = this->text[this->caretPosition - 1];
								wchar_t charBackTwo = this->text[this->caretPosition - 2];
								if ((this->IsLowSurrogate(charBackOne) && this->IsHighSurrogate(charBackTwo)) || (charBackOne == '\n' && charBackTwo == '\r'))
									count = 2;
							}

							this->caretPosition -= count;
							this->caretAnchor = this->caretPosition;

							this->text.erase(this->caretPosition, count);
							this->needUpdate = true;
						}
					}
				}
				break;

				case VK_DELETE:
				{
					if (!this->readOnly)
					{
						if (this->caretPosition != this->caretAnchor)
							this->deleteSelection();
						else
						{
							DWRITE_HIT_TEST_METRICS hitTestMetrics{};
							float caretX{}, caretY{};

							this->pTextLayout->HitTestTextPosition(this->caretPosition, false, &caretX, &caretY, &hitTestMetrics);

							this->text.erase(hitTestMetrics.textPosition, hitTestMetrics.length);
							this->needUpdate = true;
						}
					}
				}
				break;

				case VK_TAB:
				{
				}
				break;

				case VK_LEFT:
				{
					if (!heldControl)
						this->select(SelectMode::lastChar, !heldShift);
					else
						this->select(SelectMode::lastWord, !heldShift);
				}
				break;

				case VK_RIGHT:
				{
					if (!heldControl)
						this->select(SelectMode::nextChar, !heldShift);
					else
						this->select(SelectMode::nextWord, !heldShift);
				}
				break;

				case VK_UP:
				{
					if (this->multiline)
						this->select(SelectMode::up);
				}
				break;

				case VK_DOWN:
				{
					if (this->multiline)
						this->select(SelectMode::down);
				}
				break;

				case VK_HOME:
				{
					this->select(SelectMode::head);
				}
				break;

				case VK_END:
				{
					this->select(SelectMode::tile);
				}
				break;

				case 'C':
				{
					if (heldControl)
						this->copyToClipboard();
				}
				break;

				case VK_INSERT:
				{
					if (!this->readOnly)
					{
						if (heldControl)
							this->copyToClipboard();
						else if (heldShift)
							this->pasteFromClipboard();
					}
				}
				break;

				case 'V':
				{
					if (heldControl && !this->readOnly)
						this->pasteFromClipboard();
				}
				break;

				case 'X':
				{
					if (heldControl && !this->readOnly)
					{
						this->copyToClipboard();
						this->deleteSelection();
					}
				}
				break;

				case 'A':
				{
					if (heldControl)
						this->select(SelectMode::all);
				}
				break;

				default:
					return;
			}

			this->isOnScroll = false;
			this->lastInputTime = timeGetTime() / 1000.f;
		}
	}

	void TextBox::onMouseUp(const int& xPos, const int& yPos)
	{
		if (this->active)
		{
			this->mousePressed = false;

			float time = timeGetTime() / 1000.f;
			const float doubleClickInterval = 0.3f;

			if (time - this->lastClickTime < doubleClickInterval)
				if (lastSelectLength == 0)
					this->select(SelectMode::all);

			this->lastClickTime = time;

			POINT pt{};
			GetCursorPos(&pt);
			SetCursorPos(pt.x, pt.y);
		}
	}

	void TextBox::onMouseDown(const int& xPos, const int& yPos)
	{
		if (this->checkMouse(static_cast<float>(xPos), static_cast<float>(yPos)))
		{
			this->active = true;
			this->mousePressed = true;
		}
		else
		{
			this->active = false;
			this->scrollY = 0;
			this->scrollX = 0;
			return;
		}

		this->isOnScroll = false;
		this->lastSelectLength = this->getSelectionRange().length;
		this->setSelectionFromPoint(static_cast<float>(xPos + this->scrollX), static_cast<float>(yPos + this->scrollY), (GetKeyState(VK_SHIFT) & 0x80) == 0);
	}

	void TextBox::onMouseMove(const int& xPos, const int& yPos)
	{
		if (this->checkMouse(static_cast<float>(xPos), static_cast<float>(yPos)))
			this->mouseHover = true;
		else
			this->mouseHover = false;

		if ((GetKeyState(MK_LBUTTON) & 0x100) != 0)
			this->setSelectionFromPoint(static_cast<float>(xPos + this->scrollX), static_cast<float>(yPos + this->scrollY), false);
	}

	void TextBox::onMouseScroll(const int& xPos, const int& yPos, const short& delta)
	{
		if (this->active && this->multiline)
		{
			this->isOnScroll = true;

			float nextScroll = this->scrollY - delta;

			if (nextScroll < 0)
				nextScroll = 0;
			else if (nextScroll > this->maxScrollY)
				nextScroll = this->maxScrollY;

			if (nextScroll != this->scrollY)
			{
				this->scrollY = nextScroll;
				this->needUpdate = true;
			}
		}
	}

	bool TextBox::updateCursor()
	{
		if (this->mouseHover)
			SetCursor(this->hBeamCursor);
		else if (this->active && this->mousePressed)
		{
			if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
				SetCursor(this->hBeamCursor);
			else
				return false;
		}
		else
			return false;

		return true;
	}

	void TextBox::onTabStop(bool flag)
	{
		this->active = flag;
		if (flag)
			this->select(SelectMode::all);
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

		return { caretBegin, caretEnd - caretBegin };
	}

	void TextBox::copyToClipboard()
	{
		DWRITE_TEXT_RANGE selectionRange = this->getSelectionRange();
		if (selectionRange.length <= 0)
			return;

		if (OpenClipboard(0))
		{
			if (EmptyClipboard())
			{
				size_t byteSize = sizeof(wchar_t) * (selectionRange.length + 1);
				HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, byteSize);

				if (hClipboardData != nullptr)
				{
					void* memory = GlobalLock(hClipboardData);

					if (memory != nullptr)
					{
						const wchar_t* ctext = this->text.c_str();
						memcpy(memory, &ctext[selectionRange.startPosition], byteSize);
						GlobalUnlock(hClipboardData);

						if (SetClipboardData(CF_UNICODETEXT, hClipboardData) != NULL)
							hClipboardData = NULL;
					}
					GlobalFree(hClipboardData);
				}
			}
			CloseClipboard();
		}
	}

	void TextBox::deleteSelection()
	{
		DWRITE_TEXT_RANGE range = this->getSelectionRange();

		if (range.length <= 0)
			return;

		this->text.erase(range.startPosition, range.length);

		this->caretPosition = range.startPosition;
		this->caretAnchor = this->caretPosition;

		this->needUpdate = true;
	}

	void TextBox::pasteFromClipboard()
	{
		this->deleteSelection();

		UINT32 characterCount = 0;

		if (OpenClipboard(0)) {
			HGLOBAL hClipboardData = GetClipboardData(CF_UNICODETEXT);

			if (hClipboardData != nullptr)
			{
				size_t byteSize = GlobalSize(hClipboardData);
				void* memory = GlobalLock(hClipboardData);
				const wchar_t* ctext = reinterpret_cast<const wchar_t*>(memory);
				characterCount = static_cast<UINT32>(wcsnlen(ctext, byteSize / sizeof(wchar_t)));

				if (memory != nullptr)
				{
					this->text.insert(this->caretPosition, ctext, characterCount);
					if (!this->multiline)
					{
						this->text.erase(std::remove(std::begin(this->text), std::end(this->text), '\r'), std::end(this->text));
						this->text.erase(std::remove(std::begin(this->text), std::end(this->text), '\n'), std::end(this->text));
					}
					GlobalUnlock(hClipboardData);
				}
			}
			CloseClipboard();
		}

		this->caretPosition += characterCount;
		this->caretAnchor = this->caretPosition;

		this->needUpdate = true;
	}

	void TextBox::setSelectionFromPoint(float x, float y, bool moveAnchor)
	{
		BOOL isTrailingHit{};
		BOOL isInside{};
		DWRITE_HIT_TEST_METRICS caretMetrics{};

		x -= this->x;
		y -= this->y;

		this->pTextLayout->HitTestPoint(x, y, &isTrailingHit, &isInside, &caretMetrics);

		if (isTrailingHit)
			this->caretPosition = caretMetrics.textPosition + caretMetrics.length;
		else
			this->caretPosition = caretMetrics.textPosition;

		if (moveAnchor)
			this->caretAnchor = this->caretPosition;
	}

	void TextBox::select(SelectMode mode, bool moveAnchor)
	{
		switch (mode)
		{
			case SelectMode::up:
			case SelectMode::down:
			{
				std::vector<DWRITE_LINE_METRICS> lineMetrics{};
				DWRITE_TEXT_METRICS textMetrics{};
				this->pTextLayout->GetMetrics(&textMetrics);

				lineMetrics.resize(textMetrics.lineCount);
				this->pTextLayout->GetLineMetrics(&lineMetrics.front(), textMetrics.lineCount, &textMetrics.lineCount);

				UINT32 line = 0;
				UINT32 linePosition = 0;
				UINT32 nextLinePosition = 0;
				UINT32 lineCount = static_cast<UINT32>(lineMetrics.size());
				for (; line < lineCount; ++line)
				{
					linePosition = nextLinePosition;
					nextLinePosition = linePosition + lineMetrics[line].length;
					if (nextLinePosition > this->caretPosition)
						break;
				}

				if (line > lineCount - 1)
					line = lineCount - 1;

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

				DWRITE_HIT_TEST_METRICS hitTestMetrics{};
				float caretX{}, caretY{}, dummyX{};

				this->pTextLayout->HitTestTextPosition(this->caretPosition, false, &caretX, &caretY, &hitTestMetrics);
				this->pTextLayout->HitTestTextPosition(linePosition, false, &dummyX, &caretY, &hitTestMetrics);

				BOOL isInside{}, isTrailingHit{};
				this->pTextLayout->HitTestPoint(caretX, caretY, &isTrailingHit, &isInside, &hitTestMetrics);

				this->caretPosition = hitTestMetrics.textPosition;

				if (isTrailingHit)
					this->caretPosition += hitTestMetrics.length;
			}
			break;

			case SelectMode::head:
			{
				this->caretPosition = 0;
			}
			break;

			case SelectMode::tile:
			{
				this->caretPosition = this->text.length();
			}
			break;

			case SelectMode::lastChar:
			{
				if (this->caretPosition > 0)
				{
					UINT32 moveCount = 1;
					if (this->caretPosition >= 2 && this->caretPosition <= this->text.length())
					{
						if (this->isUnicodeUnit(this->text[this->caretPosition - 1], this->text[this->caretPosition - 2]))
							moveCount = 2;
					}

					if (this->caretPosition < static_cast<UINT32>(moveCount))
						this->caretPosition = 0;
					else this->caretPosition -= moveCount;
				}
			}
			break;

			case SelectMode::nextChar:
			{
				if (this->caretPosition < this->text.length())
				{
					UINT32 moveCount = 1;
					if (this->caretPosition >= 0 && this->caretPosition <= this->text.length() - 2)
					{
						wchar_t charBackOne = this->text[this->caretPosition];
						wchar_t charBackTwo = this->text[this->caretPosition + 1];
						if (this->isUnicodeUnit(this->text[this->caretPosition], this->text[this->caretPosition + 1]))
							moveCount = 2;
					}

					if (this->caretPosition > this->text.length())
						this->caretPosition = this->text.length();
					else this->caretPosition += moveCount;
				}
			}
			break;

			case SelectMode::lastWord:
			case SelectMode::nextWord:
			{
				std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics{};
				UINT32 clusterCount{};
				this->pTextLayout->GetClusterMetrics(nullptr, 0, &clusterCount);
				if (clusterCount == 0)
					break;

				clusterMetrics.resize(clusterCount);
				this->pTextLayout->GetClusterMetrics(&clusterMetrics.front(), clusterCount, &clusterCount);

				UINT32 clusterPosition = 0;
				UINT32 oldCaretPosition = this->caretPosition;

				if (mode == SelectMode::lastWord)
				{
					this->caretPosition = 0;
					for (UINT32 cluster = 0; cluster < clusterCount; ++cluster)
					{

						clusterPosition += clusterMetrics[cluster].length;
						if (clusterMetrics[cluster].canWrapLineAfter)
						{
							if (clusterPosition >= oldCaretPosition)
								break;

							this->caretPosition = clusterPosition;
						}

					}

				}
				else
				{
					for (UINT32 cluster = 0; cluster < clusterCount; ++cluster)
					{
						UINT32 clusterLength = clusterMetrics[cluster].length;

						if (clusterPosition + clusterMetrics[cluster].length > oldCaretPosition&& clusterMetrics[cluster].canWrapLineAfter)
						{
							caretPosition = clusterPosition + clusterMetrics[cluster].length;
							break;
						}

						clusterPosition += clusterLength;
						this->caretPosition = clusterPosition;
					}
				}
			}
			break;

			case SelectMode::absoluteLeading:
			{
				DWRITE_HIT_TEST_METRICS hitTestMetrics{};
				float caretX{}, caretY{};

				this->pTextLayout->HitTestTextPosition(this->caretPosition, false, &caretX, &caretY, &hitTestMetrics);

				this->caretPosition = hitTestMetrics.textPosition;
			}
			break;

			case SelectMode::absoluteTrailing:
			{
				DWRITE_HIT_TEST_METRICS hitTestMetrics{};
				float caretX{}, caretY{};

				this->pTextLayout->HitTestTextPosition(this->caretPosition, true, &caretX, &caretY, &hitTestMetrics);

				this->caretPosition = hitTestMetrics.textPosition + hitTestMetrics.length;
			}
			break;

			case SelectMode::all:
			{
				this->caretAnchor = 0;
				this->caretPosition = this->text.length();
			}
			return;

			default:
				break;
		}

		if (moveAnchor)
			this->caretAnchor = this->caretPosition;
	}

	bool TextBox::isUnicodeUnit(wchar_t char1, wchar_t char2)
	{
		return (this->IsLowSurrogate(char1) && this->IsHighSurrogate(char2)) || (char1 == '\n' && char2 == '\r');
	}

	bool TextBox::IsHighSurrogate(UINT32 ch)
	{
		return (ch & 0xFC00) == 0xD800;
	}

	bool TextBox::IsLowSurrogate(UINT32 ch)
	{
		return (ch & 0xFC00) == 0xDC00;
	}

	void TextBox::createTextLayout()
	{
		Graphics::SafeRelease(&this->pTextLayout);

		IDWriteTextFormat* textFormat{};

		Graphics::Core::getDWriteFactory()->CreateTextFormat(L"", 0,DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Graphics::pixelToDipY(this->fontSize), L"", &textFormat);
		
		if (!this->multiline)
			textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		auto correctedWidth = this->width;
		if (!this->multiline)
		{
			auto rectf = D2D1::InfiniteRect();
			correctedWidth = rectf.right;
		}

		Graphics::Core::getDWriteFactory()->CreateTextLayout(
			this->password ? this->passwordText.c_str() : this->text.c_str(),
			static_cast<UINT32>(this->text.length()),
			textFormat,
			correctedWidth, this->height,
			&this->pTextLayout
		);

		Graphics::SafeRelease(&textFormat);
	}

	bool TextBox::checkMouse(const float& xPos, const float& yPos)
	{
		BOOL contains{};
		this->pRoundRectGeometry->FillContainsPoint(D2D1::Point2F(xPos, yPos), D2D1::Matrix3x2F::Identity(), &contains);
		//this->active = contains;

		//if (!this->active)
		//	this->scrollY = 0;
	
		return static_cast<bool>(contains);
	}

	void TextBox::recreateGeometry()
	{
		Graphics::SafeRelease(&this->pRoundRectGeometry);
		Graphics::Core::getFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(this->x - this->expandedMouseAreaX, this->y, this->x + this->width + this->expandedMouseAreaX, this->y + this->height), 0.0f, 0.0f), &this->pRoundRectGeometry);
	}

	void TextBox::fillSelectedRange()
	{
		UINT32 actualHitTestCount = 0;
		auto selectedRange = this->getSelectionRange();
		if (selectedRange.length > 0)
		{
			this->pTextLayout->HitTestTextRange(selectedRange.startPosition, selectedRange.length, 0, 0, 0, 0, &actualHitTestCount);
			std::vector<DWRITE_HIT_TEST_METRICS>hitTestMetrics(actualHitTestCount);
			this->pTextLayout->HitTestTextRange(selectedRange.startPosition, selectedRange.length, this->x - this->scrollX, this->y - this->scrollY, &hitTestMetrics[0], static_cast<UINT32>(hitTestMetrics.size()), &actualHitTestCount);

			this->pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
			for (UINT32 i = 0; i < actualHitTestCount; i++)
			{
				const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];

				D2D1_RECT_F highlightRect = {
					htm.left,
					htm.top,
					(htm.left + htm.width),
					(htm.top + htm.height)
				};

				this->pRT->FillRectangle(highlightRect, this->pBrush);
			}
		}
	}

	void TextBox::drawCaret()
	{
		DWRITE_HIT_TEST_METRICS caretMetrics{};
		float caretX{}, caretY{};
		this->pTextLayout->HitTestTextPosition(this->caretPosition, false, &caretX, &caretY, &caretMetrics);

		if (!this->isOnScroll && this->multiline)
		{
			if (caretY - this->scrollY + caretMetrics.height > this->height)
				this->scrollY = caretY - this->height + caretMetrics.height;
			else if (caretY - this->scrollY < 0)
				this->scrollY = caretY;
		}

		if (!this->multiline)
		{
			if (caretX - this->scrollX + caretMetrics.width > this->width) 
				this->scrollX = caretX - this->width + caretMetrics.width;
			else if (caretX - this->scrollX < 0)
				this->scrollX = caretX;
		}

		if (sin((timeGetTime() / 1000.f - this->lastInputTime) * 6.28f) > -0.1) 
		{
			this->pBrush->SetColor(this->caretColor);
			if (this->multiline)
				this->pRT->DrawLine(D2D1::Point2F(std::ceilf(this->x + caretX) + 0.5f, std::ceilf(this->y + caretY - this->scrollY) + 0.5f), D2D1::Point2F(std::ceilf(this->x + caretX) + 0.5f, std::ceilf(this->y + caretY + caretMetrics.height - this->scrollY) + 0.5f), this->pBrush);
			else
				this->pRT->DrawLine(D2D1::Point2F(std::ceilf(this->x + caretX - this->scrollX) + 0.5f, this->y + caretY), D2D1::Point2F(std::ceilf(this->x + caretX - this->scrollX) + 0.5f, this->y + caretY + caretMetrics.height), this->pBrush);
		}
	}

	void TextBox::drawText()
	{
		this->pBrush->SetColor(this->textColor);
		if (this->multiline)
			this->pRT->DrawTextLayout(D2D1::Point2F(this->x, this->y - this->scrollY), this->pTextLayout, this->pBrush);
		else
			this->pRT->DrawTextLayout(D2D1::Point2F(this->x - this->scrollX, this->y), this->pTextLayout, this->pBrush);
	}

	TextBox::TextBox(const std::wstring&& textBoxID, const std::wstring&& text, const bool&& isTabStop, const bool&& isMultiline, const bool&& isReadOnly, const bool&& isPassword, const float&& fontSize, const float&& width, const float&& height, const float&& x, const float&& y)
		: text{ text }, multiline{ isMultiline }, readOnly{ isReadOnly }, password{ isPassword }, fontSize{ fontSize }, Widget(textBoxID, width, height, x, y)
	{
		this->handleKeyboard = true;
		this->handleMouse = true;
		this->hBeamCursor = LoadCursor(nullptr, IDC_IBEAM);
		this->tabStop = isTabStop;

		this->recreateGeometry();

		if (this->password)
			this->passwordText.insert(this->caretPosition, this->text.length(), this->passwordChar[0]);
	}

	TextBox::TextBox(const std::wstring& textBoxID, const std::wstring& text, const bool& isTabStop, const bool& isMultiline, const bool& isReadOnly, const bool& isPassword, const float& fontSize, const float& width, const float& height, const float& x, const float& y)
		: text{ text }, multiline{ isMultiline }, readOnly{ isReadOnly }, password{ isPassword }, fontSize{ fontSize }, Widget(textBoxID, width, height, x, y)
	{
		this->handleKeyboard = true;
		this->handleMouse = true;
		this->hBeamCursor = LoadCursor(nullptr, IDC_IBEAM);
		this->tabStop = isTabStop;

		this->recreateGeometry();

		if (this->password)
			this->passwordText.insert(this->caretPosition, this->text.length(), this->passwordChar[0]);
	}

	TextBox::~TextBox()
	{
		Graphics::SafeRelease(&this->pRoundRectGeometry);
		Graphics::SafeRelease(&this->pBrush);
		Graphics::SafeRelease(&this->pTextLayout);
	}

	void TextBox::setTextColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a)
	{
		this->textColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void TextBox::setTextColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		this->textColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void TextBox::setCaretColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a)
	{
		this->caretColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void TextBox::setCaretColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		this->caretColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	std::wstring TextBox::getText() const
	{
		return this->text;
	}

	void TextBox::setText(const std::wstring&& text)
	{
		this->text = text;
		if (!this->multiline)
		{
			this->text.erase(std::remove(std::begin(this->text), std::end(this->text), '\r'), std::end(this->text));
			this->text.erase(std::remove(std::begin(this->text), std::end(this->text), '\n'), std::end(this->text));
		}
		if (this->password)
		{
			this->passwordText.clear();
			this->passwordText.insert(0, this->text.length(), this->passwordChar[0]);
		}
		this->needUpdate = true;
	}

	void TextBox::setText(const std::wstring& text)
	{
		this->text = text;
		if (!this->multiline)
		{
			this->text.erase(std::remove(std::begin(this->text), std::end(this->text), '\r'), std::end(this->text));
			this->text.erase(std::remove(std::begin(this->text), std::end(this->text), '\n'), std::end(this->text));
		}
		if (this->password)
		{
			this->passwordText.clear();
			this->passwordText.insert(0, this->text.length(), this->passwordChar[0]);
		}
		this->needUpdate = true;
	}

	void TextBox::setSizeInDIP(const WidgetPoints&& widgetSizeDIP)
	{
		auto [width, height] = widgetSizeDIP;
		this->width = width;
		this->height = height;
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::setSizeInDIP(const WidgetPoints& widgetSizeDIP)
	{
		auto [width, height] = widgetSizeDIP;
		this->width = width;
		this->height = height;
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::setSizeInPixel(const WidgetPoints&& widgetSize)
	{
		auto [width, height] = widgetSize;
		this->width = Graphics::pixelToDipX(width);
		this->height = Graphics::pixelToDipY(height);
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::setSizeInPixel(const WidgetPoints& widgetSize)
	{
		auto [width, height] = widgetSize;
		this->width = Graphics::pixelToDipX(width);
		this->height = Graphics::pixelToDipY(height);
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::setPosInDIP(const WidgetPoints&& widgetPosDIP)
	{
		auto [x, y] = widgetPosDIP;
		this->x = x;
		this->y = y;
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::setPosInDIP(const WidgetPoints& widgetPosDIP)
	{
		auto [x, y] = widgetPosDIP;
		this->x = x;
		this->y = y;
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::setPosInPixel(const WidgetPoints&& widgetPos)
	{
		auto [x, y] = widgetPos;
		this->x = Graphics::pixelToDipX(x);
		this->y = Graphics::pixelToDipY(y);
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::setPosInPixel(const WidgetPoints& widgetPos)
	{
		auto [x, y] = widgetPos;
		this->x = Graphics::pixelToDipX(x);
		this->y = Graphics::pixelToDipY(y);
		this->recreateGeometry();

		this->needUpdate = true;
	}

	void TextBox::draw()
	{
		if (this->isVisible() && this->pRT != nullptr)
		{
			this->update();
			this->render();
		}
	}

	void TextBox::setRT(ID2D1DeviceContext* pRT)
	{
		this->pRT = pRT;
		this->onAttach();
	}

	void TextBox::expandMouseArea(const float&& areaX)
	{
		this->expandedMouseAreaX = Graphics::pixelToDipX(areaX);
		this->recreateGeometry();
	}

	void TextBox::expandMouseArea(const float& areaX)
	{
		this->expandedMouseAreaX = Graphics::pixelToDipX(areaX);
		this->recreateGeometry();
	}
}
