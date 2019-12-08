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
		Graphics::Core::getDWriteFactory()->CreateTextFormat(this->fontName.c_str(), nullptr, this->fontWeight, this->fontStyle, DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL, Graphics::pixelToDipY(this->fontSize), L"", &pTextFormat);
	
		pTextFormat->SetTextAlignment(this->horizontalAlign);
		pTextFormat->SetParagraphAlignment(this->verticalAlign);

		this->pBrush->SetColor(this->textColor);

		this->pRT->DrawText(this->labelText.c_str(), this->labelText.length(), pTextFormat, D2D1::RectF(std::ceilf(this->x), std::ceilf(this->y), std::ceilf(this->x + this->width), std::ceilf (this->y + this->height)), this->pBrush);

		Graphics::SafeRelease(&pTextFormat);
	}

	void Label::onAttach()
	{
		Graphics::SafeRelease(&this->pBrush);
		this->pRT->CreateSolidColorBrush(this->textColor, &this->pBrush);
	}

	void Label::onDetach()
	{
		Graphics::SafeRelease(&this->pBrush);
	}

	Label::Label(const std::wstring&& labelID, const std::wstring&& labelText, const float&& fontSize, const float&& width, const float&& height, const float&& x, const float&& y)
		: labelText(labelText), fontSize(fontSize), Widget(labelID, width, height, x, y)
	{
	}

	Label::Label(const std::wstring& labelID, const std::wstring& labelText, const float& fontSize, const float& width, const float& height, const float& x, const float& y)
		: labelText(labelText), fontSize(fontSize), Widget(labelID, width, height, x, y)
	{
	}

	Label::~Label()
	{
		Graphics::SafeRelease(&this->pBrush);
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
		this->onAttach();
	}

	void Label::setFontSize(const float&& fontSize)
	{
		this->fontSize = fontSize;
	}

	void Label::setFontSize(const float& fontSize)
	{
		this->fontSize = fontSize;
	}

	void Label::setFontName(const std::wstring&& fontName)
	{
		this->fontName = fontName;
	}

	void Label::setFontName(const std::wstring& fontName)
	{
		this->fontName = fontName;
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

	void Label::setTextAlignment(const hAlign&& horizontal, const vAlign&& vertical)
	{
		switch (horizontal)
		{
			case hAlign::Left:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING };
			}
			break;

			case hAlign::Center:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER };
			}
			break;

			case hAlign::Right:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING };
			}
			break;
		}

		switch (vertical)
		{
			case vAlign::Top:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR };
			}
			break;

			case vAlign::Center:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER };
			}
			break;

			case vAlign::Bottom:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR };
			}
			break;
		}
	}

	void Label::setTextAlignment(const hAlign& horizontal, const vAlign& vertical)
	{
		switch (horizontal)
		{
			case hAlign::Left:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING };
			}
			break;

			case hAlign::Center:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER };
			}
			break;

			case hAlign::Right:
			{
				this->horizontalAlign = { DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING };
			}
			break;
		}

		switch (vertical)
		{
			case vAlign::Top:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR };
			}
			break;

			case vAlign::Center:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER };
			}
			break;

			case vAlign::Bottom:
			{
				this->verticalAlign = { DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR };
			}
			break;
		}
	}
	
	void Label::setFontWeight(const weight&& fontWeight)
	{
		switch (fontWeight)
		{
			case weight::Thin:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_THIN;
			}
			break;

			case weight::Light:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_LIGHT;
			}
			break;

			case weight::Normal:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
			}
			break;

			case weight::Regular:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR;
			}
			break;

			case weight::Bold:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD;
			}
			break;

			case weight::Black:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BLACK;
			}
			break;
		}
	}

	void Label::setFontWeight(const weight& fontWeight)
	{
		switch (fontWeight)
		{
			case weight::Thin:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_THIN;
			}
			break;

			case weight::Light:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_LIGHT;
			}
			break;

			case weight::Normal:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
			}
			break;

			case weight::Regular:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR;
			}
			break;

			case weight::Bold:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD;
			}
			break;

			case weight::Black:
			{
				this->fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BLACK;
			}
			break;
		}
	}
	
	void Label::setFontStyle(const style&& fontStyle)
	{
		switch (fontStyle)
		{
			case style::Italic:
			{
				this->fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_ITALIC;
			}
			break;

			case style::Normal:
			{
				this->fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
			}
			break;

			case style::Oblique:
			{
				this->fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_OBLIQUE;
			}
			break;
		}
	}

	void Label::setFontStyle(const style& fontStyle)
	{
		switch (fontStyle)
		{
			case style::Italic:
			{
				this->fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_ITALIC;
			}
			break;

			case style::Normal:
			{
				this->fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
			}
			break;

			case style::Oblique:
			{
				this->fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_OBLIQUE;
			}
			break;
		}
	}
}
