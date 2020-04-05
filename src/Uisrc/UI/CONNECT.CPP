//****************************************************************************
//
//  connect.CPP
//  Diablo UI 
//
//  By Michael Morhaime
//
//****************************************************************************

#include "pch.h"
#include "stdio.h"
#include "..\battle\bnetart.h"
#include "uisnd.h"
#include <time.h>


#define MAX_SHORT_STRING_LEN 128
#define MAX_STRING_LEN 256	    

#define DIABLO_RETAIL_ID	'DRET'
#define DIABLO_SHARE_ID		'DSHR'

#define PROGRAMID	'HRTL'

#define MIN_SPACING 2
#define NUM_PORTS (3*4)
#define PORTRAIT_HEIGHT 14

#define HERO_PORT_X		0


#define HERO_LEVEL_X1	14
#define HERO_LEVEL_X2	20
#define HERO_LEVEL_Y	5

#define COLOR_BG_HILITE (RGB(0x80,0x80,0x80))	
 

BOOL Txt2HeroInfo(char *szDesc, TUIHEROINFO *pHeroInfo);
void HeroInfo2Txt(TUIHEROINFO *pHeroInfo, char *szDesc);
DWORD GetPlayerCategory(const char *szDesc);				//### MM Diablo Patch #2  3/21/97

extern int UiOkCancelDialog(HWND parent, LPCSTR okcanceltext, BOOL bErrFlag);
extern int UiBigOkCancelDialog(HWND parent, LPCSTR okcanceltext, LPCSTR title, BOOL bErrFlag);
extern int UiBigOkDialog(HWND parent, LPCSTR oktext, LPCSTR title, BOOL errorflag);
extern int UiOkDialog(HWND parent, LPCSTR okcanceltext, BOOL bErrFlag);

static LPBYTE gBmpHeroPort = NULL;
static LPBYTE gBmpSpawnedPort = NULL;
static LPBYTE gBmpSpecial  = NULL;
static LPBYTE gBmpScratchPort = NULL;
static HTRANS sgTransNum[10] = { 0 };

static SIZE gSizeHeroPort;
static SIZE gSizeHeroNum;
static SIZE gSizeSpecial;

static int gHeroNumHgt;		// Height of individual numbers/portraits
static int gHeroPortHgt;
static int gSpecialHgt;
static int gnPortBytesSize;	// Precalulated bytes per Portrait and Digit image (for easy calculations)
static int gnDigitBytesSize;

static DWORD sgMyProgramId = 0;	// This is saved during call to CreatePlayerDesc()

enum _special_icons {
	eSPECIAL_BLIZZARD,
	eSPECIAL_SYSOP_BATTLENET,
	eSPECIAL_MODERATOR,
	eSPECIAL_SPEAKER,
	eSPECIAL_SYSOP,
	eSPECIAL_SQUELCHED,
NUM_SPECIAL_ICONS 

};


// Game Description Fields
//		"%d\r%s\r%s", bDiff, szPlayerName, szPlayerDesc
//
// Player Description Fields
//		"%c%c%c%c %d %d %d %d %d %d %d %d", PROGRAMID, wLevel, bHeroClass, bStrength, bMagic, bDexterity, bVitality, dwGold, bSpawned
enum _description_fields {
	eGAMEDESC_DIFFICULTY,
	eGAMEDESC_CREATOR_NAME,

	// These are from the Player Description
	eGAMEDESC_PROGRAMID,
	eGAMEDESC_CREATOR_LEVEL,
	eGAMEDESC_CREATOR_CLASS,
	eGAMEDESC_CREATOR_STRENGTH,
	eGAMEDESC_CREATOR_MAGIC,
	eGAMEDESC_CREATOR_DEXTERITY,
	eGAMEDESC_CREATOR_VITALITY,
	eGAMEDESC_CREATOR_GOLD,
	eGAMEDESC_CREATOR_SPAWNED
};



//===========================================================================
//===========================================================================
// DATA Used in GetDataCallback

// speed to play diablo logo
#define SML_LOGO_DELAY	54

SNET_DATA_SYSCOLORTABLE sgSysColorTbl[] = {
	{ COLOR_HIGHLIGHT, RGB(0x80, 0x80, 0x80) },
	{ COLOR_HIGHLIGHTTEXT, RGB(0xff, 0xff, 0xff) },
};
	

//===========================================================================

//===========================================================================
void FreeGlobalHeroArt(void) {
	if (gBmpHeroPort) {
		FREE(gBmpHeroPort);
		gBmpHeroPort = NULL;
	}

	if (gBmpSpawnedPort) {
		FREE(gBmpSpawnedPort);
		gBmpSpawnedPort = NULL;
	}

	for (int i=0; i<10; i++) {
		if (sgTransNum[i]) {
			STransDelete(sgTransNum[i]);
			sgTransNum[i] = NULL;
		}
	}

	if (gBmpSpecial) {
		FREE(gBmpSpecial);
		gBmpSpecial = NULL;
	}

	if (gBmpScratchPort) {
		FREE(gBmpScratchPort);
		gBmpScratchPort = NULL;
	}
}

