/////////////////////////////////////////////////////////////////////////////
//
// wmpplugin1.cpp : Implementation of CWmpplugin1
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#define GDIPVER 0x0110
#include "wmpplugin1.h"
#include "color_util.h"
#include "gdiplus.h"
#include "gdipluseffects.h"
#pragma comment(lib,"gdiplus.lib")

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::CWmpplugin1
// Constructor

CWmpplugin1::CWmpplugin1() :
m_hwndParent(NULL),
m_clrForeground(0xFF0000),
m_nPreset(0),
hue(0),
bitmap(NULL),
m_pD2DFactory(NULL),
m_pDCRT(NULL),
bound(false)
{
}

/////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::~CWmpplugin1
// Destructor

CWmpplugin1::~CWmpplugin1()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWmpplugin1:::FinalConstruct
// Called when an effect is first loaded. Use this function to do one-time
// intializations that could fail (i.e. creating offscreen buffers) instead
// of doing this in the constructor, which cannot return an error.

HRESULT CWmpplugin1::FinalConstruct() {
	D2D1_FACTORY_OPTIONS options;
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &m_pD2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
		0,
		0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
    );
	m_pD2DFactory->CreateDCRenderTarget(&props, &m_pDCRT);

	m_pDCRT->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f),&m_pBrush);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWmpplugin1:::FinalRelease
// Called when an effect is unloaded. Use this function to free any
// resources allocated in FinalConstruct.

void CWmpplugin1::FinalRelease() {
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pDCRT);
	SafeRelease(&m_pD2DFactory);
    ReleaseCore();
}


//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::Render
// Called when an effect should render itself to the screen.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::Render(TimedLevel *pLevels, HDC hdc, RECT *prc) {
	HRESULT hr;
	
	if(!bound) {
		m_pDCRT->BindDC(hdc, prc);
		bound = true;
	}

	m_pDCRT->BeginDraw();

    m_pDCRT->SetTransform(D2D1::Matrix3x2F::Identity());

	HsvColor hsv;
	hsv.h = hue++;
	hsv.s = 255;
	hsv.v = 255;

	RgbColor rgb = HsvToRgb(hsv);
	m_pBrush->SetColor(D2D1::ColorF(rgb.r/255.0, rgb.g/255.0, rgb.b/255.0));

    // draw using the current preset
    switch (m_nPreset)
    {
    case PRESET_BARS:
        {
            // Walk through the frequencies until we run out of levels or drawing surface.
            for (int x = prc->left; x < prc->right && x < (SA_BUFFER_SIZE-1); ++x)
            {
                int y = static_cast<int>(((prc->bottom - prc->top)/256.0f) * pLevels->frequency[0][x - (prc->left - 1)]);
				m_pDCRT->DrawLine(D2D1::Point2F(x, prc->bottom), D2D1::Point2F(x, y), m_pBrush);
            }
        }
        break;

    case PRESET_SCOPE:
        {
            // Walk through the waveform data until we run out of samples or drawing surface.
            int y = static_cast<int>(((prc->bottom - prc->top)/256.0f) * pLevels->waveform[0][0]);
			int prevx = 0, prevy = y;
            for (int x = prc->left; x < prc->right && x < (SA_BUFFER_SIZE-1); ++x)
            {
                y = static_cast<int>(((prc->bottom - prc->top)/256.0f) * pLevels->waveform[0][x - (prc->left - 1)]);
				m_pDCRT->DrawLine(D2D1::Point2F(prevx, prevy), D2D1::Point2F(x, y), m_pBrush);
				prevx = x;
				prevy = y;
            }
        }
        break;
    }

	
    hr = m_pDCRT->EndDraw();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::MediaInfo
