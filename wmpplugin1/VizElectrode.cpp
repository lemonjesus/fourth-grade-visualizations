#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"
#include "d2d1helper.h"
#include "image_loader.h"

VizElectrode::VizElectrode() {}

VizElectrode::~VizElectrode() {
	SafeRelease(&pScene);
	SafeRelease(&blur);
	SafeRelease(&m_pBlackBrush);
	SafeRelease(&m_pStalkBrush);
	SafeRelease(&m_pElectricBrush);
}

void VizElectrode::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.5, 0.1, 0.1, 0.1f),&m_pBlackBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f),&m_pStalkBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.9f, 1.0f, 1.0f),&m_pElectricBrush);
	IWICImagingFactory *pIWICFactory = NULL;
	pScene = NULL;

	CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));
	LoadResourceBitmap(m_d2dContext, pIWICFactory, MAKEINTRESOURCE(IDB_PNG4), RT_RCDATA, 690, 420, &pScene);

	SafeRelease(&pIWICFactory);
}

#define PROP_SCALE 4

void VizElectrode::render(
	unsigned long long frame,
	D2D1_RECT_F rectf,
	ID2D1BitmapRenderTarget *bgEffectTarget,
	ID2D1Bitmap *bgEffectBitmap,
	ID2D1Bitmap *bitmap,
	ID2D1DeviceContext* m_d2dContext,
	TimedLevel *pLevels
) {
	// background effects here
	bgEffectTarget->BeginDraw();
	bgEffectTarget->DrawBitmap(bitmap, rectf);
	bgEffectTarget->EndDraw();

	m_d2dContext->BeginDraw();
	m_d2dContext->Clear(D2D1::ColorF(0.3f, 0.3f, 0.3f));
	blur->SetInput(0, bgEffectBitmap);
	blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 1.0f);
	m_d2dContext->DrawImage(blur);

	m_d2dContext->FillRectangle(rectf, m_pBlackBrush);
	m_d2dContext->DrawBitmap(pScene, rectf);

	// draw the waveforms going up the poles
	// left waveform
	D2D1_POINT_2F left_center = D2D1::Point2F(122, rectf.bottom);
	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Rotation(90, left_center));
	for(int i = 0; i < 1023/PROP_SCALE; i++) {
		m_d2dContext->DrawLine(
			D2D1::Point2F(left_center.x - i, left_center.y + (pLevels->waveform[0][i*PROP_SCALE]-128)/8),
			D2D1::Point2F(left_center.x - i - 1, left_center.y + (pLevels->waveform[0][(i+1)*PROP_SCALE]-128)/8),
			m_pStalkBrush);
	}

	// right waveform 566
	D2D1_POINT_2F right_center = D2D1::Point2F(566, rectf.bottom);
	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Rotation(270, right_center));

	for(int i = 0; i < 1023/PROP_SCALE; i++) {
		m_d2dContext->DrawLine(
			D2D1::Point2F(right_center.x + i, right_center.y + (pLevels->waveform[1][i*PROP_SCALE]-128)/8),
			D2D1::Point2F(right_center.x + i + 1, right_center.y + (pLevels->waveform[1][(i+1)*PROP_SCALE]-128)/8),
			m_pStalkBrush);
	}

	// spark
	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
	int y = 94;
	int width = (int)(rectf.right - rectf.left);
	int height = (int)(rectf.bottom - rectf.top);
	int startx = 177;
	int prevx = startx, prevy = y;
    for (int x = (int)startx; x < (int)(514) && x < (SA_BUFFER_SIZE-1); ++x) {
        y = static_cast<int>(94 + (pLevels->waveform[0][x - ((int)rectf.left - 1)]-128)/2.0f);
		m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F((float)x, (float)y), m_pElectricBrush, 2);
		prevx = x;
		prevy = y;
    }

	m_d2dContext->DrawLine(D2D1::Point2F((float)prevx, (float)prevy), D2D1::Point2F(514.0f, 94.0f), m_pElectricBrush, 3);

	m_d2dContext->EndDraw();
}