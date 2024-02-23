#include "stdafx.h"
#include "effects.h"
#include "color_util.h"
#include "Visualization.h"
#include "wmpplugin1.h"
#include "d2d1helper.h"
#include "image_loader.h"

VizPods::VizPods() {}

VizPods::~VizPods() {
	SafeRelease(&pPodBitmap);
}

void VizPods::init(ID2D1DeviceContext* m_d2dContext) {
	IWICImagingFactory *pIWICFactory = NULL;
	pPodBitmap = NULL;

	CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));
	LoadResourceBitmap(m_d2dContext, pIWICFactory, MAKEINTRESOURCE(IDB_PNG3), RT_RCDATA, 317, 508, &pPodBitmap);

	SafeRelease(&pIWICFactory);
}

#define POD_COUNT 16

void VizPods::render(
	unsigned long long frame,
	D2D1_RECT_F rectf,
	ID2D1BitmapRenderTarget *bgEffectTarget,
	ID2D1Bitmap *bgEffectBitmap,
	ID2D1Bitmap *bitmap,
	ID2D1DeviceContext* m_d2dContext,
	TimedLevel *pLevels
) {
	m_d2dContext->BeginDraw();
	m_d2dContext->Clear(D2D1::ColorF(1.0, 1.0, 1.0, 1.0));

	int window = 1024/POD_COUNT;
	float interval = ((rectf.right - rectf.left) - 20) / POD_COUNT;
	for(int i = 0; i < POD_COUNT; i++) {

		float offset = 0.0f;

		for(int j = i * window; j < (i+1) * window; j++) {
			offset += ((short) pLevels->frequency[0][1023-j]);
			offset += ((short) pLevels->frequency[1][1023-j]);
		}

		offset /= window;

		m_d2dContext->DrawBitmap(pPodBitmap, D2D1::RectF(10 + interval * i, rectf.bottom - offset - 50, 10 + interval * i + 30, rectf.bottom - offset));
	}
	m_d2dContext->EndDraw();
}