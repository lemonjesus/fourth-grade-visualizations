#pragma once
#include "Visualization.h"

class VizSquares : public Visualization {
public:
	ID2D1SolidColorBrush *m_pBrush;

	VizSquares(void);
	~VizSquares(void);

	void init(ID2D1DeviceContext* m_d2dContext);
	void VizSquares::drawSquareAroundCenter(ID2D1DeviceContext* m_d2dContext, D2D1_POINT_2F center, float size);
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
