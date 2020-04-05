//****************************************************************************
//	Doom.cpp
//	created 10.23.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"


//****************************************************************************
//****************************************************************************
	extern int FocusWidth(void);


//****************************************************************************
//****************************************************************************
static void UiDoomTextEditDraw(HWND window) {
	TPBMP tpBmp;
	BOOL cursor;
	RECT rect;
   	TCHAR buf[256];
	TCHAR * prntstr;
   	int x, y, width, len;

	tpBmp = (TPBMP) GetWindowLong(window, GWL_USERDATA);
	if (! tpBmp) return;
	if (! tpBmp->data) return;

   	GetWindowText(window, buf, 255);
	len = strlen(buf);

	// calculate the width that can be allowed
	x = FocusWidth() + ArtFontGridWidth();
	GetClientRect(window, &rect);
	width = rect.right - (2 * FocusWidth()) - (2 * ArtFontGridWidth());

	// don't use cursor in calc of width
	if (TRUE == (cursor = SendMessage(window, DEM_CURSORON, 0, 0)))
		buf[len - 1] = 0;

	prntstr = buf;
	while ((*prntstr) && (ArtFontPixelWidth(prntstr) > width))
		prntstr++;

	// add the cursor back on
	if (cursor)
		buf[len - 1] = DIABLO_EDITCURSOR;

	y = (tpBmp->datasize.cy - ArtFontHeight()) / 2;
   	ArtFontDraw(prntstr, tpBmp, x, y);
}


//****************************************************************************
//****************************************************************************
static void UiDoomTextBtnDraw(HWND window, BOOL getsfocus) {
	TPBMP tpBmp;
   	TCHAR textbuf[256];
	TCHAR * text;
   	int x, y, len, width, maxwidth;

	tpBmp = (TPBMP) GetWindowLong(window, GWL_USERDATA);
	if (! tpBmp) return;
	if (! tpBmp->data) return;

   	GetWindowText(window, textbuf, 255);
	if (strlen(textbuf))
		text = textbuf;
	else
		text = tpBmp->text;

	width = ArtFontPixelWidth(text);

	if (getsfocus) {
		// truncate string until it fits in the region
		maxwidth = tpBmp->datasize.cx - (FocusWidth() * 2);
		len = strlen(text);
		for ( ; width > maxwidth; width = ArtFontPixelWidth(text)) {
			text[--len] = 0;
		}
	}

	x = (tpBmp->datasize.cx - width - 1) / 2;
	y = (tpBmp->datasize.cy - ArtFontHeight()) / 2;
   	ArtFontDraw(text, tpBmp, x, y);

	InvalidateRect(window, NULL, FALSE);
}


//****************************************************************************
//****************************************************************************
static void UiDoomTextDraw(HWND window, LONG style) {
	TPBMP tpBmp;
   	TCHAR textbuf[256];
	TCHAR * text;
   	int x, width;

	tpBmp = (TPBMP) GetWindowLong(window, GWL_USERDATA);
	if (! tpBmp) return;
	if (! tpBmp->data) return;

   	GetWindowText(window, textbuf, 255);
	if (strlen(textbuf))
		text = textbuf;
	else
		text = tpBmp->text;

   	if (0 != (style & SS_RIGHT)) {
   		width = ArtFontPixelWidth(text);
   		x = tpBmp->datasize.cx - width - 1;
   	}
   	else if (0 != (style & SS_CENTER)) {
   		width = ArtFontPixelWidth(text);
   		x = (tpBmp->datasize.cx - width - 1) / 2;
   	}
   	else {
   		x = 0;
   	}

   	ArtFontDraw(text, tpBmp, x, 0);

	InvalidateRect(window, NULL, FALSE);
}


