//****************************************************************************
//	SelHero.cpp
//	Diablo UI select hero dialogs
//	
//	By Frank Pearce
//	created 9.19.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern int UiCreaHeroDlg(HWND parent, GETDEFHERO getstatsfcn, LPSTR newname, int * charclass);
	extern int UiOkCancelDialog(HWND parent, LPCSTR okcanceltext, BOOL errorflag);
	extern int UiOkDialog(HWND parent, LPCSTR oktext, BOOL errorflag);
	extern int SelYesNoDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);
	extern int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);

	extern BOOL CALLBACK SelListDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	extern BOOL CALLBACK SelClassDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	extern BOOL CALLBACK SelLoadDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	extern BOOL CALLBACK SelDiffDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	extern BOOL CALLBACK EnterNameDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

	extern void HeroInfo2Txt(TUIHEROINFO *pHeroInfo, char *szDesc);

	extern void SelDiffLoadFocus(BOOL loadfocus);
	static void SelHeroAbort(HWND window, int retval);


//****************************************************************************
//****************************************************************************
	extern void TitleLogoDestroy(void);
	extern void TitleLogoInit(HWND window);
	extern void TitleFramesInit(HWND window, LPCTSTR filename);
	extern void TitleLogoAnimate(HWND window);

	#define MILLISEC_PER_SEC	1000

	#define LOGO_TIMER_ID		2
	#define LOGO_FPS			17
	#define LOGO_TIMER_DELAY	55


//****************************************************************************
//****************************************************************************
	enum _wm_states { 
		WM_STATE_SELLIST = (WM_USER + 2000),
	
		WM_STATE_SELCLASS,
		WM_STATE_ENTERNAME,

		WM_STATE_SELLOAD,
		WM_STATE_SELDIFF,

		WM_STATE_CONTINUE,
		WM_STATE_NEWHERO,
		WM_STATE_DELETEHERO
	};


//****************************************************************************
//****************************************************************************
	static LPBYTE sgHerosBmp = NULL;
	static SIZE sgHerosSize;

	static int sgMode;					// single or multi player

	//*** strings to put the hero stats in
	static char selheroname[MAX_NAME_LEN];
	static char selherolevel[8];
	static char selherostr[4];
	static char selheromagic[4];
	static char selherodex[4];
	static char selherovit[4];
	static BOOL hassaved;
	
	static ENUMHEROS  sgEnumFcn;
	static CREATEHERO sgCreateFcn;
	static DELETEHERO sgDeleteFcn;
	static GETDEFHERO sgGetStatsFcn;
	static bool		  sgAllowBard;
	static bool		  sgAllowBarbarian;
	
	static TPUIHEROINFO sgHeroList;
	static TUIHEROINFO sgNewHero;		// for use when a new hero is being created
	static int sgListLen;
	static int sgDifficulty;



//****************************************************************************
//****************************************************************************
	static int sgTextIDs[] = {
		IDC_DLGTITLE,
		0
	};
	static int sgLblIDs[] = {
		IDC_LEVELLBL,
		IDC_STRLBL,
		IDC_MAGLBL,
		IDC_DEXLBL,
		IDC_VITLBL,
		0
	};
	static int sgDataIDs[] = {
		IDC_CHARLEVEL,
		IDC_CHARSTR,
		IDC_CHARMAGIC,
		IDC_CHARDEX,
		IDC_CHARVIT,
		0
	};


//****************************************************************************
//****************************************************************************
TPUIHEROINFO GetHeroList(void) {
	return sgHeroList;
}
int GetNumHeros(void) {
	return sgListLen;
}


//****************************************************************************
//****************************************************************************
void SelHeroSetDiff(int difficulty) {
	sgDifficulty = difficulty;
}


//****************************************************************************
//****************************************************************************
LPCTSTR SelectedName(void) {
	return selheroname;
}


//****************************************************************************
//****************************************************************************
TPUIHEROINFO SelHeroAllocNode(void) {
	return (TPUIHEROINFO) ALLOC(sizeof(TUIHEROINFO));
}


//****************************************************************************
//****************************************************************************
static void SelHeroFreeNode(TPUIHEROINFO node) {
	if (node) {
		FREE(node);
	}
}


