//****************************************************************************
//	Focus.cpp
//	code for animating the input focus indicator (for a "Doom-like" interface
//	
//	By Frank Pearce
//	created 10.16.96
//****************************************************************************


#include "pch.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	#define MAX_FRAMES		8

	static HTRANS sgTransHandles[MAX_FRAMES];
	static SIZE sgFrameSize;
	static int sgFrame;
	static BOOL sgFocusLostBefore;
	static HWND sgLastFocusWnd;


//****************************************************************************
//****************************************************************************
void FocusSnd(HWND focus) {
	if (focus == sgLastFocusWnd) return;
	if (sgFocusLostBefore) {
		UiSndPlayClick();
	}
	sgLastFocusWnd = focus;
}


//****************************************************************************
// returns the width of a frame of the focus animation
//****************************************************************************
int FocusWidth(void) {
	return sgFrameSize.cx;
}


//****************************************************************************
//****************************************************************************
static void FocusInvalidate(HWND child) {
	RECT rect;

	GetClientRect(child, &rect);
	rect.bottom--;
	rect.right--;

	// invalidate right side of ctrl
	rect.left = rect.right - sgFrameSize.cx;
	InvalidateRect(child, &rect, FALSE);

	// invalidate left side of ctrl
	rect.left = 0;
	rect.right = sgFrameSize.cx - 1;
	InvalidateRect(child, &rect, FALSE);
}


//****************************************************************************
//****************************************************************************
void FocusErase(HWND parent, HWND child) {
	TPBMP ParentBmp, ChildBmp;
	RECT rect;
	int xoffset;

	ParentBmp = (TPBMP) GetWindowLong(parent, GWL_USERDATA);
	ChildBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! ParentBmp) return;
	if (! ChildBmp) return;
	if (! ParentBmp->data) return;
	if (! ChildBmp->data) return;

	GetWindowRect(child,&rect);
	ScreenToClient(parent, (LPPOINT)&rect.left);
	ScreenToClient(parent, (LPPOINT)&rect.right);

	// erase the left end
   	SBltROP3 (
   		ChildBmp->data,
		ParentBmp->data + (rect.top * ParentBmp->datasize.cx) + rect.left,
		sgFrameSize.cx,
		ChildBmp->datasize.cy,
		ChildBmp->datasize.cx,
		ParentBmp->datasize.cx,
		NULL,
		SRCCOPY
   	);

	// erase the right end
	xoffset = ChildBmp->datasize.cx - sgFrameSize.cx;
   	SBltROP3 (
   		ChildBmp->data + xoffset,
		ParentBmp->data + (rect.top * ParentBmp->datasize.cx) + rect.left + xoffset,
		sgFrameSize.cx,
		ChildBmp->datasize.cy,
		ChildBmp->datasize.cx,
		ParentBmp->datasize.cx,
		NULL,
		SRCCOPY
   	);

	FocusInvalidate(child);
}


//****************************************************************************
//****************************************************************************
void FocusLost(HWND parent, HWND child) {
	int id = GetWindowLong(child, GWL_ID);
	FocusErase(parent, child);
	sgFocusLostBefore = TRUE;
}


//****************************************************************************
//****************************************************************************
BOOL FocusAnimate(HWND parent, HWND child) {
	TPBMP ChildBmp;
	int xoffset, yoffset;
	RECT rect;

	if (! child) return FALSE;

	int id = GetWindowLong(child, GWL_ID);

	if (parent != GetParent(child))
		return FALSE;

	// erase the previous image
	FocusErase(parent, child);

	ChildBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! ChildBmp) return FALSE;
	if (! ChildBmp->data) return FALSE;

	GetWindowRect(child,&rect);
	ScreenToClient(parent, (LPPOINT)&rect.left);
	ScreenToClient(parent, (LPPOINT)&rect.right);

	if (sgTransHandles[sgFrame]) {
		// center the image vertically
		yoffset = (ChildBmp->datasize.cy - sgFrameSize.cy) / 2;
//		yoffset = 0;

		// draw onto the left end
		STransBlt(ChildBmp->data, 0, yoffset, ChildBmp->datasize.cx, sgTransHandles[sgFrame]);
		
		// draw onto the right end
		xoffset = ChildBmp->datasize.cx - sgFrameSize.cx;
		STransBlt(ChildBmp->data, xoffset, yoffset, ChildBmp->datasize.cx, sgTransHandles[sgFrame]);
		
		FocusInvalidate(child);
	}


	// advance to the next frame
	sgFrame++;
	if (sgFrame >= MAX_FRAMES)
		sgFrame = 0;
	return TRUE;
}


//****************************************************************************
//****************************************************************************
void FocusAnimateDestroy(void) {
 	for (int index = 0; index < MAX_FRAMES; index++) {
		if (sgTransHandles[index]) {
			STransDelete(sgTransHandles[index]);
			sgTransHandles[index] = NULL;
		}
	}
}


//****************************************************************************
//****************************************************************************
void FocusReInit(void) {
	sgFocusLostBefore = FALSE;
	sgLastFocusWnd = NULL;
}


//****************************************************************************
//****************************************************************************
void FocusAnimateInit(LPCSTR filename) {
	LPBYTE srcframes = NULL;
	SIZE srcsize;
	RECT rect;

	sgFocusLostBefore = FALSE;
	sgLastFocusWnd = NULL;

	UiLoadBmpFile(
		filename,
		&srcframes,
		&srcsize
	);

	memset(sgTransHandles, 0, sizeof(sgTransHandles));

	if (srcframes) {
		sgFrameSize.cx = srcsize.cx;
		sgFrameSize.cy = srcsize.cy / MAX_FRAMES;

		// create an HTRANS for each frame in the animation
		for (int index = 0; index < MAX_FRAMES; index++) {

			rect.left = 0;
			rect.right = sgFrameSize.cx - 1;
			rect.top = index * sgFrameSize.cy;
			rect.bottom = rect.top + sgFrameSize.cy - 1;

			STransCreate(
				srcframes,
				sgFrameSize.cx,
				sgFrameSize.cy,
				8,
				&rect,
				PALETTEINDEX(250),
				&sgTransHandles[index]
			);
		}

		FREE(srcframes);
	}
	sgFrame = 0;
}