//===========================================================================
BOOL LoadGlobalHeroArt(void) {
	LPBYTE bmpHeroNum = NULL;
	RECT rect;

	// Make sure this doesn't get loaded twice
	if (gBmpHeroPort || sgTransNum[0]) {
		return 1;
	}

	UiLoadBmpFile(
		TEXT("ui_art\\heroport.pcx"),
		&gBmpHeroPort,
		&gSizeHeroPort
	);

	UiLoadBmpFile(
		TEXT("ui_art\\spwnport.pcx"),
		&gBmpSpawnedPort,
		NULL
	);

	UiLoadBmpFile(
		TEXT("ui_art\\heronum.pcx"),
		&bmpHeroNum,
		&gSizeHeroNum
	);

	UiLoadBmpFile(
		TEXT("ui_art\\special.pcx"),
		&gBmpSpecial,
		&gSizeSpecial					// Special 'chat' icons
	);

	gHeroNumHgt = gSizeHeroNum.cy/10;
	gHeroPortHgt = PORTRAIT_HEIGHT;
	gSpecialHgt = gSizeSpecial.cy/NUM_SPECIAL_ICONS;

	gnDigitBytesSize = gSizeHeroNum.cx * gHeroNumHgt;
	gnPortBytesSize = gSizeHeroPort.cx * gHeroPortHgt;


	// Allocate space offscreen to build portraits
	gBmpScratchPort = (LPBYTE)ALLOC(gHeroPortHgt * gSizeHeroPort.cx);
	if (!gBmpScratchPort) {
		return 0;
	}


	// Build Transparencies for numbers
	memset(sgTransNum, 0, sizeof(sgTransNum));
	if (bmpHeroNum) {
		// create an HTRANS for each number
		for (int index = 0; index < 10; index++) {		

			rect.left = 0;
			rect.right = gSizeHeroNum.cx - 1;
			rect.top = index * gHeroNumHgt;
			rect.bottom = rect.top + gHeroNumHgt - 1;

			STransCreate(
				bmpHeroNum,
				gSizeHeroNum.cx,
				gHeroNumHgt,
				8,
				&rect,
				PALETTEINDEX(239),
				&sgTransNum[index]
			);
		}
		FREE(bmpHeroNum);
	}

	return 1;
}

