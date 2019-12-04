#pragma once

#include "UIWidget.h"
#include <dwrite.h>

namespace UIKit
{
	namespace UI
	{
		class Label : public Widget
		{
		public:
			enum class hAlign
			{
				Left,
				Center,
				Right
			};

			enum class vAlign
			{
				Top,
				Center,
				Bottom
			};

		private:
			virtual void update();
			virtual void render();

		private:
			std::wstring labelText{ L"" }, fontName{ L"Arial" };
			float fontSize{};
			D2D1_COLOR_F textColor{ D2D1::ColorF(D2D1::ColorF::Black) };
			DWRITE_TEXT_ALIGNMENT horizontalAlign{};
			DWRITE_PARAGRAPH_ALIGNMENT verticalAlign{};

		public:
			Label(const std::wstring&& labelID, const std::wstring&& labelText = L"Label", const float&& fontSize = 12.0f, const float&& width = 100.0f, const float&& height = 100.0f, const float&& x = 0.0f, const float&& y = 0.0f);
			Label(const std::wstring& labelID, const std::wstring& labelText = L"Label", const float& fontSize = 12.0f, const float& width = 100.0f, const float& height = 100.0f, const float& x = 0.0f, const float& y = 0.0f);

			void draw();
			void setRT(ID2D1DeviceContext* pRT);

			void setFontSize(const float&& fontSize);
			void setFontSize(const float& fontSize);

			void setFontName(const std::wstring&& fontName);
			void setFontName(const std::wstring& fontName);

			void setText(const std::wstring text, ...);

			void setTextColor(const uint8_t&& r, const uint8_t&& g, const uint8_t&& b, const uint8_t&& a = 255);
			void setTextColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a = 255);

			void setTextAlignment(const hAlign&& horizontal, const vAlign&& vertical);
			void setTextAlignment(const hAlign& horizontal, const vAlign& vertical);

		};
	}
}