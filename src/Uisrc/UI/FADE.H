//****************************************************************************
//	fade.h
//	created 10.28.96
//****************************************************************************

#define DEFAULT_FADE_STEPS		5
#define DEFAULT_STEPS		5

//****************************************************************************
//****************************************************************************
void UiVidFade(int max, int curr);
void UiVidFadeOut(int steps);

void UiFadeInit(HWND window, BOOL turnoncursor = TRUE);
void UiAltFadeInit(HWND window);
void UiFadeStart(HWND window);

void UiFadeAbort(HWND window);
BOOL UiIsFading();
