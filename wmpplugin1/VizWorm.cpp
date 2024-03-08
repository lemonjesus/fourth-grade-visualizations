#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"
#include "image_loader.h"
#include <math.h>

VizWorm::VizWorm() {}

VizWorm::~VizWorm() {
	SafeRelease(&pHead);
	SafeRelease(&m_pHoleBrush);
	SafeRelease(&m_pBodyBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizWorm::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.15f, 0.11f, 0.06f, 1.0f),&m_pHoleBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.89f, 0.49f, 0.61f, 1.0f),&m_pBodyBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBlackBrush);

	for(int i = 0; i < 30; i++) segments[i] = 10;

	IWICImagingFactory *pIWICFactory = NULL;
	pHead = NULL;

	CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));
	LoadResourceBitmap(m_d2dContext, pIWICFactory, MAKEINTRESOURCE(IDB_PNG5), RT_RCDATA, 690, 420, &pHead);

	SafeRelease(&pIWICFactory);
}

void VizWorm::render(
	unsigned long long frame,
	D2D1_RECT_F rectf,
	ID2D1BitmapRenderTarget *bgEffectTarget,
	ID2D1Bitmap *bgEffectBitmap,
	ID2D1Bitmap *bitmap,
	ID2D1DeviceContext* m_d2dContext,
	TimedLevel *pLevels
) {
	m_d2dContext->BeginDraw();
	m_d2dContext->Clear(D2D1::ColorF(0.36f, 0.27f, 0.15f, 1.0f));

	// fade out the background a bit
	float centery = (rectf.bottom - rectf.top)/2.0f;
	float buffer = 100;

	for(int i = 467; i >= 0; i--) {
		m_d2dContext->DrawLine(D2D1::Point2F(i, centery - pLevels->waveform[0][i]/4 - buffer), D2D1::Point2F(i, centery + pLevels->waveform[0][i]/4 + buffer), m_pHoleBrush);
	}

	for (int x = 0; x < 690; ++x) {
		// do we put a dot here?
		if(rawnoise(x - frame) > 0.2) {
			// where do we put it?
			D2D1_ELLIPSE dot = D2D1::Ellipse(D2D1::Point2F(690 - x*5, abs(420 * rawnoise(x - frame + 1))), 5 * rawnoise(x - frame + 2), 5 * rawnoise(x - frame + 2));
			m_d2dContext->FillEllipse(&dot, m_pHoleBrush);
		}
	}

	if(frame % 3 == 0) {
		for(int i = 0; i < 29; i++) segments[i] = segments[i + 1];
		segments[29] = pLevels->waveform[0][0] / 4;

		for(int i = 0; i < 30; i++) wiggle[i] = sin((float)frame/8 + i) * 8;
	}

	m_d2dContext->DrawBitmap(pHead, D2D1::RectF(14*30 - 4, centery - 14 + wiggle[29], 14*30+51-4, centery + 14 + wiggle[29]), 1, D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

	for(int i = 0; i < 30; i++) {
		m_d2dContext->FillRectangle(D2D1::RectF(14*i, centery - segments[i] + wiggle[i], 14*(i+1), centery + segments[i] + wiggle[i]), m_pBodyBrush);
		m_d2dContext->DrawRectangle(D2D1::RectF(14*i, centery - segments[i] + wiggle[i], 14*(i+1), centery + segments[i] + wiggle[i]), m_pBlackBrush, 2);
	}

	m_d2dContext->EndDraw();
}

double rawnoise(int n) {
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}