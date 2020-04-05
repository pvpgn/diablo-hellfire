//****************************************************************************
//	DiabEdit.cpp
//	Diablo UI custom edit ctrl 
//	
//	By Frank Pearce
//	created 10.23.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
	#define CURSOR_TIMER_ID		1
	#define CURSOR_TIMER_DELAY	500

	#define MAX_RESTRICTED		256

	#define LIMITPROP			"LIMIT"
	#define CURSORFLAGPROP		"CURSOR"
	#define RESTRICTEDPROP		"RESTRICTED"


//****************************************************************************
//****************************************************************************
static void DiabEditNotify(HWND window, int notifycode) {
	SendMessage(
		GetParent(window),
		WM_COMMAND,
		MAKELONG(GetWindowLong(window, GWL_ID), notifycode),
		(LPARAM)window
	);
}


//****************************************************************************
//****************************************************************************
void DiabEditPaint (HWND window) {
	PAINTSTRUCT ps;
	HDC         dc = BeginPaint(window,&ps);

	// PAINT THE BITMAP
	SDlgDrawBitmap(window,SDLG_USAGE_BACKGROUND,(HRGN)0);

	EndPaint(window,&ps);
}


//****************************************************************************
//****************************************************************************
static void DiabEditFlashCursor(HWND window) {
	char buffer[256] = "";
	BOOL flag;
	int len;

	flag = (BOOL) GetProp(window, CURSORFLAGPROP);
	if (flag) {
		flag = FALSE;
		SetProp(window, CURSORFLAGPROP, (HANDLE) flag);
 		DiabEditNotify(window, DEN_UPDATE);
	}
	else {
		flag = TRUE;
		SetProp(window, CURSORFLAGPROP, (HANDLE) flag);

		GetWindowText(window, buffer, 255);
		buffer[254] = 0;
		len = strlen(buffer);

		buffer[len] = DIABLO_EDITCURSOR;
		buffer[len+1] = 0;
		SetWindowText(window, buffer);
 		DiabEditNotify(window, DEN_UPDATE);

		buffer[len] = 0;
		SetWindowText(window, buffer);
	}
}


//****************************************************************************
//****************************************************************************
static void DiabEditHandleChar(HWND window, WPARAM wparam, LPARAM lparam) {
	unsigned char c;
	LPSTR restricted;
	char buffer[256] = "";

	c = wparam & 0xff;

	if (c != '\b') {
		// prescreen some characters that should never be allowed
		if (c < ' ') return;
		if ((c > '~') && (c < 192)) return;

		// screen user specified restricted chars
		restricted = (LPSTR) GetProp(window, RESTRICTEDPROP);
		if (restricted) {
			while (*restricted) {
				if (c == *restricted) return;
				restricted++;
			}
		}
	}

	// get the current string
	GetWindowText(window,buffer,255);
	buffer[254] = 0;
	int len = strlen(buffer);

	switch (c) {

		case '\b':
			if (len) {
				buffer[len-1] = 0;
				SetWindowText(window,buffer);
			}
		break;

		default:
			// add the new char to the string
			if (len < (int) GetProp(window, LIMITPROP)) {
				buffer[len++] = c;
				buffer[len]   = 0;
				SetWindowText(window,buffer);
			}
		break;
	}

	DiabEditFlashCursor(window);
}


//****************************************************************************
//****************************************************************************
static void DiabEditHandleKeydown(HWND window, WPARAM wparam, LPARAM lparam) {
	unsigned char c;
	char buffer[256] = "";

   	// get the current string
   	GetWindowText(window,buffer,255);
   	buffer[254] = 0;
   	int len = strlen(buffer);

	switch (wparam) {
		case VK_LEFT:
			c = wparam & 0xff;
			if (len) {
				buffer[len-1] = 0;
				SetWindowText(window,buffer);
			}
			DiabEditFlashCursor(window);
		break;

		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		default:
			// don't do anything with these
		break;
	}
}


//****************************************************************************
//****************************************************************************
static void DiabEditSetRestricted(HWND window, LPSTR newrestricted) {
	LPSTR oldrestricted = (LPSTR) GetProp(window, RESTRICTEDPROP);

	if (oldrestricted) {
		strncpy(oldrestricted, newrestricted, MAX_RESTRICTED-1);
		oldrestricted[MAX_RESTRICTED-1] = 0;
	}
}


//****************************************************************************
//****************************************************************************
static void DiabEditCreate(HWND window) {
	LPSTR restricted;

	SDlgSetTimer(window, CURSOR_TIMER_ID, CURSOR_TIMER_DELAY, NULL);
	SetProp(window, CURSORFLAGPROP, (HANDLE) FALSE);

	restricted = (LPSTR) ALLOC(MAX_RESTRICTED);
	if (!restricted)
		return;
	restricted[0] = 0;
	SetProp(window, RESTRICTEDPROP, (HANDLE) restricted);
}


//****************************************************************************
//****************************************************************************
static void DiabEditDestroy(HWND window) {
	SDlgKillTimer(window, CURSOR_TIMER_ID);

	RemoveProp(window, LIMITPROP);
	RemoveProp(window, CURSORFLAGPROP);
	LPSTR restricted = (LPSTR) RemoveProp(window, RESTRICTEDPROP);
	if (restricted) {
		FREE(restricted);
	}
}


//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK DiabEditWndProc (HWND   window,
										 UINT   message,
										 WPARAM wparam,
										 LPARAM lparam) {
	switch (message) {

		case WM_CREATE:
			DiabEditCreate(window);
		break;

		case WM_DESTROY:
			DiabEditDestroy(window);
		break;

		case WM_GETDLGCODE:
		return (DLGC_WANTCHARS | DLGC_WANTARROWS);

		case WM_CHAR:
			DiabEditHandleChar(window, wparam, lparam);
		break;

		case WM_KEYDOWN:
			DiabEditHandleKeydown(window, wparam, lparam);
		return 0;

		case WM_LBUTTONDOWN:
			SetFocus(window);
		break;

		case WM_SETFOCUS:
			DiabEditNotify(window, DEN_SETFOCUS);
		break;

		case WM_PAINT:
			DiabEditPaint(window);
		return 0;

		case WM_TIMER:
			DiabEditFlashCursor(window);
		return 0;

		case DEM_CLEAR:
			SetWindowText(window, "");
			DiabEditNotify(window, DEN_UPDATE);
		return 0;

		case DEM_LIMITTEXT:
			SetProp(window, LIMITPROP, (HANDLE) wparam);
		return 0;

		case DEM_GETLIMITTEXT:
		return (LRESULT) GetProp(window, LIMITPROP);

		case DEM_CURSORON:
		return (LRESULT) GetProp(window, CURSORFLAGPROP);

		case DEM_RESTRICTCHARS:
			DiabEditSetRestricted(window, (LPSTR) lparam);
		return 0;
	}
	return DefWindowProc(window,message,wparam,lparam);
}


//****************************************************************************
//*
//*  EXPORTED FUNCTIONS
//*
//****************************************************************************


//****************************************************************************
//****************************************************************************
void DiabEditRegisterClass (void) {
	WNDCLASS wndclass;
	ZeroMemory(&wndclass,sizeof(WNDCLASS));
	wndclass.style         = CS_CLASSDC;
	wndclass.lpfnWndProc   = DiabEditWndProc;
	wndclass.hInstance     = (HINSTANCE)GetModuleHandle(NULL);
	wndclass.lpszClassName = "DIABLOEDIT";
	RegisterClass(&wndclass);
}