//****************************************************************************
//****************************************************************************
static void SelHeroFreeList(TPUIHEROINFO list) {
	TPUIHEROINFO next;

	while (list) {
		next = list->next;
		SelHeroFreeNode(list);
		list = next;
	}
}


//****************************************************************************
//****************************************************************************
TPUIHEROINFO SelHeroAddNode(TPUIHEROINFO list, TPUIHEROINFO node) {
	node->next = list;
	return node;
}


//****************************************************************************
//****************************************************************************
static TPUIHEROINFO SelHeroDeleteNode(TPUIHEROINFO list, LPCSTR namekey) {
	TPUIHEROINFO current, previous, found;

	current = list;
	previous = NULL;
	found = NULL;

	while (current && !found) {
		if (strcmp(current->name, namekey) == 0) {
			found = current;
		}
		else {
			previous = current;
			current = current->next;
		}
	}

	if (found) {
		if (previous)
			previous->next = found->next;
		else
			list = found->next;
		SelHeroFreeNode(found);
	}

	return list;
}


//****************************************************************************
//****************************************************************************
static TPUIHEROINFO SelHeroFindNode(TPUIHEROINFO list, LPCSTR namekey) {
	TPUIHEROINFO current = list;

	while (current) {
		if (_stricmp(current->name, namekey) == 0)
			break;
		current = current->next;
	}

	return current;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelHeroEnumHero(TPUIHEROINFO heroinfo) {
	TPUIHEROINFO newhero = SelHeroAllocNode();
	if (! newhero)
		return 0;
	memcpy(newhero, heroinfo, sizeof(TUIHEROINFO));
	newhero->next = NULL;
	sgHeroList = SelHeroAddNode(sgHeroList, newhero);
	sgListLen++;
	return 1;
}


//****************************************************************************
//****************************************************************************
static void SelHeroGetHeros(HWND window) {
	TPUIHEROINFO dummyslot;
	// add a "new hero" slot at the end of the list
	TPUIHEROINFO newhero = SelHeroAllocNode();
	if (!newhero)
		return;
		
	newhero->next = NULL;
	LoadString(global_hinstance, IDS_NEWHERO, newhero->name, MAX_NAME_LEN-1);
	newhero->level = 0;
	sgHeroList = SelHeroAddNode(sgHeroList, newhero);
	dummyslot = sgHeroList;
	sgListLen = 1;

	// add all the other heros to the list
	if (! sgEnumFcn(SelHeroEnumHero)) {
		TCHAR err[64];
		// the enum failed -- notify the user
		LoadString(global_hinstance, IDS_ENUMCHAR_ERR, err, 64);
		UiOkDialog(window, err, TRUE);
	}

	if (dummyslot == sgHeroList) {
		PostMessage(window, WM_STATE_SELCLASS, 0, 0);
	}
	else {
		PostMessage(window, WM_STATE_SELLIST, 0, 0);
	}
}


//****************************************************************************
//****************************************************************************
static void SelHeroResetFocus(HWND window, int childID) {
	SetFocus(GetDlgItem(window, childID));
}


//****************************************************************************
//****************************************************************************
static void SelHeroSetImage(HWND window, int heroclass) {
	RECT rect;
	HWND child = GetDlgItem(window, IDC_HEROIMAGE);

	InvalidateRect(child, NULL, FALSE);

	GetClientRect(child, &rect);
	
	// hack since the art hasn't changed.
	if (heroclass == UI_BARBARIAN)
		heroclass = UI_WARRIOR;
	else if (heroclass == UI_NUM_CLASSES)
		--heroclass;
		
	UiCalcNewRect(&rect, 0, rect.bottom * heroclass);

	SDlgSetBitmap(
		child,
		NULL,
		TEXT("Static"),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		sgHerosBmp,
		&rect,
		sgHerosSize.cx,
		sgHerosSize.cy
	);
}


//****************************************************************************
//****************************************************************************
int SelHeroGetMode(void) {
	return sgMode;
}


//****************************************************************************
//****************************************************************************
void SelHeroGetDefStats(int heroclass, TPUIDEFSTATS stats) {
	sgGetStatsFcn(heroclass, stats);;
}


//****************************************************************************
//****************************************************************************
void SelHeroClearStats(HWND window) {
	LPTSTR empty = "--";
	HWND child;

	hassaved = FALSE;
	selheroname[0] = 0;

	child = GetDlgItem(window, IDC_CHARLEVEL);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), empty);
	child = GetDlgItem(window, IDC_CHARSTR);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), empty);
	child = GetDlgItem(window, IDC_CHARMAGIC);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), empty);
	child = GetDlgItem(window, IDC_CHARDEX);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), empty);
	child = GetDlgItem(window, IDC_CHARVIT);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), empty);

	SelHeroSetImage(window, UI_NUM_CLASSES);

	UiDoomStaticReset(window, sgDataIDs, AF_SMALLGRAY);
}


