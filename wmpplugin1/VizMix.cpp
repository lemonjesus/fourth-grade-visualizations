#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

VizMix::VizMix() {}

VizMix::~VizMix() {
	SafeRelease(&affine);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizMix::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D12DAffineTransform, &affine);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.2f),&m_pBlackBrush);
}

void VizMix::render(
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
	bgEffectTarget->DrawBitmap(bitmap, rectf);
	bgEffectTarget->EndDraw();

	m_d2dContext->BeginDraw();
	m_d2dContext->Clear();
	affine->SetInput(0, bgEffectBitmap);
	
	D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F(0.9f, -0.1f,   0.1f, 0.9f,   8.0f, 45.0f);
	affine->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, D2D1::Matrix3x2F::Rotation(
            5.0f,
            D2D1::Point2F((rectf.right - rectf.left)/2, (rectf.bottom - rectf.top)/2)));

	m_d2dContext->DrawImage(affine);
	
	// Walk through the waveform data until we run out of samples or drawing surface.
    int y = static_cast<int>(((rectf.bottom - rectf.top)/256.0f) * pLevels->waveform[0][0]);
	int width = (int)(rectf.right - rectf.left);
	int height = (int)(rectf.bottom - rectf.top);
	int startx = (width - height)/2;
	int prevx = startx, prevy = y;
    for (int x = (int)startx; x < (int)(startx + height) && x < (SA_BUFFER_SIZE-1); ++x) {
        y = static_cast<int>(((rectf.bottom - rectf.top)/256.0f) * pLevels->waveform[0][x - ((int)rectf.left - 1)]);
		m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F((float)x, (float)y), m_pBrush);
		prevx = x;
		prevy = y;
    }

	D2D1_ELLIPSE border = D2D1::Ellipse(D2D1::Point2F(width/2.0f, height/2.0f), height/2 + 500, height/2 + 500);
	m_d2dContext->DrawEllipse(border, m_pBlackBrush, 1000);
	m_d2dContext->EndDraw();
}