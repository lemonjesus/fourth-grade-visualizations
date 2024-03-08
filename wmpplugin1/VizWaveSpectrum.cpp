#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

VizWaveSpectrum::VizWaveSpectrum() {}

VizWaveSpectrum::~VizWaveSpectrum() {
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pWhiteBrush);
}

void VizWaveSpectrum::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 1.0f, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f),&m_pWhiteBrush);
}

void VizWaveSpectrum::render(
	unsigned long long frame,
	D2D1_RECT_F rectf,
	ID2D1BitmapRenderTarget *bgEffectTarget,
	ID2D1Bitmap *bgEffectBitmap,
	ID2D1Bitmap *bitmap,
	ID2D1DeviceContext* m_d2dContext,
	TimedLevel *pLevels
) {
	m_d2dContext->BeginDraw(); // ensure foreground_pass calls EndDraw!
	m_d2dContext->Clear();

	int height = 140;
	
	// Walk through the waveform data until we run out of samples or drawing surface.
    int y = static_cast<int>(((rectf.bottom - rectf.top)/2.0f) + ((float)(pLevels->waveform[0][0] - 128)*(height/256.0f)));
	int prevx = 0, prevy = y;
    for (int x = (int)rectf.left; x < (int)rectf.right && x < (SA_BUFFER_SIZE-1); ++x) {
        y = static_cast<int>(((rectf.bottom - rectf.top)/2.0f) + ((float)(pLevels->waveform[0][x - ((int)rectf.left - 1)] - 128)*(height/256.0f)));
		m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F((float)x, (float)y), m_pBrush, 2);
		prevx = x;
		prevy = y;
    }

	m_d2dContext->DrawLine(D2D1::Point2F(rectf.left, ((rectf.bottom - rectf.top)/2.0f) - height/2), D2D1::Point2F(rectf.right, ((rectf.bottom - rectf.top)/2.0f) - height/2), m_pWhiteBrush, 3);
	m_d2dContext->DrawLine(D2D1::Point2F(rectf.left, ((rectf.bottom - rectf.top)/2.0f) + height/2), D2D1::Point2F(rectf.right, ((rectf.bottom - rectf.top)/2.0f) + height/2), m_pWhiteBrush, 3);
	m_d2dContext->EndDraw();
}