//****************************************************************************
//****************************************************************************
void SelHeroSetStats(HWND window, TPUIHEROINFO hero) {
	HWND child;

	if (! hero->level) {
		SelHeroClearStats(window);
		return;
	}

	hassaved = hero->hassaved;
	strcpy(selheroname, hero->name);

	child = GetDlgItem(window, IDC_CHARLEVEL);
	wsprintf(selherolevel, "%d", hero->level);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), selherolevel);

	child = GetDlgItem(window, IDC_CHARSTR);
	wsprintf(selherostr, "%d", hero->strength);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), selherostr);

	child = GetDlgItem(window, IDC_CHARMAGIC);
	wsprintf(selheromagic, "%d", hero->magic);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), selheromagic);

	child = GetDlgItem(window, IDC_CHARDEX);
	wsprintf(selherodex, "%d", hero->dexterity);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), selherodex);

	child = GetDlgItem(window, IDC_CHARVIT);
	wsprintf(selherovit, "%d", hero->vitality);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), selherovit);

	SelHeroSetImage(window, hero->heroclass);

	UiDoomStaticReset(window, sgDataIDs, AF_SMALLGRAY);
}


//****************************************************************************
//****************************************************************************
void SelHeroSetTitle(HWND window, LPCTSTR title) {
	HWND child = GetDlgItem(window, IDC_DLGTITLE);
	if (! child) return;
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), title);
	UiDoomStaticReset(window, sgTextIDs, AF_BIGGRAY);
}

//****************************************************************************
//****************************************************************************
static void convert_nasty(LPSTR nasty) {
	while (*nasty != 0) {
		(*nasty)--;
		nasty++;
	}
}


//****************************************************************************
//****************************************************************************
BOOL IsNastyName(LPCTSTR name) {
	TCHAR tempname[SNETSPI_MAXSTRINGLENGTH];
	TCHAR nasty[MAX_NAME_LEN];

	strcpy(tempname, name);
	_strlwr(tempname);

	for (int index = IDS_FIRST_NASTY; index <= IDS_LAST_NASTY; index++) {
		LoadString(global_hinstance, index, nasty, MAX_NAME_LEN-1);
		convert_nasty(nasty);
		_strlwr(nasty);
		if (strstr(tempname, nasty)) {
			return TRUE;
		}
	}
	return FALSE;
}


//****************************************************************************
// InvalidChars()
//
// NOTE: SPACE is not included as a bad char. SPACE and any other additional bad 
// characters should be placed into the additional string. Things like GAME NAME
// may include spaces.
//****************************************************************************
BOOL InvalidChars(LPCTSTR name, LPCTSTR additionalbad) {
	LPTSTR basicbad = ",<>%&\\\"?*#/";		
	unsigned char c;

	if (strpbrk(name, basicbad))
		return TRUE;

	if (strpbrk(name, additionalbad))
		return TRUE;

	while (0 != (c = *name)) {
		if (c < ' ') return TRUE;
		if ((c > '~') && (c < 0xc0)) return TRUE;
		name++;
	}
	return FALSE;
}


//****************************************************************************
//****************************************************************************
static BOOL ValidName(LPCTSTR name) {
	BOOL validname = TRUE;
	
	// single player name restrictions
	if (! strlen(name))
		validname = FALSE;

	// check multi player name restrictions
	if (sgMode == MULTIPLAYER) {
		if (IsNastyName(name) || InvalidChars(name, " ")) {
			validname = FALSE;
		}
	}

	return validname;
}


