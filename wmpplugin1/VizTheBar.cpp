#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

VizTheBar::VizTheBar() {}

VizTheBar::~VizTheBar() {
	SafeRelease(&blur);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizTheBar::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.1f),&m_pBlackBrush);
}

void VizTheBar::render(
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
	blur->SetInput(0, bgEffectBitmap);
	blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 1.0f);
	m_d2dContext->DrawImage(blur);

	// fade out the background a bit
	m_d2dContext->FillRectangle(rectf, m_pBlackBrush);

	float y = rectf.bottom - ((pLevels->waveform[0][0] + pLevels->waveform[1][0]) / 2.0f) - 60;
	m_d2dContext->DrawLine(D2D1::Point2F((float)rectf.left+70, y), D2D1::Point2F((float)rectf.right-70, y), m_pBrush);

	m_d2dContext->EndDraw();
}