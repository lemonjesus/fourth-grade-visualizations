#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"
#include "resource.h"

VizCrevice::VizCrevice() {}

VizCrevice::~VizCrevice() {
	SafeRelease(&displacement);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizCrevice::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D1DisplacementMap, &displacement);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.02f),&m_pBlackBrush);

	IWICImagingFactory *pIWICFactory = NULL;
	ppBitmap = NULL;
	ppMask = NULL;

	CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));
	VizCrevice::LoadResourceBitmap(m_d2dContext, pIWICFactory, MAKEINTRESOURCE(IDB_PNG1), RT_RCDATA, 690, 420, &ppBitmap);
	VizCrevice::LoadResourceBitmap(m_d2dContext, pIWICFactory, MAKEINTRESOURCE(IDB_PNG2), RT_RCDATA, 690, 420, &ppMask);

	SafeRelease(&pIWICFactory);
}

void VizCrevice::render(
	unsigned long long frame,
	D2D1_RECT_F rectf,
	ID2D1BitmapRenderTarget *bgEffectTarget,
	ID2D1Bitmap *bgEffectBitmap,
	ID2D1Bitmap *bitmap,
	ID2D1DeviceContext* m_d2dContext,
	TimedLevel *pLevels
) {
	HsvColor hsv;
	hsv.h = frame%255;
	hsv.s = 255;
	hsv.v = 255;

	RgbColor rgb = HsvToRgb(hsv);
	m_pBrush->SetColor(D2D1::ColorF(rgb.r/255.0f, rgb.g/255.0f, rgb.b/255.0f));

	// background effects here
	bgEffectTarget->BeginDraw();
	bgEffectTarget->DrawBitmap(bitmap, rectf);
	bgEffectTarget->EndDraw();

	m_d2dContext->BeginDraw();
	m_d2dContext->Clear();
	displacement->SetInput(0, bgEffectBitmap);
	displacement->SetValue(D2D1_DISPLACEMENTMAP_PROP_SCALE, (float)PropertyPage::displacementAmount/4);
	displacement->SetValue(D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT, D2D1_CHANNEL_SELECTOR_R);
	displacement->SetValue(D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT, D2D1_CHANNEL_SELECTOR_B);
	displacement->SetInput(1, ppBitmap);
	m_d2dContext->DrawImage(displacement);

	m_d2dContext->DrawBitmap(ppMask, rectf);

	// Walk through the waveform data until we run out of samples or drawing surface.
    int y = static_cast<int>(((rectf.top + 80)/256.0f) * pLevels->waveform[0][0] * 2);
	int prevx = 0, prevy = y;
    for (int x = (int)rectf.left; x < (int)rectf.right && x < (SA_BUFFER_SIZE-1); ++x) {
        y = static_cast<int>(((rectf.top + 80)/256.0f) * pLevels->waveform[0][x - ((int)rectf.left - 1)]) * 2;
		m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F((float)x, (float)y), m_pBrush);
		prevx = x;
		prevy = y;
    }

	y = static_cast<int>(((rectf.bottom - 80)/256.0f) * pLevels->waveform[0][0] * 2);
	prevx = 0, prevy = y;
    for (int x = (int)rectf.left; x < (int)rectf.right && x < (SA_BUFFER_SIZE-1); ++x) {
        y = static_cast<int>(((rectf.bottom - 80)/256.0f) * pLevels->waveform[0][x - ((int)rectf.left - 1)]) * 2;
		m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F((float)x, (float)y), m_pBrush);
		prevx = x;
		prevy = y;
    }

	m_d2dContext->EndDraw();
}

HRESULT VizCrevice::LoadResourceBitmap(ID2D1RenderTarget *pRenderTarget, IWICImagingFactory *pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap **ppBitmap) {
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);
    HRESULT hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        // Load the resource.
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);
        hr = imageResDataHandle ? S_OK : E_FAIL;
    }

	if (SUCCEEDED(hr)) {
        // Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);
        hr = pImageFile ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        // Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
        hr = imageFileSize ? S_OK : E_FAIL;
    }

	if (SUCCEEDED(hr)) {
        // Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
    }

    if (SUCCEEDED(hr)) {
        // Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageFile), imageFileSize);
    }

	if (SUCCEEDED(hr)) {
        // Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder);
    }

	if (SUCCEEDED(hr)) {
        // Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
    }

	if (SUCCEEDED(hr)) {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }

	if (SUCCEEDED(hr)) {
		hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr)) {
        //create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}