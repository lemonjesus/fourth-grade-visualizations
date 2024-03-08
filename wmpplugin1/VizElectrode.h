#pragma once
#include "Visualization.h"

class VizElectrode : public Visualization {
public:
	ID2D1Bitmap *pScene;
	ID2D1Effect *blur;
	ID2D1SolidColorBrush *m_pStalkBrush, *m_pBlackBrush, *m_pElectricBrush;

	VizElectrode(void);
	~VizElectrode(void);

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
