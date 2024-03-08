#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

VizDefault::VizDefault() {}

VizDefault::~VizDefault() {
	SafeRelease(&blur);
	SafeRelease(&displacement);
	SafeRelease(&turbulence);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
}

void VizDefault::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur);
	m_d2dContext->CreateEffect(CLSID_D2D1DisplacementMap, &displacement);
	m_d2dContext->CreateEffect(CLSID_D2D1Turbulence, &turbulence);

	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.1f),&m_pBlackBrush);
}

void VizDefault::render(
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

	m_d2dContext->BeginDraw(); // ensure foreground_pass calls EndDraw!
	m_d2dContext->Clear();
	turbulence->SetValue(D2D1_TURBULENCE_PROP_SIZE, D2D1::Vector2F(rectf.right, rectf.bottom));
	turbulence->SetValue(D2D1_TURBULENCE_PROP_SEED, ((unsigned int)frame)/100);
	displacement->SetInput(0, bgEffectBitmap);
	displacement->SetValue(D2D1_DISPLACEMENTMAP_PROP_SCALE, (float)PropertyPage::displacementAmount);
	displacement->SetInputEffect(1, turbulence);
	ID2D1Image *output = NULL;
	displacement->GetOutput(&output);
	blur->SetInput(0, output);
	blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 1.0f);
	m_d2dContext->DrawImage(blur);
	output->Release();

	// fade out the background a bit
	m_d2dContext->FillRectangle(rectf, m_pBlackBrush);

	// Walk through the waveform data until we run out of samples or drawing surface.
    int y = static_cast<int>(((rectf.bottom - rectf.top)/256.0f) * pLevels->waveform[0][0]);
	int prevx = 0, prevy = y;
    for (int x = (int)rectf.left; x < (int)rectf.right && x < (SA_BUFFER_SIZE-1); ++x) {
        y = static_cast<int>(((rectf.bottom - rectf.top)/256.0f) * pLevels->waveform[0][x - ((int)rectf.left - 1)]);
		m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F((float)x, (float)y), m_pBrush);
		prevx = x;
		prevy = y;
    }

	m_d2dContext->EndDraw();
}