//****************************************************************************
//****************************************************************************
BOOL CALLBACK UiArtCallback (DWORD           providerid,
                           DWORD           artid,
                           LPPALETTEENTRY  pe,
                           LPBYTE          buffer,
                           DWORD           buffersize,
                           int            *width,
                           int            *height,
                           int            *bitdepth) {

	char filename[MAX_PATH] = "";

	if (providerid == 'BNET') {
		// Check for battlenet specific art
		switch (artid) {
			case SNET_ART_BACKGROUND:		strcpy(filename,"ui_art\\bn_bkg.pcx");  break;

			case SNET_ART_JOINBACKGROUND:	strcpy(filename, "ui_art\\bnjoinbg.pcx"); break;
			case SNET_ART_HELPBACKGROUND:	strcpy(filename, "ui_art\\hpopup.pcx"); break;

			case SNET_ART_BATTLE_LOGIN_BKG:	strcpy(filename, "ui_art\\epopup.pcx"); break;
			case SNET_ART_BATTLE_CONNECT_BKG:  strcpy(filename, "ui_art\\bnconnbg.pcx"); break;			

			case SNET_ART_BATTLE_CHAT_BKG:	strcpy(filename, "ui_art\\chat_bkg.pcx"); break;
			case SNET_ART_BATTLE_BTNS:		strcpy(filename, "ui_art\\bnbuttns.pcx"); break;
			case SNET_ART_BATTLE_SELECT_CHNL_BKG:  strcpy(filename, "ui_art\\bnselchn.pcx"); break;
			case SNET_ART_BATTLE_REDLAG:	strcpy(filename, "ui_art\\redlag.pcx");  break;
			case SNET_ART_BATTLE_YELLOWLAG:	strcpy(filename, "ui_art\\yellolag.pcx");  break;
			case SNET_ART_BATTLE_GREENLAG:	strcpy(filename, "ui_art\\greenlag.pcx");  break;	

			case SNET_ART_BUTTON_XSML:		strcpy(filename,"ui_art\\but_xsm.pcx"); break;
			case SNET_ART_BUTTON_SML:		strcpy(filename,"ui_art\\but_sml.pcx"); break;
			case SNET_ART_BUTTON_MED:		strcpy(filename,"ui_art\\but_med.pcx"); break;
			case SNET_ART_BUTTON_LRG:		strcpy(filename,"ui_art\\but_lrg.pcx"); break;

			case SNET_ART_BATTLE_BADCONNECTION: strcpy(filename,"ui_art\\badconn.pcx"); break;
			case SNET_ART_BATTLE_WELCOME_AD: strcpy(filename,"ui_art\\welcome.pcx"); break;

			case SNET_ART_BATTLE_LRG_EDIT_POPUP_BKG: strcpy(filename,"ui_art\\lepopup.pcx"); break;
		}

	}
	else if (providerid == 'IPXN') {
		switch (artid) {
			case SNET_ART_BACKGROUND:		strcpy(filename, "ui_art\\ipx_bkg.pcx"); break;
		}
	}
	else if (providerid == 0) {
		switch (artid) {
			case SNET_ART_BACKGROUND:		strcpy(filename, "ui_art\\connect.pcx"); break;
		}
	}

	if (!filename[0]) {
		// Check for generic artwork
		switch (artid) {
			case SNET_ART_JOINBACKGROUND:	// fall through
			case SNET_ART_BACKGROUND:		strcpy(filename,"ui_art\\menu.pcx");  break;
			case SNET_ART_BUTTONTEXTURE:	strcpy(filename,"ui_art\\but_lrg.pcx");  break;

			case SNET_ART_POPUPBACKGROUND:	strcpy(filename,"ui_art\\lpopup.pcx");  break;
			case SNET_ART_HELPBACKGROUND:	strcpy(filename,"ui_art\\lpopup.pcx"); break;

			case SNET_ART_APP_LOGO_SML:		strcpy(filename, "ui_art\\xsmlogo.pcx"); break;

			case SNET_ART_BUTTON_XSML:		strcpy(filename,"ui_art\\but_xsm.pcx"); break;
			case SNET_ART_BUTTON_SML:		strcpy(filename,"ui_art\\but_sml.pcx"); break;
			case SNET_ART_BUTTON_MED:		strcpy(filename,"ui_art\\but_med.pcx"); break;
			case SNET_ART_BUTTON_LRG:		strcpy(filename,"ui_art\\but_lrg.pcx"); break;

			case SNET_ART_PROGRESS_BACKGROUND: strcpy(filename, "ui_art\\prog_bg.pcx"); break;
			case SNET_ART_PROGRESS_FILLER:	strcpy(filename, "ui_art\\prog_fil.pcx"); break;
			case SNET_ART_POPUPBACKGROUND_SML:	strcpy(filename,"ui_art\\spopup.pcx");  break;

			case SNET_ART_SCROLLBARARROWS:	strcpy(filename, "ui_art\\scrlarrw.pcx"); break;
			case SNET_ART_SCROLLTHUMB:		strcpy(filename, "ui_art\\scrlthmb.pcx"); break;
			case SNET_ART_SCROLLBAR:		strcpy(filename, "ui_art\\scrlbar.pcx"); break;

		}
	}

	if (filename[0])
	  return SBmpLoadImage(filename,pe,buffer,buffersize,width,height,bitdepth);

    return 0;
}

//****************************************************************************
//****************************************************************************
BOOL CALLBACK UiGetDataCallback(DWORD providerid, DWORD dataid, LPVOID buffer, DWORD buffersize, DWORD *bytesused) {
	DWORD  dwSize = 0;

	if (providerid == 'BNET') {
		switch (dataid) {
			case SNET_DATA_BATTLE_LOGODELAY:
				dwSize = sizeof(DWORD);
				if (buffer) {
					DWORD *pdword;

					if (buffersize >= dwSize) {
						pdword = (DWORD *)buffer;
						*pdword = SML_LOGO_DELAY;
					}
					else return 0;
				}
			break;
		}
	}

	
	if (!dwSize) {			// Have we filled the data in yet?
		switch (dataid) {	// No... so move on to snp nonspecific constants
			case SNET_DATA_SYSCOLORS:  
				dwSize = sizeof(sgSysColorTbl);
				if (buffer) {
					if (buffersize >= dwSize) 
						memcpy((LPBYTE)buffer, (LPBYTE)sgSysColorTbl, dwSize);
					else return 0;
				}
			break;

			case SNET_DATA_CURSORARROW:
				dwSize = sizeof(HCURSOR);

				if (buffer) {
					if (buffersize >= dwSize) {
						HCURSOR *phCursor;

						phCursor = (HCURSOR *)buffer;
						*phCursor = LoadCursor(global_hinstance, "DIABLO_ARROWCURSOR");

						if (*phCursor == NULL)
							return 0;
					}
					else return 0;
				}
			break;

			case SNET_DATA_CURSORIBEAM:
				dwSize = sizeof(HCURSOR);

				if (buffer) {
					if (buffersize >= dwSize) {
						HCURSOR *phCursor;

						phCursor = (HCURSOR *)buffer;
						*phCursor = LoadCursor(global_hinstance, "DIABLOIBEAM");

						if (*phCursor == NULL)
							return 0;
					}
					else return 0;
				}
			break;

			case SNET_DATA_CURSORLINK:
				dwSize = sizeof(HCURSOR);

				if (buffer) {
					if (buffersize >= dwSize) {
						HCURSOR *phCursor;

						phCursor = (HCURSOR *)buffer;
						*phCursor = LoadCursor(global_hinstance, "DIABLO_LINKCURSOR");

						if (*phCursor == NULL)
							return 0;
					}
					else return 0;
				}
			break;

		}
	}


	if (bytesused)
		*bytesused = dwSize;

	if (dwSize)
		return 1;

	return 0;
}

