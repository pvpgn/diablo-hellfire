//****************************************************************************
//	Doom.h
//	created 9.13.96
//****************************************************************************


//****************************************************************************
//****************************************************************************
void UiDoomButtonsInit(HWND Parent,
					   int * BtnIDs,
					   int fontnum = 0,
					   BOOL getsfocus = TRUE);
void UiDoomButtonsReset(HWND Parent,
						int * BtnIDs,
						int fontnum = 0,
					    BOOL getsfocus = TRUE);
void UiDoomStaticInit(HWND Parent, int * TextIDs, int fontnum = 0);
void UiDoomStaticReset(HWND Parent, int * TextIDs, int fontnum = 0);

void UiDoomEditInit(HWND Parent, int * EditIDs, int fontnum = 0);
void UiDoomEditReset(HWND Parent, int * EditIDs, int fontnum = 0);

void UiDoomCtrlsDestroy(HWND Parent, int * BtnIDs);

