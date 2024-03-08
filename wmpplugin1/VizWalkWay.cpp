#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

#define _USE_MATH_DEFINES 
#include <math.h>

VizWalkWay::VizWalkWay() {}

VizWalkWay::~VizWalkWay() {
	SafeRelease(&affine);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizWalkWay::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D12DAffineTransform, &affine);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBlackBrush);
}

void VizWalkWay::render(
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
	m_d2dContext->FillRectangle(rectf, m_pBlackBrush);
	affine->SetInput(0, bgEffectBitmap);
	affine->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, D2D1::Matrix3x2F::Scale(0.95f, 0.95f, D2D1::Point2F((rectf.right - rectf.left)/2, rectf.top)));
	m_d2dContext->DrawImage(affine);

	int y = 380;
	int width = (int)(rectf.right - rectf.left);
	int height = (int)(rectf.bottom - rectf.top);
	int startx = 130;
	int prevx = startx, prevy = y;
    for (int x = (int)startx; x <= (int)(560) && x < (SA_BUFFER_SIZE-1); ++x) {
        y = static_cast<int>(380 + (pLevels->waveform[0][x - ((int)rectf.left - 1)]-128)/4.0f);
		m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F((float)x, (float)y), m_pBrush, 2);
		prevx = x;
		prevy = y;
    }

	m_d2dContext->EndDraw();
}