//****************************************************************************
//****************************************************************************
BOOL CALLBACK UiSoundCallback(DWORD providerid, DWORD soundid, DWORD flags) {
	switch (soundid) {
		case SNET_SND_SELECTITEM:
			UiSndPlayEnter();
			break;

		case SNET_SND_CHANGEFOCUS:
			UiSndPlayClick();
			break;
	}

	return 0;
}

typedef BOOL (CALLBACK *SNETCHECKAUTHPROC    )(DWORD,LPCSTR,LPCSTR,DWORD,LPCSTR,LPSTR,DWORD);

//****************************************************************************
//****************************************************************************
BOOL CALLBACK UiAuthCallback(DWORD dwItemType, LPCSTR szName, LPCSTR szDesc, DWORD dwUserFlags, LPCSTR szItem, LPSTR szErrorBuf, DWORD dwErrorBufSize) {
	TUIHEROINFO heroInfo;
	char szString[256];
	char szFmt[256];
	BOOL bAuth = FALSE;								// we are not authorized unless we find a reason to be
	char *p;
	char szDescCopy[SNETSPI_MAXSTRINGLENGTH*2];		// trashable copy of description string
	char szItemCopy[SNETSPI_MAXSTRINGLENGTH*2];		// trashable copy of item string
	int nDescLen;
	int nItemLen;


	// Default to NULL error string
	if (dwErrorBufSize)
		szErrorBuf[0] = 0;

	// First make trashable copies of description and item strings
	nDescLen = strlen(szDesc) + 1;
	nItemLen = strlen(szItem) + 1;
	if (nDescLen > sizeof(szDescCopy) || nItemLen > sizeof(szItemCopy)) {
		if (szErrorBuf) 
			LoadString(global_hinstance, IDS_AUTH_STRTOOLONG, szErrorBuf, dwErrorBufSize);
		return 0;
	}
	memcpy(szDescCopy, szDesc, nDescLen);
	memcpy(szItemCopy, szItem, nItemLen);

	// Put data into our heroinfo struct so we can understand it.
	if (!Txt2HeroInfo(szDescCopy, &heroInfo)) {
		if (szErrorBuf) 
			LoadString(global_hinstance, IDS_AUTH_BADHERODESC, szErrorBuf, dwErrorBufSize);
		return 0;
	}

	if (dwItemType == SNET_AUTHTYPE_CHANNEL) {
		// Blizzard employees and Sysop can go anywhere
		if ((dwUserFlags & SNET_DDPF_BLIZZARD) || (dwUserFlags & SNET_DDPF_SYSOP))
			return 1;

		for (int i=0; i<UI_NUM_CLASSES; i++) {
			LoadString(global_hinstance, IDS_WARRIOR+i, szString, sizeof(szString));
			if (strstr(szItemCopy, szString)) {
				// Found a reserved class word, make sure we are of the correct class
				if (heroInfo.heroclass != i) 
					goto CHANNEL_UNAUTHORIZED;
				
				bAuth = TRUE;
				break;
			}
		}

		// Check if there is a level requirement
		LoadString(global_hinstance, IDS_LEVEL_AUTHORIZATION, szString, sizeof(szString));
		p = strstr(szItemCopy, szString);
		if (p != NULL) {
			// There is a level requirement. Let's see if we pass
			p += strlen(szString);

			int nMinLevel = atoi(p);
			if (heroInfo.level >= nMinLevel)
				return TRUE;
		}

		// ok, bAuth contains our answer at this point
		if (bAuth)
			return TRUE;
		// fall through to Unauthorized

CHANNEL_UNAUTHORIZED:
		if (szErrorBuf) {
			int nLen;

			// Generate error string
			LoadString(global_hinstance, IDS_AUTH_CHANNELRESTRICTED, szFmt, sizeof(szFmt));
			nLen = 1+sprintf(szString, szFmt, szItemCopy); 
			if ((int)dwErrorBufSize < nLen) {
				memcpy(szErrorBuf, szString, dwErrorBufSize);
				szErrorBuf[dwErrorBufSize-1] = 0;
			}
			else
				memcpy(szErrorBuf, szString, nLen);
		}

		return 0;
	}
	else {			// SNET_AUTHTYPE_GAME
		TGAMEDATA GameData;

		// Sysop can join any game
		if (dwUserFlags & SNET_DDPF_SYSOP)
			return 1;

		Txt2GameData((char *)szItemCopy, &GameData, NULL, NULL);
		if (GameData.bDiff == DIFF_NIGHTMARE && heroInfo.level < 20) {
			if (szErrorBuf)
				LoadString(global_hinstance, IDS_NIGHTMARE_MINREQ, szErrorBuf, dwErrorBufSize);
			return 0;
		}

 		if (GameData.bDiff == DIFF_HELL && heroInfo.level < 30) {
			if (szErrorBuf)
				LoadString(global_hinstance, IDS_HELL_MINREQ, szErrorBuf, dwErrorBufSize);
			return 0;
		}

		return 1;
	}
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK UiDrawDescCallback (DWORD			providerid,
								  DWORD			itemtype,
								  LPCSTR		itemname,
								  LPCSTR		itemdescription,
								  DWORD			itemflags,
								  DWORD			drawflags,
								  DWORD			time,
								  LPDRAWITEMSTRUCT lpdis) {

	char szItemDescCopy[SNETSPI_MAXSTRINGLENGTH*2];
	char szDiff[MAX_SHORT_STRING_LEN] = "";
	BOOL bSelected = lpdis->itemState & ODS_SELECTED;
	COLORREF oldTextColor, oldBkColor;
	static sbDrawingGameDesc = FALSE;
	int nDescLen;

	// Make a trashable copy of the item description
	nDescLen = strlen(itemdescription)+1;
	if (nDescLen > sizeof(szItemDescCopy))
		return 0;
	memcpy(szItemDescCopy, itemdescription, nDescLen); 

	// First check if this is a game or player description
	if (itemtype == SNET_DRAWTYPE_GAME) {
		char szString[128];
		char szFmt[32];
		TGAMEDATA gameData;
		LPSTR pszPlayerName = NULL;		// Used to describe player who created the games
		LPSTR pszPlayerDesc = NULL;

		// Put data into our gamedata struct so we can understand it.
		if (!Txt2GameData(szItemDescCopy, &gameData, &pszPlayerName, &pszPlayerDesc))
			return 0;

		// Now display whatever we need to display
		oldTextColor = SetTextColor(lpdis->hDC, RGB(0xff, 0xff, 0xff));	// white text
		oldBkColor = SetBkColor(lpdis->hDC, (bSelected) ? COLOR_BG_HILITE : RGB(0, 0, 0));

		// get format string and create text description
		if (drawflags & SNET_DDF_INCLUDENAME) {
			// Just print game name only
			ExtTextOut(
				lpdis->hDC, 
				lpdis->rcItem.left, 
				lpdis->rcItem.top, 
				ETO_CLIPPED | ETO_OPAQUE, 
				&lpdis->rcItem, 
				itemname,
				strlen(itemname), 
				NULL);
		}
		else {
			BOOL bHaveCreatorInfo;

			bHaveCreatorInfo = ((pszPlayerName != NULL) && (pszPlayerDesc != NULL));

			// Build string for display
			if ((unsigned char)gameData.bDiff < NUM_DIFFICULTIES)
				LoadString(global_hinstance, IDS_DIFF_NORMAL+gameData.bDiff, szDiff, sizeof(szDiff));


			if ((drawflags & SNET_DDF_MULTILINE) && bHaveCreatorInfo) {
				DRAWITEMSTRUCT disCopy;
				POINT oldPos;
				int nSaveAlignMode;
				int nLineHgt;
				TEXTMETRIC tm;

				// Get font height information
				GetTextMetrics(lpdis->hDC, &tm);
				nLineHgt = tm.tmHeight + tm.tmExternalLeading;

				// Print game description on first line
				MoveToEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, &oldPos);
				nSaveAlignMode = SetTextAlign(lpdis->hDC, TA_UPDATECP);

				TextOut(lpdis->hDC, 0, 0, szDiff, strlen(szDiff));

				LoadString(global_hinstance, IDS_CREATOR, szString, sizeof(szString));
				MoveToEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top+nLineHgt, NULL);
				TextOut(lpdis->hDC, 0, 0, szString, strlen(szString));


				// make a copy of the draw item struct so we can modify the draw rect location
				disCopy = *lpdis;

				// Use Current Position as top-left of draw rect for player desc.
				GetCurrentPositionEx(lpdis->hDC, (POINT *)&disCopy.rcItem.left);
				SetTextAlign(lpdis->hDC, TA_NOUPDATECP);

				// Draw creator description on first second line
				sbDrawingGameDesc = TRUE;
				UiDrawDescCallback(
						providerid, 
						SNET_DRAWTYPE_PLAYER,
						pszPlayerName,
						pszPlayerDesc,
						0,					// No player flags
						SNET_DDF_INCLUDENAME,
						0,					// No time stamp
						&disCopy);
				sbDrawingGameDesc = FALSE;

				if (time) {
			        struct tm *newtime;
					char szTime[32];
					char szAmPm[10];
					BOOL bAfternoon;

					SetTextAlign(lpdis->hDC, TA_UPDATECP);

					// Display Creation Time info
					LoadString(global_hinstance, IDS_CREATIONTIME_FMT, szFmt, sizeof(szFmt));
					LoadString(global_hinstance, IDS_TIME, szTime, sizeof(szTime));

					newtime = localtime((time_t *)&time ); /* Convert to local time. */

					if (newtime) {	//### BUG FIX MM 1/13/97  Protect against time out of range
						bAfternoon = (newtime->tm_hour > 12);
						LoadString(global_hinstance, IDS_AM + bAfternoon, szAmPm, sizeof(szAmPm));
						if( newtime->tm_hour > 12 )        /* Convert from 24-hour */
								newtime->tm_hour -= 12;    /*   to 12-hour clock.  */
						if( newtime->tm_hour == 0 )        /* Set hour to 12 if midnight. */
								newtime->tm_hour = 12;
						sprintf(szString, szFmt, szTime, newtime->tm_hour, newtime->tm_min, szAmPm);

						MoveToEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top+2*nLineHgt, NULL);
						TextOut(lpdis->hDC, 0, 0, szString, strlen(szString));
					}
				}

				MoveToEx(lpdis->hDC, oldPos.x, oldPos.y, NULL);
				SetTextAlign(lpdis->hDC, nSaveAlignMode);
			}
			else {
				// For single line, don't worry about creator info
				TextOut(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, szDiff, strlen(szDiff));
			}
		}

		// Restore colors to hdc
		SetTextColor(lpdis->hDC, oldTextColor);
		SetBkColor(lpdis->hDC, oldBkColor);
		return 1;
	}
	else {			// if (itemtype == SNET_DRAWTYPE_PLAYER) {
		TUIHEROINFO heroInfo;
		RECT srcRect;
		int digit;
		int nLineHgt;
		int nPortOffset;
		TEXTMETRIC tm;

		// Get font height information for centering of portrait on line
		GetTextMetrics(lpdis->hDC, &tm);
		nLineHgt = tm.tmHeight;  // don't include tm.tmExternalLeading for centering purposes
		nPortOffset = (nLineHgt - gHeroPortHgt)/2 + 1;


		if (!sbDrawingGameDesc) {
			oldTextColor = SetTextColor(lpdis->hDC, RGB(0xff, 0xff, 0));	// yellow text
			oldBkColor = SetBkColor(lpdis->hDC, (bSelected) ? COLOR_BG_HILITE : RGB(0, 0, 0));
		}
		
		if (drawflags & SNET_DDF_INCLUDENAME) {
			// Draw username 
			ExtTextOut(
				lpdis->hDC, 
				lpdis->rcItem.left + HERO_PORT_X + gSizeHeroPort.cx + MIN_SPACING, 
				lpdis->rcItem.top, 
				ETO_CLIPPED | ETO_OPAQUE, 
				&lpdis->rcItem, 
				itemname,
				strlen(itemname), 
				NULL);
		}

		// Handle special user types, if necessary
		int nIcon = -1;
		if (itemflags) {

			// User is special, so display a special icon instead of a character class
			if (itemflags & SNET_DDPF_SQUELCHED) {
				nIcon = eSPECIAL_SQUELCHED;
			}
			else if (itemflags & SNET_DDPF_BLIZZARD) {
				nIcon = eSPECIAL_BLIZZARD;
			}
			else if (itemflags & SNET_DDPF_MODERATOR) {
				nIcon = eSPECIAL_MODERATOR;
			}
			else if (itemflags & SNET_DDPF_SPEAKER) {
				nIcon = eSPECIAL_SPEAKER;
			}
			else if (itemflags & SNET_DDPF_SYSOP) {
				if (providerid == 'BNET')
					nIcon = eSPECIAL_SYSOP_BATTLENET;
				else
					nIcon = eSPECIAL_SYSOP;
			}
		}

		if (nIcon >= 0 && gBmpSpecial) {
			SetRect(&srcRect, 0, 0, gSizeSpecial.cx - 1, gSpecialHgt - 1);
			OffsetRect(&srcRect, 0, gSpecialHgt * nIcon);

			// Now draw icon in place of the hero icon
			SDlgBltToWindow(
				lpdis->hwndItem, 
				NULL, 
				lpdis->rcItem.left + HERO_PORT_X, 
				lpdis->rcItem.top + nPortOffset,
				gBmpSpecial,
				&srcRect,
				&gSizeSpecial);
		}
		else {
			// Player is not a special player type.
			// Make sure that he/she is playing diablo before drawing 
			// hero information.
			if (Txt2HeroInfo(szItemDescCopy, &heroInfo)) {
				// compose portrait in offscreen portrait buffer.
				// That way we don't have to clip on each blit.

				// Check for spawned here...
				if (heroInfo.spawned) {
					if (gBmpScratchPort && gBmpSpawnedPort)
						SBltROP3(
							gBmpScratchPort,
							gBmpSpawnedPort,	// Only one class in spawned version
							gSizeHeroPort.cx,
							gHeroPortHgt,
							gSizeHeroPort.cx,
							gSizeHeroPort.cx,
							NULL,
							SRCCOPY);
				}
				else {
					if (gBmpScratchPort && gBmpHeroPort)
						SBltROP3(
							gBmpScratchPort,
							gBmpHeroPort + (gnPortBytesSize*(heroInfo.heroclass + (heroInfo.herorank*UI_NUM_CLASSES))),
							gSizeHeroPort.cx,
							gHeroPortHgt,
							gSizeHeroPort.cx,
							gSizeHeroPort.cx,
							NULL,
							SRCCOPY);
				}

				// Draw level number

				// Draw 10's digit
				digit = heroInfo.level/10;
				if (digit && gBmpScratchPort) {
					STransBlt(
						gBmpScratchPort + (gSizeHeroPort.cx*HERO_LEVEL_Y) + HERO_LEVEL_X1,
						0, 0,
						gSizeHeroPort.cx,
						sgTransNum[digit]);
				}
				
				// Draw 1's digit
				digit = heroInfo.level % 10;

				if (gBmpScratchPort)
					STransBlt(
						gBmpScratchPort + (gSizeHeroPort.cx*HERO_LEVEL_Y) + HERO_LEVEL_X2,
						0, 0,
						gSizeHeroPort.cx,
						sgTransNum[digit]);

				// Now copy portrait to the screen
				// init source rect
				SetRect(&srcRect, 0, 0, gSizeHeroPort.cx - 1, gHeroPortHgt -1);												

				if (gBmpScratchPort)
					SDlgBltToWindow(
						lpdis->hwndItem, 
						NULL, 
						lpdis->rcItem.left + HERO_PORT_X,
						lpdis->rcItem.top + nPortOffset,
						gBmpScratchPort,
						&srcRect,
						&gSizeHeroPort);
			}
		}

		// Restore colors to hdc
		if (!sbDrawingGameDesc) {
			SetTextColor(lpdis->hDC, oldTextColor);
			SetBkColor(lpdis->hDC, oldBkColor);
		}
		return 1;
	}	
}					   


