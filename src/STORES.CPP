/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 Control panel file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/STORES.CPP 6     3/19/97 11:19a Jmcreynolds $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"engine.h"
#include	"control.h"
#include	"scrollrt.h"
#include	"gendung.h"
#include	"items.h"
#include	"itemdat.h"
#include	"player.h"
#include	"stores.h"
#include	"minitext.h"
#include	"cursor.h"
#include	"inv.h"
#include	"quests.h"
#include	"effects.h"
#include	"multi.h"
#include	"towners.h"
#include	"spelldat.h"
#include	"textdat.h"
#include	"spells.h"

/*-----------------------------------------------------------------------**
** Local defines
**-----------------------------------------------------------------------*/

#define STEXT_SMALL		0
#define STEXT_LARGE		1

#define MAXHOLDITEMS	48

/*-----------------------------------------------------------------------**
** Global variables
**-----------------------------------------------------------------------*/

BYTE *pSTextBoxCels;
BYTE *pSTextSpinCels;
BYTE *pSTextSlidCels;

char stextflag, stextsize;
int stextspin, stextsel;
int stextlhold, stextshold, stextvhold;

BOOL stextscrl;
int stextsval, stextsmax;
int stextup, stextdown;

char stextscrlubtn, stextscrldbtn;

int SStringY[24] = {
	0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144,
	156, 168, 180, 192, 204, 216, 228, 240, 252, 264, 276 };

STextStruct	stext[NUMSTLINES];

ItemStruct smithitem[MAXSMITHITEMS];

ItemStruct premiumitem[MAXPREMIUM];
int numpremium, premiumlevel;

ItemStruct witchitem[MAXWITCHITEMS];

ItemStruct boyitem;
int boylevel;

ItemStruct healitem[MAXHEALITEMS];

ItemStruct storehold[MAXHOLDITEMS];
char storehidx[MAXHOLDITEMS];

ItemStruct golditem;

int	storenumh;

int gossipstart, gossipend;

/*-----------------------------------------------------------------------*/

char *talkname[] = {
	"Griswold",
	"Pepin",
	"",
	"Ogden",
	"Cain",
	"Farnham",
	"Adria",
	"Gillian",
	"Wirt"
};

int talker;
extern QuestData questlist[];

/*-----------------------------------------------------------------------*/

