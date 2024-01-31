/////////////////////////////////////////////////////////////////////////////
//
// wmpplugin1.h : Declaration of the CWmpplugin1
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __WMPPLUGIN1_H_
#define __WMPPLUGIN1_H_

#include "resource.h"
#include "effects.h"
#include "wmpplugin1_h.h"

#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxguid.lib")

template<class Interface>
inline void SafeRelease(
    Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// preset values
enum {
    PRESET_BARS = 0,
    PRESET_SCOPE,
    PRESET_COUNT
};

/////////////////////////////////////////////////////////////////////////////
// CWmpplugin1
class ATL_NO_VTABLE CWmpplugin1 : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CWmpplugin1, &CLSID_Wmpplugin1>,
    public IDispatchImpl<IWmpplugin1, &IID_IWmpplugin1, &LIBID_WMPPLUGIN1Lib>,
    public IWMPEffects2
{
private:
    LONG        m_nPreset;          // current preset

    HRESULT WzToColor(const WCHAR *pwszColor, COLORREF *pcrColor);
    HRESULT ColorToWz( BSTR* pbstrColor, COLORREF crColor);
    DWORD SwapBytes(DWORD dwRet);

	unsigned long long frame;
	ID2D1Factory* m_pD2DFactory;
	ID2D1DeviceContext* m_d2dContext;
	ID2D1DCRenderTarget* m_pDCRT;
	ID2D1SolidColorBrush *m_pBrush, *m_pBlackBrush;

	ID2D1BitmapRenderTarget *bitmapTarget;
	ID2D1BitmapRenderTarget *bgEffectTarget;
	ID2D1Effect *blur, *displacement, *turbulence;
	ID2D1Bitmap *bitmap;
	ID2D1Bitmap *bgEffectBitmap;
public:
    CWmpplugin1();
    ~CWmpplugin1();

DECLARE_REGISTRY_RESOURCEID(IDR_WMPPLUGIN1)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWmpplugin1)
    COM_INTERFACE_ENTRY(IWmpplugin1)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IWMPEffects)
    COM_INTERFACE_ENTRY(IWMPEffects2)
END_COM_MAP()

public:

    // CComCoClass Overrides
    HRESULT FinalConstruct();
    void FinalRelease();

    // IWmpplugin1
    STDMETHOD(get_foregroundColor)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_foregroundColor)(/*[in]*/ BSTR newVal);

    // IWMPEffects
    STDMETHOD(Render)(TimedLevel *pLevels, HDC hdc, RECT *rc);
    STDMETHOD(MediaInfo)(LONG lChannelCount, LONG lSampleRate, BSTR bstrTitle);
    STDMETHOD(GetCapabilities)(DWORD * pdwCapabilities);
    STDMETHOD(GoFullscreen)(BOOL fFullScreen) { return E_NOTIMPL; };
    STDMETHOD(RenderFullScreen)(TimedLevel *pLevels) { return E_NOTIMPL; };
    STDMETHOD(DisplayPropertyPage)(HWND hwndOwner) { return E_NOTIMPL; };
    STDMETHOD(GetTitle)(BSTR *bstrTitle);
    STDMETHOD(GetPresetTitle)(LONG nPreset, BSTR *bstrPresetTitle);
    STDMETHOD(GetPresetCount)(LONG *pnPresetCount);
    STDMETHOD(SetCurrentPreset)(LONG nPreset);
    STDMETHOD(GetCurrentPreset)(LONG *pnPreset);

    // IWMPEffects2
    STDMETHOD(SetCore)(IWMPCore * pCore);
    STDMETHOD(Create)(HWND hwndParent);
    STDMETHOD(Destroy)();
    STDMETHOD(NotifyNewMedia)(IWMPMedia *pMedia);
    STDMETHOD(OnWindowMessage)(UINT msg, WPARAM WParam, LPARAM LParam, LRESULT *plResultParam );
    STDMETHOD(RenderWindowed)(TimedLevel *pLevels, BOOL fRequiredRender );

private:
    void         ReleaseCore();

    HWND                        m_hwndParent;
    CComPtr<IWMPCore>           m_spCore;
};

#endif //__WMPPLUGIN1_H_
