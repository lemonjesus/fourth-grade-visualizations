#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"
#include "d2d1helper.h"

#include <math.h>

#define MEN_COUNT 20
#define GRAVITY 2

class Man {
public:
	bool rest; // true if man can jump
	float height; // man's current height
	float prev_height; //man's height previous frame
	float velocity; // man's current velocity

	Man::Man():rest(true),height(0),prev_height(0),velocity(0) {}
	Man::~Man() {}

	void Man::render(ID2D1DeviceContext* m_d2dContext, ID2D1BitmapRenderTarget *bgEffectTarget, ID2D1SolidColorBrush* m_pBrush, ID2D1SolidColorBrush* m_pColorBrush, ID2D1SolidColorBrush* m_pBackgroundBrush, float x, float y, float target) {
		// if man is at rest, set his v0 to meet his target, otherwise, continue with the previous jump
		if(rest) {
			velocity = -sqrt(2*GRAVITY*target);
			rest = false;
		}
		height += velocity;
		velocity += GRAVITY;

		if(height >= 0) {
			rest = true;
			height = 0;
			velocity = 0;
		}

		D2D1_ELLIPSE head = D2D1::Ellipse(D2D1::Point2F(x, height + y), 10, 10);
		float army = height + y + 20 - velocity;
		if(velocity > 10) army = height + y + 20 - 10;
		if(velocity < -15) army = height + y + 20 + 15;
		m_d2dContext->FillEllipse(head, m_pBackgroundBrush);
		m_d2dContext->DrawEllipse(head, m_pBrush, 2);
		m_d2dContext->DrawLine(D2D1::Point2F(x, height + y + 10), D2D1::Point2F(x, height + y + 30), m_pBrush, 2);
		m_d2dContext->DrawLine(D2D1::Point2F(x, height + y + 20), D2D1::Point2F(x + 12, army), m_pBrush, 2);
		m_d2dContext->DrawLine(D2D1::Point2F(x, height + y + 20), D2D1::Point2F(x - 12, army), m_pBrush, 2);
		m_d2dContext->DrawLine(D2D1::Point2F(x, height + y + 30), D2D1::Point2F(x + 6, height + y + 50), m_pBrush, 2);
		m_d2dContext->DrawLine(D2D1::Point2F(x, height + y + 30), D2D1::Point2F(x - 6, height + y + 50), m_pBrush, 2);
		
		D2D1_ELLIPSE mouth = D2D1::Ellipse(D2D1::Point2F(x, height + y + 3), 4, 3);
		m_d2dContext->DrawEllipse(mouth, m_pBrush, 1);
		m_d2dContext->FillRectangle(D2D1::RectF(x - 5, height + y - 1, x + 5, height + y + 3), m_pBackgroundBrush);
		m_d2dContext->DrawLine(D2D1::Point2F(x - 3, height + y - 1), D2D1::Point2F(x - 3, height + y - 4), m_pBrush, 1);
		m_d2dContext->DrawLine(D2D1::Point2F(x + 3, height + y - 1), D2D1::Point2F(x + 3, height + y - 4), m_pBrush, 1);

		bgEffectTarget->BeginDraw();
		bgEffectTarget->DrawLine(D2D1::Point2F(x - 3, height + y - 10), D2D1::Point2F(x, prev_height + y - 10), m_pColorBrush, 2);
		bgEffectTarget->EndDraw();

		prev_height = height;
	}
};

VizTheMan::VizTheMan() {}

VizTheMan::~VizTheMan() {
	SafeRelease(&blur);
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pBlackBrush);
	SafeRelease(&m_pWhiteBrush);
	for(int i = 0; i < MEN_COUNT; i++) {
		delete ((Man**)men)[i];
	}
	free(men);
}

void VizTheMan::init(ID2D1DeviceContext* m_d2dContext) {
	m_d2dContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBlackBrush);
	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f),&m_pWhiteBrush);
	men = malloc(MEN_COUNT * sizeof(Man*));
	for(int i = 0; i < MEN_COUNT; i++) {
		((Man**)men)[i] = new Man();
	}
}

void VizTheMan::render(
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
	
	m_d2dContext->BeginDraw();
	m_d2dContext->Clear(D2D1::ColorF(1.0, 1.0, 1.0, 1.0));
	
	blur->SetInput(0, bgEffectBitmap);
	blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 0.4f);
	m_d2dContext->DrawImage(blur);
	m_d2dContext->Flush();

	bgEffectTarget->BeginDraw();
	displacedrect = D2D1::RectF(rectf.left + 3, rectf.top - 1, rectf.right + 3, rectf.bottom - 1);
	bgEffectTarget->DrawBitmap(bitmap, &displacedrect);
	bgEffectTarget->EndDraw();

	// m_d2dContext->DrawBitmap(bgEffectBitmap);

	int window = 1024/MEN_COUNT;
	float interval = ((rectf.right - rectf.left) - 120) / MEN_COUNT;
	for(int i = 0; i < MEN_COUNT; i++) {

		float offset = 0.0f;

		for(int j = i * window; j < (i+1) * window; j++) {
			offset += ((short) pLevels->frequency[0][1023-j]);
			offset += ((short) pLevels->frequency[1][1023-j]);
		}

		offset /= window * 2;

		((Man**)men)[i]->render(m_d2dContext, bgEffectTarget, m_pBlackBrush, m_pBrush, m_pWhiteBrush, 60 + interval * i, 300, offset);
	}

	// draw the trampoline
	m_d2dContext->DrawLine(D2D1::Point2F(40, 350), D2D1::Point2F(rectf.right - 40, 350), m_pBlackBrush, 2);
	m_d2dContext->DrawLine(D2D1::Point2F(40, 350), D2D1::Point2F(40, 420), m_pBlackBrush, 2);
	m_d2dContext->DrawLine(D2D1::Point2F(rectf.right - 40, 350), D2D1::Point2F(rectf.right - 40, 420), m_pBlackBrush, 2);
	m_d2dContext->DrawLine(D2D1::Point2F((rectf.right - rectf.left)/2, 350), D2D1::Point2F((rectf.right - rectf.left)/2, 390), m_pBlackBrush, 2);

	m_d2dContext->EndDraw();
}