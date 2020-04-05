//****************************************************************************
//	Sbar.cpp
//	Diablo UI scroll bar to accompany "Doom style" lists
//	
//	By Frank Pearce
//	created 11.11.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
	#define SBAR_ENABLED	1		// 1 when they work correctly

	#define ARROW_HGT		22
	#define	ARROW_FRAMES	2		// 2 up frames and 2 down frames

	#define THUMB_X			3
	#define	THUMB_Y			ARROW_HGT
	#define	THUMB_WDT		18
	#define THUMB_RANGE(x)	(x->ctrlsize.cy - x->thumbsize.cy - (2*ARROW_HGT))

	#define FSB_UP		0x00000001
	#define FSB_PGUP	0x00000002
	#define	FSB_DN		0x00000004
	#define	FSB_PGDN	0x00000008
	#define FSB_THMB	0x00000010


//****************************************************************************
//****************************************************************************
typedef struct _sbardata {
	LONG	flags;

	LPBYTE	ctrldata;		// the composed image of the scrollbar
	SIZE	ctrlsize;

	LPBYTE	bgdata;			// the scrollbar bg art
	SIZE	bgsize;

	LPBYTE	thumbdata;		// the art for the thumb
	SIZE	thumbsize;

	LPBYTE	arrowdata;		// the art for the arrows
	SIZE	arrowsize;

	int		max;			// max entries int the list
	int		current;		// currently selected entry (0 based)

} TSBAR, * TPSBAR;


//****************************************************************************
//****************************************************************************
BOOL SbarUpClick(HWND sbar) {
	TPSBAR tpSbar = (TPSBAR) GetWindowLong(sbar, GWL_USERDATA);
	if (! tpSbar) return FALSE;
	if (tpSbar->flags) {
		tpSbar->flags = 0;
		return TRUE;
	}
	return FALSE;
}


//****************************************************************************
//****************************************************************************
int SbarInterpretClick(HWND sbar, int x, int y) {
#if SBAR_ENABLED
	int thumbtop, thumbbot;
	POINT pt = { x, y};

	// confirm sbar exists
	if (! sbar) return DSB_NONE;
	if (! IsWindowVisible(sbar)) return DSB_NONE;

	// get a pointer to the sbar data
	TPSBAR tpSbar = (TPSBAR) GetWindowLong(sbar, GWL_USERDATA);
	if (! tpSbar) return DSB_NONE;

	// calculate the thumb top and bottom
	if (tpSbar->max > 1)
		thumbtop = THUMB_Y + (THUMB_RANGE(tpSbar) * tpSbar->current) / (tpSbar->max - 1);
	else
		thumbtop = THUMB_Y;
	thumbbot = thumbtop + tpSbar->thumbsize.cy;

	// convert mouse coords to sbar coords
	ScreenToClient(sbar, &pt);

	// determine location of click
	if (pt.y < ARROW_HGT) {
		tpSbar->flags = FSB_UP;
		return DSB_UP;
	}
	if (pt.y < thumbtop) {
		tpSbar->flags = FSB_PGUP;
		return DSB_PAGEUP;
	}
	if (pt.y < thumbbot) {
		tpSbar->flags = FSB_THMB;
		return DSB_THUMB;
	}
	if (pt.y < (tpSbar->ctrlsize.cy - ARROW_HGT)) {
		tpSbar->flags = FSB_PGDN;
		return DSB_PAGEDOWN;
	}
	tpSbar->flags = FSB_DN;
	return DSB_DOWN;
#else
	return DSB_NONE;
#endif
}


