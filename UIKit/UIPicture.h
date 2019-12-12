#pragma once

#include "UIWidget.h"

namespace UIKit
{
	namespace UI
	{
		class Picture : public Widget
		{
		private:
			virtual void update();
			virtual void render();

			virtual void onAttach();
			virtual void onDetach();

		private:
			ID2D1Bitmap* pBitmap{};

		public:
			Picture(const std::wstring&& pictureID, const float&& width = 0.0f, const float&& height = 0.0f, const float&& x = 0.0f, const float&& y = 0.0f);
			Picture(const std::wstring& pictureID, const float& width = 0.0f, const float& height = 0.0f, const float& x = 0.0f, const float& y = 0.0f);
			~Picture();

			bool loadFromFile(const std::wstring&& path);
			bool loadFromResource(const std::wstring&& resName, const std::wstring&& resType);

		};
	}
}