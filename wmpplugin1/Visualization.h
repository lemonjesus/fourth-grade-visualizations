#pragma once

#include "effects.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d2d1effects.h>

class Visualization {
public:
	Visualization(void);
	~Visualization(void);

	// Initialize all of the one-time things you need (brushes, bitmaps, etc.)
	virtual void init(ID2D1DeviceContext* m_d2dContext) = 0;

	// Called for each frame you need to draw.
	virtual void render(
		unsigned long long frame, // counts up each time this function is called
		D2D1_RECT_F rectf, // the size of the area you can render to
		ID2D1BitmapRenderTarget *bgEffectTarget, // target for setting up the background effect render
		ID2D1Bitmap *bgEffectBitmap, // temporary bitmap holding the previous frame for background effects
		ID2D1Bitmap *bitmap, // the previous frame (to draw to bgEffectTarget, producing bgEffectBitmap which gets fed through the effect chain)
		ID2D1DeviceContext* m_d2dContext, // for drawing the foreground
		TimedLevel *pLevels // audio data
	) = 0;
};