//****************************************************************************
//****************************************************************************
static BOOL SelHeroCreateNew(HWND window, LPCTSTR newname) {
	TCHAR title[32];
	TCHAR format[128];
	TCHAR confirm[128 + MAX_NAME_LEN];
	TPUIHEROINFO oldhero;

	if (SelHeroGetMode() == MULTIPLAYER)
		LoadString(global_hinstance, IDS_NEWMULTHERO, title, 32-1);
	else
		LoadString(global_hinstance, IDS_NEWSINGHERO, title, 32-1);

	// make sure the name doesn't have reserved words or chars
	if (! ValidName(newname)) {
		LoadString(global_hinstance, IDS_NAME_ERR, format, 127);
		SelOkDialog(window, format, title, TRUE);
		return FALSE;
	}

	// see if this name is already used
	oldhero = SelHeroFindNode(sgHeroList, newname);
	if (oldhero) {
		// the name already exists -- confirm the overwrite
		LoadString(global_hinstance, IDS_CONFIRM_OVERWRITE, format, 127);
		wsprintf(confirm, format, oldhero->name);
		if (IDCANCEL == SelYesNoDialog(window, confirm, title, TRUE)) {
			return FALSE;
		}
	}

	strcpy(sgNewHero.name, newname);
	sgNewHero.hassaved = FALSE;

	// create the hero file
	if (! sgCreateFcn(&sgNewHero)) {
		// notify user of the failure
		LoadString(global_hinstance, IDS_CREATE_ERR, format, 127);
		UiOkDialog(window, format, TRUE);
		return FALSE;
	}

	return TRUE;
}


//****************************************************************************
//****************************************************************************
static void DoDeleteHero(HWND window) {
	TCHAR title[32];
	TCHAR format[64];
	TCHAR confirm[64 + MAX_NAME_LEN];
	TPUIHEROINFO hero;

	if (SelHeroGetMode() == MULTIPLAYER)
		LoadString(global_hinstance, IDS_DELETEMULTHERO, title, 32-1);
	else
		LoadString(global_hinstance, IDS_DELETESINGHERO, title, 32-1);

	// confirm the delete
	LoadString(global_hinstance, IDS_CONFIRM_DELETE, format, 63);
	wsprintf(confirm, format, selheroname);
	if (IDCANCEL == SelYesNoDialog(window, confirm, title, TRUE)) {
		PostMessage(window, WM_STATE_SELLIST, 0, 0);
		return;
	}

	// find the hero node to delete
   	hero = SelHeroFindNode(sgHeroList, selheroname);
	if (! hero) {
		PostMessage(window, WM_STATE_SELLIST, 0, 0);
		return;
	}

   	// make the callback for the delete
   	if (! sgDeleteFcn(hero)) {
		// the delete failed
		LoadString(global_hinstance, IDS_DELETE_ERR, format, 63);
		SelOkDialog(window, format, title, TRUE);
		PostMessage(window, WM_STATE_SELLIST, 0, 0);
		return;
	}

   	// delete the node
   	sgHeroList = SelHeroDeleteNode(sgHeroList, selheroname);
	sgListLen--;

	// if there are no more heros, go straight to create
	LoadString(global_hinstance, IDS_NEWHERO, format, MAX_NAME_LEN-1);
	if (0 == strcmp(format, sgHeroList->name)) {
		PostMessage(window, WM_STATE_SELCLASS, 0, 0);
	}
	else {
		SelHeroSetStats(window, sgHeroList);
		PostMessage(window, WM_STATE_SELLIST, 0, 0);
	}
}


//****************************************************************************
//****************************************************************************
static void DoSelList(HWND window) {
	int result;

  	// bring up the hero selection list
   	result = SDlgDialogBox(
   		global_hinstance,
   		TEXT("SELLIST_DIALOG"),
   		window,
   		SelListDialogProc
   	);

   	if (result == IDOK) {
		if (! strlen(selheroname)) {
			PostMessage(window, WM_STATE_SELCLASS, 0, 0);
		}
		else if (sgMode == MULTIPLAYER) {
   			// a multiplayer hero was selected
			PostMessage(window, WM_STATE_CONTINUE, 0, 0);
		}
		else {
			// a single player hero was selected
			if (hassaved) {
				PostMessage(window, WM_STATE_SELLOAD, 0, 0);
			}
			else {
				sgDifficulty = DIFF_NORMAL;
   				SelHeroAbort(window, SELHERO_NEW_DUNGEON);
			}
		}
   	}
	else if (result == IDDELETE) {
		PostMessage(window, WM_STATE_DELETEHERO, 0, 0);
	}
   	else {
		SelHeroAbort(window, SELHERO_PREVIOUS);
   	}
}


