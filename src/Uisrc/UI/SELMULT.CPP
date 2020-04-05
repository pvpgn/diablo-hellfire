//****************************************************************************
//	SelMult.cpp
//	Diablo UI select multi player game character dialog
//	
//	By Frank Pearce
//	created 9.19.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
extern int CreaHeroDlg(HWND parent, char * newname, int * charclass);


//****************************************************************************
//****************************************************************************
static SIZE bgsize;
static LPBYTE backgroundbitmap = NULL;
static LPBYTE buttonbitmap     = NULL;

//*** strings to put the hero stats in
static char selheroname[NAME_LEN];
static char selheroclass[CLASS_LEN];
static char selherolevel[8];
static char selherostr[8];
static char selheromagic[8];
static char selherodex[8];
static char selherovit[8];
static char selherogold[16];


static ENUMHEROS global_enumfcn;
static CREATEHERO global_createfcn;
static DELETEHERO global_deletefcn;

static TPUIHEROINFO herolist;


//****************************************************************************
//****************************************************************************
TPUIHEROINFO SelMultAllocNode(void) {
	return (TPUIHEROINFO) ALLOC(sizeof(TUIHEROINFO));
}


//****************************************************************************
//****************************************************************************
static void SelMultFreeNode(TPUIHEROINFO node) {
	FREE(node);
}


//****************************************************************************
//****************************************************************************
static void SelMultFreeList(TPUIHEROINFO list) {
	TPUIHEROINFO next;

	while (list) {
		next = list->next;
		SelMultFreeNode(list);
		list = next;
	}
}


//****************************************************************************
//****************************************************************************
TPUIHEROINFO SelMultAddNode(TPUIHEROINFO list, TPUIHEROINFO node) {
	node->next = list;
	return node;
}


//****************************************************************************
//****************************************************************************
static TPUIHEROINFO SelMultDeleteNode(TPUIHEROINFO list, const char * namekey) {
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
		SelMultFreeNode(found);
	}

	return list;
}


