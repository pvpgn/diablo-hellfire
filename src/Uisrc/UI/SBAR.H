//****************************************************************************
//	Sbar.h
//	Diablo UI scroll bar to accompany "Doom style" lists
//	
//	By Frank Pearce
//	created 11.11.96
//****************************************************************************


//****************************************************************************
//****************************************************************************
enum _clickloc {		// defines click locations in the scroll bar
	DSB_NONE,
	DSB_UP,
	DSB_DOWN,
	DSB_PAGEUP,
	DSB_PAGEDOWN,
	DSB_THUMB
};


//****************************************************************************
//****************************************************************************
void SbarInit(HWND parent, int SbarID);
void SbarDestroy(HWND parent, int SbarID);

void SbarDraw(HWND parent, int SbarID, int Entries, int Current);

BOOL SbarUpClick(HWND sbar);
int  SbarInterpretClick(HWND sbar, int x, int y);