//****************************************************************************
//****************************************************************************
static void DoSelClass(HWND window) {
	int result;

  	// bring up the hero selection list
   	if (sgAllowBarbarian)
   	{
	   	result = SDlgDialogBox(
	   		global_hinstance,
	   		TEXT("SELCLASS_DIALOG3"),
	   		window,
	   		SelClassDialogProc
	   	);
   	}
   	else if (sgAllowBard)
   	{
	   	result = SDlgDialogBox(
	   		global_hinstance,
	   		TEXT("SELCLASS_DIALOG2"),
	   		window,
	   		SelClassDialogProc
	   	);
   	}
   	else
   	{
	   	result = SDlgDialogBox(
	   		global_hinstance,
	   		TEXT("SELCLASS_DIALOG"),
	   		window,
	   		SelClassDialogProc
	   	);
   	}

   	if ((result == -1) || (result == IDCANCEL)) {
		TCHAR buf[32];
		LoadString(global_hinstance, IDS_NEWHERO, buf, 32-1);
		if (strcmp(buf, sgHeroList->name) == 0) {
   			SelHeroAbort(window, SELHERO_PREVIOUS);
		}
		else {
			PostMessage(window, WM_STATE_SELLIST, 0, 0);
		}
   	}
   	else {
		switch (result) {
			case IDC_ROGUEBTN:
				sgNewHero.heroclass = UI_ROGUE;
			break;
			case IDC_SORCERERBTN:
				sgNewHero.heroclass = UI_SORCERER;
			break;
			case IDC_MONKBTN:
				sgNewHero.heroclass = UI_MONK;
				break;
			case IDC_BARDBTN:
				sgNewHero.heroclass = UI_BARD;
				break;
			case IDC_BARBARIANBTN:
				sgNewHero.heroclass = UI_BARBARIAN;
				break;
			case IDC_WARRIORBTN:
			default:
				sgNewHero.heroclass = UI_WARRIOR;
			break;
		}
		PostMessage(window, WM_STATE_ENTERNAME, 0, 0);
   	}
}


//****************************************************************************
//****************************************************************************
static void DoEnterName(HWND window) {
	TCHAR newname[MAX_NAME_LEN];
	int result;

  	// bring up the enter name screen
   	result = SDlgDialogBoxParam(
   		global_hinstance,
   		TEXT("ENTERNAME_DIALOG"),
   		window,
   		EnterNameDialogProc,
		(LPARAM) &newname
   	);

	if (result == IDOK) {
		newname[MAX_NAME_LEN-1] = 0;
		if (SelHeroCreateNew(window, newname)) {
			PostMessage(window, WM_STATE_NEWHERO, 0, 0);
		}
		else {
			PostMessage(window, WM_STATE_ENTERNAME, 0, 0);
		}
	}
	else {
		PostMessage(window, WM_STATE_SELCLASS, 0, 0);
	}
}


//****************************************************************************
//****************************************************************************
static void DoSelLoad(HWND window) {
	int result;

  	// bring up the enter name screen
   	result = SDlgDialogBox(
   		global_hinstance,
   		TEXT("SELLOAD_DIALOG"),
   		window,
   		SelLoadDialogProc
   	);

	if ((result == -1) || (result == IDCANCEL)) {
		PostMessage(window, WM_STATE_SELLIST, 0, 0);
	}
	else {
		if (result == IDC_LOADBTN) {
			PostMessage(window, WM_STATE_CONTINUE, 0, 0);
		}
		else {
			PostMessage(window, WM_STATE_SELDIFF, 0, 0);
		}
	}
}