//****************************************************************************
//****************************************************************************
static void UiDoomCreateBg(HWND Parent, HWND Child, int usage) {
	TPBMP ChildBmp;
	RECT rect;

	// create a TBMP for the child
	GetClientRect(Child, &rect);
	ChildBmp = UiAllocBmp();
	ChildBmp->datasize.cx = rect.right;
	ChildBmp->datasize.cy = rect.bottom;
	ChildBmp->data = (LPBYTE) ALLOC(rect.right * rect.bottom);
	SetWindowLong(Child, GWL_USERDATA, (LONG) ChildBmp);

	// set it so the ctrl uses the bmp just created
	SDlgSetBitmap(
		Child,
		NULL,
		TEXT(""),
		SDLG_STYLE_ANY,
		usage,
		ChildBmp->data,
		NULL,
		ChildBmp->datasize.cx,
		ChildBmp->datasize.cy
	);
}


//****************************************************************************
//****************************************************************************
static void UiDoomDrawBg(HWND Parent, HWND Child) {
	TPBMP ChildBmp, ParentBmp;
	RECT rect;

	ParentBmp = (TPBMP) GetWindowLong(Parent, GWL_USERDATA);
	ChildBmp = (TPBMP) GetWindowLong(Child, GWL_USERDATA);
	if (! ParentBmp) return;
	if (! ParentBmp->data) return;
	if (! ChildBmp) return;
	if (! ChildBmp->data) return;

	GetWindowRect(Child,&rect);
	ScreenToClient(Parent, (LPPOINT)&rect.left);
	ScreenToClient(Parent, (LPPOINT)&rect.right);

   	SBltROP3 (
   		ChildBmp->data,
		ParentBmp->data + (rect.top * ParentBmp->datasize.cx) + rect.left,
		ChildBmp->datasize.cx,
		ChildBmp->datasize.cy,
		ChildBmp->datasize.cx,
		ParentBmp->datasize.cx,
		NULL,
		SRCCOPY
   	);
}


//****************************************************************************
//****************************************************************************
static void UiDoomEditCreate(HWND Parent, HWND Child, int fontnum) {
	if (! Child) return;

	// create a bmp and draw parent image into it
	UiDoomCreateBg(Parent, Child, SDLG_USAGE_BACKGROUND);
	UiDoomDrawBg(Parent, Child);

	// draw the text into the bmp
   	ArtFontSet(fontnum);
	UiDoomTextEditDraw(Child);
}


//****************************************************************************
//****************************************************************************
static void UiDoomEditReset(HWND Parent, HWND Child, int fontnum) {
	if (! Child) return;

	UiDoomDrawBg(Parent, Child);

	// draw the text into the bmp
   	ArtFontSet(fontnum);
	UiDoomTextEditDraw(Child);
}


//****************************************************************************
//****************************************************************************
static void UiDoomStaticCreate(HWND Parent, HWND Child, int fontnum) {
	if (! Child) return;

	// create a bmp and draw parent image into it
	UiDoomCreateBg(Parent, Child, SDLG_USAGE_BACKGROUND);
	UiDoomDrawBg(Parent, Child);

	// draw the text into the bmp
   	ArtFontSet(fontnum);
	UiDoomTextDraw(Child, GetWindowLong(Child, GWL_STYLE));

	// save the window text in the bmp text buffer
	TCHAR textbuf[256];
	GetWindowText(Child, textbuf, 255);
	if (strlen(textbuf)) {
		UiSetBmpText((TPBMP) GetWindowLong(Child, GWL_USERDATA), textbuf);
		SetWindowText(Child, TEXT(""));
	}
}


//****************************************************************************
//****************************************************************************
static void UiDoomStaticReset(HWND Parent, HWND Child, int fontnum) {
	if (! Child) return;

	UiDoomDrawBg(Parent, Child);

	// draw the text into the bmp
   	ArtFontSet(fontnum);
	UiDoomTextDraw(Child, GetWindowLong(Child, GWL_STYLE));

	// save the window text in the bmp text buffer
	TCHAR textbuf[256];
	GetWindowText(Child, textbuf, 255);
	if (strlen(textbuf)) {
		UiSetBmpText((TPBMP) GetWindowLong(Child, GWL_USERDATA), textbuf);
		SetWindowText(Child, TEXT(""));
	}
}