//### MM Diablo Patch #2  3/21/97   BEGIN
#define DIABLO_CATEGORY_MASK	0xffff		// Directs Battle.net to match on all category bits.
//===========================================================================
// UiSelectCategory()
//
//===========================================================================
BOOL CALLBACK UiCategoryCallback (
				BOOL				userinitiated,
				SNETPROGRAMDATAPTR	programdata,
				SNETPLAYERDATAPTR	playerdata,
				SNETUIDATAPTR		interfacedata,
				SNETVERSIONDATAPTR	versiondata,
				DWORD *			categorybits,
				DWORD *			categorymask) {

	*categorymask = DIABLO_CATEGORY_MASK;		
	*categorybits = GetPlayerCategory(playerdata->playerdescription);
	return TRUE;
}



//===========================================================================
//
//===========================================================================
DWORD GetPlayerCategory(const char *szDesc) {
	TUIHEROINFO HeroInfo;
	TCHAR szDescCopy[512];
	DWORD dwCategory = 0;
	int nLevel;


	// First make a trashable copy of the player description string
	strcpy(szDescCopy, szDesc);

	if (!Txt2HeroInfo(szDescCopy, &HeroInfo)) 
		return 0;

	nLevel = HeroInfo.level;
	if (nLevel == 1)			// Level 1
		return dwCategory;

	dwCategory++;
	if (nLevel < 4)				// Level 2-3
		return dwCategory;

	dwCategory++;
	if (nLevel < 6)				// Level 4-5
		return dwCategory;

	dwCategory++;
	if (nLevel < 8)				// Level 6-7
		return dwCategory;

	dwCategory++;
	if (nLevel < 10)			// Level 8-9
		return dwCategory;
	
	dwCategory++;
	if (nLevel < 13)			// Level 10-12
		return dwCategory;

	dwCategory++;
	if (nLevel < 17)			// Level 13-16
		return dwCategory;

	dwCategory++;
	if (nLevel < 20)			// Level 17-19
		return dwCategory;

	dwCategory++;
	if (nLevel < 25)			// Level 20-24
		return dwCategory;

	dwCategory++;
	if (nLevel < 30)			// Level 25-29
		return dwCategory;

	dwCategory++;
	if (nLevel < 35)			// Level 30-34
		return dwCategory;

	dwCategory++;
	if (nLevel < 40)			// Level 35-39
		return dwCategory;

	dwCategory++;
	if (nLevel < 48)			// Level 40-47
		return dwCategory;

	dwCategory++;				// Level 48+
	return dwCategory;
}
//### MM Diablo Patch #2  3/21/97   END


