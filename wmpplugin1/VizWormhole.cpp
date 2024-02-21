#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

VizWormhole::VizWormhole() {}

VizWormhole::~VizWormhole() {
	SafeRelease(&affine);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizWormhole::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D12DAffineTransform, &affine);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.2f),&m_pBlackBrush);
	prev_volume = 10;
}

void VizWormhole::render(
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
	
	D2D1_POINT_2F center = D2D1::Point2F((rectf.right - rectf.left)/2, (rectf.bottom - rectf.top)/2);
	D2D1_MATRIX_3X2_F rot_matrix = D2D1::Matrix3x2F::Rotation(-2.0f, center);
	D2D1_MATRIX_3X2_F scale_matrix = D2D1::Matrix3x2F::Scale(0.999, 0.999, center);
	affine->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, rot_matrix * scale_matrix);
	affine->SetValue(D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
	m_d2dContext->DrawImage(affine);
	
	unsigned char volume = (pLevels->waveform[0][0] + pLevels->waveform[0][0])/20.0;

	m_d2dContext->DrawLine(D2D1::Point2F(center.x, 15 - prev_volume), D2D1::Point2F(center.x + 10, 15 - volume), m_pBrush, 2);
	m_d2dContext->DrawLine(D2D1::Point2F(center.x, 20), D2D1::Point2F(center.x + 10, 20), m_pBrush, 2);
	m_d2dContext->DrawLine(D2D1::Point2F(center.x, 25 + prev_volume), D2D1::Point2F(center.x + 10, 25 + volume), m_pBrush, 2);
    
	int width = (int)(rectf.right - rectf.left);
	int height = (int)(rectf.bottom - rectf.top);
	D2D1_ELLIPSE border = D2D1::Ellipse(D2D1::Point2F(width/2.0f, height/2.0f), height/2 + 500, height/2 + 500);
	m_d2dContext->DrawEllipse(border, m_pBlackBrush, 1000);
	m_d2dContext->EndDraw();

	prev_volume = volume;
}