void ClearSText(int, int);

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void InitStores()
{
	app_assert(! pSTextBoxCels);
	pSTextBoxCels = LoadFileInMemSig("Data\\TextBox2.CEL",NULL,'STOR');
	pSTextSpinCels = LoadFileInMemSig("Data\\PentSpn2.CEL",NULL,'STOR');
	pSTextSlidCels = LoadFileInMemSig("Data\\TextSlid.CEL",NULL,'STOR');
	ClearSText(0, NUMSTLINES);
	stextflag = STORE_NONE;
	stextspin = 1;
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	numpremium = 0;
	premiumlevel = 1;
	for (int i = 0; i < MAXPREMIUM; i++) premiumitem[i]._itype = -1;
	boyitem._itype = -1;
	boylevel = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetupTownStores()
{
	int i, l;

	// PATCH2.JMM.3/10/97
	SetRndSeed( glSeedTbl[currlevel] * GetTickCount( ) );
	// ENDPATCh2.JMM.3/10/97


	// How deep have I gone?
	if (gbMaxPlayers == 1) {
		l = 0;
		for (i = 0; i < NUMLEVELS; i++)
			if (plr[myplr]._pLvlVisited[i]) l = i;
	} else {
		l = plr[myplr]._pLevel >> 1;
	}

	// Make sure they carry relevant items all the time
	l += 2;
	if (l < 6) l = 6;
	if (l > 16) l = 16;

	// Setup a gold item for transactions
	SpawnStoreGold();
	// Setup blacksmith items
	SpawnSmith(l);
	// Setup witch items
	SpawnWitch(l);
	// Setup healer items
	SpawnHealer(l);

	// Setup boy items can alter the random seed
	SpawnBoy(plr[myplr]._pLevel);
	// SpawnPremium can alter the random seed
	SpawnPremium(myplr);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FreeStoreMem()
{
	DiabloFreePtr(pSTextBoxCels);
	DiabloFreePtr(pSTextSpinCels);
	DiabloFreePtr(pSTextSlidCels);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*

BOOL StoreQuestTalk(int t)
{
	int i;
	for (i = 0; i < MAXQUESTS; i++) {
		if ((quests[i]._qactive == QUEST_NOTDONE) && (Qtalklist[t][i] != -1)) return(TRUE);
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawSTextBack()
{
	DrawCel(408, 487, pSTextBoxCels, 1, 271);

	app_assert(gpBuffer);
	__asm {
		mov		edi,dword ptr [gpBuffer]
		add		edi,372123

		xor		eax,eax
		mov		edx,148
_YLp:	mov		ecx,132
_XLp1:	stosb
		inc		edi
		loop	_XLp1
		stosb
		sub		edi,1033
		mov		ecx,132
_XLp2:	inc		edi
		stosb
		loop	_XLp2
		sub		edi,1032
		dec		edx
		jnz		_YLp
		mov		ecx,132
_XLp3:	stosb
		inc		edi
		loop	_XLp3
		stosb
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void PrintSString(int x, int y, BOOL cjustflag, char str[], char col, int val)
{
	long	boffset;
	int		sl,i,w,tw,aw,xx,yy,xa;
	char	valstr[32];

	yy = SStringY[y] + stext[y]._syoff;
	if (stextsize == STEXT_SMALL) xa = 416;
	else xa = 96;
	boffset = nBuffWTbl[yy + 204] + x + xa;
	sl = strlen(str);
	if (stextsize == STEXT_SMALL) aw = 257;
	else aw = 577;
	w = 0;
	if (cjustflag) {
		tw = 0;
		for (i = 0; i < sl; i++) {
			BYTE c = char2print(str[i]);
			c = fonttrans[c];
			tw += fontkern[c]+1;
		}
		if (tw < aw) w = (aw - tw) >> 1;
		boffset += w;
	}
	if (stextsel == y) {
		if (cjustflag) xx = x + w + xa - 20;
		else xx = x + xa - 20;
		DrawCel(xx, yy + 205, pSTextSpinCels, stextspin, 12);
	}
	for (i = 0; i < sl; i++) {
		BYTE c = char2print(str[i]);
		c = fonttrans[c];
		w += fontkern[c]+1;
		if ((c != 0) && (w <= aw)) DrawPanelFont(boffset, c, col);
		boffset += fontkern[c]+1;
	}
	if ((!cjustflag) && (val >= 0)) {
		sprintf(valstr, "%i", val);
		boffset = nBuffWTbl[yy + 204] + 656 - x;
		sl = strlen(valstr);
		for (i = sl-1; i >= 0; i--) {
			BYTE c = char2print(valstr[i]);
			c = fonttrans[c];
			boffset -= fontkern[c]+1;
			if (c != 0) DrawPanelFont(boffset, c, col);
		}
	}
	if (stextsel == y) {
		if (cjustflag) xx = x + w + xa + 4;
		else xx = 660 - x;
		DrawCel(xx, yy + 205, pSTextSpinCels, stextspin, 12);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawSLine(int y)
{
	long	soffset, doffset;
	long	width, wadd;
	int		yy;

	yy = SStringY[y];
	if (stextsize == STEXT_LARGE) {
		soffset = 142170;
		doffset = nBuffWTbl[yy + 198] + 90;
		width = 146;
		wadd = 182;
	} else {
		soffset = 142490;
		doffset = nBuffWTbl[yy + 198] + 410;
		width = 66;
		wadd = 502;
	}

	app_assert(gpBuffer);
	__asm {
		mov		esi,dword ptr [gpBuffer]
		mov		edi,esi
		add		esi,dword ptr [soffset]
		add		edi,dword ptr [doffset]

		mov		ebx,dword ptr [wadd]

		mov		edx,3
_YLp:	mov		ecx,dword ptr [width]
		rep movsd
		movsw
		add		esi,ebx
		add		edi,ebx
		dec		edx
		jnz		_YLp
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawScrlBar(int y1, int y2)
{
	int i, yy1, yy2, v, s;

	yy1 = SStringY[y1] + 204;
	yy2 = SStringY[y2] + 204;
	if (stextscrlubtn != -1) DrawCel(665, yy1, pSTextSlidCels, 12, 12);
	else DrawCel(665, yy1, pSTextSlidCels, 10, 12);
	if (stextscrldbtn != -1) DrawCel(665, yy2, pSTextSlidCels, 11, 12);
	else DrawCel(665, yy2, pSTextSlidCels, 9, 12);
	for (i = (yy1+12); i < yy2; i+=12) DrawCel(665, i, pSTextSlidCels, 14, 12);

	if (stextsel != 22) s = stextsel;
	else s = stextlhold;
	if (storenumh > 1) {
		v = ((s - stextup) >> 2) + stextsval;
		v = v * 1000;
		v = v / (storenumh-1);
		v = v * (SStringY[y2]-SStringY[y1]-24);
		v = v / 1000;
	} else v = 0;
	v += SStringY[y1+1] + 204;
	DrawCel(665, v, pSTextSlidCels, 13, 12);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetupSTextWin()
{
	stextsize = STEXT_SMALL;
	stextsel = -1;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetupSTextLWin()
{
	stextsize = STEXT_LARGE;
	stextsel = -1;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ClearSText(int s, int e)
{
	int i;

	for (i = s; i < e; i++) {
		stext[i]._sx = 0;
		stext[i]._syoff = 0;
		stext[i]._sstr[0] = 0;
		stext[i]._sjust = FALSE;
		stext[i]._sclr = ICOLOR_WHITE;
		stext[i]._sline = FALSE;
		stext[i]._ssel = FALSE;
		stext[i]._sval = -1;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AddSLine(int y)
{
	stext[y]._sx = 0;
	stext[y]._syoff = 0;
	stext[y]._sstr[0] = 0;
	stext[y]._sline = TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AddSTextVal(int y, int val)
{
	stext[y]._sval = val;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void OffsetSTextY(int y, int yo)
{
	stext[y]._syoff = yo;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AddSText(int x, int y, BOOL j, char str[], char clr, BOOL sel)
{
	stext[y]._sx = x;
	stext[y]._syoff = 0;
	strcpy(stext[y]._sstr,str);
	stext[y]._sjust = j;
	stext[y]._sclr = clr;
	stext[y]._sline = FALSE;
	stext[y]._ssel = sel;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void PrintStoreItem(const ItemStruct * x, int l, char iclr)
{
	char sstr[128];

	sstr[0] = 0;
	if (x->_iIdentified) {
		if ((x->_iMagical != IMAGIC_UNIQUE) && (x->_iPrePower != -1)) {
			PrintItemPower(x->_iPrePower, x);
			strcat(sstr, tempstr);
		}
		if (x->_iSufPower != -1) {
			PrintItemPower(x->_iSufPower, x);
			if (sstr[0] != 0) strcat(sstr, ",  ");
			strcat(sstr, tempstr);
		}
	}
	if ((x->_iMiscId == IMID_STAFF) && (x->_iMaxCharges != 0)) {
		sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
		if (sstr[0] != 0) strcat(sstr, ",  ");
		strcat(sstr, tempstr);
	}

	if (sstr[0] != 0) {
		AddSText(40, l, FALSE, sstr, iclr, FALSE);
		l++;
	}

	sstr[0] = 0;
	if (x->_iClass == IC_WEAP) sprintf(sstr, "Damage: %i-%i  ", x->_iMinDam, x->_iMaxDam);
	if (x->_iClass == IC_ARMOR) sprintf(sstr, "Armor: %i  ", x->_iAC);

	if ((x->_iMaxDur == INFINITE_DUR) || (x->_iMaxDur == 0)) strcat(sstr, "Indestructible,  ");
	else {
		sprintf(tempstr, "Dur: %i/%i,  ", x->_iDurability, x->_iMaxDur);
		strcat(sstr, tempstr);
	}
	if (x->_itype == IT_MISC) sstr[0] = 0;
	if ((x->_iMinStr + x->_iMinMag + x->_iMinDex) == 0) {
		strcat(sstr, "No required attributes");
	} else {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr != 0) sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag != 0) sprintf(tempstr, "%s %i Mag", tempstr, (byte) x->_iMinMag);
		if (x->_iMinDex != 0) sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		strcat(sstr, tempstr);
	}
	AddSText(40, l++, FALSE, sstr, iclr, FALSE);

	if ((x->_iMagical == IMAGIC_UNIQUE) && (x->_iIdentified))
		AddSText(40, l++, FALSE, "Unique Item", iclr, FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
extern int AP2x2Tbl[10];

void StoreAutoPlace()
{
	int i, w, h, idx;
	BOOL done;

	SetICursor(plr[myplr].HoldItem._iCurs + ICSTART);
	// all items are either 1x1, 1x2, 1x3, 2x2, or 2x3
	// the inv is 10x4
	w = icursW28;
	h = icursH28;
	done = FALSE;
	if ((w == 1) && (h == 1)) {
		// Check to see if speed bar available first
		idx = plr[myplr].HoldItem.IDidx;
		if ((plr[myplr].HoldItem._iStatFlag) && (AllItemsList[idx].iUsable)) {
			for (i = 0; (i < MAXSPD) && (!done); i++) {
				if (plr[myplr].SpdList[i]._itype == -1) {
					plr[myplr].SpdList[i] = plr[myplr].HoldItem;
					done = TRUE;
				}
			}
		}
		// Start in lower left and continue trying right then up a line
		for (i = 30; (i <= 39) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 20; (i <= 29) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 10; (i <= 19) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 0; (i <= 9) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
	}
	if ((w == 1) && (h == 2)) {
		// Try 3rd row, 1st row, and then 2nd row
		for (i = 29; (i >= 20) && (!done); i--) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 9; (i >= 0) && (!done); i--) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 19; (i >= 10) && (!done); i--) done = AutoPlace(myplr, i, w, h, TRUE);
	}
	if ((w == 1) && (h == 3)) {
		// Try 1st row then 2nd row
		for (i = 0; (i < 20) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
	}
	if ((w == 2) && (h == 2)) {
		// Try 1st and 3rd row starting right and moving left
		for (i = 0; (i < 10) && (!done); i++) done = AutoPlace(myplr, AP2x2Tbl[i], w, h, TRUE);
		// Try 3rd row, 1st row, and then 2nd row
		for (i = 21; (i < 29) && (!done); i+=2) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 1; (i < 9) && (!done); i+=2) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 10; (i < 19) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
	}
	if ((w == 2) && (h == 3)) {
		// Try 1st row then 2nd row
		for (i = 0; (i < 9) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
		for (i = 10; (i < 19) && (!done); i++) done = AutoPlace(myplr, i, w, h, TRUE);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartSmith()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 1, TRUE, "Welcome to the", ICOLOR_GOLD, FALSE);
	AddSText(0, 3, TRUE, "Blacksmith's shop", ICOLOR_GOLD, FALSE);
	AddSText(0, 7, TRUE, "Would you like to:", ICOLOR_GOLD, FALSE);
	AddSText(0, 10, TRUE, "Talk to Griswold", ICOLOR_BLUE, TRUE);
	AddSText(0, 12, TRUE, "Buy basic items", ICOLOR_WHITE, TRUE);
	AddSText(0, 14, TRUE, "Buy premium items", ICOLOR_WHITE, TRUE);
	AddSText(0, 16, TRUE, "Sell items", ICOLOR_WHITE, TRUE);
	AddSText(0, 18, TRUE, "Repair items", ICOLOR_WHITE, TRUE);
	AddSText(0, 20, TRUE, "Leave the shop", ICOLOR_WHITE, TRUE);
	AddSLine(5);
	storenumh = 20;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_ScrollSBuy(int idx)
{
	int l, ls;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;
	for (l = 5; l < 20; l+=4) {
		if (smithitem[idx]._itype != -1) {
			ls = l;
			iclr = ICOLOR_WHITE;
			if (smithitem[idx]._iMagical) iclr = ICOLOR_BLUE;
			if (!smithitem[idx]._iStatFlag) iclr = ICOLOR_RED;
			if (smithitem[idx]._iMagical) AddSText(20, l, FALSE, smithitem[idx]._iIName, iclr, TRUE);
			else AddSText(20, l, FALSE, smithitem[idx]._iName, iclr, TRUE);
			AddSTextVal(l, smithitem[idx]._iIvalue);
			PrintStoreItem(&smithitem[idx], l+1, iclr);
			stextdown = ls;
			idx++;
		}
	}
	if ((!stext[stextsel]._ssel) && (stextsel != 22)) stextsel = stextdown;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartSBuy()
{
	int i;

	stextsize = STEXT_LARGE;
	stextscrl = TRUE;
	stextsval = 0;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSBuy(stextsval);
	AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, FALSE);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (i = 0; smithitem[i]._itype != -1; i++) storenumh++;
	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
	//if (storenumh == 0) StartStore(STORE_SMITH);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_ScrollSPBuy(int idx)
{
	int l, ls;
	char iclr;
	int boughtitems;

	ClearSText(5, 21);
	stextup = 5;
	// Premium items don't get deleted from the list, so idx is not an index
	// into premium array, but rather to the nth available item.
	boughtitems = idx;
	idx = 0;
	while (boughtitems) {
		if (premiumitem[idx]._itype != -1)
			boughtitems--;
		idx++;
	}
	for (l = 5; ((l < 20) && (idx < MAXPREMIUM)); l+=4) {
		if (premiumitem[idx]._itype != -1) {
			ls = l;
			iclr = ICOLOR_WHITE;
			if (premiumitem[idx]._iMagical) iclr = ICOLOR_BLUE;
			if (!premiumitem[idx]._iStatFlag) iclr = ICOLOR_RED;
			AddSText(20, l, FALSE, premiumitem[idx]._iIName, iclr, TRUE);
			AddSTextVal(l, premiumitem[idx]._iIvalue);
			PrintStoreItem(&premiumitem[idx], l+1, iclr);
			stextdown = ls;
		} else if (idx < MAXPREMIUM) l-=4;
		idx++;
	}
	if ((!stext[stextsel]._ssel) && (stextsel != 22)) stextsel = stextdown;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL S_StartSPBuy()
{
	int i;

	storenumh = 0;
	for (i = 0; i < MAXPREMIUM; i++) if (premiumitem[i]._itype != -1) storenumh++;
	if (storenumh == 0) {
		StartStore(STORE_SMITH);
		stextsel = 14;
		return(FALSE);
	} else {
		stextsize = STEXT_LARGE;
		stextscrl = TRUE;
		stextsval = 0;
		sprintf(tempstr, "I have these premium items for sale :   Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, FALSE);
		OffsetSTextY(22, 6);

		stextsmax = storenumh - 4;
		if (stextsmax < 0) stextsmax = 0;

		S_ScrollSPBuy(stextsval);
	}
	return(TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL SmithSellOk(int i)
{
	ItemStruct *pI;

	if (i >= 0)
		pI = &plr[myplr].InvList[i];
	else
		pI = &plr[myplr].SpdList[-(i+1)];

	if (pI->_itype == -1) return FALSE;
	
	if (pI->_iMiscId > IMID_FIRSTOIL &&
	 	pI->_iMiscId < IMID_LASTOIL)
	 	return TRUE;
		
	if (pI->_itype == IT_MISC) return FALSE;
	if (pI->_itype == IT_GOLD) return FALSE;
	if (pI->_itype == IT_FOOD) return FALSE;
	if (pI->_itype == IT_STAFF && pI->_iSpell != SPL_NONE) return FALSE;
	if (pI->_iClass == IC_SPECIAL) return FALSE;
	if (pI->IDidx == IDI_LAZSTAFF) return FALSE;
	
//	if (plr[myplr].InvList[i]._iMagical && plr[myplr].InvList[i]._iIdentified) {
//		if (plr[myplr].InvList[i]._iIvalue == 0) return FALSE;
//	}
	return TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_ScrollSSell(int idx)
{
	int l, ls, sidx, v;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;
	sidx = idx;
	for (l = 5; l < 20 && idx < storenumh; l+=4) {
		if (storehold[idx]._itype != -1) {
			ls = l;
			iclr = ICOLOR_WHITE;
			if (storehold[idx]._iMagical) iclr = ICOLOR_BLUE;
			if (!storehold[idx]._iStatFlag) iclr = ICOLOR_RED;
			if (storehold[idx]._iMagical && storehold[idx]._iIdentified) {
				AddSText(20, l, FALSE, storehold[idx]._iIName, iclr, TRUE);
				v = storehold[idx]._iIvalue;
			} else {
				AddSText(20, l, FALSE, storehold[idx]._iName, iclr, TRUE);
				v = storehold[idx]._ivalue;
			}
			AddSTextVal(l, v);
			PrintStoreItem(&storehold[idx], l+1, iclr);
			stextdown = ls;
		}
		idx++;
	}
	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartSSell()
{
	int i;
	BOOL sellok;

	stextsize = STEXT_LARGE;
	sellok = FALSE;
	storenumh = 0;
	for (i = 0; i < MAXHOLDITEMS; i++) storehold[i]._itype = -1;
	for (i = 0; i < plr[myplr]._pNumInv && storenumh < MAXHOLDITEMS; i++) {
		if (SmithSellOk(i)) {
			sellok = TRUE;
			storehold[storenumh] = plr[myplr].InvList[i];
			if (storehold[storenumh]._iMagical && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;
			storehold[storenumh]._ivalue = storehold[storenumh]._ivalue >> 2;
			if (!storehold[storenumh]._ivalue)
				storehold[storenumh]._ivalue = 1;
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = i;
			storenumh++;
		}
	}
	for (i = 0; i < MAXSPD && storenumh < MAXHOLDITEMS; i++) {
		if (SmithSellOk(-(i+1))) {
			sellok = TRUE;
			storehold[storenumh] = plr[myplr].SpdList[i];
			if (storehold[storenumh]._iMagical && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;
			storehold[storenumh]._ivalue = storehold[storenumh]._ivalue >> 2;
			if (!storehold[storenumh]._ivalue)
				storehold[storenumh]._ivalue = 1;
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = -(i+1);
			storenumh++;
		}
	}
	
	if (!sellok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing I want.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Which item is for sale?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL SmithRepairOk(int i)
{
	if (plr[myplr].InvList[i]._itype == -1) return FALSE;
	if (plr[myplr].InvList[i]._itype == IT_MISC) return FALSE;
	if (plr[myplr].InvList[i]._itype == IT_GOLD) return FALSE;
	if (plr[myplr].InvList[i]._itype == IT_FOOD) return FALSE;
	if (plr[myplr].InvList[i]._iDurability == plr[myplr].InvList[i]._iMaxDur) return FALSE;
	return TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void AddStoreHoldRepair(ItemStruct * itm,int i)
{
	int v;

	app_assert(itm->_iMaxDur > 0);

	storehold[storenumh] = *itm;
	itm = &storehold[storenumh];
	if (itm->_iMagical && itm->_iIdentified)
		itm->_ivalue = (itm->_iIvalue * 30) / 100;
	v = ((itm->_iMaxDur - itm->_iDurability) * 100) / itm->_iMaxDur;
	v = (v * itm->_ivalue) / 100;
	if (v == 0) {
		if (itm->_iMagical && itm->_iIdentified) return;
		else v = 1;
	}
	if (v > 1) v >>= 1;
	itm->_ivalue = itm->_iIvalue = v;
	storehidx[storenumh++] = i;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartSRepair()
{
	int i;
	BOOL repairok;

	stextsize = STEXT_LARGE;
	repairok = FALSE;
	storenumh = 0;
	for (i = 0; i < MAXHOLDITEMS; i++) storehold[i]._itype = -1;
	if ((plr[myplr].HeadItem._itype != -1) && (plr[myplr].HeadItem._iDurability != plr[myplr].HeadItem._iMaxDur)) {
		repairok = TRUE;
		AddStoreHoldRepair(&plr[myplr].HeadItem, -1);
	}
	if ((plr[myplr].BodyItem._itype != -1) && (plr[myplr].BodyItem._iDurability != plr[myplr].BodyItem._iMaxDur)) {
		repairok = TRUE;
		AddStoreHoldRepair(&plr[myplr].BodyItem, -2);
	}
	if ((plr[myplr].Hand1Item._itype != -1) && (plr[myplr].Hand1Item._iDurability != plr[myplr].Hand1Item._iMaxDur)) {
		repairok = TRUE;
		AddStoreHoldRepair(&plr[myplr].Hand1Item, -3);
	}
	if ((plr[myplr].Hand2Item._itype != -1) && (plr[myplr].Hand2Item._iDurability != plr[myplr].Hand2Item._iMaxDur)) {
		repairok = TRUE;
		AddStoreHoldRepair(&plr[myplr].Hand2Item, -4);
	}
	for (i = 0; i < plr[myplr]._pNumInv && storenumh < MAXHOLDITEMS; i++) {
		if (SmithRepairOk(i)) {
			repairok = TRUE;
			AddStoreHoldRepair(&plr[myplr].InvList[i], i);
		}
	}
	if (!repairok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing to repair.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Repair which item?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartWitch()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 2, TRUE, "Witch's shack", ICOLOR_GOLD, FALSE);
	AddSText(0, 9, TRUE, "Would you like to:", ICOLOR_GOLD, FALSE);
	AddSText(0, 12, TRUE, "Talk to Adria", ICOLOR_BLUE, TRUE);
	AddSText(0, 14, TRUE, "Buy items", ICOLOR_WHITE, TRUE);
	AddSText(0, 16, TRUE, "Sell items", ICOLOR_WHITE, TRUE);
	AddSText(0, 18, TRUE, "Recharge staves", ICOLOR_WHITE, TRUE);
	AddSText(0, 20, TRUE, "Leave the shack", ICOLOR_WHITE, TRUE);
	AddSLine(5);
	storenumh = 20;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_ScrollWBuy(int idx)
{
	int l, ls, sidx;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;
	sidx = idx;
	for (l = 5; l < 20; l+=4) {
		if (witchitem[idx]._itype != -1) {
			ls = l;
			iclr = ICOLOR_WHITE;
			if (witchitem[idx]._iMagical) iclr = ICOLOR_BLUE;
			if (!witchitem[idx]._iStatFlag) iclr = ICOLOR_RED;
			if (witchitem[idx]._iMagical) AddSText(20, l, FALSE, witchitem[idx]._iIName, iclr, TRUE);
			else AddSText(20, l, FALSE, witchitem[idx]._iName, iclr, TRUE);
			AddSTextVal(l, witchitem[idx]._iIvalue);
			PrintStoreItem(&witchitem[idx], l+1, iclr);
			stextdown = ls;
			idx++;
		}
	}
	if ((!stext[stextsel]._ssel) && (stextsel != 22)) stextsel = stextdown;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartWBuy()
{
	int i;

	stextsize = STEXT_LARGE;
	stextscrl = TRUE;
	stextsval = 0;
	stextsmax = 20;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
	AddSLine(3);
	AddSLine(21);
	S_ScrollWBuy(stextsval);
	AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, FALSE);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (i = 0; witchitem[i]._itype != -1; i++) storenumh++;
	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL WitchSellOk(int i)
{
	BOOL rv;
	ItemStruct *pI;

	rv = FALSE;
	if (i >= 0)
		pI = &plr[myplr].InvList[i];
	else
		pI = &plr[myplr].SpdList[-(i+1)];

	if (pI->_itype == IT_MISC) rv = TRUE;
	if ((pI->_iMiscId > IMID_FIRSTOIL) && (pI->_iMiscId < IMID_LASTOIL))
		rv = FALSE;
	if (pI->_iClass == IC_SPECIAL)
		rv = FALSE;
	
	if (pI->_itype == IT_STAFF && pI->_iSpell != SPL_NONE) rv = TRUE;
//	if (pI->_iMagical && pI->_iIdentified) {
//		if (pI->_iIvalue == 0) rv = FALSE;
//	}
	if (pI->IDidx >= IDI_FIRSTQUEST
		&& pI->IDidx <= IDI_LASTQUEST)
		rv = FALSE;
	if (pI->IDidx == IDI_LAZSTAFF) rv = FALSE;
	return(rv);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartWSell()
{
	int i;
	BOOL sellok;

	stextsize = STEXT_LARGE;
	sellok = FALSE;
	storenumh = 0;
	for (i = 0; i < MAXHOLDITEMS; i++) storehold[i]._itype = -1;
	for (i = 0; i < plr[myplr]._pNumInv && storenumh < MAXHOLDITEMS; i++) {
		if (WitchSellOk(i)) {
			sellok = TRUE;
			storehold[storenumh] = plr[myplr].InvList[i];
			if (storehold[storenumh]._iMagical && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;
			storehold[storenumh]._ivalue = storehold[storenumh]._ivalue >> 2;
			if (!storehold[storenumh]._ivalue)
				storehold[storenumh]._ivalue = 1;
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = i;
			storenumh++;
		}
	}
	for (i = 0; i < MAXSPD && storenumh < MAXHOLDITEMS; i++) {
		if ((plr[myplr].SpdList[i]._itype != -1) && (WitchSellOk(-(i+1)))) {
			sellok = TRUE;
			storehold[storenumh] = plr[myplr].SpdList[i];
			if (storehold[storenumh]._iMagical && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;
			storehold[storenumh]._ivalue = storehold[storenumh]._ivalue >> 2;
			if (!storehold[storenumh]._ivalue)
				storehold[storenumh]._ivalue = 1;
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = -(i+1);
			storenumh++;
		}
	}
	if (!sellok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing I want.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Which item is for sale?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL WitchRechargeOk(int i)
{
	BOOL rv;

	rv = FALSE;
	if ((plr[myplr].InvList[i]._itype == IT_STAFF) &&
		(plr[myplr].InvList[i]._iCharges != plr[myplr].InvList[i]._iMaxCharges))
	{
		rv = TRUE;
	}
	
	// new: you can recharge unique items too.
	if ((plr[myplr].InvList[i]._iMiscId == IMID_UNIQUE ||
		(plr[myplr].InvList[i]._iMiscId == IMID_STAFF)) &&
		(plr[myplr].InvList[i]._iCharges < plr[myplr].InvList[i]._iMaxCharges))
	{
		rv = TRUE;
	}
	//*****
	return(rv);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AddStoreHoldRecharge(ItemStruct itm, int i)
{
	int v;

	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue += spelldata[itm._iSpell].sStaffCost;
	v = ((storehold[storenumh]._iMaxCharges - storehold[storenumh]._iCharges) * 100) / storehold[storenumh]._iMaxCharges;
	storehold[storenumh]._ivalue = ((v * storehold[storenumh]._ivalue) / 100) >> 1;
	storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
	storehidx[storenumh] = i;
	storenumh++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartWRecharge()
{
	int i;
	BOOL rechargeok;

	stextsize = STEXT_LARGE;
	rechargeok = FALSE;
	storenumh = 0;
	for (i = 0; i < MAXHOLDITEMS; i++) storehold[i]._itype = -1;
	if (( (plr[myplr].Hand1Item._itype == IT_STAFF)
		   || (plr[myplr].Hand1Item._iMiscId == IMID_UNIQUE))
		 && (plr[myplr].Hand1Item._iCharges != plr[myplr].Hand1Item._iMaxCharges))
	{
		rechargeok = TRUE;
		AddStoreHoldRecharge(plr[myplr].Hand1Item, -1);
	}
	for (i = 0; i < plr[myplr]._pNumInv && storenumh < MAXHOLDITEMS; i++) {
		if (WitchRechargeOk(i)) {
			rechargeok = TRUE;
			AddStoreHoldRecharge(plr[myplr].InvList[i], i);
		}
	}
	if (!rechargeok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing to recharge.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Recharge which item?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartNoMoney()
{
	StartStore(stextshold);
	stextsize = STEXT_LARGE;
	stextscrl = FALSE;
	ClearSText(5, 23);
	AddSText(0, 14, TRUE, "You do not have enough gold", ICOLOR_WHITE, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartNoRoom()
{
	StartStore(stextshold);
	stextscrl = FALSE;
	ClearSText(5, 23);
	AddSText(0, 14, TRUE, "You do not have enough room in inventory", ICOLOR_WHITE, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartConfirm()
{
	char iclr;
	BOOL idprint;

	StartStore(stextshold);
	stextscrl = FALSE;
	ClearSText(5, 23);
	iclr = ICOLOR_WHITE;
	if (plr[myplr].HoldItem._iMagical) iclr = ICOLOR_BLUE;
	if (!plr[myplr].HoldItem._iStatFlag) iclr = ICOLOR_RED;

	if (plr[myplr].HoldItem._iMagical) idprint = TRUE;
	else idprint = FALSE;

	if (stextshold == STORE_STORYID) idprint = FALSE;

	if ((plr[myplr].HoldItem._iMagical) && (!plr[myplr].HoldItem._iIdentified)) {
		if (stextshold == STORE_SSELL) idprint = FALSE;
		if (stextshold == STORE_WSELL) idprint = FALSE;
		if (stextshold == STORE_SREPAIR) idprint = FALSE;
		if (stextshold == STORE_WRECHARGE) idprint = FALSE;
	}

	if (idprint)
		AddSText(20, 8, FALSE, plr[myplr].HoldItem._iIName, iclr, FALSE);
	else
		AddSText(20, 8, FALSE, plr[myplr].HoldItem._iName, iclr, FALSE);

	AddSTextVal(8, plr[myplr].HoldItem._iIvalue);
	PrintStoreItem(&plr[myplr].HoldItem, 9, iclr);
	switch(stextshold) {
		case STORE_SBUY:
		case STORE_WBUY:
		case STORE_HBUY:
		case STORE_SPBUY:
			strcpy(tempstr, "Are you sure you want to buy this item?");
			break;
		case STORE_SSELL:
		case STORE_WSELL:
			strcpy(tempstr, "Are you sure you want to sell this item?");
			break;
		case STORE_SREPAIR:
			strcpy(tempstr, "Are you sure you want to repair this item?");
			break;
		case STORE_WRECHARGE:
			strcpy(tempstr, "Are you sure you want to recharge this item?");
			break;
		case STORE_BBUY:
			strcpy(tempstr, "Do we have a deal?");
			break;
		case STORE_STORYID:
			strcpy(tempstr, "Are you sure you want to identify this item?");
			break;
	}
	AddSText(0, 15, TRUE, tempstr, ICOLOR_WHITE, FALSE);
	AddSText(0, 18, TRUE, "Yes", ICOLOR_WHITE, TRUE);
	AddSText(0, 20, TRUE, "No", ICOLOR_WHITE, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartBoy()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 2, TRUE, "Wirt the Peg-legged boy", ICOLOR_GOLD, FALSE);
	AddSLine(5);
	if (boyitem._itype != -1) {
		AddSText(0, 8, TRUE, "Talk to Wirt", ICOLOR_BLUE, TRUE);
		AddSText(0, 12, TRUE, "I have something for sale,", ICOLOR_GOLD, FALSE);
		AddSText(0, 14, TRUE, "but it will cost 50 gold", ICOLOR_GOLD, FALSE);
		AddSText(0, 16, TRUE, "just to take a look. ", ICOLOR_GOLD, FALSE);
		AddSText(0, 18, TRUE, "What have you got?", ICOLOR_WHITE, TRUE);
		AddSText(0, 20, TRUE, "Say goodbye", ICOLOR_WHITE, TRUE);
	}
	else {
		AddSText(0, 12, TRUE, "Talk to Wirt", ICOLOR_BLUE, TRUE);
		AddSText(0, 18, TRUE, "Say goodbye", ICOLOR_WHITE, TRUE);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartBBoy()
{
	int iclr;

	stextsize = STEXT_LARGE;
	stextscrl = FALSE;
	sprintf(tempstr, "I have this item for sale :           Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
	AddSLine(3);
	AddSLine(21);
	iclr = ICOLOR_WHITE;
	if (boyitem._iMagical) iclr = ICOLOR_BLUE;
	if (!boyitem._iStatFlag) iclr = ICOLOR_RED;
	if (boyitem._iMagical) AddSText(20, 10, FALSE, boyitem._iIName, iclr, TRUE);
	else AddSText(20, 10, FALSE, boyitem._iName, iclr, TRUE);
// PATCH2.JMM
	// No actual change--just note that the value of the item = 75% value
	AddSTextVal(10, boyitem._iIvalue - (boyitem._iIvalue >> 2));
// ENDPATCH2.JMM
	PrintStoreItem(&boyitem, 11, iclr);
	AddSText(0, 22, TRUE, "Leave", ICOLOR_WHITE, TRUE);
	OffsetSTextY(22, 6);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartHealer()
{
	// heal automatically
	if (plr[myplr]._pHitPoints != plr[myplr]._pMaxHP) PlaySFX(IS_CAST8);
	plr[myplr]._pHitPoints = plr[myplr]._pMaxHP;
	plr[myplr]._pHPBase = plr[myplr]._pMaxHPBase;
	drawhpflag = TRUE;
	
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 1, TRUE, "Welcome to the", ICOLOR_GOLD, FALSE);
	AddSText(0, 3, TRUE, "Healer's home", ICOLOR_GOLD, FALSE);
	AddSText(0, 9, TRUE, "Would you like to:", ICOLOR_GOLD, FALSE);
	AddSText(0, 12, TRUE, "Talk to Pepin", ICOLOR_BLUE, TRUE);
//	AddSText(0, 14, TRUE, "Receive healing", ICOLOR_WHITE, TRUE);
	AddSText(0, 14, TRUE, "Buy items", ICOLOR_WHITE, TRUE);
	AddSText(0, 16, TRUE, "Leave Healer's home", ICOLOR_WHITE, TRUE);
	AddSLine(5);
	storenumh = 20;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_ScrollHBuy(int idx)
{
	int l, ls;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;
	for (l = 5; l < 20; l+=4) {
		if (healitem[idx]._itype != -1) {
			ls = l;
			iclr = ICOLOR_WHITE;
			if (!healitem[idx]._iStatFlag) iclr = ICOLOR_RED;
			AddSText(20, l, FALSE, healitem[idx]._iName, iclr, TRUE);
			AddSTextVal(l, healitem[idx]._iIvalue);
			PrintStoreItem(&healitem[idx], l+1, iclr);
			stextdown = ls;
			idx++;
		}
	}
	if ((!stext[stextsel]._ssel) && (stextsel != 22)) stextsel = stextdown;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartHBuy()
{
	int i;

	stextsize = STEXT_LARGE;
	stextscrl = TRUE;
	stextsval = 0;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
	AddSLine(3);
	AddSLine(21);
	S_ScrollHBuy(stextsval);
	AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, FALSE);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (i = 0; healitem[i]._itype != -1; i++) storenumh++;
	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartStory()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 2, TRUE, "The Town Elder", ICOLOR_GOLD, FALSE);
	AddSText(0, 9, TRUE, "Would you like to:", ICOLOR_GOLD, FALSE);
	AddSText(0, 12, TRUE, "Talk to Cain", ICOLOR_BLUE, TRUE);
	AddSText(0, 14, TRUE, "Identify an item", ICOLOR_WHITE, TRUE);
	AddSText(0, 18, TRUE, "Say goodbye", ICOLOR_WHITE, TRUE);
	AddSLine(5);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL IdItemOk(ItemStruct *i)
{
	if (i->_itype == -1) return(FALSE);
	if (i->_iMagical == IMAGIC_NONE) return(FALSE);
	if (i->_iIdentified) return(FALSE);
	return(TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#define STOREIDCOST		100

void AddStoreHoldId(ItemStruct itm, int i)
{
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue = STOREIDCOST;
	storehold[storenumh]._iIvalue = STOREIDCOST;
	storehidx[storenumh] = i;
	storenumh++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartSIdentify()
{
	int i;
	BOOL idok;

	stextsize = STEXT_LARGE;
	idok = FALSE;
	storenumh = 0;
	for (i = 0; i < MAXHOLDITEMS; i++) storehold[i]._itype = -1;
	if (IdItemOk(&plr[myplr].HeadItem)) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].HeadItem, -1);
	}
	if (IdItemOk(&plr[myplr].BodyItem)) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].BodyItem, -2);
	}
	if (IdItemOk(&plr[myplr].Hand1Item)) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].Hand1Item, -3);
	}
	if (IdItemOk(&plr[myplr].Hand2Item)) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].Hand2Item, -4);
	}
	if (IdItemOk(&plr[myplr].Ring1Item)) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].Ring1Item, -5);
	}
	if (IdItemOk(&plr[myplr].Ring2Item)) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].Ring2Item, -6);
	}
	if (IdItemOk(&plr[myplr].NeckItem)) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].NeckItem, -7);
	}
	for (i = 0; i < plr[myplr]._pNumInv && storenumh < MAXHOLDITEMS; i++) {
		if (IdItemOk(&plr[myplr].InvList[i])) {
			idok = TRUE;
			AddStoreHoldId(plr[myplr].InvList[i], i);
		}
	}
	if (!idok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing to identify.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Identify which item?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, TRUE, tempstr, ICOLOR_GOLD, FALSE);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
		OffsetSTextY(22, 6);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartIdShow()
{
	char iclr;

	StartStore(stextshold);
	stextscrl = FALSE;
	ClearSText(5, 23);
	iclr = ICOLOR_WHITE;
	if (plr[myplr].HoldItem._iMagical) iclr = ICOLOR_BLUE;
	if (!plr[myplr].HoldItem._iStatFlag) iclr = ICOLOR_RED;

	AddSText(0, 7, TRUE, "This item is:", ICOLOR_WHITE, FALSE);

	AddSText(20, 11, FALSE, plr[myplr].HoldItem._iIName, iclr, FALSE);
	PrintStoreItem(&plr[myplr].HoldItem, 12, iclr);

	AddSText(0, 18, TRUE, "Done", ICOLOR_WHITE, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#if !IS_VERSION(SHAREWARE)
void S_StartTalk()
{
	int i, tq, sn, la, gl;

	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	sprintf(tempstr, "Talk to %s", talkname[talker]);
	AddSText(0, 2, TRUE, tempstr, ICOLOR_GOLD, FALSE);
	AddSLine(5);

	tq = 0;
	for (i = 0; i < MAXQUESTS; i++) {
		if ((quests[i]._qactive == QUEST_NOTDONE) && (Qtalklist[talker][i] != -1) && quests[i]._qlog) tq++;
	}
	if (tq > 6) {
		sn = 14 - (tq >> 1);
		la = 1;
	} else {
		sn = 15 - tq;
		la = 2;
	}
	gl = sn - 2;
	for (i = 0; i < MAXQUESTS; i++) {
		if ((quests[i]._qactive == QUEST_NOTDONE) && (Qtalklist[talker][i] != -1) && quests[i]._qlog) {
			AddSText(0,  sn, TRUE, questlist[i]._qlstr, ICOLOR_WHITE, TRUE);
			sn += la;
		}
	}

	AddSText(0, gl, TRUE, "Gossip", ICOLOR_BLUE, TRUE);

	AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
}

#else
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartShareTalk()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	sprintf(tempstr, "Talk to %s", talkname[talker]);
	AddSText(0, 2, TRUE, tempstr, ICOLOR_GOLD, FALSE);
	AddSLine(5);

	sprintf(tempstr, "Talking to %s", talkname[talker]);
	AddSText(0, 10, TRUE, tempstr, ICOLOR_WHITE, FALSE);
	AddSText(0, 12, TRUE, "is not available", ICOLOR_WHITE, FALSE);
	AddSText(0, 14, TRUE, "in the shareware", ICOLOR_WHITE, FALSE);
	AddSText(0, 16, TRUE, "version.", ICOLOR_WHITE, FALSE);
	AddSText(0, 22, TRUE, "Back", ICOLOR_WHITE, TRUE);
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartTavern()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 1, TRUE, "Welcome to the", ICOLOR_GOLD, FALSE);
	AddSText(0, 3, TRUE, "Rising Sun", ICOLOR_GOLD, FALSE);
	AddSText(0, 9, TRUE, "Would you like to:", ICOLOR_GOLD, FALSE);
	AddSText(0, 12, TRUE, "Talk to Ogden", ICOLOR_BLUE, TRUE);
	AddSText(0, 18, TRUE, "Leave the tavern", ICOLOR_WHITE, TRUE);
	AddSLine(5);
	storenumh = 20;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartBarMaid()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 2, TRUE, "Gillian", ICOLOR_GOLD, FALSE);
	AddSText(0, 9, TRUE, "Would you like to:", ICOLOR_GOLD, FALSE);
	AddSText(0, 12, TRUE, "Talk to Gillian", ICOLOR_BLUE, TRUE);
	AddSText(0, 18, TRUE, "Say goodbye", ICOLOR_WHITE, TRUE);
	AddSLine(5);
	storenumh = 20;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StartDrunk()
{
	stextsize = STEXT_SMALL;
	stextscrl = FALSE;
	AddSText(0, 2, TRUE, "Farnham the Drunk", ICOLOR_GOLD, FALSE);
	AddSText(0, 9, TRUE, "Would you like to:", ICOLOR_GOLD, FALSE);
	AddSText(0, 12, TRUE, "Talk to Farnham", ICOLOR_BLUE, TRUE);
	AddSText(0, 18, TRUE, "Say Goodbye", ICOLOR_WHITE, TRUE);
	AddSLine(5);
	storenumh = 20;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void StartStore(char s)
{
	int i;

	sbookflag = FALSE;
	invflag = FALSE;
	chrflag = FALSE;
	questlog = FALSE;
	dropGoldFlag = FALSE;
	ClearSText(0, NUMSTLINES);
	ReleaseStoreBtn();
	switch(s) {
		case STORE_SMITH:
			S_StartSmith();
			break;
		case STORE_SBUY:
			if (storenumh > 0) S_StartSBuy();
			break;
		case STORE_SSELL:
			S_StartSSell();
			break;
		case STORE_SREPAIR:
			S_StartSRepair();
			break;
		case STORE_WITCH:
			S_StartWitch();
			break;
		case STORE_WBUY:
			if (storenumh > 0) S_StartWBuy();
			break;
		case STORE_WSELL:
			S_StartWSell();
			break;
		case STORE_WRECHARGE:
			S_StartWRecharge();
			break;
		case STORE_NOMONEY:
			S_StartNoMoney();
			break;
		case STORE_NOROOM:
			S_StartNoRoom();
			break;
		case STORE_CONFIRM:
			S_StartConfirm();
			break;
		case STORE_BOY:
			S_StartBoy();
			break;
		case STORE_BBUY:
			S_StartBBoy();
			break;
		case STORE_HEALER:
			S_StartHealer();
			break;
		case STORE_STORYTLR:
			S_StartStory();
			break;
		case STORE_HBUY:
			if (storenumh > 0) S_StartHBuy();
			break;
		case STORE_STORYID:
			S_StartSIdentify();
			break;
		case STORE_SPBUY:
			if (! S_StartSPBuy()) return;
			break;
		case STORE_TALK:
			#if IS_VERSION(SHAREWARE)
			S_StartShareTalk();
			#else
			S_StartTalk();
			#endif
			break;
		case STORE_IDSHOW:
			S_StartIdShow();
			break;
		case STORE_TAVERN:
			S_StartTavern();
			break;
		case STORE_DRUNK:
			S_StartDrunk();
			break;
		case STORE_BARMAID:
			S_StartBarMaid();
			break;
	}

	for (i = 0; (i < NUMSTLINES) && (!stext[i]._ssel); i++)
		NULL;
	if (i == NUMSTLINES) stextsel = -1;
	else stextsel = i;
	stextflag = s;
	if (s == STORE_SBUY && storenumh == 0) {
		StartStore(STORE_SMITH);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawSText()
{
	int i;

	// Draw background
	if (stextsize == STEXT_SMALL) DrawSTextBack();
	else DrawQTextBack();

	if (stextscrl) {
		switch(stextflag) {
			case STORE_SBUY:
				S_ScrollSBuy(stextsval);
				break;
			case STORE_SPBUY:
				S_ScrollSPBuy(stextsval);
				break;
			case STORE_SSELL:
			case STORE_SREPAIR:
			case STORE_WSELL:
			case STORE_WRECHARGE:
			case STORE_STORYID:
				S_ScrollSSell(stextsval);
				break;
			case STORE_WBUY:
				S_ScrollWBuy(stextsval);
				break;
			case STORE_HBUY:
				S_ScrollHBuy(stextsval);
				break;
		}
	}

	for (i = 0; i < NUMSTLINES; i++) {
		if (stext[i]._sline) DrawSLine(i);
		if (stext[i]._sstr[0] != 0) PrintSString(stext[i]._sx, i, stext[i]._sjust, stext[i]._sstr, stext[i]._sclr, stext[i]._sval);
	}

	if (stextscrl) DrawScrlBar(4, 20);

	stextspin = (stextspin & 0x7) + 1;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void STextESC()
{
	if (qtextflag) {
		qtextflag = FALSE;
		if (leveltype == 0) stream_stop();
		return;
	}
	switch(stextflag) {
		case STORE_SMITH:
		case STORE_WITCH:
		case STORE_BOY:
		case STORE_BBUY:
		case STORE_HEALER:
		case STORE_STORYTLR:
		case STORE_TAVERN:
		case STORE_BARMAID:
		case STORE_DRUNK:
			stextflag = STORE_NONE;
			break;
		case STORE_TALK:
			StartStore(stextshold);
			stextsel = stextlhold;
			break;
		case STORE_SBUY:
			StartStore(STORE_SMITH);
			stextsel = 12;
			break;
		case STORE_SPBUY:
			StartStore(STORE_SMITH);
			stextsel = 14;
			break;
		case STORE_SSELL:
			StartStore(STORE_SMITH);
			stextsel = 16;
			break;
		case STORE_SREPAIR:
			StartStore(STORE_SMITH);
			stextsel = 18;
			break;
		case STORE_WBUY:
			StartStore(STORE_WITCH);
			stextsel = 14;
			break;
		case STORE_WSELL:
			StartStore(STORE_WITCH);
			stextsel = 16;
			break;
		case STORE_WRECHARGE:
			StartStore(STORE_WITCH);
			stextsel = 18;
			break;
		case STORE_HBUY:
			StartStore(STORE_HEALER);
			stextsel = 16;
			break;
		case STORE_STORYID:
			StartStore(STORE_STORYTLR);
			stextsel = 14;
			break;
		case STORE_IDSHOW:
			StartStore(STORE_STORYID);
			break;
		case STORE_NOMONEY:
		case STORE_NOROOM:
		case STORE_CONFIRM:
			StartStore(stextshold);
			stextsel = stextlhold;
			stextsval = stextvhold;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void STextUp()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1) {
		if (stextscrl) {
			if (stextsel == stextup) {
				if (stextsval != 0) stextsval--;
			} else {
				stextsel--;
				while (!stext[stextsel]._ssel) {
					if (stextsel == 0) stextsel = NUMSTLINES-1;
					else stextsel--;
				}
			}
		} else {
			if (stextsel == 0) stextsel = NUMSTLINES-1;
			else stextsel--;
			while (!stext[stextsel]._ssel) {
				if (stextsel == 0) stextsel = NUMSTLINES-1;
				else stextsel--;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void STextDown()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1) {
		if (stextscrl) {
			if (stextsel == stextdown) {
				if (stextsval < stextsmax) stextsval++;
			} else {
				stextsel++;
				while (!stext[stextsel]._ssel) {
					if (stextsel == NUMSTLINES-1) stextsel = 0;
					else stextsel++;
				}
			}
		} else {
			if (stextsel == NUMSTLINES-1) stextsel = 0;
			else stextsel++;
			while (!stext[stextsel]._ssel) {
				if (stextsel == NUMSTLINES-1) stextsel = 0;
				else stextsel++;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void STextPgUp()
{
	PlaySFX(IS_TITLEMOV);
	if ((stextsel != -1) && (stextscrl)) {
		if (stextsel == stextup) {
			if (stextsval != 0) stextsval-=4;
			if (stextsval < 0) stextsval = 0;
		} else {
			stextsel = stextup;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void STextPgDown()
{
	PlaySFX(IS_TITLEMOV);
	if ((stextsel != -1) && (stextscrl)) {
		if (stextsel == stextdown) {
			if (stextsval < stextsmax) stextsval+=4;
			if (stextsval > stextsmax) stextsval = stextsmax;
		} else {
			stextsel = stextdown;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_SmithEnter()
{
	switch(stextsel) {
		case 10:
			talker = TWN_BLKSMITH;
			stextshold = STORE_SMITH;
			stextlhold = 10;
			gossipstart = TXT_GRIS2;
			gossipend = TXT_GRIS13;
			StartStore(STORE_TALK);
			break;
		case 12:
			StartStore(STORE_SBUY);
			break;
		case 14:
			StartStore(STORE_SPBUY);
			break;
		case 16:
			StartStore(STORE_SSELL);
			break;
		case 18:
			StartStore(STORE_SREPAIR);
			break;
		case 20:
			stextflag = STORE_NONE;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetGoldCurs(int pnum, int i)
{
	if (plr[pnum].InvList[i]._ivalue >= GOLD_VT2) {
		plr[pnum].InvList[i]._iCurs = ITEM_5GOLD;
	} else {
		if (plr[pnum].InvList[i]._ivalue <= GOLD_VT1) plr[pnum].InvList[i]._iCurs = ITEM_1GOLD;
		else plr[pnum].InvList[i]._iCurs = ITEM_3GOLD;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetSpdbarGoldCurs(int pnum, int i)
{
	if (plr[pnum].SpdList[i]._ivalue >= GOLD_VT2) {
		plr[pnum].SpdList[i]._iCurs = ITEM_5GOLD;
	} else {
		if (plr[pnum].SpdList[i]._ivalue <= GOLD_VT1) plr[pnum].SpdList[i]._iCurs = ITEM_1GOLD;
		else plr[pnum].SpdList[i]._iCurs = ITEM_3GOLD;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TakePlrsMoney(long cost)
{
	int	i;
			
	// Take players money
	plr[myplr]._pGold = CalculateGold(myplr) - cost;

	// Remove gold from speed list first; if there is any
	// Try non GOLD_VMAX first, then break into the GOLD_VMAX piles
	for (i = 0; ((i < MAXSPD) && (cost > 0)); i++) {
		if ((plr[myplr].SpdList[i]._itype == IT_GOLD) &&
			(plr[myplr].SpdList[i]._ivalue != GOLD_VMAX)) {
			if (cost < plr[myplr].SpdList[i]._ivalue) {
				plr[myplr].SpdList[i]._ivalue -= cost;
				SetSpdbarGoldCurs(myplr, i);
				cost = 0;
			} else {
				cost -= plr[myplr].SpdList[i]._ivalue;
				RemoveSpdBarItem(myplr, i);
				i = -1;		// restart looping
			} 
		} 
	} 
	if (cost > 0) {
		// GOLD_VMAX piles
		for (i = 0; ((i < MAXSPD) && (cost > 0)); i++) {
			if (plr[myplr].SpdList[i]._itype == IT_GOLD) {
				if (cost < plr[myplr].SpdList[i]._ivalue) {
					plr[myplr].SpdList[i]._ivalue -= cost;
					SetSpdbarGoldCurs(myplr, i);
					cost = 0;
				} else {
					cost -= plr[myplr].SpdList[i]._ivalue;
					RemoveSpdBarItem(myplr, i);
					i = -1;		// restart looping
				} 
			} 
		} 
	} 
	force_redraw = FULLDRAW;

	// Remove gold from inventory list if there was no gold in the speed list
	// or all gold from the speed list was deplenished.
	// Try non GOLD_VMAX first, then break into the GOLD_VMAX piles
	if (cost > 0) {
		for (i = 0; ((i < plr[myplr]._pNumInv) && (cost > 0)); i++) {
			if ((plr[myplr].InvList[i]._itype == IT_GOLD) &&
				(plr[myplr].InvList[i]._ivalue != GOLD_VMAX)) {
				if (cost < plr[myplr].InvList[i]._ivalue) {
					plr[myplr].InvList[i]._ivalue -= cost;
					SetGoldCurs(myplr, i);
					cost = 0;
				} else {
					cost -= plr[myplr].InvList[i]._ivalue;
					RemoveInvItem(myplr, i);
					i = -1;		// restart looping
				}
			}
		}
		if (cost > 0) {
			// GOLD_VMAX piles
			for (i = 0; ((i < plr[myplr]._pNumInv) && (cost > 0)); i++) {
				if (plr[myplr].InvList[i]._itype == IT_GOLD) {
					if (cost < plr[myplr].InvList[i]._ivalue) {
						plr[myplr].InvList[i]._ivalue -= cost;
						SetGoldCurs(myplr, i);
						cost = 0;
					} else {
						cost -= plr[myplr].InvList[i]._ivalue;
						RemoveInvItem(myplr, i);
						i = -1;		// restart looping
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SmithBuyItem()
{
	int idx;

	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	// Put in players inv
	if (!plr[myplr].HoldItem._iMagical) plr[myplr].HoldItem._iIdentified = FALSE;
	StoreAutoPlace();

	// Remove from smithitems
	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	if (idx == MAXSMITHITEMS-1) smithitem[idx]._itype = -1;
	else {
		while (smithitem[idx+1]._itype != -1) {
			smithitem[idx] = smithitem[idx+1];
			idx++;
		}
		smithitem[idx]._itype = -1;
	}

	CalcPlrInv(myplr, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_SBuyEnter()
{
	int idx, i;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 12;
	} else {
		stextshold = STORE_SBUY;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		if (plr[myplr]._pGold < smithitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = smithitem[idx];
			SetCursor(plr[myplr].HoldItem._iCurs + ICSTART);
			done = FALSE;
			for (i = 0; (i < MAXINV) && (!done); i++) done = AutoPlace(myplr, i, cursW/28, cursH/28, FALSE);
			if (done) StartStore(STORE_CONFIRM);
			else StartStore(STORE_NOROOM);
			SetCursor(GLOVE_CURS);
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SmithBuyPItem()
{
	int idx, i;

	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	// Put in players inv
	if (!plr[myplr].HoldItem._iMagical) plr[myplr].HoldItem._iIdentified = FALSE;
	StoreAutoPlace();

	// Remove from premiumitems
	int xx = ((stextlhold - stextup) >> 2) + stextvhold;
	idx = 0;
	for (i = 0; xx >= 0; i++) {
		if (premiumitem[i]._itype != -1) {
			xx--;
			idx = i;
		}
	}
	premiumitem[idx]._itype = -1;
	numpremium--;
	
	// replenish premium list
	SpawnPremium(myplr);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_SPBuyEnter()
{
	int idx, i;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 14;
	} else {
		stextshold = STORE_SPBUY;
		stextlhold = stextsel;
		stextvhold = stextsval;
		int xx = ((stextsel - stextup) >> 2) + stextsval;
		idx = 0;
		for (i = 0; xx >= 0; i++) {
			if (premiumitem[i]._itype != -1) {
				xx--;
				idx = i;
			}
		}
		if (plr[myplr]._pGold < premiumitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = premiumitem[idx];
			SetCursor(plr[myplr].HoldItem._iCurs + ICSTART);
			done = FALSE;
			for (i = 0; (i < MAXINV) && (!done); i++) done = AutoPlace(myplr, i, cursW/28, cursH/28, FALSE);
			if (done) StartStore(STORE_CONFIRM);
			else StartStore(STORE_NOROOM);
			SetCursor(GLOVE_CURS);
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL StoreGoldFit(int idx)
{
	int sz, numsqrs, i;
	long cost;

	// How many squares am I going to need?
	cost = storehold[idx]._iIvalue;
	numsqrs = cost / GOLD_VMAX;
	if ((cost % GOLD_VMAX) != 0) numsqrs++;
	SetCursor(storehold[idx]._iCurs + ICSTART);
	sz = (cursW/28) * (cursH/28);
	SetCursor(GLOVE_CURS);
	if (sz >= numsqrs) return(TRUE);
	for (i = 0; i < MAXINV; i++) {
		if (plr[myplr].InvGrid[i] == 0) sz++;
	}
	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if ((plr[myplr].InvList[i]._itype == IT_GOLD) &&
			(plr[myplr].InvList[i]._ivalue != GOLD_VMAX)) {
			if ((cost + plr[myplr].InvList[i]._ivalue) <= GOLD_VMAX) cost = 0;
			else cost -= (GOLD_VMAX - plr[myplr].InvList[i]._ivalue);
		}
	}
	numsqrs = cost / GOLD_VMAX;
	if ((cost % GOLD_VMAX) != 0) numsqrs++;
	if (sz >= numsqrs) return(TRUE);
	else return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void PlaceStoreGold(long v)
{
	int i, ii, xx, yy;
	BOOL done;

	done = FALSE;
	for (ii = 0; (ii < MAXINV) && (!done); ii++) {
		yy = (ii / 10) * 10;
		xx = ii % 10;
		if (plr[myplr].InvGrid[xx+yy] == 0) {
			i = plr[myplr]._pNumInv;
			// drb.patch1.start.02/13/97
			void GetGoldSeed(int pnum, ItemStruct *h);
			GetGoldSeed(myplr, &golditem);
			// drb.patch1.end.02/13/97
			plr[myplr].InvList[i] = golditem;
			plr[myplr]._pNumInv++;
			plr[myplr].InvGrid[xx+yy] = plr[myplr]._pNumInv;
			plr[myplr].InvList[i]._ivalue = v;
			SetGoldCurs(myplr, i);
			done = TRUE;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void StoreSellItem()
{
	int idx, i;
	long cost;

	// Remove item from inv
	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	if (storehidx[idx] >= 0) RemoveInvItem(myplr, storehidx[idx]);
	else RemoveSpdBarItem(myplr, -(storehidx[idx]+1));

	// Get value
	cost = storehold[idx]._iIvalue;

	// Remove from hold list
	storenumh--;
	if (idx != storenumh) {
		while (idx < storenumh) {
			storehold[idx] = storehold[idx+1];
			storehidx[idx] = storehidx[idx+1];
			idx++;
		}
	}

	// Add value to player
	plr[myplr]._pGold += cost;
	// Try placing with other gold first
	for (i = 0; ((i < plr[myplr]._pNumInv) && (cost > 0)); i++) {
		if ((plr[myplr].InvList[i]._itype == IT_GOLD) &&
			(plr[myplr].InvList[i]._ivalue != GOLD_VMAX)) {
			if ((cost + plr[myplr].InvList[i]._ivalue) <= GOLD_VMAX) {
				plr[myplr].InvList[i]._ivalue += cost;
				SetGoldCurs(myplr, i);
				cost = 0;
			} else {
				cost -= (GOLD_VMAX - plr[myplr].InvList[i]._ivalue);
				plr[myplr].InvList[i]._ivalue = GOLD_VMAX;
				SetGoldCurs(myplr, i);
			}
		}
	}
	// Place new gold slots
	if (cost > 0) {
		while (cost > GOLD_VMAX) {
			PlaceStoreGold(GOLD_VMAX);
			cost -= GOLD_VMAX;
		}
		PlaceStoreGold(cost);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_SSellEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 16;
	} else {
		stextshold = STORE_SSELL;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		plr[myplr].HoldItem = storehold[idx];
		if (StoreGoldFit(idx)) StartStore(STORE_CONFIRM);
		else StartStore(STORE_NOROOM);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SmithRepairItem()
{
	int i, idx;

	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	storehold[idx]._iDurability = storehold[idx]._iMaxDur;
	i = storehidx[idx];
	if (i < 0) {
		if (i == -1) plr[myplr].HeadItem._iDurability = plr[myplr].HeadItem._iMaxDur;
		if (i == -2) plr[myplr].BodyItem._iDurability = plr[myplr].BodyItem._iMaxDur;
		if (i == -3) plr[myplr].Hand1Item._iDurability = plr[myplr].Hand1Item._iMaxDur;
		if (i == -4) plr[myplr].Hand2Item._iDurability = plr[myplr].Hand2Item._iMaxDur;
	} else plr[myplr].InvList[i]._iDurability = plr[myplr].InvList[i]._iMaxDur;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_SRepairEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 18;
	} else {
		stextshold = STORE_SREPAIR;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		plr[myplr].HoldItem = storehold[idx];
		if (plr[myplr]._pGold < storehold[idx]._iIvalue) StartStore(STORE_NOMONEY);
		else StartStore(STORE_CONFIRM);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_WitchEnter()
{
	switch(stextsel) {
		case 12:
			talker = TWN_WITCH;
			stextshold = STORE_WITCH;
			stextlhold = 12;
			gossipstart = TXT_ADRIA2;
			gossipend = TXT_ADRIA13;
			StartStore(STORE_TALK);
			break;
		case 14:
			StartStore(STORE_WBUY);
			break;
		case 16:
			StartStore(STORE_WSELL);
			break;
		case 18:
			StartStore(STORE_WRECHARGE);
			break;
		case 20:
			stextflag = STORE_NONE;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void WitchBuyItem()
{
	int idx;

	// Check if mana, full mana, portal, at top of list. These need new seeds
	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	if (idx < 3) plr[myplr].HoldItem._iSeed = GetRndSeed();

	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	// Put in players inv
	StoreAutoPlace();

	// Remove from witchitems
	// If item index is mana, don't remove from her items list
	if (idx < 3) {
		CalcPlrInv(myplr, TRUE);
		return;
	}
	if (idx == MAXWITCHITEMS-1) witchitem[idx]._itype = -1;
	else {
		while (witchitem[idx+1]._itype != -1) {
			witchitem[idx] = witchitem[idx+1];
			idx++;
		}
		witchitem[idx]._itype = -1;
	}
		
	CalcPlrInv(myplr, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_WBuyEnter()
{
	int idx, i;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 14;
	} else {
		stextshold = STORE_WBUY;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		if (plr[myplr]._pGold < witchitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = witchitem[idx];
			SetCursor(plr[myplr].HoldItem._iCurs + ICSTART);
			done = FALSE;
			for (i = 0; (i < MAXINV) && (!done); i++) done = SpecialAutoPlace(myplr, i, cursW/28, cursH/28, FALSE);
			if (done) StartStore(STORE_CONFIRM);
			else StartStore(STORE_NOROOM);
			SetCursor(GLOVE_CURS);
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_WSellEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 16;
	} else {
		stextshold = STORE_WSELL;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		plr[myplr].HoldItem = storehold[idx];
		if (StoreGoldFit(idx)) StartStore(STORE_CONFIRM);
		else StartStore(STORE_NOROOM);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void WitchRechargeItem()
{
	int i, idx;

	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	// Recharge staff
	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	storehold[idx]._iCharges = storehold[idx]._iMaxCharges;
	i = storehidx[idx];
	if (i < 0) plr[myplr].Hand1Item._iCharges = plr[myplr].Hand1Item._iMaxCharges;
	else plr[myplr].InvList[i]._iCharges = plr[myplr].InvList[i]._iMaxCharges;
	CalcPlrInv(myplr, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_WRechargeEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 18;
	} else {
		stextshold = STORE_WRECHARGE;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		plr[myplr].HoldItem = storehold[idx];
		if (plr[myplr]._pGold < storehold[idx]._iIvalue) StartStore(STORE_NOMONEY);
		else StartStore(STORE_CONFIRM);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_BoyEnter()
{
	if ((boyitem._itype != -1) && (stextsel == 18)) {
		if (plr[myplr]._pGold < 50) {
			stextshold = STORE_BOY;
			stextlhold = stextsel;
			stextvhold = stextsval;
			StartStore(STORE_NOMONEY);
		} else {
			TakePlrsMoney(50);
			StartStore(STORE_BBUY);
		}
	} else {
		if ( ((stextsel == 8) && (boyitem._itype != -1)) || ((stextsel == 12) && (boyitem._itype == -1)) ) {
			talker = TWN_BOY;
			stextshold = STORE_BOY;
			stextlhold = stextsel;
			gossipstart = TXT_WIRT2;
			gossipend = TXT_WIRT12;
			StartStore(STORE_TALK);
		} else stextflag = STORE_NONE;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void BoyBuyItem()
{
	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	// Put in players inv
	StoreAutoPlace();

	// Remove from boyitem
	boyitem._itype = -1;
	stextshold = STORE_BOY;

	CalcPlrInv(myplr, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void HealerBuyItem()
{
	int idx;

	// Check if healing, full healing at top of list. These need new seeds
	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	if (gbMaxPlayers == 1) {
		if (idx < 2) plr[myplr].HoldItem._iSeed = GetRndSeed();
	} else {
		if (idx < 3) plr[myplr].HoldItem._iSeed = GetRndSeed();
	}

	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	// Put in players inv
	if (!plr[myplr].HoldItem._iMagical) plr[myplr].HoldItem._iIdentified = FALSE;
	StoreAutoPlace();

	// Remove from healitems
	// If item index is mana, don't remove from his items list
	if (gbMaxPlayers == 1) {
		if (idx < 2) return;
	} else {
		if (idx < 3) return;
	}
	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	if (idx == MAXHEALITEMS-1) healitem[idx]._itype = -1;
	else {
		while (healitem[idx+1]._itype != -1) {
			healitem[idx] = healitem[idx+1];
			idx++;
		}
		healitem[idx]._itype = -1;
	}

	CalcPlrInv(myplr, TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_BBuyEnter()
{
	int i;
	BOOL done;

	if (stextsel == 10) {
		stextshold = STORE_BBUY;
		stextlhold = stextsel;
		stextvhold = stextsval;
		// PATCH2.JMM
		//if (plr[myplr]._pGold < ( boyitem._iIvalue + ) StartStore(STORE_NOMONEY);
		if (plr[myplr]._pGold < ( boyitem._iIvalue - (boyitem._iIvalue >> 2) ) ) StartStore(STORE_NOMONEY);
		// ENDPATCH2.JMM
		else {
			plr[myplr].HoldItem = boyitem;
			plr[myplr].HoldItem._iIvalue -= (plr[myplr].HoldItem._iIvalue >> 2);
			SetCursor(plr[myplr].HoldItem._iCurs + ICSTART);
			done = FALSE;
			for (i = 0; (i < MAXINV) && (!done); i++) done = AutoPlace(myplr, i, cursW/28, cursH/28, FALSE);
			if (done) StartStore(STORE_CONFIRM);
			else StartStore(STORE_NOROOM);
			SetCursor(GLOVE_CURS);
		}
	} else stextflag = STORE_NONE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void StoryIdItem()
{
	int i, idx;

	idx = ((stextlhold - stextup) >> 2) + stextvhold;
	i = storehidx[idx];
	if (i < 0) {
		if (i == -1) plr[myplr].HeadItem._iIdentified = TRUE;
		if (i == -2) plr[myplr].BodyItem._iIdentified = TRUE;
		if (i == -3) plr[myplr].Hand1Item._iIdentified = TRUE;
		if (i == -4) plr[myplr].Hand2Item._iIdentified = TRUE;
		if (i == -5) plr[myplr].Ring1Item._iIdentified = TRUE;
		if (i == -6) plr[myplr].Ring2Item._iIdentified = TRUE;
		if (i == -7) plr[myplr].NeckItem._iIdentified = TRUE;
	}
	else
		plr[myplr].InvList[i]._iIdentified = TRUE;

	plr[myplr].HoldItem._iIdentified = TRUE;

	// Take players money
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	CalcPlrInv(myplr,TRUE);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_ConfirmEnter()
{
	if (stextsel == 18) {
		switch(stextshold) {
			case STORE_SBUY:
				SmithBuyItem();
				break;
			case STORE_SPBUY:
				SmithBuyPItem();
				break;
			case STORE_SSELL:
			case STORE_WSELL:
				StoreSellItem();
				break;
			case STORE_SREPAIR:
				SmithRepairItem();
				break;
			case STORE_WBUY:
				WitchBuyItem();
				break;
			case STORE_WRECHARGE:
				WitchRechargeItem();
				break;
			case STORE_BBUY:
				BoyBuyItem();
				break;
			case STORE_HBUY:
				HealerBuyItem();
				break;
			case STORE_STORYID:
				StoryIdItem();
				StartStore(STORE_IDSHOW);
				return;
		}
		StartStore(stextshold);
	} else {
		StartStore(stextshold);
		stextsel = stextlhold;
		stextsval = stextvhold;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_HealerEnter()
{
	switch(stextsel) {
		case 12:
			talker = TWN_HEALER;
			stextshold = STORE_HEALER;
			stextlhold = 12;
			gossipstart = TXT_PEPIN2;
			gossipend = TXT_PEPIN11;
			StartStore(STORE_TALK);
			break;
//		case 14:
//			if (plr[myplr]._pHitPoints != plr[myplr]._pMaxHP) PlaySFX(IS_CAST8);
//			plr[myplr]._pHitPoints = plr[myplr]._pMaxHP;
//			plr[myplr]._pHPBase = plr[myplr]._pMaxHPBase;
//			drawhpflag = TRUE;
//			break;
		case 14:
			StartStore(STORE_HBUY);
			break;
		case 16:
			stextflag = STORE_NONE;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_HBuyEnter()
{
	int idx, i;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_HEALER);
		stextsel = 16;
	} else {
		stextshold = STORE_HBUY;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		if (plr[myplr]._pGold < healitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = healitem[idx];
			SetCursor(plr[myplr].HoldItem._iCurs + ICSTART);
			done = FALSE;
			for (i = 0; (i < MAXINV) && (!done); i++) done = SpecialAutoPlace(myplr, i, cursW/28, cursH/28, FALSE);
			if (done) StartStore(STORE_CONFIRM);
			else StartStore(STORE_NOROOM);
			SetCursor(GLOVE_CURS);
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_StoryEnter()
{
	switch(stextsel) {
		case 12:
			talker = TWN_TELLER;
			stextshold = STORE_STORYTLR;
			stextlhold = 12;
			gossipstart = TXT_STORY2;
			gossipend = TXT_STORY11;
			StartStore(STORE_TALK);
			break;
		case 14:
			StartStore(STORE_STORYID);
			break;
		case 18:
			stextflag = STORE_NONE;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_SIDEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_STORYTLR);
		stextsel = 14;
	} else {
		stextshold = STORE_STORYID;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = ((stextsel - stextup) >> 2) + stextsval;
		plr[myplr].HoldItem = storehold[idx];
		if (plr[myplr]._pGold < storehold[idx]._iIvalue) StartStore(STORE_NOMONEY);
		else StartStore(STORE_CONFIRM);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_TalkEnter()
{
	int i, tq, sn, la;

	if (stextsel == 22) {
		StartStore(stextshold);
		stextsel = stextlhold;
	} else {
		tq = 0;
		for (i = 0; i < MAXQUESTS; i++) {
			if ((quests[i]._qactive == QUEST_NOTDONE) && (Qtalklist[talker][i] != -1) && quests[i]._qlog) tq++;
		}
		if (tq > 6) {
			sn = 14 - (tq >> 1);
			la = 1;
		} else {
			sn = 15 - tq;
			la = 2;
		}
		if (stextsel == (sn - 2)) {
			// Gossip
			SetRndSeed(towner[talker]._tSeed);
			int m = random(0, gossipend - gossipstart + 1) + gossipstart;
			InitQTextMsg(m);
		} else {
			// Quest info
			for (i = 0; i < MAXQUESTS; i++) {
				if ((quests[i]._qactive == QUEST_NOTDONE) && (Qtalklist[talker][i] != -1) && quests[i]._qlog) {
					if (sn == stextsel) InitQTextMsg(Qtalklist[talker][i]);
					sn += la;
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_TavernEnter()
{
	switch(stextsel) {
		case 12:
			talker = TWN_BAROWNER;
			stextshold = STORE_TAVERN;
			stextlhold = 12;
			gossipstart = TXT_OGDEN2;
			gossipend = TXT_OGDEN10;
			StartStore(STORE_TALK);
			break;
		case 18:
			stextflag = STORE_NONE;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_BarmaidEnter()
{
	switch(stextsel) {
		case 12:
			talker = TWN_BARMAID;
			stextshold = STORE_BARMAID;
			stextlhold = 12;
			gossipstart = TXT_GILIAN2;
			gossipend = TXT_GILIAN10;
			StartStore(STORE_TALK);
			break;
		case 18:
			stextflag = STORE_NONE;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void S_DrunkEnter()
{
	switch(stextsel) {
		case 12:
			talker = TWN_DRUNK;
			stextshold = STORE_DRUNK;
			stextlhold = 12;
			gossipstart = TXT_FARN2;
			gossipend = TXT_FARN13;
			StartStore(STORE_TALK);
			break;
		case 18:
			stextflag = STORE_NONE;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void STextEnter()
{
	if (qtextflag) {
		qtextflag = FALSE;
		if (leveltype == 0) stream_stop();
		return;
	}
	PlaySFX(IS_TITLSLCT);
	switch(stextflag) {
		case STORE_SMITH:
			S_SmithEnter();
			break;
		case STORE_SBUY:
			S_SBuyEnter();
			break;
		case STORE_SPBUY:
			S_SPBuyEnter();
			break;
		case STORE_SSELL:
			S_SSellEnter();
			break;
		case STORE_SREPAIR:
			S_SRepairEnter();
			break;
		case STORE_WITCH:
			S_WitchEnter();
			break;
		case STORE_WBUY:
			S_WBuyEnter();
			break;
		case STORE_WSELL:
			S_WSellEnter();
			break;
		case STORE_WRECHARGE:
			S_WRechargeEnter();
			break;
		case STORE_NOMONEY:
		case STORE_NOROOM:
			StartStore(stextshold);
			stextsel = stextlhold;
			stextsval = stextvhold;
			break;
		case STORE_CONFIRM:
			S_ConfirmEnter();
			break;
		case STORE_BOY:
			S_BoyEnter();
			break;
		case STORE_BBUY:
			S_BBuyEnter();
			break;
		case STORE_HEALER:
			S_HealerEnter();
			break;
		case STORE_STORYTLR:
			S_StoryEnter();
			break;
		case STORE_HBUY:
			S_HBuyEnter();
			break;
		case STORE_STORYID:
			S_SIDEnter();
			break;
		case STORE_TALK:
			S_TalkEnter();
			break;
		case STORE_IDSHOW:
			StartStore(STORE_STORYID);
			break;
		case STORE_TAVERN:
			S_TavernEnter();
			break;
		case STORE_BARMAID:
			S_BarmaidEnter();
			break;
		case STORE_DRUNK:
			S_DrunkEnter();
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckStoreBtn()
{
	int y;

	if (qtextflag) {
		qtextflag = FALSE;
		if (leveltype == 0) stream_stop();
		return;
	}
	if (stextsel == -1) return;
	if ((MouseY < 32) || (MouseY > 320)) return;
	if (stextsize == STEXT_SMALL) {
		if ((MouseX < 344) || (MouseX > 616)) return;
	} else {
		if ((MouseX < 24) || (MouseX > 616)) return;
	}
	y = (MouseY - 32) / 12;
	if ((stextscrl) && (MouseX > 600)) {
		if (y == 4) {
			if (stextscrlubtn <= 0) {
				STextUp();
				stextscrlubtn = 10;
			} else stextscrlubtn--;
		}
		if (y == 20) {
			if (stextscrldbtn <= 0) {
				STextDown();
				stextscrldbtn = 10;
			} else stextscrldbtn--;
		}
	} else {
		if (y < 5) return;
		if (y >= 23) y = 22;
		if ((stextscrl) && (y < 21) && (!stext[y]._ssel)) {
			if (stext[y-2]._ssel) y -= 2;
			else if (stext[y-1]._ssel) y--;
		}
		if ((stext[y]._ssel) || (stextscrl && (y == 22))) {
			stextsel = y;
			STextEnter();
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ReleaseStoreBtn()
{
	stextscrlubtn = -1;
	stextscrldbtn = -1;
}