//===========================================================================
// These routines will translate to and from the game description string that storm uses.
//
// The game description format is as follows: 
//		"%d\r%s\r%s", bDiff, szPlayerName, szPlayerDesc
//
// NOTE: if this format changes, update CreateGameCriteria(), and enum _description_fields
//===========================================================================
BOOL Txt2GameData(char *szDesc, TGAMEDATA *pGameData, LPSTR *ppszPlayerName, LPSTR *ppszPlayerDesc) {
	LPSTR p;

	if (szDesc[0] == 0)
		return 0;

	pGameData->bDiff = atoi(szDesc);
	if ((unsigned int)pGameData->bDiff >= NUM_DIFFICULTIES)
		return 0;

	// Find player name
	p = strchr(szDesc, '\r');
	if (p == NULL)
		return 1;
	*p++ = 0;
	if (ppszPlayerName)
		*ppszPlayerName = p;

	// Find player description
	p = strchr(p, '\r');
	if (p == NULL)
		return 1;
	*p++ = 0;
	if (ppszPlayerDesc)
		*ppszPlayerDesc = p;

	return 1;

}

//===========================================================================
void GameData2Txt(TGAMEDATA *pGameData, LPCSTR szPlayerName, LPCSTR szPlayerDesc, LPSTR szDesc, int nMaxLength) {

	// fill string with game info text, including creator's player info if it will fit.
	if (nMaxLength > (int)(strlen(szPlayerName)+strlen(szPlayerDesc)+5))
		sprintf(szDesc, "%d\r%s\r%s", pGameData->bDiff, szPlayerName, szPlayerDesc);
	else
		itoa(pGameData->bDiff, szDesc, 10);
}

