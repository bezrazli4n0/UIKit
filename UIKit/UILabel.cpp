#include "UILabel.h"
#include "UIGraphics.h"
#include "UIGraphicsHelper.hpp"

namespace UIKit::UI
{
	void Label::update()
	{
	}

	void Label::render()
	{
		IDWriteTextFormat* pTextFormat{};
		Graphics::Core::getDWriteFactory()->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL, Graphics::pixelToDipY(this->fontSize), L"", &pTextFormat);
	
		pTextFormat->SetTextAlignment(this->horizontalAlign);
		pTextFormat->SetParagraphAlignment(this->verticalAlign);

		ID2D1SolidColorBrush* brush{};
		this->pRT->CreateSolidColorBrush(this->textColor, &brush);

		this->pRT->DrawText(this->labelText.c_str(), this->labelText.length(), pTextFormat, D2D1::RectF(this->x, this->y, this->x + this->width, this->y + this->height), brush);

		Graphics::SafeRelease(&pTextFormat);
		Graphics::SafeRelease(&brush);
	}

	Label::Label(const std::wstring&& labelID, const std::wstring&& labelText, const float&& fontSize, const float&& width, const float&& height, const float&& x, const float&& y)
		: labelText(labelText), fontSize(fontSize), Widget(labelID, width, height, x, y)
	{
	}

	Label::Label(const std::wstring& labelID, const std::wstring& labelText, const float& fontSize, const float& width, const float& height, const float& x, const float& y)
		: labelText(labelText), fontSize(fontSize), Widget(labelID, width, height, x, y)
	{
	}

	void Label::draw()
	{
		if (this->isVisible() && this->pRT != nullptr)
		{
			this->update();
			this->render();
		}
	}

	void Label::setRT(ID2D1DeviceContext* pRT)
	{
		this->pRT = pRT;
	}

	void Label::setFontSize(const float&& fontSize)
	{
		this->fontSize = fontSize;
	}

	void Label::setFontSize(const float& fontSize)
	{
		this->fontSize = fontSize;
	}

	void Label::setText(const std::wstring text, ...)
	{
		va_list args{};
		va_start(args, text);
		const auto len = _vscwprintf(text.c_str(), args);

		wchar_t* buffer = new wchar_t[len + 1]{};
		ZeroMemory(buffer, len + 1);

		vswprintf_s(buffer, len + 1, text.c_str(), args);

		this->labelText = buffer;

		delete[] buffer;
		buffer = nullptr;

		va_end(args);
	}
	
	void Label::setTextColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a)
	{
		this->textColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void Label::setTextColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		this->textColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	}

	void Label::setTextAlignment(const TextAlignmentHorizontal&& horizontal, const TextAlignmentVertical&& vertical)
	{
		switch (horizontal)
		{
			case TextAlignmentHorizontal::Left:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING };
			}
			break;

			case TextAlignmentHorizontal::Center:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER };
			}
			break;

			case TextAlignmentHorizontal::Right:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING };
			}
			break;
		}

		switch (vertical)
		{
			case TextAlignmentVertical::Top:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR };
			}
			break;

			case TextAlignmentVertical::Center:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER };
			}
			break;

			case TextAlignmentVertical::Bottom:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR };
			}
			break;
		}
	}

	void Label::setTextAlignment(const TextAlignmentHorizontal& horizontal, const TextAlignmentVertical& vertical)
	{
		switch (horizontal)
		{
			case TextAlignmentHorizontal::Left:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING };
			}
			break;

			case TextAlignmentHorizontal::Center:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER };
			}
			break;

			case TextAlignmentHorizontal::Right:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING };
			}
			break;
		}

		switch (vertical)
		{
			case TextAlignmentVertical::Top:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR };
			}
			break;

			case TextAlignmentVertical::Center:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER };
			}
			break;

			case TextAlignmentVertical::Bottom:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR };
			}
			break;
		}
	}
}
