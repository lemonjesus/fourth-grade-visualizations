#include "StdAfx.h"
#include "PropertyPage.h"

#include <stdlib.h> 

PropertyPage::PropertyPage(void) {}

PropertyPage::~PropertyPage(void) {}

// defaults for the property page goes here
int PropertyPage::displacementAmount = 25;

void PropertyPage::Show(HWND hwndOwner) {
	const wchar_t CLASS_NAME[]  = L"Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Visualization Properties",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 280, 120,
		hwndOwner,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	HWND hwndLabel      = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING,
                              WC_STATIC,
                              L"Displacment:",
                              WS_CHILDWINDOW | WS_VISIBLE | SS_RIGHT,
                              12, 20,
                              120, 23,
                              hwnd,
                              NULL,
                              GetModuleHandle(NULL),
                              NULL);

    displaceEdit		= CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE | WS_EX_CONTEXTHELP,
                              WC_EDIT,
                              NULL,
                              WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT,
                              140, 20,
                              90, 23,
                              hwnd,
                              NULL,
                              GetModuleHandle(NULL),
                              NULL);

    updown				= CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING,
                              UPDOWN_CLASS,
                              NULL,
                              WS_CHILDWINDOW | WS_VISIBLE
                              | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
                              0, 0,
                              0, 0,
                              hwnd,
                              NULL,
                              GetModuleHandle(NULL),
                              NULL);

	SendMessage(updown, UDM_SETRANGE, 0, MAKELPARAM(1000, 0));
	SendMessage(updown, UDM_SETPOS, 0, displacementAmount);
    
	ShowWindow(hwnd, SW_SHOWNORMAL);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	UINT nCode;
    int iPosIndicated;
    LPNMUPDOWN lpnmud;

	switch (uMsg) {
	case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
			EndPaint(hwnd, &ps);
		}
		return 0;
	case WM_NOTIFY:
        nCode = ((LPNMHDR)lParam)->code;

        switch (nCode) {
        case UDN_DELTAPOS:
            lpnmud = (LPNMUPDOWN)lParam;
			PropertyPage::displacementAmount = lpnmud->iPos;
            break;
        }
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}