//===========================================================================
// These routines will translate to and from the hero description string that storm uses.
//
// The game description format is as follows: 
//		"%d %d %d %d %d %d %d", wLevel, bHeroClass, bStrength, bMagic, bDexterity, bVitality, dwGold
//
// NOTE: if this format changes, update CreatGameCriteria(), and enum _description_fields
//===========================================================================

BOOL Txt2HeroInfo(char *szDesc, TUIHEROINFO *pHeroInfo) {
	int nLevel, nHeroClass, nHeroRank, nStrength, nMagic, nDexterity, nVitality, nGold, nSpawned;
	DWORD *pdwProgramId = (DWORD *)szDesc;

	// Reset everything, in case this routine fails
	ZeroMemory(pHeroInfo, sizeof(TUIHEROINFO));

	// Only parse the hero info string if it was built by a user game with a programid we recognize.
	if (szDesc[0] == 0 || *pdwProgramId != PROGRAMID)
		return 0;


	// If item count changes, change if constant
	if (9 != sscanf(&szDesc[4], "%d %d %d %d %d %d %d %d %d", 
					&nLevel, 
					&nHeroClass, 
					&nHeroRank,
					&nStrength, 
					&nMagic, 
					&nDexterity, 
					&nVitality, 
					&nGold,
					&nSpawned)) {
		return 0;
	}


	// Validate some of the data
	if (*pdwProgramId == PROGRAMID && nSpawned)
		return 0;

	//if ((*pdwProgramId == 'DSHR') && !nSpawned)
	//	return 0;


	//### BUG FIX MM 2/3/97  Character Level 0 is invalid also.
	if (nLevel == 0)
		return 0;
	//### END

	if ((unsigned long)nLevel > 99)
		return 0;

	if ((unsigned long)nHeroClass >= UI_NUM_CLASSES)
		return 0;

	if ((unsigned long)nHeroRank > NUM_DIFFICULTIES)
		return 0;

	if (nStrength < 0 || nMagic < 0 || nDexterity < 0 || nVitality < 0)
		return 0;

	// Now write values into hero info struct
	pHeroInfo->level = (WORD) nLevel;
	pHeroInfo->heroclass = (BYTE) nHeroClass;
	pHeroInfo->herorank = (BYTE) nHeroRank;
	pHeroInfo->strength = (BYTE) nStrength;
	pHeroInfo->magic = (BYTE) nMagic;
	pHeroInfo->dexterity = (BYTE) nDexterity;
	pHeroInfo->vitality = (BYTE) nVitality;
	pHeroInfo->gold = (DWORD) nGold;
	pHeroInfo->spawned = (BOOL) nSpawned;

	return 1;
}

