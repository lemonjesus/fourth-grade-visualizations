#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

#define _USE_MATH_DEFINES 
#include <math.h>

VizRipples::VizRipples() {}

VizRipples::~VizRipples() {
	SafeRelease(&affine);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizRipples::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D12DAffineTransform, &affine);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.005f),&m_pBlackBrush);
}

void VizRipples::render(
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
	affine->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, D2D1::Matrix3x2F::Scale(1.02f, 1.02f, D2D1::Point2F((rectf.right - rectf.left)/2, (rectf.bottom - rectf.top)/2)));
	m_d2dContext->DrawImage(affine);

	// fade out the background a bit
	m_d2dContext->FillRectangle(rectf, m_pBlackBrush);

	// find circles' center point
	D2D1_POINT_2F center = D2D1::Point2F((rectf.right - rectf.left)/2, (rectf.bottom - rectf.top)/2);
	float phi, next_phi, radius, next_radius;

	// draw center circle
	if(frame % 20 == 0) {
		for(int i = 0; i < 1024; i++) {
			phi = ((float)i/1024.0f)*(M_PI*2);
			radius = 10 + ((short)pLevels->waveform[0][i] + (short)pLevels->waveform[1][i]) / 12;
			next_phi = ((float)(i+1)/1024.0f)*(M_PI*2);
			if(i == 1023) next_radius = 10 + ((short)pLevels->waveform[0][0] + (short)pLevels->waveform[1][0]) / 12;
			else next_radius = 10 + ((short)pLevels->waveform[0][i+1] + (short)pLevels->waveform[1][i+1]) / 12;

			m_d2dContext->DrawLine(
				D2D1::Point2F(radius * cos(phi) + center.x, radius * sin(phi) + center.y),
				D2D1::Point2F(next_radius * cos(next_phi) + center.x, next_radius * sin(next_phi) + center.y),
				m_pBrush, 2);
		}
	}

	m_d2dContext->EndDraw();
}