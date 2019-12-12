#include "UIPicture.h"
#include "UIGraphics.h"

namespace UIKit::UI
{
	void Picture::update()
	{
	}

	void Picture::render()
	{
		if (this->pBitmap != nullptr)
			this->pRT->DrawBitmap(this->pBitmap, D2D1::RectF(this->x, this->y, this->x + this->width, this->y + this->height));
	}

	void Picture::onAttach()
	{
	}

	void Picture::onDetach()
	{
	}

	Picture::Picture(const std::wstring&& pictureID, const float&& width, const float&& height, const float&& x, const float&& y)
		: Widget(pictureID, width, height, x, y)
	{
	}

	Picture::Picture(const std::wstring& pictureID, const float& width, const float& height, const float& x, const float& y)
		: Widget(pictureID, width, height, x, y)
	{
	}

	Picture::~Picture()
	{
		Graphics::SafeRelease(&this->pBitmap);
	}

	bool Picture::loadFromFile(const std::wstring&& path)
	{
		IWICBitmapDecoder* pDecoder{};
		IWICBitmapFrameDecode* pSource{};
		IWICStream* pStream{};
		IWICFormatConverter* pConverter{};
		IWICBitmapScaler* pScaler{};

		if (this->pRT == nullptr)
			return false;

		if (!SUCCEEDED(Graphics::Core::getWICFactory()->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder)))
			return false;

		if (!SUCCEEDED(pDecoder->GetFrame(0, &pSource)))
			return false;

		if (!SUCCEEDED(Graphics::Core::getWICFactory()->CreateFormatConverter(&pConverter)))
			return false;

		if (!SUCCEEDED(pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut)))
			return false;

		Graphics::SafeRelease(&this->pBitmap);
		if (!SUCCEEDED(this->pRT->CreateBitmapFromWicBitmap(pConverter, nullptr, &this->pBitmap)))
			return false;

		Graphics::SafeRelease(&pDecoder);
		Graphics::SafeRelease(&pSource);
		Graphics::SafeRelease(&pStream);
		Graphics::SafeRelease(&pConverter);
		Graphics::SafeRelease(&pScaler);

		return true;
	}

	bool Picture::loadFromResource(const std::wstring&& resName, const std::wstring&& resType)
	{
		IWICBitmapDecoder* pDecoder{};
		IWICBitmapFrameDecode* pSource{};
		IWICStream* pStream{};
		IWICFormatConverter* pConverter{};
		IWICBitmapScaler* pScaler{};

		if (this->pRT == nullptr)
			return false;

		HRSRC imageResHandle{ FindResourceW(nullptr, resName.c_str(), resType.c_str()) };
		if (!imageResHandle)
			return false;

		HGLOBAL imageResDataHandle{ LoadResource(nullptr, imageResHandle) };
		if (!imageResDataHandle)
			return false;

		void* pImageFile{ LockResource(imageResDataHandle) };
		if (pImageFile == nullptr)
			return false;

		DWORD imageFileSize{ SizeofResource(nullptr, imageResHandle) };
		if (imageFileSize <= 0)
			return false;

		if (!SUCCEEDED(Graphics::Core::getWICFactory()->CreateStream(&pStream)))
			return false;

		if (!SUCCEEDED(pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageFile), imageFileSize)))
			return false;

		if (!SUCCEEDED(Graphics::Core::getWICFactory()->CreateDecoderFromStream(pStream, nullptr, WICDecodeMetadataCacheOnLoad, &pDecoder)))
			return false;

		if (!SUCCEEDED(pDecoder->GetFrame(0, &pSource)))
			return false;

		if (!SUCCEEDED(Graphics::Core::getWICFactory()->CreateFormatConverter(&pConverter)))
			return false;

		if (!SUCCEEDED(pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut)))
			return false;

		Graphics::SafeRelease(&this->pBitmap);
		if (!SUCCEEDED(this->pRT->CreateBitmapFromWicBitmap(pConverter, nullptr, &this->pBitmap)))
			return false;

		Graphics::SafeRelease(&pDecoder);
		Graphics::SafeRelease(&pSource);
		Graphics::SafeRelease(&pStream);
		Graphics::SafeRelease(&pConverter);
		Graphics::SafeRelease(&pScaler);

		return true;
	}
}
