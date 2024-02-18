#pragma once
#include "Visualization.h"

class VizTheMan : public Visualization {
public:
	ID2D1Effect *blur;
	ID2D1SolidColorBrush *m_pBrush, *m_pBlackBrush, *m_pWhiteBrush;
	D2D1_RECT_F displacedrect;
	void* men;

	VizTheMan(void);
	~VizTheMan(void);

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