//****************************************************************************
//****************************************************************************
static void DoSelDiff(HWND window) {
	int result;

//	if (SelHeroGetMode() == SINGLEPLAYER) {
//		// single player is always NORMAL difficulty
//		SelHeroSetDiff(DIFF_NORMAL);
//		SelHeroAbort(window, SELHERO_NEW_DUNGEON);
//		return;
//	}

  	// bring up the select difficulty options
	SelDiffLoadFocus(TRUE);
   	result = SDlgDialogBoxParam(
   		global_hinstance,
   		TEXT("SELDIFF_DIALOG"),
   		window,
   		SelDiffDialogProc,
		sgMode
   	);

	if (result == IDOK) {
		// confirm the charcter can create a dungeon of the difficulty setting specified
		char szGameDesc[SNETSPI_MAXSTRINGLENGTH];
		char szPlayerDesc[SNETSPI_MAXSTRINGLENGTH];
		char err[256];
		TGAMEDATA GameData;
		TPUIHEROINFO pHeroInfo;

		pHeroInfo = SelHeroFindNode(sgHeroList, selheroname);
		if (pHeroInfo) {
			UiCreatePlayerDescription(pHeroInfo, 'DBLO', szPlayerDesc);
	
			// Create Game description for Storm (and for display of game later)
			GameData.bDiff = sgDifficulty;
			GameData2Txt(
				&GameData, 
				selheroname, 
				szPlayerDesc, 
				szGameDesc, 
				sizeof(szGameDesc)
			);
	
			if ((sgMode == MULTIPLAYER) &&
				! UiAuthCallback(
					SNET_AUTHTYPE_GAME, 
					selheroname, 
					szPlayerDesc,
					0,			
					szGameDesc,
					err,
					sizeof(err)
				)
			) {
				SelOkDialog(window, err, NULL, TRUE);
				PostMessage(window, WM_STATE_SELDIFF, 0, 0);
			}
			else {
				SelHeroAbort(window, SELHERO_NEW_DUNGEON);
			}
		}
		else {
			SelHeroAbort(window, SELHERO_NEW_DUNGEON);
		}
	}
	else {
		PostMessage(window, WM_STATE_SELLOAD, 0, 0);
	}
}


//****************************************************************************
//****************************************************************************
static void SelHeroDestroy(HWND window) {
	UiDoomCtrlsDestroy(window, sgDataIDs);
	UiDoomCtrlsDestroy(window, sgLblIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);

	SelHeroFreeList(sgHeroList);

	if (sgHerosBmp) {
		FREE(sgHerosBmp);
		sgHerosBmp = NULL;
	}

	// free the background of the dlg
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	TitleLogoDestroy();
}


//****************************************************************************
//****************************************************************************
static void SelHeroAbort(HWND window, int retval) {
	UiFadeAbort(window);
	SDlgKillTimer(window, LOGO_TIMER_ID);
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, retval);
}