//****************************************************************************
//****************************************************************************
void SbarDraw(HWND parent, int SbarID, int Entries, int Current) {
#if SBAR_ENABLED
	int yoff, arrowframe;
	HWND sbar = GetDlgItem(parent, SbarID);
	if (! sbar) return;
	TPSBAR tpSbar = (TPSBAR) GetWindowLong(sbar, GWL_USERDATA);
	if (! tpSbar) return;
	if (! tpSbar->ctrldata) return;

	// save Entries and Current
	tpSbar->max		= Entries;
	tpSbar->current = Current;

	// draw the sbar bg
	if (! tpSbar->bgdata) return;
	RECT dstrect, srcrect;
	dstrect.left = srcrect.left = 0;
	dstrect.top  = srcrect.top  = 0;
	dstrect.right = tpSbar->ctrlsize.cx - 1;
	dstrect.bottom = tpSbar->ctrlsize.cy - 1;
	srcrect.right = tpSbar->ctrlsize.cx - 1;
	srcrect.bottom = tpSbar->bgsize.cy - 1;
	SBltROP3Tiled(
   		tpSbar->ctrldata,		// dest ptr
		&dstrect,				// dest rect ptr
		tpSbar->ctrlsize.cx,	// dest cx
		tpSbar->bgdata,			// src ptr
		&srcrect,				// src rect
		tpSbar->bgsize.cx,		// src cx
		0,						// src xoff
		0,						// src yoff
		NULL,					// pattern,
		SRCCOPY					// copy cmd
	);

	// draw the thumb
	if (! tpSbar->thumbdata) return;
	if (Entries > 1)
		yoff = THUMB_Y + (THUMB_RANGE(tpSbar) * Current) / (Entries - 1);
	else
		yoff = THUMB_Y;
   	SBltROP3 (
   		tpSbar->ctrldata + (yoff * tpSbar->ctrlsize.cx) + THUMB_X,		// dest ptr
		tpSbar->thumbdata,		// src ptr
		THUMB_WDT,				// blt wdth
		tpSbar->thumbsize.cy,	// blt hgt
		tpSbar->ctrlsize.cx,	// dest cx
		tpSbar->thumbsize.cx,	// src cx
		NULL,					// pattern
		SRCCOPY					// copy cmd
   	);

	// draw the up arrow
	if (tpSbar->flags & FSB_UP)
		arrowframe = 0;
	else
		arrowframe = 1;
   	SBltROP3 (
   		tpSbar->ctrldata,				// dest ptr
		tpSbar->arrowdata + (arrowframe * ARROW_HGT * tpSbar->arrowsize.cx),		// src ptr
		tpSbar->ctrlsize.cx,			// blt wdth
		ARROW_HGT,						// blt hgt
		tpSbar->ctrlsize.cx,			// dest cx
		tpSbar->arrowsize.cx,			// src cx
		NULL,							// pattern
		SRCCOPY							// copy cmd
   	);

	// draw the down arrow
	if (tpSbar->flags & FSB_DN)
		arrowframe = 2;
	else
		arrowframe = 3;
   	SBltROP3 (
   		tpSbar->ctrldata + ((tpSbar->ctrlsize.cy - ARROW_HGT) * tpSbar->ctrlsize.cx), // dest ptr
		tpSbar->arrowdata + (arrowframe * ARROW_HGT * tpSbar->arrowsize.cx),		  // src ptr
		tpSbar->ctrlsize.cx,			// blt wdth
		ARROW_HGT,						// blt hgt
		tpSbar->ctrlsize.cx,			// dest cx
		tpSbar->arrowsize.cx,			// src cx
		NULL,							// pattern
		SRCCOPY							// copy cmd
   	);


	InvalidateRect(sbar, NULL, FALSE);
#endif
}


//****************************************************************************
//****************************************************************************
void SbarInit(HWND parent, int SbarID) {
#if SBAR_ENABLED
	TPSBAR tpSbar;
	RECT rect;

	HWND sbar = GetDlgItem(parent, SbarID);
	if (! sbar) return;

	// alloc a structure to contain all the sbar's data
	tpSbar = (TPSBAR) ALLOC(sizeof(TSBAR));
	if (! tpSbar) return;
	SetWindowLong(sbar, GWL_USERDATA, (LONG) tpSbar);

	tpSbar->flags = 0;
	// set up a bmp for the ctrl
	GetClientRect(sbar, &rect);
	tpSbar->ctrlsize.cx = rect.right;
	tpSbar->ctrlsize.cy = rect.bottom;
	tpSbar->ctrldata = (LPBYTE) ALLOC(rect.right * rect.bottom);
	if (! tpSbar->ctrldata) return;
	SDlgSetBitmap(
		sbar,
		NULL,
		TEXT(""),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		tpSbar->ctrldata,
		NULL,
		tpSbar->ctrlsize.cx,
		tpSbar->ctrlsize.cy
	);

	// load some image data for the scroll bar
	UiLoadBmpFile("ui_art\\sb_bg.pcx", &tpSbar->bgdata, &tpSbar->bgsize);
	UiLoadBmpFile("ui_art\\sb_thumb.pcx", &tpSbar->thumbdata, &tpSbar->thumbsize);
	UiLoadBmpFile("ui_art\\sb_arrow.pcx", &tpSbar->arrowdata, &tpSbar->arrowsize);
#else
	HWND sbar = GetDlgItem(parent, SbarID);
	if (! sbar) return;
	ShowWindow(sbar, SW_HIDE);
#endif
}


//****************************************************************************
//****************************************************************************
void SbarDestroy(HWND parent, int SbarID) {
#if SBAR_ENABLED
	HWND sbar = GetDlgItem(parent, SbarID);
	if (! sbar) return;

	TPSBAR tpSbar = (TPSBAR) GetWindowLong(sbar, GWL_USERDATA);
	if (! tpSbar) return;

	if (tpSbar->ctrldata)
		FREE(tpSbar->ctrldata);
	if (tpSbar->bgdata)
		FREE(tpSbar->bgdata);
	if (tpSbar->thumbdata)
		FREE(tpSbar->thumbdata);
	if (tpSbar->arrowdata)
		FREE(tpSbar->arrowdata);

	FREE(tpSbar);
	SetWindowLong(sbar, GWL_USERDATA, (LONG) NULL);
#endif
}


