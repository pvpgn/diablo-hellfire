//****************************************************************************
//	local.cpp
//	created 9.13.96
//****************************************************************************


//****************************************************************************
//****************************************************************************
extern HINSTANCE global_hinstance;


//****************************************************************************
//****************************************************************************
	enum _modes {
		SINGLEPLAYER,
		MULTIPLAYER,
	};

	#define BMP_TEXT_LEN	256
	typedef struct _bmp {
		LPBYTE	data;
		SIZE	datasize;
		LONG	userdata;
		TCHAR	text[BMP_TEXT_LEN];
	} TBMP, * TPBMP;



typedef struct _gameinfo TGAMEINFO;
struct _gameinfo {
	char szName[MAX_GAME_LEN];
	char szPassword[MAX_PASSWORD_LEN];
};



//****************************************************************************
//****************************************************************************
#define PALETTE_REGISTERS	256
PALETTEENTRY * UiPaletteEntry(int index);
void UiClearPalette(PALETTEENTRY * pe);


//****************************************************************************
//****************************************************************************
BOOL LoadArtFile (	HWND     window,
					HWND	 parent,
					LPCTSTR  controltype,
					DWORD	 controlstyle,
					LONG	 usageflags,
					LPCTSTR  filename,
					LPBYTE  *bitmapbits,
					SIZE	*bitmapsize = NULL,
					BOOL	 updatepal = TRUE);

BOOL UiLoadBmpFile (LPCTSTR filename,
					LPBYTE  *bitmapbits,
					SIZE    *bitmapsize);

//****************************************************************************
BOOL UiSetStaticBmp(HWND	parent,
					int		child_id,
					LPBYTE	bmpbits,
					SIZE	*bmpsize);

//****************************************************************************
void UiCalcNewRect(RECT *rect,
				   int xoffset,
				   int yoffset);

//****************************************************************************
void UiSetBtnBmp(HWND button,
				 DWORD usage,
				 RECT *rect,
				 LPBYTE bmpbits,
				 SIZE *bmpsize);

void UiSetButtonBitmaps(HWND parent,
						int *btn_ids,
						LPBYTE bmpbits,
						SIZE *bmpsize);

//****************************************************************************
void UiSetTextYellow(HDC hdc);

//****************************************************************************
BOOL UiIsPtInWindow(HWND parent, HWND window, int xpos, int ypos);

//****************************************************************************
void UiCreateIdentityPal(void);
void UiFreeIdentityPal(void);
HPALETTE UiGetIdentityPal(void);

//****************************************************************************
void UiLoadCursor(void);
void UiDestroyCursor(void);
void UiSetCursor(void);
void UiClearCursor(void);

void UiSetWindowsCursors(HWND hWnd);

//****************************************************************************
void UiPaintBtn(HWND window);
void UiOnPaintBtns(HWND window, int * BtnIDs);
void UiActiveDownBtns(HWND window, int * BtnIDs);

//****************************************************************************
void UiFlushIOMessages(HWND window);


BOOL Txt2GameData(char *szDesc, TGAMEDATA *pGameData, LPSTR *ppszPlayerName, LPSTR *ppszPlayerDesc);
void GameData2Txt(TGAMEDATA *pGameData, LPCSTR szPlayerName, LPCSTR szPlayerDesc, LPSTR szDesc, int nMaxLength);

//****************************************************************************
TPBMP UiAllocBmp(void);
void  UiFreeBmp(TPBMP tpBmp);
void  UiSetBmpText(TPBMP tpBmp, LPCSTR text);

//****************************************************************************
BOOL UiIsSpawned(void);

//****************************************************************************
void FocusAnimateInit(LPCTSTR filename);
void FocusReInit(void);
void FocusAnimateDestroy(void);
BOOL FocusAnimate(HWND parent, HWND child);
void FocusLost(HWND parent, HWND child);
void FocusErase(HWND parent, HWND child);
void FocusSnd(HWND focus);

//****************************************************************************
BOOL UiRegLoadString (LPCTSTR keyname,
                      LPCTSTR valuename,
                      DWORD   flags,
                      LPTSTR  buffer,
                      DWORD   bufferchars);