//****************************************************************************
//****************************************************************************
static void SelHeroInit(HWND window) {
	TPBMP tpBmp;

	// create a list of available heros
	SelHeroGetHeros(window);

	// set up the animating diablo logo
	TitleLogoInit(window);
	TitleFramesInit(window,TEXT("ui_art\\hf_logo2.pcx"));
	TitleLogoAnimate(window);
	SDlgSetTimer(window, LOGO_TIMER_ID, LOGO_TIMER_DELAY, NULL);

	// load texture maps for this dialog
	tpBmp = UiAllocBmp();
	if (tpBmp) {
		SetWindowLong(window, GWL_USERDATA, (LONG) tpBmp);
		LoadArtFile(
			window,
			NULL,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			TEXT("ui_art\\selhero.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
		UiFadeInit(window);
	}

	UiLoadBmpFile(
		TEXT("ui_art\\heros.pcx"),
		&sgHerosBmp,
		&sgHerosSize
	);

	// init the ctrls to use doom text
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomStaticInit(window, sgLblIDs, AF_SMALLGRAY);
	UiDoomStaticInit(window, sgDataIDs, AF_SMALLGRAY);

	SelHeroClearStats(window);
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelHeroDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {

		case WM_DESTROY:
			SelHeroDestroy(window);
		break;

		case WM_INITDIALOG:
			SelHeroInit(window);
			PostMessage(window, WM_USER+1000, 0, 0);
		return 0;

		case WM_STATE_SELLIST:
			DoSelList(window);
		return 0;

		case WM_STATE_SELCLASS:
			DoSelClass(window);
		return 0;

		case WM_STATE_ENTERNAME:
			DoEnterName(window);
		return 0;

		case WM_STATE_SELLOAD:
			DoSelLoad(window);
		return 0;

		case WM_STATE_SELDIFF:
			DoSelDiff(window);
		return 0;

		case WM_STATE_NEWHERO:
			strcpy(selheroname, sgNewHero.name);
			if (sgMode == MULTIPLAYER) {
   				SelHeroAbort(window, SELHERO_CONNECT);
			}
			else {
				//sgDifficulty = DIFF_NORMAL;
   				//SelHeroAbort(window, SELHERO_NEW_DUNGEON);
				PostMessage(window, WM_STATE_SELDIFF, 0, 0);
			}
		return 0;

		case WM_STATE_DELETEHERO:
			DoDeleteHero(window);
		return 0;

		case WM_STATE_CONTINUE:
			if (sgMode == MULTIPLAYER) {
   				SelHeroAbort(window, SELHERO_CONNECT);
			}
			else {
   				SelHeroAbort(window, SELHERO_CONTINUE);
			}
		return 0;

		case WM_USER+1000:
			if (! UiIsFading()) {
				UiFadeStart(window);
			}
		return 0;

		case WM_TIMER:
			TitleLogoAnimate(window);
		return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}


//****************************************************************************
//*
//*  EXPORTED FUNCTIONS
//*
//****************************************************************************


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiSelHeroMultDialog (
									ENUMHEROS   enumfcn,
									CREATEHERO  createfcn,
									DELETEHERO  deletefcn,
									GETDEFHERO	getstatsfcn,
									DWORD      *selection,
									LPSTR		heroname,
									bool		allowBard,
									bool		allowBarbarian) {
	DWORD result = 0;

	// make sure fonts are loaded
	ArtFontLoad();

	sgEnumFcn     = enumfcn;
	sgCreateFcn   = createfcn;
	sgDeleteFcn   = deletefcn;
	sgGetStatsFcn = getstatsfcn;
	sgAllowBard   = allowBard;
	sgAllowBarbarian   = allowBarbarian;

	sgHeroList = NULL;
	sgMode = MULTIPLAYER;

	// DISPLAY THE DIALOG BOX
	result = (DWORD)SDlgDialogBox(global_hinstance,
								  TEXT("SELHERO_DIALOG"),
								  SDrawGetFrameWindow(),
								  SelHeroDialogProc);

	// set the results
	if (selection)
		*selection = result;
	if (heroname)
		strcpy(heroname, selheroname);

	return 1;
}


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiSelHeroSingDialog (
									ENUMHEROS	enumfcn,
									CREATEHERO	createfcn,
									DELETEHERO	deletefcn,
									GETDEFHERO	getstatsfcn,
									DWORD		*selection,
									LPSTR		heroname,
									int			*difficulty,
									bool		allowBard,
									bool		allowBarbarian) {
	DWORD result = 0;

	// make sure fonts are loaded
	ArtFontLoad();

	sgEnumFcn     = enumfcn;
	sgCreateFcn   = createfcn;
	sgDeleteFcn   = deletefcn;
	sgGetStatsFcn = getstatsfcn;
	sgAllowBard   = allowBard;
	sgAllowBarbarian	= allowBarbarian;

	sgHeroList = NULL;
	sgMode = SINGLEPLAYER;

	// DISPLAY THE DIALOG BOX
	result = (DWORD)SDlgDialogBox(global_hinstance,
								  TEXT("SELHERO_DIALOG"),
								  SDrawGetFrameWindow(),
								  SelHeroDialogProc);
	// set the results
	if (selection)
		*selection = result;
	if (heroname)
		strcpy(heroname, selheroname);
	if (difficulty)
		*difficulty = sgDifficulty;

	// if the result is any option that starts the game, free fonts, snds. etc.
	if (result != SELHERO_PREVIOUS) {
		ArtFontDestroy();
	}

	return 1;
}