//===========================================================================
void HeroInfo2Txt(TUIHEROINFO *pHeroInfo, char *szDesc) {
	DWORD *pdword;

	// fill string with hero info text
	pdword = (DWORD *)szDesc;
	*pdword = sgMyProgramId;

	sprintf(&szDesc[4], " %d %d %d %d %d %d %d %d %d", 
			pHeroInfo->level, 
			pHeroInfo->heroclass, 
			pHeroInfo->herorank,
			pHeroInfo->strength, 
			pHeroInfo->magic, 
			pHeroInfo->dexterity, 
			pHeroInfo->vitality, 
			pHeroInfo->gold,
			pHeroInfo->spawned);
}


//===========================================================================
// UiCreateGameCriteria()
//
// This routine directs the Battle.net server how to prioritize games in the join game list.
// If the game description string changes, this must change as well.
//===========================================================================
int APIENTRY UiCreateGameCriteria(TPUIHEROINFO pHeroInfo, char *szCriteria) {
	return sprintf(szCriteria, "#%d?%d", eGAMEDESC_CREATOR_LEVEL, pHeroInfo->level);
}

//***************************************************************************
//***************************************************************************
BOOL APIENTRY UiCreatePlayerDescription(TPUIHEROINFO pHeroInfo, DWORD dwProgramId, LPSTR pPlayerDesc) {
	sgMyProgramId = dwProgramId;
	HeroInfo2Txt(pHeroInfo, pPlayerDesc);
	return 1;
}


