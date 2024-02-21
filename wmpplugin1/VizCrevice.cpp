#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"
#include "resource.h"

VizCrevice::VizCrevice() {}

VizCrevice::~VizCrevice() {
	SafeRelease(&affine);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizCrevice::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D12DAffineTransform, &affine);
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
	affine->SetInput(0, bgEffectBitmap);
	affine->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, D2D1::Matrix3x2F::Scale(0.995f, 0.995f, D2D1::Point2F((rectf.right - rectf.left)/2, (rectf.bottom - rectf.top)/2)));
	m_d2dContext->DrawImage(affine);

	m_d2dContext->DrawBitmap(ppBitmap, rectf);

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