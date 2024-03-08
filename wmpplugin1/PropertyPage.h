#pragma once

#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

class PropertyPage {
public:
	static int displacementAmount;
	
	HWND displaceEdit;
	HWND updown;

	PropertyPage(void);
	~PropertyPage(void);

	void Show(HWND hwndOwner);
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
