#pragma once
#include "Visualization.h"

class VizWorm : public Visualization {
public:
	ID2D1Bitmap *pHead;
	ID2D1SolidColorBrush *m_pHoleBrush, *m_pBodyBrush, *m_pBlackBrush;
	unsigned char segments[30];
	char wiggle[30];

	VizWorm(void);
	~VizWorm(void);

	void init(ID2D1DeviceContext* m_d2dContext);
	void render(
		unsigned long long frame,
		D2D1_RECT_F rectf,
		ID2D1BitmapRenderTarget *bgEffectTarget,
		ID2D1Bitmap *bgEffectBitmap,
		ID2D1Bitmap *bitmap,
		ID2D1DeviceContext* m_d2dContext,
		TimedLevel *pLevels
	);
};

double rawnoise(int n);