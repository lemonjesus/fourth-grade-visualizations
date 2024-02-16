#include "StdAfx.h"
#include "VizSquares.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"

VizSquares::VizSquares() {}

VizSquares::~VizSquares() {
	SafeRelease(&m_pBrush);
}

void VizSquares::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
}

void VizSquares::drawSquareAroundCenter(ID2D1DeviceContext* m_d2dContext, D2D1_POINT_2F center, float size) {
	D2D1_RECT_F rect = D2D1::RectF(center.x - size/2, center.y - size/2, center.x + size/2, center.y + size/2);
	m_d2dContext->DrawRectangle(rect, m_pBrush);
}

void VizSquares::render(
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

	m_d2dContext->BeginDraw();
	m_d2dContext->Clear();

	float square_count = 10;
	float margin = 6;
	float height = rectf.bottom - rectf.top;
	float calculated_gap = (height - margin)/square_count;
	D2D1_POINT_2F center = D2D1::Point2F((rectf.right - rectf.left)/2, height/2);
	int window = 1024/square_count;

	for(int i = 0; i < square_count; i++) {
		float size = calculated_gap * (i+1) + margin;
		float offset = 0.0f;

		for(int j = i * window; j < (i+1) * window; j++) {
			offset += ((short) pLevels->frequency[0][j]) - 127;
			offset += ((short) pLevels->frequency[1][j]) - 127;
		}

		offset /= window * 4;

		hsv.h = (frame*6 + (255/(int)square_count)*i)%255;
		rgb = HsvToRgb(hsv);
		m_pBrush->SetColor(D2D1::ColorF(rgb.r/255.0f, rgb.g/255.0f, rgb.b/255.0f));
		drawSquareAroundCenter(m_d2dContext, center, size + offset);
	}

	m_d2dContext->EndDraw();
}