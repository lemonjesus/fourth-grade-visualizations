#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

VizTheWave::VizTheWave() {}

VizTheWave::~VizTheWave() {
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
	SafeRelease(&m_pWhiteBrush);
}

void VizTheWave::init(ID2D1DeviceContext* m_d2dContext) {
	shouldClear = true;
	prevy = 0.0f;
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBlackBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f),&m_pWhiteBrush);
}

void VizTheWave::render(
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

	bgEffectTarget->BeginDraw();
	bgEffectTarget->FillRectangle(D2D1::RectF(rectf.left, rectf.top, rectf.left+62, rectf.bottom), m_pBlackBrush);
	bgEffectTarget->DrawBitmap(bitmap, D2D1::RectF(rectf.left + 1, rectf.top, rectf.right + 1, rectf.bottom));
	if (shouldClear) {
		bgEffectTarget->Clear();
		shouldClear = false;
		prevy = (rectf.bottom - rectf.top)/2.0f;
	}
	bgEffectTarget->EndDraw();

	m_d2dContext->BeginDraw();
	m_d2dContext->Clear();
	m_d2dContext->DrawBitmap(bgEffectBitmap, rectf);
	

	float y = rectf.bottom - ((pLevels->waveform[0][0] + pLevels->waveform[1][0]) / 2.0f) - 60;
	m_d2dContext->FillRectangle(D2D1::RectF(rectf.left, rectf.top, rectf.left+62, rectf.bottom), m_pBlackBrush);
	m_d2dContext->DrawLine(D2D1::Point2F((float)rectf.left - 70, (rectf.bottom - rectf.top)/2.0f), D2D1::Point2F((float)rectf.left+60, y), m_pWhiteBrush);
	m_d2dContext->DrawLine(D2D1::Point2F((float)rectf.left+61, y), D2D1::Point2F((float)rectf.left+62, prevy), m_pBrush);
	prevy = y;

	m_d2dContext->EndDraw();
}