//****************************************************************************
//****************************************************************************
static TPUIHEROINFO SelMultFindNode(TPUIHEROINFO list, const char * namekey) {
	TPUIHEROINFO current = list;

	while (current) {
		if (strcmp(current->name, namekey) == 0)
			break;
		current = current->next;
	}

	return current;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelMultEnumHero(TPUIHEROINFO heroinfo) {
	TPUIHEROINFO newhero = SelMultAllocNode();
	memcpy(newhero, heroinfo, sizeof(TUIHEROINFO));
	herolist = SelMultAddNode(herolist, newhero);
	return 1;
}


//****************************************************************************
//****************************************************************************
static void SelMultGetHeros(void) {
	global_enumfcn(SelMultEnumHero);
}


//****************************************************************************
//****************************************************************************
static void SelMultSetStats(HWND window, HWND lbox) {
	TPUIHEROINFO hero;
	HWND child;

	SendMessage(
		lbox, 
		LB_GETTEXT,
		SendMessage(lbox, LB_GETCURSEL, 0, 0),
		(LPARAM) selheroname
	);

	hero = SelMultFindNode(herolist, selheroname);

	child = GetDlgItem(window, IDC_CHARNAME);
	SetWindowText(child, selheroname);

	child = GetDlgItem(window, IDC_CHARCLASS);
	strcpy(selheroclass, hero->heroclass);
	SetWindowText(child, selheroclass);

	child = GetDlgItem(window, IDC_CHARLEVEL);
	wsprintf(selherolevel, "%d", hero->level);
	SetWindowText(child, selherolevel);

	child = GetDlgItem(window, IDC_CHARSTR);
	wsprintf(selherostr, "%d", hero->strength);
	SetWindowText(child, selherostr);

	child = GetDlgItem(window, IDC_CHARMAGIC);
	wsprintf(selheromagic, "%d", hero->magic);
	SetWindowText(child, selheromagic);

	child = GetDlgItem(window, IDC_CHARDEX);
	wsprintf(selherodex, "%d", hero->dexterity);
	SetWindowText(child, selherodex);

	child = GetDlgItem(window, IDC_CHARVIT);
	wsprintf(selherovit, "%d", hero->vitality);
	SetWindowText(child, selherovit);

	child = GetDlgItem(window, IDC_CHARGOLD);
	wsprintf(selherogold, "%d", hero->gold);
	SetWindowText(child, selherogold);
}


//****************************************************************************
//****************************************************************************
static void SelMultClearStats(HWND window) {
	HWND child;

	child = GetDlgItem(window, IDC_CHARNAME);
	SetWindowText(child, NULL);
	child = GetDlgItem(window, IDC_CHARCLASS);
	SetWindowText(child, NULL);
	child = GetDlgItem(window, IDC_CHARLEVEL);
	SetWindowText(child, NULL);
	child = GetDlgItem(window, IDC_CHARSTR);
	SetWindowText(child, NULL);
	child = GetDlgItem(window, IDC_CHARMAGIC);
	SetWindowText(child, NULL);
	child = GetDlgItem(window, IDC_CHARDEX);
	SetWindowText(child, NULL);
	child = GetDlgItem(window, IDC_CHARVIT);
	SetWindowText(child, NULL);
	child = GetDlgItem(window, IDC_CHARGOLD);
	SetWindowText(child, NULL);
}


//****************************************************************************
//****************************************************************************
static void SelMultCreateNew(HWND window) {
	TPUIHEROINFO newhero;
	char newname[NAME_LEN];
	int charclass;
	HWND lbox;

	// bring up the create char popup
	if (IDCANCEL == CreaHeroDlg(window, newname, &charclass))
		return;

#if 0
	lbox = GetDlgItem(window, IDC_MULTICHARS_LBOX);

	// ~~~fgp.add more code here
	// if the name has no length use a default name

	if (NULL != (newhero = SelMultFindNode(herolist, newname))) {
		// the name already exists
		// confirm the overwrite
		TCHAR buf[64];
		wsprintf(buf, "Overwrite %s?", newname);
		if (IDCANCEL == SDrawMessageBox(buf,TEXT("Confirm Overwrite"),MB_OKCANCEL))
			return;

		// make the new hero the currently selected lbox entry
		SendMessage(lbox, LB_SELECTSTRING, -1, (LPARAM)newname);
	}
	else {
		// the user didn't enter a duplicate name
		int index;

		// add the new hero to the hero list
		newhero = SelMultAllocNode();
		herolist = SelMultAddNode(herolist, newhero);

		// add the new hero to the listbox and select it
		index = SendMessage(lbox, LB_ADDSTRING, 0, (LPARAM)newname);
		SendMessage(lbox, LB_SETCURSEL, index, 0);
	}

	// call the create fcn for the new hero
	global_createfcn(newhero);

	// update the displayed stats
	SelMultSetStats(window, lbox);
#endif
}


//****************************************************************************
//****************************************************************************
static void SelMultDeleteHero(HWND window) {
	TPUIHEROINFO hero;
	TCHAR buf[64];
	HWND lbox;

	// confirm the delete
	wsprintf(buf, "Delete %s?", selheroname);
	if (IDCANCEL == SDrawMessageBox(buf,TEXT("Confirm Delete"),MB_OKCANCEL))
		return;

	// find the hero node to delete
   	lbox = GetDlgItem(window, IDC_MULTICHARS_LBOX);
   	hero = SelMultFindNode(herolist, selheroname);

   	// make the callback for the delete
   	global_deletefcn(hero);

   	// delete the node
   	herolist = SelMultDeleteNode(herolist, selheroname);

   	// remove it from the listbox
   	SendMessage(
   		lbox,
   		LB_DELETESTRING,
   		SendMessage(lbox, LB_GETCURSEL, 0, 0),
   		0
   	);

   	// select a new hero automatically
   	if (SendMessage(lbox, LB_GETCOUNT, 0, 0)) {
   		SendMessage(lbox, LB_SETCURSEL, 0, 0);
   		SelMultSetStats(window, lbox);
   	}
   	else {
   		// disable the DELETE button
   		EnableWindow(GetDlgItem(window, IDC_DELETECHAR_BTN), FALSE);
   		SelMultClearStats(window);
   	}
}


//****************************************************************************
//****************************************************************************
static void SelMultInitLbox(HWND window, HWND lbox) {
	TPUIHEROINFO currenthero = herolist;

	SendMessage(lbox, WM_SETREDRAW, FALSE, 0);

	// fill the list box with all available multiplayer game characters
	while (currenthero) {
		SendMessage(lbox, LB_ADDSTRING, 0, (LPARAM)(currenthero->name));
		currenthero = currenthero->next;
	}

	// if heros already exist default selection to first hero
	if (SendMessage(lbox, LB_GETCOUNT, 0, 0)) {
		SendMessage(lbox, LB_SETCURSEL, 0, 0);
		SelMultSetStats(window, lbox);
	}
	else {
		// disable the DELETE button
		EnableWindow(GetDlgItem(window, IDC_DELETECHAR_BTN), FALSE);
	}

	SendMessage(lbox, WM_SETREDRAW, TRUE, 0);
}


//****************************************************************************
//****************************************************************************
static BOOL SelMultConfirmSelected(HWND window) {
	// if the listbox is empty the create dlg needs to be popped up
	return 1;
}


//****************************************************************************
//****************************************************************************
static void SelMultDestroy(HWND window) {
	if (buttonbitmap) {
		FREE(buttonbitmap);
		buttonbitmap = NULL;
	}
	if (backgroundbitmap) {
		FREE(backgroundbitmap);
		backgroundbitmap = NULL;
	}

	SelMultFreeList(herolist);
}


//****************************************************************************
//****************************************************************************
static BOOL SelMultInit(HWND window) {
	SIZE artsize;

	// load texture maps for this dialog
	if (!LoadArtFile(window,NULL,TEXT(""),TEXT("ui_art\\samp_bkg.pcx"),&backgroundbitmap,&bgsize))
		return 0;
	if (!LoadArtFile(NULL,window,TEXT("Button"),TEXT("ui_art\\samp_btn.pcx"),&buttonbitmap,&artsize))
		return 0;

	SelMultGetHeros();
	SelMultInitLbox(window, GetDlgItem(window, IDC_MULTICHARS_LBOX));

	// set bg bitmaps for stat displays
	UiSetChildBmp(window, IDC_CHARNAME, backgroundbitmap, &bgsize);
	UiSetChildBmp(window, IDC_CHARCLASS, backgroundbitmap, &bgsize);
	UiSetChildBmp(window, IDC_CHARLEVEL, backgroundbitmap, &bgsize);
	UiSetChildBmp(window, IDC_CHARSTR, backgroundbitmap, &bgsize);
	UiSetChildBmp(window, IDC_CHARMAGIC, backgroundbitmap, &bgsize);
	UiSetChildBmp(window, IDC_CHARDEX, backgroundbitmap, &bgsize);
	UiSetChildBmp(window, IDC_CHARVIT, backgroundbitmap, &bgsize);
	UiSetChildBmp(window, IDC_CHARGOLD, backgroundbitmap, &bgsize);

	return 1;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelMultDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDC_CREATECHAR_BTN:
					SelMultCreateNew(window);
				break;
				case IDC_DELETECHAR_BTN: 
					SelMultDeleteHero(window);
				break;
				case IDC_CONNECT_BTN:
					if (SelMultConfirmSelected(window)) {
						SDlgEndDialog(window,SELMULT_CONNECT);
					}
				break;
				case IDC_MULTICHARS_LBOX:
					if (HIWORD(wparam) == LBN_SELCHANGE) {
						// a character was selected -- update the stats on the screen
						SelMultSetStats(window, (HWND)lparam);
					}
				break;
				case IDCANCEL:
					SDlgEndDialog(window,SELMULT_PREVIOUS);
				break;
		}
		break;

		case WM_DESTROY:
			SelMultDestroy(window);
		break;

		case WM_INITDIALOG:
			SelMultInit(window);
		return 1;

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
BOOL APIENTRY UiSelMultDialog (
							   ENUMHEROS   enumfcn,
							   CREATEHERO  createfcn,
							   DELETEHERO  deletefcn,
							   DWORD      *selection,
							   char       *heroname) {
	DWORD result = 0;

	global_enumfcn   = enumfcn;
	global_createfcn = createfcn;
	global_deletefcn = deletefcn;
	herolist = NULL;

	// DISPLAY THE DIALOG BOX
	result = (DWORD)SDlgDialogBox(global_hinstance,
								  TEXT("SELMULTI_DIALOG"),
								  SDrawGetFrameWindow(),
								  SelMultDialogProc);

	// set the results
	if (selection)
		*selection = result;
	if (heroname)
		strcpy(heroname, selheroname);

	return 1;
}