// Everytime new media is loaded, this method is called to pass the
// number of channels (mono/stereo), the sample rate of the media, and the
// title of the media
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::MediaInfo(LONG lChannelCount, LONG lSampleRate, BSTR bstrTitle )
{
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::GetCapabilities
// Returns the capabilities of this effect. Flags that can be returned are:
//  EFFECT_CANGOFULLSCREEN      -- effect supports full-screen rendering
//  EFFECT_HASPROPERTYPAGE      -- effect supports a property page
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::GetCapabilities(DWORD * pdwCapabilities)
{
    if (NULL == pdwCapabilities)
    {
        return E_POINTER;
    }

    *pdwCapabilities = 0;
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::GetTitle
// Invoked when a host wants to obtain the title of the effect
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::GetTitle(BSTR* bstrTitle)
{
    if (NULL == bstrTitle)
    {
        return E_POINTER;
    }

    CComBSTR bstrTemp;
    bstrTemp.LoadString(IDS_EFFECTNAME); 
        
    if ((!bstrTemp) || (0 == bstrTemp.Length()))
    {
        return E_FAIL;
    }

    *bstrTitle = bstrTemp.Detach();

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::GetPresetTitle
// Invoked when a host wants to obtain the title of the given preset
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::GetPresetTitle(LONG nPreset, BSTR *bstrPresetTitle)
{
    if (NULL == bstrPresetTitle)
    {
        return E_POINTER;
    }

    if ((nPreset < 0) || (nPreset >= PRESET_COUNT))
    {
        return E_INVALIDARG;
    }

    CComBSTR bstrTemp;
    
    switch (nPreset)
    {
    case PRESET_BARS:
        bstrTemp.LoadString(IDS_BARSPRESETNAME); 
        break;

    case PRESET_SCOPE:
        bstrTemp.LoadString(IDS_SCOPEPRESETNAME); 
        break;
    }
    
    if ((!bstrTemp) || (0 == bstrTemp.Length()))
    {
        return E_FAIL;
    }

    *bstrPresetTitle = bstrTemp.Detach();

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::GetPresetCount
// Invoked when a host wants to obtain the number of supported presets
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::GetPresetCount(LONG *pnPresetCount)
{
    if (NULL == pnPresetCount)
    {
        return E_POINTER;
    }

    *pnPresetCount = PRESET_COUNT;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::SetCurrentPreset
// Invoked when a host wants to change the index of the current preset
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::SetCurrentPreset(LONG nPreset)
{
    if ((nPreset < 0) || (nPreset >= PRESET_COUNT))
    {
        return E_INVALIDARG;
    }

    m_nPreset = nPreset;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::GetCurrentPreset
// Invoked when a host wants to obtain the index of the current preset
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::GetCurrentPreset(LONG *pnPreset)
{
    if (NULL == pnPreset)
    {
        return E_POINTER;
    }

    *pnPreset = m_nPreset;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::SetCore
// Set WMP core interface
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::SetCore(IWMPCore * pCore)
{
    HRESULT hr = S_OK;

    // release any existing WMP core interfaces
    ReleaseCore();

    // If we get passed a NULL core, this  means
    // that the plugin is being shutdown.

    if (pCore == NULL)
    {
        return S_OK;
    }

    m_spCore = pCore;

    return hr;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::Create
// Invoked when the visualization should be initialized.
//
// If hwndParent != NULL, RenderWindowed() will be called and the visualization
// should draw into the window specified by hwndParent. This will be the
// behavior when the visualization is hosted in a window.
//
// If hwndParent == NULL, Render() will be called and the visualization
// should draw into the DC passed to Render(). This will be the behavior when
// the visualization is hosted windowless (like in a skin for example).
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::Create(HWND hwndParent)
{
    m_hwndParent = hwndParent;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::Destroy
// Invoked when the visualization should be released.
//
// Any resources allocated for rendering should be released.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::Destroy()
{
    m_hwndParent = NULL;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::NotifyNewMedia
// Invoked when a new media stream begins playing.
//
// The visualization can inspect this object for properties (like name or artist)
// that might be interesting for visualization.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::NotifyNewMedia(IWMPMedia *pMedia)
{
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::OnWindowMessage
// Window messages sent to the parent window.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::OnWindowMessage(UINT msg, WPARAM WParam, LPARAM LParam, LRESULT *plResultParam )
{
    // return S_OK only if the plugin has handled the window message
    // return S_FALSE to let the defWindowProc handle the message
	return S_FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::RenderWindowed
// Called when an effect should render itself to the screen.
//
// The fRequiredRender flag specifies if an update is required, otherwise the
// update is optional. This allows visualizations that are fairly static (for example,
// album art visualizations) to only render when the parent window requires it,
// instead of n times a second for dynamic visualizations.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::RenderWindowed(TimedLevel *pLevels, BOOL fRequiredRender )
{
    // NULL parent window should not happen 
    if (NULL == m_hwndParent)
    {
        return E_UNEXPECTED;
    }

    // At this point the visualization should draw directly into the parent
    // window. This sample just calls windowless render for simplicity.

    HDC hdc = ::GetDC(m_hwndParent);

    if (NULL == hdc)
    {
        return E_FAIL;
    }

    RECT rParent = { 0 };
    ::GetClientRect(m_hwndParent, &rParent);

    Render(pLevels, hdc, &rParent);
    
    ::ReleaseDC(m_hwndParent, hdc);

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::ReleaseCore
// Release WMP core interfaces
//////////////////////////////////////////////////////////////////////////////
void CWmpplugin1::ReleaseCore()
{
    if (m_spCore)
    {
        m_spCore = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::get_foregroundColor
// Property get to retrieve the foregroundColor prop via the public interface.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::get_foregroundColor(BSTR *pVal)
{
    return ColorToWz( pVal, m_clrForeground);
}


//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::put_foregroundColor
// Property put to set the foregroundColor prop via the public interface.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CWmpplugin1::put_foregroundColor(BSTR newVal)
{
    return WzToColor(newVal, &m_clrForeground);
}


//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::WzToColor
// Helper function used to convert a string into a COLORREF.
//////////////////////////////////////////////////////////////////////////////
HRESULT CWmpplugin1::WzToColor(const WCHAR *pwszColor, COLORREF *pcrColor)
{ 
    if (NULL == pwszColor)
    {
        //NULL color string passed in
        return E_POINTER;
    }

    if (0 == lstrlenW(pwszColor))
    {
        //Empty color string passed in
        return E_INVALIDARG;
    }

    if (NULL == pcrColor)
    {
        //NULL output color DWORD passed in
        return E_POINTER;
    }
    
    if (lstrlenW(pwszColor) != 7)
    {
        //hex color string is not of the correct length
        return E_INVALIDARG;
    }

    DWORD dwRet = 0;
    for (int i = 1; i < 7; i++)
    {
        // shift dwRet by 4
        dwRet <<= 4;
        // and add in the value of this string

        if ((pwszColor[i] >= L'0') && (pwszColor[i] <= L'9'))
        {
            dwRet += pwszColor[i] - '0';
        }
        else if ((pwszColor[i] >= L'A') && (pwszColor[i] <= L'F'))
        {
            dwRet += 10 + (pwszColor[i] - L'A');
        }
        else if ((pwszColor[i] >= L'a') && (pwszColor[i] <= L'f'))
        {
            dwRet += 10 + (pwszColor[i] - L'a');
        }
        else
        {
           //Invalid hex digit in color string
            return E_INVALIDARG;
        }
    }

    *pcrColor = SwapBytes(dwRet);

    return S_OK;
} 


//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::ColorToWz
// Helper function used to convert a COLORREF to a BSTR.
//////////////////////////////////////////////////////////////////////////////
HRESULT CWmpplugin1::ColorToWz( BSTR* pbstrColor, COLORREF crColor)
{
    _ASSERT( NULL != pbstrColor );
    _ASSERT( (crColor & 0x00FFFFFF) == crColor );

    *pbstrColor = NULL;

    WCHAR wsz[8];
    HRESULT hr  = S_OK;

    swprintf_s( wsz, sizeof(wsz)/sizeof(wsz[0]), L"#%06X", SwapBytes(crColor) );
    
    *pbstrColor = ::SysAllocString( wsz );

    if (!pbstrColor)
    {
        hr = E_FAIL;
    }

    return hr;
}


//////////////////////////////////////////////////////////////////////////////
// CWmpplugin1::SwapBytes
// Used to convert between a DWORD and COLORREF.  Simply swaps the lowest 
// and 3rd order bytes.
//////////////////////////////////////////////////////////////////////////////
inline DWORD CWmpplugin1::SwapBytes(DWORD dwRet)
{
    return ((dwRet & 0x0000FF00) | ((dwRet & 0x00FF0000) >> 16) | ((dwRet & 0x000000FF) << 16));
}