//****************************************************************************
//****************************************************************************
static void UiDoomButtonCreate(HWND Parent, HWND Child, int fontnum, BOOL getsfocus) {
	if (! Child) return;

	// create a bmp and draw parent image into it
	UiDoomCreateBg(
		Parent,
		Child,
		SDLG_USAGE_BACKGROUND | SDLG_USAGE_NORMAL | SDLG_USAGE_SELECTED | SDLG_USAGE_GRAYED
	);
	UiDoomDrawBg(Parent, Child);

	// draw the text into the bmp
   	ArtFontSet(fontnum);
	UiDoomTextBtnDraw(Child, getsfocus);

	// save the window text in the bmp text buffer
	TCHAR textbuf[256];
	GetWindowText(Child, textbuf, 255);
	if (strlen(textbuf)) {
		UiSetBmpText((TPBMP) GetWindowLong(Child, GWL_USERDATA), textbuf);
		SetWindowText(Child, TEXT(""));
	}
}


//****************************************************************************
//****************************************************************************
static void UiDoomButtonReset(HWND Parent, HWND Child, int fontnum, BOOL getsfocus) {
	if (! Child) return;

	UiDoomDrawBg(Parent, Child);

	// draw the text into the bmp
   	ArtFontSet(fontnum);
	UiDoomTextBtnDraw(Child, getsfocus);

	// save the window text in the bmp text buffer
	TCHAR textbuf[256];
	GetWindowText(Child, textbuf, 255);
	if (strlen(textbuf)) {
		UiSetBmpText((TPBMP) GetWindowLong(Child, GWL_USERDATA), textbuf);
		SetWindowText(Child, TEXT(""));
	}
}


//****************************************************************************
//****************************************************************************
void UiDoomButtonsInit(HWND Parent, int * BtnIDs, int fontnum, BOOL getsfocus) {
	for ( ; *BtnIDs; BtnIDs++) {
		UiDoomButtonCreate(
			Parent,
			GetDlgItem(Parent, *BtnIDs),
			fontnum,
			getsfocus
		);
	}
}


//****************************************************************************
//****************************************************************************
void UiDoomButtonsReset(HWND Parent, int * BtnIDs, int fontnum, BOOL getsfocus) {
	for ( ; *BtnIDs; BtnIDs++) {
		UiDoomButtonReset(
			Parent,
			GetDlgItem(Parent, *BtnIDs),
			fontnum,
			getsfocus
		);
	}
}


//****************************************************************************
//****************************************************************************
void UiDoomStaticInit(HWND Parent, int * TextIDs, int fontnum) {
	for ( ; *TextIDs; TextIDs++) {
		UiDoomStaticCreate(
			Parent,
			GetDlgItem(Parent, *TextIDs),
			fontnum
		);
	}
}


//****************************************************************************
//****************************************************************************
void UiDoomStaticReset(HWND Parent, int * TextIDs, int fontnum) {
	for ( ; *TextIDs; TextIDs++) {
		UiDoomStaticReset(
			Parent,
			GetDlgItem(Parent, *TextIDs),
			fontnum
		);
	}
}


//****************************************************************************
//****************************************************************************
void UiDoomEditInit(HWND Parent, int * EditIDs, int fontnum) {
	for ( ; *EditIDs; EditIDs++) {
		UiDoomEditCreate(
			Parent,
			GetDlgItem(Parent, *EditIDs),
			fontnum
		);
	}
}


//****************************************************************************
//****************************************************************************
void UiDoomEditReset(HWND Parent, int * EditIDs, int fontnum) {
	for ( ; *EditIDs; EditIDs++) {
		UiDoomEditReset(
			Parent,
			GetDlgItem(Parent, *EditIDs),
			fontnum
		);
	}
}


//****************************************************************************
//****************************************************************************
void UiDoomCtrlsDestroy(HWND Parent, int * CtrlIDs) {
	HWND child;

	for ( ; *CtrlIDs; CtrlIDs++) {
		child = GetDlgItem(Parent, *CtrlIDs);
		if (! child) continue;
		UiFreeBmp((TPBMP) GetWindowLong(child, GWL_USERDATA));
		SetWindowLong(child, GWL_USERDATA, (LONG) NULL);
	}
}
