/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 Invetory file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/INV.CPP 11    2/24/97 7:53p Jmcreynolds $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"gendung.h"		//required for player.h
#include	"items.h"
#include	"itemdat.h"
#include	"player.h"
#include	"inv.h"
#include	"engine.h"
#include	"lighting.h"
#include	"cursor.h"
#include	"scrollrt.h"
#include	"control.h"
#include	"monster.h"
#include	"effects.h"
#include	"spelldat.h"
#include	"spells.h"
#include	"objects.h"
#include	"quests.h"
#include	"doom.h"
#include	"msg.h"
#include	"stores.h"
#include	"missiles.h"
#include	"minitext.h"
#include	"textdat.h"
#include	"multi.h"

ItemStruct *PlrHasItem(int pnum, int item, int &i);
void SetupItem(int);

void CheckItemStats(int pnum);
void DeleteItem(int ii, int i);
void OpenNest();
void OpenCrypt();
void InitTownTriggers();

// JMM.PATCH1.2.22.97
void sysmsg_add_string(const char * pszMsg);
// END.JMM.PATCH1.2.22.97


/*-----------------------------------------------------------------------**
** Global variables
**-----------------------------------------------------------------------*/
BOOL	invflag;
BOOL	drawsbarflag;

/*-----------------------------------------------------------------------**
** private
**-----------------------------------------------------------------------*/
// Offscreen control panel buffer
static BYTE *pInvCels;

int AP2x2Tbl[10] = { 8, 28, 6, 26, 4, 24, 2, 22, 0, 20 };

// InvRect defines lower-left corners of inventory screen rectangles
static const POINT InvRect[] = {	
	{ 452, 31 },{ 480, 31 },		// Head (0-3)
	{ 452, 59 },{ 480, 59 },

	{ 365,205 },					// Ring 1 (4)

	{ 567,205 },					// Ring 2 (5)

	{ 524, 59 },					// Neck (6)

	{ 337,104 },{ 366,104 },		// Left Hand (7-12)
	{ 337,132 },{ 366,132 },
	{ 337,160 },{ 366,160 },

	{ 567,104 },{ 596,104 },		// Right Hand (13-18)
	{ 567,132 },{ 596,132 },
	{ 567,160 },{ 596,160 },

	{ 452,104 },{ 480,104 },		// Body (19-24)
	{ 452,132 },{ 480,132 },
	{ 452,160 },{ 480,160 },

	// Inv (25-64)
	{ 337,250 },{ 366,250 },{ 394,250 },{ 423,250 },{ 451,250 },{ 480,250 },{ 509,250 },{ 538,250 },{ 567,250 },{ 596,250 },
	{ 337,279 },{ 366,279 },{ 394,279 },{ 423,279 },{ 451,279 },{ 480,279 },{ 509,279 },{ 538,279 },{ 567,279 },{ 596,279 },
	{ 337,308 },{ 366,308 },{ 394,308 },{ 423,308 },{ 451,308 },{ 480,308 },{ 509,308 },{ 538,308 },{ 567,308 },{ 596,308 },
	{ 337,336 },{ 366,336 },{ 394,336 },{ 423,336 },{ 451,336 },{ 480,336 },{ 509,336 },{ 538,336 },{ 567,336 },{ 596,336 },

	// Speed Bar(65-72)
	{ 205,385 },{ 234,385 },{ 263,385 },{ 292,385 },{ 321,385 },{ 350,385 },{ 379,385 },{ 408,385 }
};
#define INVRECTS (sizeof(InvRect) / sizeof(InvRect[0]))


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void FreeInvGFX() {
	DiabloFreePtr (pInvCels);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitInv() {
	app_assert(! pInvCels);
	if (plr[myplr]._pClass == CLASS_WARRIOR) pInvCels = LoadFileInMemSig("Data\\Inv\\Inv.CEL",NULL,'INVC');
	#if !IS_VERSION(SHAREWARE)
	else if (plr[myplr]._pClass == CLASS_ROGUE) pInvCels = LoadFileInMemSig("Data\\Inv\\Inv_rog.CEL",NULL,'INVC');
	else if (plr[myplr]._pClass == CLASS_SORCEROR) pInvCels = LoadFileInMemSig("Data\\Inv\\Inv_Sor.CEL",NULL,'INVC');
	else if (plr[myplr]._pClass == CLASS_MONK) pInvCels = LoadFileInMemSig("Data\\Inv\\Inv_Sor.CEL",NULL,'INVC');	// GWP Fix this
	else if (plr[myplr]._pClass == CLASS_BARD) pInvCels = LoadFileInMemSig("Data\\Inv\\Inv_rog.CEL",NULL,'INVC');	// GWP Fix this
	else if (plr[myplr]._pClass == CLASS_BARBARIAN) pInvCels = LoadFileInMemSig("Data\\Inv\\Inv.CEL",NULL,'INVC');	// GWP Fix this
	#endif
	invflag = FALSE;
	drawsbarflag = FALSE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void ChangeBackground(int x, int y, int w, int h)
{
	BYTE *p;

	app_assert(gpBuffer);
	p = gpBuffer + nBuffWTbl[y] + x;
	__asm {
		mov		edi,dword ptr [p]
		xor		edx,edx
		xor		ebx,ebx
		mov		dx,word ptr [h]
		mov		bx,word ptr [w]

_YLp:	mov		ecx,ebx
_XLp:	mov		al,byte ptr [edi]
		cmp		al,176
		jb		_Skip2
		cmp		al,191
		ja		_Try2
		sub		al,16
		jmp		_Save
_Try2:	cmp		al,240
		jb		_Skip2
		sub		al,80
_Save:	mov		byte ptr [edi],al
_Skip2:	inc		edi
		loop	_XLp
		sub		edi,768
		sub		edi,ebx
		dec		edx
		jnz		_YLp
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#define INVOCLR		197
#define INVOMCLR	181
#define INVOICLR	229

void DrawInv()
{
	int i, f, w, ii, xx, yy, oc;
	BYTE *pB;
	BOOL	invtest[MAXINV];		// @@@@ Dave test

	app_assert(gpBuffer);
	DrawCel(384, 511, pInvCels, 1, 320);

	if (plr[myplr].HeadItem._itype != -1) {
		ChangeBackground(517, 219, 56, 56);
		f = plr[myplr].HeadItem._iCurs + ICSTART;
		w = CursorWidth[f];
		if (cursinvitem == INVLOC_HEAD) {
			oc = INVOCLR;
			if (plr[myplr].HeadItem._iMagical) oc = INVOMCLR;
			if (!plr[myplr].HeadItem._iStatFlag) oc = INVOICLR;
			
			if (f <= ICLAST )
				OutlineSlabCel(oc, 517, 219, pCursCels, f, w, 0, 8);
			else
				OutlineSlabCel(oc, 517, 219, pCursCels2, f - ICLAST, w, 0, 8);
		}
		if (plr[myplr].HeadItem._iStatFlag) {
			app_assert(pCursCels);
			if (f <= ICLAST)
				DrawSlabCel(517, 219, pCursCels, f, w, 0, 8);
			else	
				DrawSlabCel(517, 219, pCursCels2, f - ICLAST, w, 0, 8);
		}
		else {
			if (f <= ICLAST)
				DrawSlabCelI(517, 219, pCursCels, f, w, 0, 8, LIGHT_INFRA);
			else
				DrawSlabCelI(517, 219, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
		}
	}
	if (plr[myplr].Ring1Item._itype != -1) {
		ChangeBackground(432, 365, 28, 28);
		f = plr[myplr].Ring1Item._iCurs + ICSTART;
		w = CursorWidth[f];
		if (cursinvitem == INVLOC_RING1) {
			oc = INVOCLR;
			if (plr[myplr].Ring1Item._iMagical) oc = INVOMCLR;
			if (!plr[myplr].Ring1Item._iStatFlag) oc = INVOICLR;
				
			if (f <= ICLAST)
				OutlineSlabCel(oc, 432, 365, pCursCels, f, w, 0, 8);
			else
				OutlineSlabCel(oc, 432, 365, pCursCels2, f - ICLAST, w, 0, 8);
		}
		if (plr[myplr].Ring1Item._iStatFlag) {
			app_assert(pCursCels);
			if (f <= ICLAST)
				DrawSlabCel(432, 365, pCursCels, f, w, 0, 8);
			else
				DrawSlabCel(432, 365, pCursCels2, f - ICLAST, w, 0, 8);
		}
		else {
			if (f <= ICLAST)
				DrawSlabCelI(432, 365, pCursCels, f, w, 0, 8, LIGHT_INFRA);
			else
				DrawSlabCelI(432, 365, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
		}
	}
	if (plr[myplr].Ring2Item._itype != -1) {
		ChangeBackground(633, 365, 28, 28);
		f = plr[myplr].Ring2Item._iCurs + ICSTART;
		w = CursorWidth[f];
		if (cursinvitem == INVLOC_RING2) {
			oc = INVOCLR;
			if (plr[myplr].Ring2Item._iMagical) oc = INVOMCLR;
			if (!plr[myplr].Ring2Item._iStatFlag) oc = INVOICLR;
			if (f <= ICLAST)
				OutlineSlabCel(oc, 633, 365, pCursCels, f, w, 0, 8);
			else
				OutlineSlabCel(oc, 633, 365, pCursCels2, f - ICLAST, w, 0, 8);
		}
		if (plr[myplr].Ring2Item._iStatFlag) {
			app_assert(pCursCels);
			if (f <= ICLAST)
				DrawSlabCel(633, 365, pCursCels, f, w, 0, 8);
			else
				DrawSlabCel(633, 365, pCursCels2, f - ICLAST, w, 0, 8);
		}
		else {
			if (f <= ICLAST)
				DrawSlabCelI(633, 365, pCursCels, f, w, 0, 8, LIGHT_INFRA);
			else
				DrawSlabCelI(633, 365, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
		}
	}
	if (plr[myplr].NeckItem._itype != -1) {
		ChangeBackground(589, 220, 28, 28);
		f = plr[myplr].NeckItem._iCurs + ICSTART;
		w = CursorWidth[f];
		if (cursinvitem == INVLOC_NECK) {
			oc = INVOCLR;
			if (plr[myplr].NeckItem._iMagical) oc = INVOMCLR;
			if (!plr[myplr].NeckItem._iStatFlag) oc = INVOICLR;
			if (f <= ICLAST)
				OutlineSlabCel(oc, 589, 220, pCursCels, f, w, 0, 8);
			else
				OutlineSlabCel(oc, 589, 220, pCursCels2, f - ICLAST, w, 0, 8);
		}
		if (plr[myplr].NeckItem._iStatFlag) {
			app_assert(pCursCels);
			if (f <= ICLAST)
				DrawSlabCel(589, 220, pCursCels, f, w, 0, 8);
			else
				DrawSlabCel(589, 220, pCursCels2, f - ICLAST, w, 0, 8);
		}
		else {
			if (f <= ICLAST)
				DrawSlabCelI(589, 220, pCursCels, f, w, 0, 8, LIGHT_INFRA);
			else
				DrawSlabCelI(589, 220, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
		}
	}
	if (plr[myplr].Hand1Item._itype != -1) {
		ChangeBackground(401, 320, 56, 84);
		f = plr[myplr].Hand1Item._iCurs + ICSTART;
		w = CursorWidth[f];
		if (w == 28) xx = 415;
		else xx = 401;
		if (CursorHeight[f] == 84) yy = 320;
		else yy = 306;
		if (cursinvitem == INVLOC_HAND1) {
			oc = INVOCLR;
			if (plr[myplr].Hand1Item._iMagical) oc = INVOMCLR;
			if (!plr[myplr].Hand1Item._iStatFlag) oc = INVOICLR;
			if (f <= ICLAST)
				OutlineSlabCel(oc, xx, yy, pCursCels, f, w, 0, 8);
			else
				OutlineSlabCel(oc, xx, yy, pCursCels2, f - ICLAST, w, 0, 8);
		}
		if (plr[myplr].Hand1Item._iStatFlag) {
			app_assert(pCursCels);
			if (f <= ICLAST)
				DrawSlabCel(xx, yy, pCursCels, f, w, 0, 8);
			else
				DrawSlabCel(xx, yy, pCursCels2, f - ICLAST, w, 0, 8);
		}
		else {
			if (f <= ICLAST)
				DrawSlabCelI(xx, yy, pCursCels, f, w, 0, 8, LIGHT_INFRA);
			else
				DrawSlabCelI(xx, yy, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
		}
		if (plr[myplr].Hand1Item._iLoc == IL_2HAND
			&& !(plr[myplr]._pClass == CLASS_BARBARIAN
				 && (plr[myplr].Hand1Item._itype == IT_SWORD ||
				     plr[myplr].Hand1Item._itype == IT_MACE)
				 )
			) {
			ChangeBackground(631, 320, 56, 84);
			nLVal = 0;
			nTrans = TRUE;
			if (w == 28) pB = gpBuffer + 246405;
			else pB = gpBuffer + 246391;
			if (f <= ICLAST)
				TDrawSlabCelPL (pB, pCursCels, f, w, 0, 8);
			else
				TDrawSlabCelPL (pB, pCursCels2, f - ICLAST, w, 0, 8);
			nTrans = FALSE;
		}
	}
	if (plr[myplr].Hand2Item._itype != -1) {
		ChangeBackground(631, 320, 56, 84);
		f = plr[myplr].Hand2Item._iCurs + ICSTART;
		w = CursorWidth[f];
		if (w == 28) xx = 645;
		else xx = 633;
		if (CursorHeight[f] == 84) yy = 320;
		else yy = 306;
		if (cursinvitem == INVLOC_HAND2) {
			oc = INVOCLR;
			if (plr[myplr].Hand2Item._iMagical) oc = INVOMCLR;
			if (!plr[myplr].Hand2Item._iStatFlag) oc = INVOICLR;
			if (f <= ICLAST)
				OutlineSlabCel(oc, xx, yy, pCursCels, f, w, 0, 8);
			else
				OutlineSlabCel(oc, xx, yy, pCursCels2, f - ICLAST, w, 0, 8);
		}
		if (plr[myplr].Hand2Item._iStatFlag) {
			app_assert(pCursCels);
			if (f <= ICLAST)
				DrawSlabCel(xx, yy, pCursCels, f, w, 0, 8);
			else
				DrawSlabCel(xx, yy, pCursCels2, f - ICLAST, w, 0, 8);
		}
		else {
			if (f <= ICLAST)
				DrawSlabCelI(xx, yy, pCursCels, f, w, 0, 8, LIGHT_INFRA);
			else
				DrawSlabCelI(xx, yy, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
		}
	}
	if (plr[myplr].BodyItem._itype != -1) {
		ChangeBackground(517, 320, 56, 84);
		f = plr[myplr].BodyItem._iCurs + ICSTART;
		w = CursorWidth[f];
		if (cursinvitem == INVLOC_BODY) {
			oc = INVOCLR;
			if (plr[myplr].BodyItem._iMagical) oc = INVOMCLR;
			if (!plr[myplr].BodyItem._iStatFlag) oc = INVOICLR;
			if (f <= ICLAST)
				OutlineSlabCel(oc, 517, 320, pCursCels, f, w, 0, 8);
			else
				OutlineSlabCel(oc, 517, 320, pCursCels2, f - ICLAST, w, 0, 8);
		}
		if (plr[myplr].BodyItem._iStatFlag) {
			app_assert(pCursCels);
			if (f <= ICLAST)
				DrawSlabCel(517, 320, pCursCels, f, w, 0, 8);
			else
				DrawSlabCel(517, 320, pCursCels2, f - ICLAST, w, 0, 8);
		}
		else {
			if (f <= ICLAST)
				DrawSlabCelI(517, 320, pCursCels, f, w, 0, 8, LIGHT_INFRA);
			else
				DrawSlabCelI(517, 320, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
		}
	}

	for (i = 0; i < MAXINV; i++) {
		invtest[i] = FALSE;
		if (plr[myplr].InvGrid[i] != 0)
			ChangeBackground(InvRect[i+25].x + 64, InvRect[i+25].y + 159, 28, 28);
	}

	for (i = 0; i < MAXINV; i++) {
		if (plr[myplr].InvGrid[i] > 0) {
			app_assert(!invtest[i]);
			invtest[i] = TRUE;
			app_assert(plr[myplr].InvGrid[i] <= plr[myplr]._pNumInv);
			ii = plr[myplr].InvGrid[i] - 1;
			f = plr[myplr].InvList[ii]._iCurs + ICSTART;
			w = CursorWidth[f];
			if (cursinvitem == (ii + NUM_INVLOC)) {
				oc = INVOCLR;
				if (plr[myplr].InvList[ii]._iMagical) oc = INVOMCLR;
				if (!plr[myplr].InvList[ii]._iStatFlag) oc = INVOICLR;
				if (f <= ICLAST)
					OutlineSlabCel(oc, InvRect[i+25].x + 64, InvRect[i+25].y + 159, pCursCels, f, w, 0, 8);
				else
					OutlineSlabCel(oc, InvRect[i+25].x + 64, InvRect[i+25].y + 159, pCursCels2, f - ICLAST, w, 0, 8);
			}
			if (plr[myplr].InvList[ii]._iStatFlag) {
				app_assert(pCursCels);
				if (f <= ICLAST)
					DrawSlabCel(InvRect[i+25].x + 64, InvRect[i+25].y + 159, pCursCels, f, w, 0, 8);
				else
					DrawSlabCel(InvRect[i+25].x + 64, InvRect[i+25].y + 159, pCursCels2, f - ICLAST, w, 0, 8);
			}
			else {
				if (f <= ICLAST)
					DrawSlabCelI(InvRect[i+25].x + 64, InvRect[i+25].y + 159, pCursCels, f, w, 0, 8, LIGHT_INFRA);
				else
					DrawSlabCelI(InvRect[i+25].x + 64, InvRect[i+25].y + 159, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawSpdBar()
{
	int i, f, w, oc, idata;
	long poffset;

	if (talkflag) return;
	CopyCtrlPan(205, 21, 232, 28, 269, 517);
	for (i = 0; i < MAXSPD; i++) {
		if (plr[myplr].SpdList[i]._itype != -1) {
			ChangeBackground(InvRect[i+65].x + 64, InvRect[i+65].y + 159, 28, 28);
			f = plr[myplr].SpdList[i]._iCurs + ICSTART;
			w = CursorWidth[f];
			if (cursinvitem == (i + 47)) {
				oc = INVOCLR;
				if (plr[myplr].SpdList[i]._iMagical) oc = INVOMCLR;
				if (!plr[myplr].SpdList[i]._iStatFlag) oc = INVOICLR;
				if (f <= ICLAST)
					OutlineSlabCel(oc, InvRect[i+65].x + 64, InvRect[i+65].y + 159, pCursCels, f, w, 0, 8);
				else
					OutlineSlabCel(oc, InvRect[i+65].x + 64, InvRect[i+65].y + 159, pCursCels2, f - ICLAST, w, 0, 8);
			}
			if (plr[myplr].SpdList[i]._iStatFlag) {
				app_assert(pCursCels);
				if (f <= ICLAST)
					DrawSlabCel(InvRect[i+65].x + 64, InvRect[i+65].y + 159, pCursCels, f, w, 0, 8);
				else
					DrawSlabCel(InvRect[i+65].x + 64, InvRect[i+65].y + 159, pCursCels2, f - ICLAST, w, 0, 8);
			}
			else {
				if (f <= ICLAST)
					DrawSlabCelI(InvRect[i+65].x + 64, InvRect[i+65].y + 159, pCursCels, f, w, 0, 8, LIGHT_INFRA);
				else
					DrawSlabCelI(InvRect[i+65].x + 64, InvRect[i+65].y + 159, pCursCels2, f - ICLAST, w, 0, 8, LIGHT_INFRA);
			}
			idata = plr[myplr].SpdList[i].IDidx;
			if (AllItemsList[idata].iUsable && plr[myplr].SpdList[i]._iStatFlag &&
				(plr[myplr].SpdList[i]._itype != IT_GOLD))
			{
				BYTE c = char2print(i + '1');
				c = fonttrans[c];
				poffset = nBuffWTbl[InvRect[i+65].y + 159] + InvRect[i+65].x + 92 - fontkern[c];
				DrawPanelFont(poffset, c, ICOLOR_WHITE);
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL AutoPlace(int pnum, int ii, int sx, int sy, BOOL saveflag)
{
	int i, j, xx, yy;
	BOOL done;

	done = TRUE;
	yy = (ii / 10) * 10;
	if (yy < 0) yy = 0;
	for (j = 0; (j < sy) && done; j++) {
		if (yy >= 40) done = FALSE;
		xx = (ii % 10);
		if (xx < 0) xx = 0;
		for (i = 0; (i < sx) && done; i++) {
			if (xx >= 10) done = FALSE;
			else done = (plr[pnum].InvGrid[xx+yy] == 0);
			xx++;
		}
		yy+=10;
	}

	if ((done) && (saveflag)) {
		i = plr[pnum]._pNumInv;
		plr[pnum].InvList[i] = plr[pnum].HoldItem;
		plr[pnum]._pNumInv++;
		yy = (ii / 10) * 10;
		if (yy < 0) yy = 0;
		for (j = 0; j < sy; j++) {
			xx = (ii % 10);
			if (xx < 0) xx = 0;
			for (i = 0; i < sx; i++) {
				if ((i == 0) && (j == sy-1)) plr[pnum].InvGrid[xx+yy] = plr[pnum]._pNumInv;
				else plr[pnum].InvGrid[xx+yy] = -plr[pnum]._pNumInv;
				xx++;
			}
			yy+=10;
		}
		CalcPlrScrolls(pnum);
	}
	return(done);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL SpecialAutoPlace(int pnum, int ii, int sx, int sy, BOOL saveflag)
{
	int i, j, xx, yy;
	BOOL done;

	//Check InvList
	done = TRUE;
	yy = (ii / 10) * 10;
	if (yy < 0) yy = 0;
	for (j = 0; (j < sy) && done; j++) {
		if (yy >= 40) done = FALSE;
		xx = (ii % 10);
		if (xx < 0) xx = 0;
		for (i = 0; (i < sx) && done; i++) {
			if (xx >= 10) done = FALSE;
			else done = (plr[pnum].InvGrid[xx+yy] == 0);
			xx++;
		}
		yy+=10;
	}
	//Inventory is full so check SpdList
	if (!done) {
		if (sx > 1 || sy > 1) done = FALSE;
		else {
			for (i = 0; i < MAXSPD; i++) {
				if (plr[pnum].SpdList[i]._itype == -1) {
					done = TRUE;
					break;
				}
			}
		}
	}

	if ((done) && (saveflag)) {
		i = plr[pnum]._pNumInv;
		plr[pnum].InvList[i] = plr[pnum].HoldItem;
		plr[pnum]._pNumInv++;
		yy = (ii / 10) * 10;
		if (yy < 0) yy = 0;
		for (j = 0; j < sy; j++) {
			xx = (ii % 10);
			if (xx < 0) xx = 0;
			for (i = 0; i < sx; i++) {
				if ((i == 0) && (j == sy-1)) plr[pnum].InvGrid[xx+yy] = plr[pnum]._pNumInv;
				else plr[pnum].InvGrid[xx+yy] = -plr[pnum]._pNumInv;
				xx++;
			}
			yy+=10;
		}
		CalcPlrScrolls(pnum);
	}
	return(done);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL GoldAutoPlace(int pnum)
{
	int i, ii, xx, yy;
	long gt;
	BOOL done;

	// Check if it can be added on to a gold location
	done = FALSE;
	for (i = 0; (i < plr[pnum]._pNumInv) && (!done); i++) {
		if (plr[pnum].InvList[i]._itype == IT_GOLD) {
			gt = plr[pnum].HoldItem._ivalue + plr[pnum].InvList[i]._ivalue;
			if (gt <= GOLD_VMAX) {
				plr[pnum].InvList[i]._ivalue += plr[pnum].HoldItem._ivalue;
				if (gt >= GOLD_VT2) {
					plr[pnum].InvList[i]._iCurs = ITEM_5GOLD;
				} else {
					if (gt <= GOLD_VT1) plr[pnum].InvList[i]._iCurs = ITEM_1GOLD;
					else plr[pnum].InvList[i]._iCurs = ITEM_3GOLD;
				}
				plr[pnum]._pGold = CalculateGold(pnum);
				plr[pnum].HoldItem._ivalue = 0;
				done = TRUE;
			}
			else if (plr[pnum].InvList[i]._ivalue < GOLD_VMAX) {
				// There's room, add to existing gold.
				int const depositGold = GOLD_VMAX - plr[pnum].InvList[i]._ivalue;
				plr[pnum].InvList[i]._ivalue = GOLD_VMAX;
				plr[pnum].InvList[i]._iCurs = ITEM_5GOLD;
				
				plr[pnum].HoldItem._ivalue -= depositGold;
				// Just in case.
				if (plr[pnum].HoldItem._ivalue < 0)
				{
					plr[pnum].HoldItem._ivalue = 0;
					done = TRUE;
				}
				GetPlrHandSeed(&plr[pnum].HoldItem);
				
				SetDropGoldCursor(pnum);
				plr[pnum]._pGold = CalculateGold(pnum);
			}
		}
	}

	
#if 0
	// If there's still gold in my hand, try to place it in more empty slots
	if (!done) {
		for (i = 0; (i < plr[pnum]._pNumInv) && (!done); i++) {
			if (plr[pnum].InvList[i]._itype == IT_GOLD) {
				if (plr[pnum].InvList[i]._ivalue < GOLD_VMAX) {
					gt = plr[pnum].InvList[i]._ivalue + plr[pnum].HoldItem._ivalue;
					if (gt <= GOLD_VMAX) {
						plr[pnum].InvList[i]._ivalue += plr[pnum].HoldItem._ivalue;
						if (gt >= GOLD_VT2) {
							plr[pnum].InvList[i]._iCurs = ITEM_5GOLD;
						} else {
							if (gt <= GOLD_VT1) plr[pnum].InvList[i]._iCurs = ITEM_1GOLD;
							else plr[pnum].InvList[i]._iCurs = ITEM_3GOLD;
						}
						plr[pnum]._pGold = CalculateGold(pnum);
						done = TRUE;
					}
				}
			}
		}
	}
#endif	

	// Lastly check for any empty space to drop put gold into
	if (!done) {
		for (ii = MAXINV-1; (ii >= 0) && (!done); --ii) {
			yy = (ii / 10) * 10;
			xx = ii % 10;
			if (plr[pnum].InvGrid[xx+yy] == 0) {
				i = plr[pnum]._pNumInv;
				plr[pnum].InvList[i] = plr[pnum].HoldItem;	// struct copy.
				plr[pnum]._pNumInv++;
				plr[pnum].InvGrid[xx+yy] = plr[pnum]._pNumInv;
				if (plr[pnum].HoldItem._ivalue >= GOLD_VT2) {
					plr[pnum].InvList[i]._iCurs = ITEM_5GOLD;
				} else {
					if (plr[pnum].HoldItem._ivalue <= GOLD_VT1) plr[pnum].InvList[i]._iCurs = ITEM_1GOLD;
					else plr[pnum].InvList[i]._iCurs = ITEM_3GOLD;
				}
				if (plr[pnum].HoldItem._ivalue > GOLD_VMAX) {
					plr[pnum].HoldItem._ivalue -= GOLD_VMAX;
					GetPlrHandSeed(&plr[pnum].HoldItem);
					// We have to set the inventory value because we did a struct copy above.
					plr[pnum].InvList[i]._ivalue = GOLD_VMAX;
				} else {
					done = TRUE;
					plr[pnum].HoldItem._ivalue = 0;
					plr[pnum]._pGold = CalculateGold(pnum);
					SetCursor(GLOVE_CURS);
				}
			}
		}
	}
	return(done);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL WeaponAutoPlace(int pnum)
{
	if (plr[pnum]._pClass == CLASS_MONK)	// Monks don't autoequip weapons
		return FALSE;						// (open-hand is often better)

	if (plr[pnum].HoldItem._iLoc == IL_2HAND 
		&& !(plr[pnum]._pClass == CLASS_BARBARIAN
			&& (plr[pnum].HoldItem._itype == IT_SWORD ||
			    plr[pnum].HoldItem._itype == IT_MACE)
			)
		) 
	{
		if ((plr[pnum].Hand1Item._itype != -1) || (plr[pnum].Hand2Item._itype != -1)) return(FALSE);
		NetSendCmdChItem(TRUE, INVLOC_HAND1);
		plr[pnum].Hand1Item = plr[pnum].HoldItem;
		return(TRUE);
	}
	else
	{
		if (plr[pnum]._pClass != CLASS_BARD)
		{
			if ((plr[pnum].Hand1Item._itype != -1) && (plr[pnum].Hand1Item._iClass == IC_WEAP)) return(FALSE);
			if ((plr[pnum].Hand2Item._itype != -1) && (plr[pnum].Hand2Item._iClass == IC_WEAP)) return(FALSE);
		}
		if (plr[pnum].Hand1Item._itype == -1) {
			NetSendCmdChItem(TRUE, INVLOC_HAND1);
			plr[pnum].Hand1Item = plr[pnum].HoldItem;
		} else {
			if ((plr[pnum].Hand2Item._itype == -1) && (plr[pnum].Hand1Item._iLoc != IL_2HAND)) {
				NetSendCmdChItem(TRUE, INVLOC_HAND2);
				plr[pnum].Hand2Item = plr[pnum].HoldItem;
			} else return(FALSE);
		}
		return(TRUE);
	} 
	
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int SwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;
	return(h._iCurs + ICSTART);
}

/*-----------------------------------------------------------------------*
**  Place cursor item into inventory window
**-----------------------------------------------------------------------*/

void CheckInvPaste(int pnum, int mx, int my)
{
	int r;
	int i, j, xx, yy, ii;
	int il, cn, it, iv, ig;
	long gt;
	ItemStruct tempitem;
	
	SetICursor(plr[pnum].HoldItem._iCurs + ICSTART);

	int cx = mx + (icursW>>1);	// center of cursor graphic
	int cy = my + (icursH>>1);
	int const sx = icursW28;			// size of cursor, in 28x28 blocks
	int const sy = icursH28;

	BOOL done = FALSE;
	for (r = 0; (r < INVRECTS) && (!done); r++) {
		if ((cx >= InvRect[r].x) && (cx < InvRect[r].x + 28) && (cy >= InvRect[r].y - 29) && (cy < InvRect[r].y)) {
			done = TRUE;
			r--;
		}
		// shift for inv grid
		if (r == 24) {
			if (!(sx & 1)) cx -= 14;
			if (!(sy & 1)) cy -= 14;
		}
		// shift back y for spd bar
		if (r == 64) {
			if (!(sy & 1)) cy += 14;
		}
	}

	if (done) {
		// Check if correct item location type
		il = IL_INV;
		if ((r >= 0) && (r <= 3)) il = IL_HEAD;
		if ((r >= 4) && (r <= 5)) il = IL_RING;
		if (r == 6) il = IL_NECK;
		if ((r >= 7) && (r <= 18)) il = IL_HAND;
		if ((r >= 19) && (r <= 24)) il = IL_BODY;
		if ((r >= 65) && (r <= 72)) il = IL_SPD;

		done = FALSE;
		if (plr[pnum].HoldItem._iLoc == il) done = TRUE;
		if ((il == IL_HAND) && (plr[pnum].HoldItem._iLoc == IL_2HAND)) {
			
			// Hack so placement will work for a two handed weapon.
			if (plr[pnum]._pClass == CLASS_BARBARIAN
				&& (plr[pnum].HoldItem._itype == IT_SWORD ||
				    plr[pnum].HoldItem._itype == IT_MACE)
				)
			{
				il = IL_HAND;
			}
			else
			{
				il = IL_2HAND;
			}
			done = TRUE;
		}
		if ((plr[pnum].HoldItem._iLoc == IL_INV) && (il == IL_SPD) && (sx == 1) && (sy == 1)) {
			done = TRUE;
			if (!AllItemsList[plr[pnum].HoldItem.IDidx].iUsable) done = FALSE;
			if (!plr[pnum].HoldItem._iStatFlag) done = FALSE;
			if (plr[pnum].HoldItem._itype == IT_GOLD) done = FALSE;
		}

		if (il == IL_INV) {
			ii = r - 25;
			iv = 0;
			done = TRUE;
			if (plr[pnum].HoldItem._itype == IT_GOLD) {
				yy = (ii / 10) * 10;
				xx = ii % 10;
				if (plr[pnum].InvGrid[xx+yy] != 0) {
					ig = plr[pnum].InvGrid[xx+yy];
					if (ig > 0) {
						if (plr[pnum].InvList[ig-1]._itype != IT_GOLD) iv = ig;
					} else iv = -ig;
				}
			} else {
				yy = ((ii / 10) - ((sy - 1) >> 1)) * 10;
				if (yy < 0) yy = 0;
				for (j = 0; (j < sy) && done; j++) {
					if (yy >= 40) done = FALSE;
					xx = (ii % 10) - ((sx - 1) >> 1);
					if (xx < 0) xx = 0;
					for (i = 0; (i < sx) && done; i++) {
						if (xx >= 10) done = FALSE;
						else {
							if (plr[pnum].InvGrid[xx+yy] != 0) {
								ig = plr[pnum].InvGrid[xx+yy];
								if (ig < 0) ig = -ig;
								if (iv != 0) {
									if (iv != ig) done = FALSE;
								} else iv = ig;
							}
						}
						xx++;
					}
					yy+=10;
				}
			}
		}

		// Do I have the min stats to place this item?
		if ((done) && (il != IL_INV) && (il != IL_SPD) && (!plr[pnum].HoldItem._iStatFlag)) {
			done = FALSE;
			if (plr[pnum]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR13);
			#if !IS_VERSION(SHAREWARE)
			else if (plr[pnum]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE13);
			else if (plr[pnum]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE13);
			else if (plr[pnum]._pClass == CLASS_MONK) PlaySFX(PS_MONK13);
			else if (plr[pnum]._pClass == CLASS_BARD) PlaySFX(PS_BARD13);
			else if (plr[pnum]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN13);
			#endif
		}

		if (done) {
			it = ItemCAnimTbl[plr[pnum].HoldItem._iCurs];
			if(pnum == myplr)
				PlaySFX(ItemInvSnds[it]);
			cn = GLOVE_CURS;			// Go back to gaunts unless something else there
			switch (il) {
				case IL_HEAD :
					NetSendCmdChItem(FALSE, INVLOC_HEAD);
					if (plr[pnum].HeadItem._itype == -1) plr[pnum].HeadItem = plr[pnum].HoldItem;
					else cn = SwapItem(&plr[pnum].HeadItem, &plr[pnum].HoldItem);
					break;
				case IL_RING :
					if (r == 4) {
						NetSendCmdChItem(FALSE, INVLOC_RING1);
						if (plr[pnum].Ring1Item._itype == -1) plr[pnum].Ring1Item = plr[pnum].HoldItem;
						else cn = SwapItem(&plr[pnum].Ring1Item, &plr[pnum].HoldItem);
					} else {
						NetSendCmdChItem(FALSE, INVLOC_RING2);
						if (plr[pnum].Ring2Item._itype == -1) plr[pnum].Ring2Item = plr[pnum].HoldItem;
						else cn = SwapItem(&plr[pnum].Ring2Item, &plr[pnum].HoldItem);
					}
					break;
				case IL_NECK:
					NetSendCmdChItem(FALSE, INVLOC_NECK);
					if (plr[pnum].NeckItem._itype == -1) plr[pnum].NeckItem = plr[pnum].HoldItem;
					else cn = SwapItem(&plr[pnum].NeckItem, &plr[pnum].HoldItem);
					break;
				case IL_HAND :
					if (r <= 12) {
						if (plr[pnum].Hand1Item._itype == -1) {
							if ((plr[pnum].Hand2Item._itype != -1) 
							    && (plr[pnum].Hand2Item._iClass == plr[pnum].HoldItem._iClass)
							    && !(plr[pnum]._pClass == CLASS_BARD 
							         && plr[pnum].Hand2Item._iClass == IC_WEAP
							         && plr[pnum].HoldItem._iClass == IC_WEAP)
							    ) {
								NetSendCmdChItem(FALSE, INVLOC_HAND2);
								cn = SwapItem(&plr[pnum].Hand2Item, &plr[pnum].HoldItem);
							} else {
								NetSendCmdChItem(FALSE, INVLOC_HAND1);
								plr[pnum].Hand1Item = plr[pnum].HoldItem;
							}
						} else {
							if ((plr[pnum].Hand2Item._itype != -1) 
								&& (plr[pnum].Hand2Item._iClass == plr[pnum].HoldItem._iClass)
							    && !(plr[pnum]._pClass == CLASS_BARD 
							         && plr[pnum].Hand2Item._iClass == IC_WEAP
							         && plr[pnum].HoldItem._iClass == IC_WEAP)
								) {
								NetSendCmdChItem(FALSE, INVLOC_HAND2);
								cn = SwapItem(&plr[pnum].Hand2Item, &plr[pnum].HoldItem);
							} else {
								NetSendCmdChItem(FALSE, INVLOC_HAND1);
								cn = SwapItem(&plr[pnum].Hand1Item, &plr[pnum].HoldItem);
							}
						}
					} else {
						if (plr[pnum].Hand2Item._itype == -1) {
							if ((plr[pnum].Hand1Item._itype != -1) 
								&& (plr[pnum].Hand1Item._iLoc == IL_2HAND
									&& !(plr[pnum]._pClass == CLASS_BARBARIAN
										&& (plr[pnum].Hand1Item._itype == IT_SWORD ||
										    plr[pnum].Hand1Item._itype == IT_MACE )
										)
									)
								) {
								NetSendCmdChItem(FALSE, INVLOC_HAND1);
								SwapItem(&plr[pnum].Hand2Item, &plr[pnum].Hand1Item);
								cn = SwapItem(&plr[pnum].Hand2Item, &plr[pnum].HoldItem);
							} else {
								if ((plr[pnum].Hand1Item._itype != -1) 
									&& (plr[pnum].Hand1Item._iClass == plr[pnum].HoldItem._iClass)
									&& !(plr[pnum]._pClass == CLASS_BARD
										&& plr[pnum].Hand1Item._iClass == IC_WEAP
										&& plr[pnum].HoldItem._iClass == IC_WEAP)
									) {
									NetSendCmdChItem(FALSE, INVLOC_HAND1);
									cn = SwapItem(&plr[pnum].Hand1Item, &plr[pnum].HoldItem);
								} else {
									NetSendCmdChItem(FALSE, INVLOC_HAND2);
									plr[pnum].Hand2Item = plr[pnum].HoldItem;
								}
							}
						} else {
							if ((plr[pnum].Hand1Item._itype != -1) 
								&& (plr[pnum].Hand1Item._iClass == plr[pnum].HoldItem._iClass
									&& !(plr[pnum]._pClass == CLASS_BARD
										&& plr[pnum].Hand1Item._iClass == IC_WEAP
										&& plr[pnum].HoldItem._iClass == IC_WEAP)
									)
								) {
								NetSendCmdChItem(FALSE, INVLOC_HAND1);
								cn = SwapItem(&plr[pnum].Hand1Item, &plr[pnum].HoldItem);
							} else {
								NetSendCmdChItem(FALSE, INVLOC_HAND2);
								cn = SwapItem(&plr[pnum].Hand2Item, &plr[pnum].HoldItem);
							}
						}
					}
					break;
				case IL_2HAND :
					NetSendCmdDelItem(FALSE, INVLOC_HAND2);
					if ((plr[pnum].Hand1Item._itype != -1) 
						&& (plr[pnum].Hand2Item._itype != -1)) 
					{
						tempitem = plr[pnum].HoldItem;
						if (plr[pnum].Hand2Item._itype == IT_SHIELD) {
							plr[pnum].HoldItem = plr[pnum].Hand2Item;
						} else {
							plr[pnum].HoldItem = plr[pnum].Hand1Item;
						}
						if (pnum == myplr) {
							SetCursor(plr[pnum].HoldItem._iCurs + ICSTART);
						}
						else {
							SetICursor(plr[pnum].HoldItem._iCurs + ICSTART);
						}
						
						BOOL done2h(FALSE);
						
						for (i = 0; (i < MAXINV) && (!done2h); ++i) {
							done2h = AutoPlace(pnum, i, icursW28, icursH28, TRUE);
						}
						
						plr[pnum].HoldItem = tempitem;
						if (pnum == myplr) 
							SetCursor(plr[pnum].HoldItem._iCurs + ICSTART);
						else 
							SetICursor(plr[pnum].HoldItem._iCurs + ICSTART);
						if (done2h) {
							if (plr[pnum].Hand2Item._itype == IT_SHIELD) {
								plr[pnum].Hand2Item._itype = -1;
							} else {
								plr[pnum].Hand1Item._itype = -1;
							}
						} else 
							return;
					}
					if ((plr[pnum].Hand1Item._itype == -1) 
						&& (plr[pnum].Hand2Item._itype == -1)) {
						NetSendCmdChItem(FALSE, INVLOC_HAND1);
						plr[pnum].Hand1Item = plr[pnum].HoldItem;
					} else {
						NetSendCmdChItem(FALSE, INVLOC_HAND1);
						if (plr[pnum].Hand1Item._itype == -1) SwapItem(&plr[pnum].Hand1Item, &plr[pnum].Hand2Item);
						cn = SwapItem(&plr[pnum].Hand1Item, &plr[pnum].HoldItem);
					}
					if (plr[pnum].Hand1Item._itype == IT_STAFF) {
						if ((plr[pnum].Hand1Item._iSpell) && (plr[pnum].Hand1Item._iCharges > 0)) {
							plr[pnum]._pRSpell = plr[pnum].Hand1Item._iSpell;
							plr[pnum]._pRSplType = SPT_ITEM;
							force_redraw = FULLDRAW;
						}
					}
					break;
				case IL_BODY :
					NetSendCmdChItem(FALSE, INVLOC_BODY);
					if (plr[pnum].BodyItem._itype == -1) plr[pnum].BodyItem = plr[pnum].HoldItem;
					else cn = SwapItem(&plr[pnum].BodyItem, &plr[pnum].HoldItem);
					break;
				case IL_INV :
					// Gold is stackable
					if ((plr[pnum].HoldItem._itype == IT_GOLD) && (iv == 0)) {
						ii = r - 25;
						yy = (ii / 10) * 10;
						xx = ii % 10;
						// Empty (== 0) or gold (> 0)
						if (plr[pnum].InvGrid[xx+yy] > 0) {
							i = plr[pnum].InvGrid[xx+yy] - 1;
							gt = plr[pnum].InvList[i]._ivalue + plr[pnum].HoldItem._ivalue;
							// Gold total <= GOLD_VMAX then all done
							if (gt <= GOLD_VMAX) {
								plr[pnum].InvList[i]._ivalue += plr[pnum].HoldItem._ivalue;
								plr[pnum]._pGold +=  plr[pnum].HoldItem._ivalue;
								if (gt >= GOLD_VT2) {
									plr[pnum].InvList[i]._iCurs = ITEM_5GOLD;
								} else {
									if (gt <= GOLD_VT1) plr[pnum].InvList[i]._iCurs = ITEM_1GOLD;
									else plr[pnum].InvList[i]._iCurs = ITEM_3GOLD;
								}
							} else {
								// Fill as much as possible and keep holding extra
								gt = GOLD_VMAX - plr[pnum].InvList[i]._ivalue;
								plr[pnum]._pGold += gt;
								plr[pnum].HoldItem._ivalue -= gt;
								plr[pnum].InvList[i]._ivalue = GOLD_VMAX;
								plr[pnum].InvList[i]._iCurs = ITEM_5GOLD;
								if (plr[pnum].HoldItem._ivalue >= GOLD_VT2) cn = ITEM_5GOLD + ICSTART;
								else {
									if (plr[pnum].HoldItem._ivalue <= GOLD_VT1) cn = ITEM_1GOLD + ICSTART;
									else cn = ITEM_3GOLD + ICSTART;
								}
							}
						} else {
							// Place gold in empty spot
							ii = plr[pnum]._pNumInv;
							plr[pnum].InvList[ii] = plr[pnum].HoldItem;
							plr[pnum]._pNumInv++;
							plr[pnum].InvGrid[xx+yy] = plr[pnum]._pNumInv;
							plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
							//Choose correct gold cursor
							gt = plr[pnum].HoldItem._ivalue; 
							if (gt <= GOLD_VMAX) {
								if (gt >= GOLD_VT2) {
									plr[pnum].InvList[ii]._iCurs = ITEM_5GOLD;
								} else {
									if (gt <= GOLD_VT1) plr[pnum].InvList[ii]._iCurs = ITEM_1GOLD;
									else plr[pnum].InvList[ii]._iCurs = ITEM_3GOLD;
								}
							} else {
								plr[pnum].InvList[ii]._iCurs = ITEM_5GOLD;
							}
						}
					} else {
						// Item not gold
						if (iv == 0) {
							// No swaping item
							ii = plr[pnum]._pNumInv;
							plr[pnum].InvList[ii] = plr[pnum].HoldItem;
							plr[pnum]._pNumInv++;
							iv = plr[pnum]._pNumInv;
						} else {
							ii = iv-1;
							if (plr[pnum].HoldItem._itype == IT_GOLD) plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
							cn = SwapItem(&plr[pnum].InvList[ii], &plr[pnum].HoldItem);
							if (plr[pnum].HoldItem._itype == IT_GOLD) plr[pnum]._pGold = CalculateGold(pnum);
							for (i = 0; i < MAXINV; i++) {
								if (plr[pnum].InvGrid[i] == iv) plr[pnum].InvGrid[i] = 0;
								if (plr[pnum].InvGrid[i] == -iv) plr[pnum].InvGrid[i] = 0;
							}
						}
						ii = r - 25; 
						yy = ((ii / 10) - ((sy - 1) >> 1)) * 10;
						if (yy < 0) yy = 0;
						for (j = 0; j < sy; j++) {
							xx = (ii % 10) - ((sx - 1) >> 1);
							if (xx < 0) xx = 0;
							for (i = 0; i < sx; i++) {
								if ((i == 0) && (j == sy-1)) plr[pnum].InvGrid[xx+yy] = iv;
								else plr[pnum].InvGrid[xx+yy] = -iv;
								xx++;
							}
							yy+=10;
						}
					}
					break;
				case IL_SPD:
					ii = r - 65;
					// Gold is stackable
					if (plr[pnum].HoldItem._itype == IT_GOLD) {
						// Check if something is already there
						if (plr[pnum].SpdList[ii]._itype != -1) {
							if (plr[pnum].SpdList[ii]._itype == IT_GOLD) {
								gt = plr[pnum].SpdList[ii]._ivalue + plr[pnum].HoldItem._ivalue;
								// Gold total <= GOLD_VMAX then all done
								if (gt <= GOLD_VMAX) {
									plr[pnum].SpdList[ii]._ivalue += plr[pnum].HoldItem._ivalue;
									plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
									if (gt >= GOLD_VT2) {
										plr[pnum].SpdList[ii]._iCurs = ITEM_5GOLD;
									} else {
										if (gt <= GOLD_VT1) plr[pnum].SpdList[ii]._iCurs = ITEM_1GOLD;
										else plr[pnum].SpdList[ii]._iCurs = ITEM_3GOLD;
									}
								} else {
									// Fill as much as possible and keep holding extra
									gt = GOLD_VMAX - plr[pnum].SpdList[ii]._ivalue;
									plr[pnum]._pGold += gt;
									plr[pnum].HoldItem._ivalue -= gt;
									plr[pnum].SpdList[ii]._ivalue = GOLD_VMAX;
									plr[pnum].SpdList[ii]._iCurs = ITEM_5GOLD;
									if (plr[pnum].HoldItem._ivalue >= GOLD_VT2) cn = ITEM_5GOLD + ICSTART;
									else {
										if (plr[pnum].HoldItem._ivalue <= GOLD_VT1) cn = ITEM_1GOLD + ICSTART;
										else cn = ITEM_3GOLD + ICSTART;
									}
								}
							} else {
								// Swap with non gold
								plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
								cn = SwapItem(&plr[pnum].SpdList[ii], &plr[pnum].HoldItem);
							}
						} else {
							// Place gold in empty spot
							plr[pnum].SpdList[ii] = plr[pnum].HoldItem;
							plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
						}
					} else {
						// Not gold. Place or swap
						if (plr[pnum].SpdList[ii]._itype == -1) plr[pnum].SpdList[ii] = plr[pnum].HoldItem;
						else {
							cn = SwapItem(&plr[pnum].SpdList[ii], &plr[pnum].HoldItem);
							if (plr[pnum].HoldItem._itype == IT_GOLD) plr[pnum]._pGold = CalculateGold(pnum);
						}
					}
					drawsbarflag = TRUE;
					break;
			}

			CalcPlrInv(pnum,TRUE);
			if (pnum == myplr) {
				if (cn == GLOVE_CURS) SetCursorPos(MouseX + (cursW>>1),MouseY + (cursH>>1));
				SetCursor(cn);
			}
		}
	}
}

/*-----------------------------------------------------------------------**
** Change items for another player
**-----------------------------------------------------------------------*/
// drb.patch1.start.02/10/97
//void SyncInvPaste(int pnum, BYTE bLoc, int idx, WORD icreateinfo, int iseed)
void SyncInvPaste(int pnum, BYTE bLoc, int idx, WORD icreateinfo, int iseed, BOOL Id)
// drb.patch1.end.02/10/97
{
	// temp use the first slot to grab an item
	RecreateItem(TEMPAVAIL, idx, icreateinfo, iseed, 0);
	PlayerStruct * p = &plr[pnum];
	p->HoldItem = item[TEMPAVAIL];
	// drb.patch1.start.02/10/97
	if (Id) p->HoldItem._iIdentified = TRUE;
	// drb.patch1.end.02/10/97

	if (bLoc < NUM_INVLOC) {
		p->InvBody[bLoc] = p->HoldItem;
		if (bLoc == INVLOC_HAND1) {
			if (p->HoldItem._iLoc == IL_2HAND)
				p->Hand2Item._itype = -1;
		}
		else if (bLoc == INVLOC_HAND2) {
			if (p->HoldItem._iLoc == IL_2HAND)
				p->Hand1Item._itype = -1;
		}
	}

	CalcPlrInv(pnum,TRUE);
}

/*-----------------------------------------------------------------------**
**  Pick up items from inventory window and set cursor appropriately
**-----------------------------------------------------------------------*/

void CheckInvCut(int pnum, int mx, int my)
{
	int r;
	BOOL done;
	int ii,iv,i;

	if (plr[pnum]._pmode > PM_WALK3) return;
	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	done = FALSE;
	for (r = 0; (r < INVRECTS) && (!done); r++) {
		if ((mx >= InvRect[r].x) && (mx < InvRect[r].x + 29) && (my >= InvRect[r].y - 29) && (my < InvRect[r].y)) {
			done = TRUE;
			r--;
		}
	}
	if (done) {
		plr[pnum].HoldItem._itype = -1;
		if ((r >= 0) && (r <= 3) && (plr[pnum].HeadItem._itype != -1)) {
			NetSendCmdDelItem(FALSE, INVLOC_HEAD);
			plr[pnum].HoldItem = plr[pnum].HeadItem;
			plr[pnum].HeadItem._itype = -1;
		}
		if ((r == 4) && (plr[pnum].Ring1Item._itype != -1)) {
			NetSendCmdDelItem(FALSE, INVLOC_RING1);
			plr[pnum].HoldItem = plr[pnum].Ring1Item;
			plr[pnum].Ring1Item._itype = -1;
		}
		if ((r == 5) && (plr[pnum].Ring2Item._itype != -1)) {
			NetSendCmdDelItem(FALSE, INVLOC_RING2);
			plr[pnum].HoldItem = plr[pnum].Ring2Item;
			plr[pnum].Ring2Item._itype = -1;
		}
		if ((r == 6) && (plr[pnum].NeckItem._itype != -1)) {
			NetSendCmdDelItem(FALSE, INVLOC_NECK);
			plr[pnum].HoldItem = plr[pnum].NeckItem;
			plr[pnum].NeckItem._itype = -1;
		}
		if ((r >= 7) && (r <= 12) && (plr[pnum].Hand1Item._itype != -1)) {
			NetSendCmdDelItem(FALSE, INVLOC_HAND1);
			plr[pnum].HoldItem = plr[pnum].Hand1Item;
			plr[pnum].Hand1Item._itype = -1;
		}
		if ((r >= 13) && (r <= 18) && (plr[pnum].Hand2Item._itype != -1)) {
			NetSendCmdDelItem(FALSE, INVLOC_HAND2);
			plr[pnum].HoldItem = plr[pnum].Hand2Item;
			plr[pnum].Hand2Item._itype = -1;
		}
		if ((r >= 19) && (r <= 24) && (plr[pnum].BodyItem._itype != -1)) {
			NetSendCmdDelItem(FALSE, INVLOC_BODY);
			plr[pnum].HoldItem = plr[pnum].BodyItem;
			plr[pnum].BodyItem._itype = -1;
		}

		if ((r >= 25) && (r <= 64)) {
			ii = r - 25;
			if (plr[pnum].InvGrid[ii] != 0) {
				if (plr[pnum].InvGrid[ii] > 0) iv = plr[pnum].InvGrid[ii];
				else iv = -plr[pnum].InvGrid[ii];
				for (i = 0; i < MAXINV; i++) {
					if ((plr[pnum].InvGrid[i] == iv) || (plr[pnum].InvGrid[i] == -iv))
						plr[pnum].InvGrid[i] = 0;
				}
				iv--;
				plr[pnum].HoldItem = plr[pnum].InvList[iv];
				plr[pnum]._pNumInv--;
				if ((plr[pnum]._pNumInv > 0) && (plr[pnum]._pNumInv != iv)) {
					plr[pnum].InvList[iv] = plr[pnum].InvList[plr[pnum]._pNumInv];
					for (i = 0; i < MAXINV; i++) {
						if (plr[pnum].InvGrid[i] == plr[pnum]._pNumInv+1) plr[pnum].InvGrid[i] = iv+1;
						if (plr[pnum].InvGrid[i] == -(plr[pnum]._pNumInv+1)) plr[pnum].InvGrid[i] = -(iv+1);
					}
				}
			}
		}
		if (r >= 65) {
			ii = r - 65;
			if (plr[pnum].SpdList[ii]._itype != -1) {
				plr[pnum].HoldItem = plr[pnum].SpdList[ii];
				plr[pnum].SpdList[ii]._itype = -1;
				drawsbarflag = TRUE;
			}
		}

		if (plr[pnum].HoldItem._itype != -1) {
			if (plr[pnum].HoldItem._itype == IT_GOLD) plr[pnum]._pGold = CalculateGold(pnum);
			CalcPlrInv(pnum,TRUE);
			CheckItemStats(pnum);
			if (pnum == myplr) {
				PlaySFX(IS_IGRAB);
				SetCursor(plr[pnum].HoldItem._iCurs + ICSTART);
				SetCursorPos(mx - (cursW>>1),MouseY - (cursH>>1));
			}
		}
	}
}

/*-----------------------------------------------------------------------*
** Remove an item from another player
**-----------------------------------------------------------------------*/
void SyncInvCut(int pnum, BYTE bLoc) {
	if (bLoc < NUM_INVLOC)
		plr[pnum].InvBody[bLoc]._itype = -1;
	if (plr[pnum]._pmode != PM_DEATH) CalcPlrInv(pnum,TRUE);
	else CalcPlrInv(pnum,FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RemoveInvItem(int pnum, int iv)
{
	int i;

	iv++;
	for (i = 0; i < MAXINV; i++) {
		if ((plr[pnum].InvGrid[i] == iv) || (plr[pnum].InvGrid[i] == -iv))
			plr[pnum].InvGrid[i] = 0;
	}
	iv--;
	plr[pnum]._pNumInv--;
	if ((plr[pnum]._pNumInv > 0) && (plr[pnum]._pNumInv != iv)) {
		plr[pnum].InvList[iv] = plr[pnum].InvList[plr[pnum]._pNumInv];
		for (i = 0; i < MAXINV; i++) {
			if (plr[pnum].InvGrid[i] == plr[pnum]._pNumInv+1) plr[pnum].InvGrid[i] = iv+1;
			if (plr[pnum].InvGrid[i] == -(plr[pnum]._pNumInv+1)) plr[pnum].InvGrid[i] = -(iv+1);
		}
	}

	CalcPlrScrolls(pnum);
	if ((plr[pnum]._pRSplType == SPT_SCROLL) && (plr[pnum]._pRSpell != -1)) {
		if (!(plr[pnum]._pScrlSpells & (1 << plr[pnum]._pRSpell-1))) plr[pnum]._pRSpell = -1;
		force_redraw = FULLDRAW;
	}
}

StripPlayer(int pnum)
{
	int i;

	// Give the plr no items in hands gfx
	if (plr[pnum]._pgfxnum != PGFX_NGUY) {
		plr[pnum]._pgfxnum = PGFX_NGUY;
		plr[pnum]._pGFXLoad = 0;
		SetPlrAnims(pnum);
	}
	
	ItemStruct *pi = &plr[pnum].InvBody[0];
	for (i = NUM_INVLOC; i--; pi++)
		 pi->_itype = -1;
	
	
	int oldnum = plr[pnum]._pNumInv;
	
	ZeroMemory(plr[pnum].InvGrid,sizeof(plr[pnum].InvGrid));
	plr[pnum]._pNumInv = 0;
	for (i = 0; i < oldnum; ++i)
	{
		pi = &plr[pnum].InvList[i];
		if (pi->_itype == IT_GOLD)
		{
			int num = plr[pnum]._pNumInv;
			ItemStruct tmp = *pi;
			
			pi->_itype = -1;
			plr[pnum].InvList[num] = tmp;
			plr[pnum]._pNumInv++;
			plr[pnum].InvGrid[i] = plr[pnum]._pNumInv;
		}
		else
			pi->_itype = -1;
	}

	// zero speedbar
	pi = &plr[pnum].SpdList[0];
	for (i = MAXSPD; i--; pi++)
		pi->_itype = -1;
	
	CalcPlrItemVals(pnum,FALSE);
	return FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RemoveSpdBarItem(int pnum, int iv)
{
	plr[pnum].SpdList[iv]._itype = -1;
	CalcPlrScrolls(pnum);
	if ((plr[pnum]._pRSplType == SPT_SCROLL) && (plr[pnum]._pRSpell != -1)) {
		if (!(plr[pnum]._pScrlSpells & (1 << plr[pnum]._pRSpell-1))) plr[pnum]._pRSpell = -1;
	}
	force_redraw = FULLDRAW;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckInvScrn() {
	if (curs >= ICSTART) CheckInvPaste(myplr, MouseX, MouseY);
	else CheckInvCut(myplr, MouseX, MouseY);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckSpdBar() {
	if ((MouseX > 190) && (MouseX < 437) && (MouseY > 352) && (MouseY < 385))
		CheckInvScrn();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckItemStats(int pnum) {
	PlayerStruct * p = &plr[pnum];
	ItemStruct * pi = &p->HoldItem;

	pi->_iStatFlag = FALSE;
	if (p->_pStrength < pi->_iMinStr) return;
	if (p->_pMagic < (byte)pi->_iMinMag) return;
	if (p->_pDexterity < pi->_iMinDex) return;
	pi->_iStatFlag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CheckBookLevel(int pnum)
{
	if (plr[pnum].HoldItem._iMiscId != IMID_BOOK) return;
	plr[pnum].HoldItem._iMinMag = spelldata[plr[pnum].HoldItem._iSpell].sMinInt;
	int slvl = plr[pnum]._pSplLvl[plr[pnum].HoldItem._iSpell];
	while (slvl != 0) {
		plr[pnum].HoldItem._iMinMag += ((plr[pnum].HoldItem._iMinMag * 20) / 100);
		slvl--;
		if ((plr[pnum].HoldItem._iMinMag + ((plr[pnum].HoldItem._iMinMag * 20) / 100)) > 255) {
			plr[pnum].HoldItem._iMinMag = 255;
			slvl = 0;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CheckQuestItem(int pnum)
{
	if (plr[pnum].HoldItem.IDidx == IDI_OPTAMULET) quests[Q_BLIND]._qactive = QUEST_DONE;
	if (plr[pnum].HoldItem.IDidx == IDI_MUSHROOM
		&& quests[Q_BKMUSHRM]._qactive == QUEST_NOTDONE
		&& quests[Q_BKMUSHRM]._qvar1 == QS_MUSHSPAWNED) {
		// say "That is a big mushroom"
		#if !IS_VERSION(SHAREWARE)
		sfxdelay = 10;
		if (plr[pnum]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR95;
		else if (plr[pnum]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE95;
		else if (plr[pnum]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE95;
		else if (plr[pnum]._pClass == CLASS_MONK) sfxdnum = PS_MONK95;
		else if (plr[pnum]._pClass == CLASS_BARD) sfxdnum = PS_BARD95;
		else if (plr[pnum]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN95;
		#endif
		quests[Q_BKMUSHRM]._qvar1 = QS_MUSHPICKED;
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ANVIL) {
		if (quests[Q_ANVIL]._qactive == QUEST_NOTACTIVE) {
			quests[Q_ANVIL]._qactive = QUEST_NOTDONE;
			quests[Q_ANVIL]._qvar1 = 1;
		}
		if (quests[Q_ANVIL]._qlog == TRUE) {
			#if !IS_VERSION(SHAREWARE)
			sfxdelay = 10;
			if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR89;
			else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE89;
			else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE89;
			else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK89;
			else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD89;
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN89;
			#endif
		}
	}

	if (plr[pnum].HoldItem.IDidx == IDI_GLDNELIX) {
		#if !IS_VERSION(SHAREWARE)
		sfxdelay = 30;
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR88;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE88;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE88;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK88;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD88;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN88;
		#endif
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ROCK) {
		if (quests[Q_ROCK]._qactive == QUEST_NOTACTIVE) {
			quests[Q_ROCK]._qactive = QUEST_NOTDONE;
			quests[Q_ROCK]._qvar1 = 1;
		}
		if (quests[Q_ROCK]._qlog == TRUE) {
			#if !IS_VERSION(SHAREWARE)
			sfxdelay = 10;
			if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR87;
			else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE87;
			else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE87;
			else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK87;
			else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD87;
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN87;
			#endif
		}
	}

	if (plr[pnum].HoldItem.IDidx == IDI_ARMOFVAL) {
		quests[Q_BLOOD]._qactive = QUEST_DONE;
		#if !IS_VERSION(SHAREWARE)
		sfxdelay = 20;
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR91;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE91;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE91;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK91;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD91;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN91;
		#endif
	}

	// Add pick up item triggers here. JKE
	if (plr[pnum].HoldItem.IDidx == IDI_MAPOFDOOM) {
			quests[Q_CRYPTMAP]._qactive = QUEST_NOTDONE;
			quests[Q_CRYPTMAP]._qvar1 = 1;
			quests[Q_CRYPTMAP]._qlog = FALSE;
		
		#if !IS_VERSION(SHAREWARE)
			sfxdelay = 10;
			if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR79;
			else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE79;
			else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE79;
			else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK79;
			else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD79;
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN79;
		#endif
		
	}
	
	if ((plr[pnum].HoldItem.IDidx == IDI_NOTE1) ||
		(plr[pnum].HoldItem.IDidx == IDI_NOTE2) ||
		(plr[pnum].HoldItem.IDidx == IDI_NOTE3))
	{
	#if !IS_VERSION(SHAREWARE)
	
		int noteflag = 0;
		int idi = plr[pnum].HoldItem.IDidx;
		int x1, x2, x3;
		
		if (PlrHasItem(pnum, IDI_NOTE1, x1) || idi == IDI_NOTE1)
			noteflag |= 0x01;
		 
		if (PlrHasItem(pnum, IDI_NOTE2, x2) || idi == IDI_NOTE2)
			noteflag |= 0x02;
			
		if (PlrHasItem(pnum, IDI_NOTE3, x3) || idi == IDI_NOTE3)
			noteflag |= 0x04;
			
		if (noteflag == 0x07)
		{
			// "Just what I was looking for"
			sfxdelay = 10;
			if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR46;
			else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE46;
			else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE46;
			else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK46;
			else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD46;
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN46;
			
			// Put the items together
			switch(idi)
			{
			case IDI_NOTE1:
				PlrHasItem(pnum, IDI_NOTE2, x2);
				RemoveInvItem(pnum, x2);
				PlrHasItem(pnum, IDI_NOTE3, x3);
				RemoveInvItem(pnum, x3);
				break;
			case IDI_NOTE2:
				PlrHasItem(pnum, IDI_NOTE1, x1);
				RemoveInvItem(pnum, x1);
				PlrHasItem(pnum, IDI_NOTE3, x3);
				RemoveInvItem(pnum, x3);
				break;
			case IDI_NOTE3:
				PlrHasItem(pnum, IDI_NOTE1, x1);
				RemoveInvItem(pnum, x1);
				PlrHasItem(pnum, IDI_NOTE2, x2);
				RemoveInvItem(pnum, x2);
				break;
			}
			
			int ii = itemavail[0];		// use this item slot temporarily
			ItemStruct tmp = item[ii];
				
			GetItemAttrs(ii, IDI_FULLNOTE, 16);
			SetupItem(ii);
			plr[pnum].HoldItem = item[ii];
			
			item[ii] = tmp;
		}
	#endif	
	}

}

/*-----------------------------------------------------------------------*
**	Processes CMD_GETITEM player command
**-----------------------------------------------------------------------*/

void InvGetItem(int pnum, int ii)
{
		
	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}
	if (dItem[item[ii]._ix][item[ii]._iy] == 0) return;		// Already picked up?
	// Next line can happen in mulitplayer due to latecency
	if ((myplr == pnum) && (curs >= ICSTART)) {
		NetSendCmdPItem(TRUE,CMD_SYNCPUTITEM,plr[myplr]._px,plr[myplr]._py);
		//InvPutItem(myplr,plr[myplr]._px,plr[myplr]._py);
	}

	// once an item is picked up it is no longer a pregen item
	if (item[ii]._iUid != 0)
		item[ii]._iCreateInfo &= ICI_PREGENMASK;
	plr[pnum].HoldItem = item[ii];
	CheckQuestItem(pnum);
	CheckBookLevel(pnum);
	CheckItemStats(pnum);
	
	// added 970911 by DKT
	BOOL done = FALSE;
	if (plr[pnum].HoldItem._itype == IT_GOLD) {
		if (GoldAutoPlace(pnum))
			done = TRUE;
	}
	
	// PATCH1.JMM
	DROPLOG("  InvGetItem:  deleting item %d.\n",ii);
//	item[ii]._iDelFlag = TRUE;
	// ENDPATCH1.JMM

	dItem[item[ii]._ix][item[ii]._iy] = 0;
	if (currlevel == CRYPTSTART)
	{
		if (item[ii]._ix == CornerStone.x &&
			item[ii]._iy == CornerStone.y)
		{
			CornerStone.item.IDidx = -1;
			CornerStone.item._itype = 0;
			CornerStone.item._ix = 0;
			CornerStone.item._iy = 0;
			CornerStone.item._iAnimFlag = FALSE;
			CornerStone.item._iSelFlag = ISEL_NONE;
			CornerStone.item._iIdentified = FALSE;
			CornerStone.item._iPostDraw = FALSE;

		}
	}

	// PATCH1.JMM 
	int j;
	int jj;

	#if _DEBUG
		for(j = 0; j < numitems; j++) {
			DROPLOG("  InvGetItem:  itemactive[%2.2d]:  %2.2d\n",j,itemactive[j]);
		}
	#endif

	j = 0;
	while (j < numitems) {
		jj = itemactive[j];
	
		if (jj == ii) {
			DROPLOG("  InvGetItem:  Removing item index %d (struct %d,numitems %d) from item list!\n",j,jj,numitems);
			DeleteItem(jj, j);
			j = 0;
		} 
		else { 
			j++;
		}

	}
	// ENDPATCH1.JMM

	cursitem = -1;
	if (!done)
		NewCursor(plr[pnum].HoldItem._iCurs + ICSTART);
}

/*-----------------------------------------------------------------------*
**	Processes CMD_AGETITEM player command
**-----------------------------------------------------------------------*/

void AutoGetItem(int pnum, int ii)
{
	int i, g, w, h, idx;
	BOOL done;

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	// drb.patch1.start.02/15/97
	if (ii != TEMPAVAIL) {
	// drb.patch1.end.02/15/97
		if (dItem[item[ii]._ix][item[ii]._iy] == 0) return;		// Already picked up?
	}

	done = FALSE;
	// once an item is picked up it is no longer a pregen item
	if (item[ii]._iUid != 0)
		item[ii]._iCreateInfo &= ICI_PREGENMASK;
	// Pick it up to get params for placing UID_OPTAMULET
	plr[pnum].HoldItem = item[ii];
	CheckQuestItem(pnum);
	CheckBookLevel(pnum);
	CheckItemStats(pnum);
	SetICursor(plr[pnum].HoldItem._iCurs + ICSTART);

	// Try and fit it in
	if (plr[pnum].HoldItem._itype == IT_GOLD) {
		done = GoldAutoPlace(pnum);
		
		if (!done)
		{
			// If we only particially filled our inventory with gold.
			// reduce the temp value by to the new amount we are holding.
			item[ii]._ivalue = plr[pnum].HoldItem._ivalue;
		}
	} else {
		done = FALSE;
		g = plr[pnum]._pgfxnum & PGFX_MASK;
		if (((g == PGFX_NGUY) || (g == PGFX_SGUY) ||
		     (plr[pnum]._pClass == CLASS_BARD && (g == PGFX_ZGUY || g == PGFX_XGUY))) &&
			(plr[pnum]._pmode <= PM_WALK3) &&
			((plr[pnum].HoldItem._iStatFlag) && (plr[pnum].HoldItem._iClass == IC_WEAP))) {
				done = WeaponAutoPlace(pnum);
				if (done) CalcPlrInv(pnum,TRUE);
		}
		if (!done) {
			// all items are either 1x1, 1x2, 1x3, 2x2, or 2x3
			// the inv is 10x4
			w = icursW28;
			h = icursH28;
			if ((w == 1) && (h == 1)) {
				// Check to see if speed bar available first
				idx = plr[pnum].HoldItem.IDidx;
				if ((plr[pnum].HoldItem._iStatFlag) && (AllItemsList[idx].iUsable)) {
					for (i = 0; (i < MAXSPD) && (!done); i++) {
						if (plr[pnum].SpdList[i]._itype == -1) {
							plr[pnum].SpdList[i] = plr[pnum].HoldItem;
							CalcPlrScrolls(pnum);
							drawsbarflag = TRUE;
							done = TRUE;
						}
					}
				}
				// Start in lower left and continue trying right then up a line
				for (i = 30; (i <= 39) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 20; (i <= 29) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 10; (i <= 19) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 0; (i <= 9) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
			}
			if ((w == 1) && (h == 2)) {
				// Try 3rd row, 1st row, and then 2nd row
				for (i = 29; (i >= 20) && (!done); i--) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 9; (i >= 0) && (!done); i--) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 19; (i >= 10) && (!done); i--) done = AutoPlace(pnum, i, w, h, TRUE);
			}
			if ((w == 1) && (h == 3)) {
				// Try 1st row then 2nd row
				for (i = 0; (i < 20) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
			}
			if ((w == 2) && (h == 2)) {
				// Try 1st and 3rd row starting right and moving left
				for (i = 0; (i < 10) && (!done); i++) done = AutoPlace(pnum, AP2x2Tbl[i], w, h, TRUE);
				// Try 3rd row, 1st row, and then 2nd row
				for (i = 21; (i < 29) && (!done); i+=2) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 1; (i < 9) && (!done); i+=2) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 10; (i < 19) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
			}
			if ((w == 2) && (h == 3)) {
				// Try 1st row then 2nd row
				for (i = 0; (i < 9) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
				for (i = 10; (i < 19) && (!done); i++) done = AutoPlace(pnum, i, w, h, TRUE);
			}
		}
	}

	if (done) {
		// PATCH1.JMM
		DROPLOG("  AutoGetItem:  set item[%d].iDelFlag = TRUE",ii);
		//item[ii]._iDelFlag = TRUE;
		dItem[item[ii]._ix][item[ii]._iy] = 0;
		if (currlevel == CRYPTSTART)
		{
			if (item[ii]._ix == CornerStone.x &&
				item[ii]._iy == CornerStone.y)
			{
				CornerStone.item.IDidx = -1;
				CornerStone.item._itype = 0;
				CornerStone.item._ix = 0;
				CornerStone.item._iy = 0;
				CornerStone.item._iAnimFlag = FALSE;
				CornerStone.item._iSelFlag = ISEL_NONE;
				CornerStone.item._iIdentified = FALSE;
				CornerStone.item._iPostDraw = FALSE;
			}
		}

		int j;
		int jj;
		
		j = 0;
		while (j < numitems) {
			jj = itemactive[j];

			if (jj == ii) {
				DROPLOG("  AutogetItem:  Removing item index %d (struct %d,numitems %d) from item list!\n",j,jj,numitems);
				DeleteItem(jj, j);
				j = 0;
			} 
			else { 
				j++;
			}

		}
		// ENDPATCH1.JMM

	} else {
		if (pnum == myplr) {
			if (plr[pnum]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR14+random(0,3));
			#if !IS_VERSION(SHAREWARE)
			else if (plr[pnum]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE14+random(0,3));
			else if (plr[pnum]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE14+random(0,3));
			else if (plr[pnum]._pClass == CLASS_MONK) PlaySFX(PS_MONK14+random(0,3));
			else if (plr[pnum]._pClass == CLASS_BARD) PlaySFX(PS_BARD14+random(0,3));
			else if (plr[pnum]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN14+random(0,3));
			#endif
		}
		plr[pnum].HoldItem = item[ii];
		DROPLOG("AutogetItem\n");
		RespawnItem(ii, TRUE);
		NetSendCmdPItem(TRUE,CMD_RESPAWNITEM,item[ii]._ix,item[ii]._iy);
		plr[pnum].HoldItem._itype = -1;
		// we're going to drop the item so change the cursor back to a glove.
		NewCursor(GLOVE_CURS);
	}
}

// PATCH1.JMM
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if _DEBUG
void VerifyItemActiveList( void ) 
{
	int i, j;
	UINT nItemIndex;

	int nCurrIndex;
	WORD wCurrCI;
	int dwCurrSeed;
	BYTE x, y;

	for( i = 0; i < numitems; i++ ) {
		nItemIndex = itemactive[i];

		nCurrIndex = item[nItemIndex].IDidx;
		wCurrCI = item[nItemIndex]._iCreateInfo;
		dwCurrSeed = item[nItemIndex]._iSeed;
		x = item[nItemIndex]._ix;
		y = item[nItemIndex]._iy;

		for( j = i+1; j < numitems; j++ ) {
			nItemIndex = itemactive[j];
			if((item[nItemIndex].IDidx == nCurrIndex) && (item[nItemIndex]._iSeed == dwCurrSeed) && (item[nItemIndex]._iCreateInfo == wCurrCI))
				DROPLOG("  VerifyItemActiveList:  Duplicate item detected:  itemactive[%d] = %d (%d %8.8x %4.4x)\n",j, nItemIndex, nCurrIndex, dwCurrSeed, wCurrCI);

//			app_assert(!((item[nItemIndex].IDidx == nCurrIndex) && (item[nItemIndex]._iSeed == dwCurrSeed) && (item[nItemIndex]._iCreateInfo == wCurrCI)));
//			app_assert(!((item[nItemIndex]._ix == x) && (item[nItemIndex]._iy == y)));
		}
	}
}
#else 
	#define VerifyItemActiveList() 
#endif
// ENDPATCH1.JMM


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int FindGetItem(int idx, WORD ci, int iseed)
{
	int i, ii;

	// PATCH1.JMM
	VerifyItemActiveList();

	for (i = 0; i < numitems; i++) {
		ii = itemactive[i];
		if ((item[ii].IDidx == idx) && (item[ii]._iSeed == iseed) && (item[ii]._iCreateInfo == ci)) {
			return (ii);
		}
	}
	// ENDPATCH1.JMM

	return (-1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SyncGetItem(int x, int y, int idx, WORD ci, int iseed)
{
	int ii;

	// PATCH1.JMM
	VerifyItemActiveList();

	DROPLOG("  Doing SyncGetItem...\n");
	// try optimal location first
	if (dItem[x][y] != 0) {
		DROPLOG("  Attempting to find item by (x,y)\n");
		ii = dItem[x][y] - 1;
		if ((item[ii].IDidx != idx) || (item[ii]._iSeed != iseed) || (item[ii]._iCreateInfo != ci)) {
			ii = FindGetItem(idx, ci, iseed);
			DROPLOG("  Bad (x,y).  Did FindGetItem->%d.\n");
		}
		else {
			DROPLOG("  (x,y) said ii == %d, FindGetItem says ii == %d\n",ii,FindGetItem(idx,ci,iseed));
		}
	} 
	else { 
		DROPLOG("  Nongood (x,y)\n");
		ii = FindGetItem(idx, ci, iseed);
	}

	DROPLOG("  FindGetItem returned %d\n",ii);

	if (ii != -1) {
		DROPLOG("  Removing item from map!\n");
		// remove from map
		dItem[item[ii]._ix][item[ii]._iy] = 0;
		
		if (currlevel == CRYPTSTART)
		{
			if (item[ii]._ix == CornerStone.x &&
				item[ii]._iy == CornerStone.y)
			{
				CornerStone.item.IDidx = -1;
				CornerStone.item._itype = 0;
				CornerStone.item._ix = 0;
				CornerStone.item._iy = 0;
				CornerStone.item._iAnimFlag = FALSE;
				CornerStone.item._iSelFlag = ISEL_NONE;
				CornerStone.item._iIdentified = FALSE;
				CornerStone.item._iPostDraw = FALSE;
			}
		}
		// remove from list
		int j = 0;
		while (j < numitems) {
			int jj = itemactive[j];

			if (jj == ii) {
				DROPLOG("  Removing item index %d (struct %d,numitems %d) from item list!\n",j,jj,numitems);
				
				DeleteItem(jj, j);
				
				jj = FindGetItem(idx,ci,iseed);
				DROPLOG("  Item removed.  FindGetItem->%d, numitems->%d\n", jj, numitems);
				
				j = 0;
			} 
			else { 
				j++;
			}

		}

		app_assert(FindGetItem(idx, ci, iseed) == -1);
		DROPLOG("  Item is definitely removed!\n");
	}
	// ENDPATCH1.JMM
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL CanPut(int i, int j) {
	int oi;
	
	if (dItem[i][j]) return FALSE;
	if (nSolidTable[dPiece[i][j]]) return FALSE;

	if (dObject[i][j] != 0) {
		if (dObject[i][j] > 0) oi = dObject[i][j]-1;
		else oi = -(dObject[i][j]+1);
		if (object[oi]._oSolidFlag) return FALSE;
	}
	if (dObject[i+1][j+1] > 0) {
		oi = dObject[i+1][j+1]-1;
		if (object[oi]._oSelFlag != OSEL_NONE) return FALSE;
	}
	if (dObject[i+1][j+1] < 0) {
		oi = -(dObject[i+1][j+1]+1);
		if (object[oi]._oSelFlag != OSEL_NONE) return FALSE;
	}
	if ((dObject[i+1][j] > 0) && (dObject[i][j+1] > 0)) {
		oi = dObject[i+1][j]-1;
		if (object[oi]._oSelFlag != OSEL_NONE) {
			oi = dObject[i][j+1]-1;
			if (object[oi]._oSelFlag != OSEL_NONE) return FALSE;
		}
	}

	if (((currlevel == 0) && (dMonster[i][j] != 0)) ||
		((currlevel == 0) && (dMonster[i+1][j+1] != 0))) 
		return FALSE;

	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL TryInvPut() {
	// can't drop if too many items on floor already
	if (numitems >= MAXITEMS) return FALSE;

	int d = GetDirection(plr[myplr]._px, plr[myplr]._py, cursmx, cursmy);
	int x = plr[myplr]._px + offset_x[d];
	int y = plr[myplr]._py + offset_y[d];
	if (CanPut(x,y)) return TRUE;

	d = (d - 1) & 0x7;
	x = plr[myplr]._px + offset_x[d];
	y = plr[myplr]._py + offset_y[d];
	if (CanPut(x,y)) return TRUE;

	d = (d + 2) & 0x7;
	x = plr[myplr]._px + offset_x[d];
	y = plr[myplr]._py + offset_y[d];
	if (CanPut(x,y)) return TRUE;
	return(CanPut(plr[myplr]._px, plr[myplr]._py));
}


// PATCH1.JMM.2/24/97
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#define DUP_DISP_INTERVAL 5
static DWORD sgdwLastTime = 0;

void ShowDupString( LPCSTR lpszMessage ) {
	DWORD dwCurrTime;

	dwCurrTime = GetTickCount( );

	if((dwCurrTime - sgdwLastTime) < (DUP_DISP_INTERVAL*1000))
		return;

	sgdwLastTime = dwCurrTime;

	sysmsg_add_string( lpszMessage );
}


// ENDPATCH1.JMM.2/24/97


/*-----------------------------------------------------------------------*
**	Processes CMD_PUTITEM player command from myself
**-----------------------------------------------------------------------*/
int InvPutItem(int pnum, int x, int y) {
	// can't drop if too many items on floor already
	if (numitems >= MAXITEMS) {
		DROPLOG("numitems >= MAXITEMS\n");
		return -1;
	}

	// drb.patch1.start.02/18/97
	// already in list?
	int ii = FindGetItem(plr[pnum].HoldItem.IDidx,
		plr[pnum].HoldItem._iCreateInfo, plr[pnum].HoldItem._iSeed);
	if (ii != -1) {
		DROPLOG("  Duplicate item detected!\n");
		// JMM.PATCH1.2.22.97
		// NetSendString(1 << myplr, "A duplicate item has been detected. Unable to drop.");
		ShowDupString("A duplicate item has been detected.  Destroying duplicate...");
		SyncGetItem(x,y,plr[pnum].HoldItem.IDidx,plr[pnum].HoldItem._iCreateInfo,plr[pnum].HoldItem._iSeed);
		// END.JMM.PATCH1.2.22.97
	}
	// drb.patch1.end.02/18/97

	VerifyItemActiveList();

	// if square is too far away, then drop on adjacent square
	int d = GetDirection(plr[pnum]._px, plr[pnum]._py, x, y);
	int dx = x - plr[pnum]._px;
	int dy = y - plr[pnum]._py;
	if (abs(dx) > 1 || abs(dy) > 1) {
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
	}

	if (!CanPut(x,y)) {
		d = (d - 1) & 0x7;
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
		if (!CanPut(x,y)) {
			d = (d + 2) & 0x7;
			x = plr[pnum]._px + offset_x[d];
			y = plr[pnum]._py + offset_y[d];
			if (!CanPut(x,y)) {
				// radial search outward until a space is found
				BOOL done = FALSE;
				for (int l = 1; (l < 50) && !done; l++) {
					for (int j = -l; (j <= l) && !done; j++) {
						int yy = plr[pnum]._py + j;
						for (int i = -l; (i <= l) && !done; i++) {
							int xx = plr[pnum]._px + i;
							if (!CanPut(xx,yy)) continue;
							done = TRUE;
							x = xx;
							y = yy;
						}
					}
				}
				if (! done) return -1;
			}
		}
	}
// JKEQUESTS	Dropping stuff
	if (currlevel == 0)
	{
		if (plr[pnum].HoldItem._iCurs == ITEM_RUNEBOMB)
		{
			if(((cursmx >= 79) && (cursmx <= 82)) && ((cursmy >= 61) && (cursmy <= 64)))
			{
				NetSendCmdLocParam2(FALSE,
									CMD_OPEN_NEST,
									plr[pnum]._px,
									plr[pnum]._py,
									dx,
									dy );
				quests[Q_FARMER]._qactive = QUEST_DONE;
				if (gbMaxPlayers != 1)
					NetSendCmdQuest(TRUE, Q_FARMER);
				return -1;
			}
		}
		if (plr[pnum].HoldItem.IDidx == IDI_MAPOFDOOM)
		{
			if(((cursmx >= 35) && (cursmx <= 38)) && ((cursmy >= 20) && (cursmy <= 24)))
			{
				NetSendCmd(FALSE, CMD_OPEN_CRYPT);
				quests[Q_CRYPTMAP]._qactive = QUEST_DONE;
				if (gbMaxPlayers != 1)
					NetSendCmdQuest(TRUE, Q_CRYPTMAP);
				return -1;
			}
		}
	}


	app_assert(CanPut(x,y));
	ii = itemavail[0];
	dItem[x][y] = ii+1;
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;

	item[ii] = plr[pnum].HoldItem;
	item[ii]._ix = x;
	item[ii]._iy = y;
	

	DROPLOG("  InvPutItem\n");
	RespawnItem(ii, TRUE);
	numitems++;

	if (currlevel == CRYPTSTART)
	{
		if (x == CornerStone.x && y == CornerStone.y)
		{
//			memcpy(&(CornerStone.item), &(item[ii]), SAVE_ITEM_SIZE);
			CornerStone.item = item[ii];

			// Jims stuff
			InitQTextMsg(TXT_CORNERSTONE1);

			quests[Q_CORNERSTONE]._qlog = FALSE;
			quests[Q_CORNERSTONE]._qactive = QUEST_DONE;


		}
	}

	NewCursor(GLOVE_CURS);
	return ii;
}


/*-----------------------------------------------------------------------*
**	Processes CMD_PUTITEM player command from another person
**-----------------------------------------------------------------------*/
#if CHEATS
BOOL syncdebug = FALSE;
#endif


int SyncPutItem(
	int pnum, 
	int x, 
	int y, 
	int idx, 
	WORD icreateinfo, 
	int iseed, 
	BOOL Id, 
	int dur, 
	int mdur, 
	int ch, 
	int mch, 
	int ivalue, 
	DWORD ibuff,
	int PLToHit,
	int MaxDam,
	int MinStr,
	int MinMag,
	int MinDex,
	int AC) {

	#if CHEATS
	if (syncdebug) {
		sprintf(tempstr, "Player %i dropping item.", pnum);
		NetSendString((1 << myplr), tempstr);
	}
	#endif

	// can't drop if too many items on floor already
	if (numitems >= MAXITEMS) {
		DROPLOG("numitems >= MAXITEMS\n");
		return -1;
	}

	// drb.patch1.start.02/18/97
	// already in list?
	int ii = FindGetItem(idx, icreateinfo, iseed);
	if (ii != -1) {
		DROPLOG("  Duplicate item detected!\n");
		// JMM.PATCH1.2.22.97
		//NetSendString(1 << myplr, "A duplicate item has been detected from another player.");
		ShowDupString("A duplicate item has been detected from another player.");
		SyncGetItem(x,y,idx,icreateinfo,iseed);
		// END.JMM.PATCH1.2.22.97
		// return(-1);
	}
	// drb.patch1.end.02/18/97

	// if square is too far away, then drop on adjacent square
	int d = GetDirection(plr[pnum]._px, plr[pnum]._py, x, y);
	int dx = x - plr[pnum]._px;
	int dy = y - plr[pnum]._py;
	if (abs(dx) > 1 || abs(dy) > 1) {
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
	}

	if (!CanPut(x,y)) {
		d = (d - 1) & 0x7;
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
		if (!CanPut(x,y)) {
			d = (d + 2) & 0x7;
			x = plr[pnum]._px + offset_x[d];
			y = plr[pnum]._py + offset_y[d];
			if (!CanPut(x,y)) {
				BOOL done = FALSE;
				// radial search outward until a space is found
				for (int l = 1; (l < 50) && !done; l++) {
					for (int j = -l; (j <= l) && !done; j++) {
						int yy = plr[pnum]._py + j;
						for (int i = -l; (i <= l) && !done; i++) {
							int xx = plr[pnum]._px + i;
							if (!CanPut(xx,yy)) continue;
							done = TRUE;
							x = xx;
							y = yy;
						}
					}
				}
				if (! done) {
					DROPLOG("Failed to find a good square to drop in!\n");
   					return -1;
				}
			}
		}
	}


	app_assert(CanPut(x,y));
	ii = itemavail[0];
	dItem[x][y] = ii+1;
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;

	app_assert(ii >= 0);

 	if (idx == IDI_EAR) {
		RecreateEar(ii, icreateinfo, iseed, Id, dur, mdur, ch, mch, ivalue, ibuff);
	} else {
		RecreateItem(ii, idx, icreateinfo, iseed, ivalue);
		if (Id) item[ii]._iIdentified = TRUE;
		item[ii]._iDurability = dur;
		item[ii]._iMaxDur = mdur;
		item[ii]._iCharges = ch;
		item[ii]._iMaxCharges = mch;
		item[ii]._iPLToHit = PLToHit;
		item[ii]._iMaxDam = MaxDam;
		item[ii]._iMinStr = MinStr;
		item[ii]._iMinMag = MinMag;
		item[ii]._iMinDex = MinDex;
		item[ii]._iAC = AC;
	}
	item[ii]._ix = x;
	item[ii]._iy = y;

	DROPLOG("SyncPutItem: respawning object %d!\n",ii);
	RespawnItem(ii, TRUE);
	numitems++;

	if (currlevel == CRYPTSTART)
	{
		if (x == CornerStone.x && y == CornerStone.y)
		{
//			memcpy(&(CornerStone.item), &(item[ii]), SAVE_ITEM_SIZE);
			CornerStone.item = item[ii];

			// Jims stuff
			InitQTextMsg(TXT_CORNERSTONE1);

			quests[Q_CORNERSTONE]._qlog = FALSE;
			quests[Q_CORNERSTONE]._qactive = QUEST_DONE;

		}
	}

	return(ii);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
char CheckInvHLight() {

	// find box
	for (int r = 0; r < INVRECTS; r++) {
		if ((MouseX >= InvRect[r].x) && 
			(MouseX < InvRect[r].x + 29) && 
			(MouseY >= InvRect[r].y - 29) && 
			(MouseY < InvRect[r].y))
			break;
	}
	if (r >= INVRECTS) return -1;

	char rv = -1;
	infoclr = ICOLOR_WHITE;
	ItemStruct * pi = NULL;
	PlayerStruct * p = &plr[myplr];
	ClearPanel();

	if (r >= 0 && r <= 3) {
		rv = INVLOC_HEAD;
		pi = &p->HeadItem;
	}
	else if (r == 4) {
		rv = INVLOC_RING1;
		pi = &p->Ring1Item;
	}
	else if (r == 5) {
		rv = INVLOC_RING2;
		pi = &p->Ring2Item;
	}
	else if (r == 6) {
		rv = INVLOC_NECK;
		pi = &p->NeckItem;
	}
	else if (r >= 7 && r <= 12) {
		rv = INVLOC_HAND1;
		pi = &p->Hand1Item;
	}
	else if (r >= 13 && r <= 18) {
		// check for two handed item
		pi = &p->Hand1Item;
		if (pi->_itype != -1 && pi->_iLoc == IL_2HAND
			&& !(p->_pClass == CLASS_BARBARIAN 
				&& ( p->Hand1Item._itype == IT_SWORD ||
				     p->Hand1Item._itype == IT_MACE )
				)
			) {
			rv = INVLOC_HAND1;
		}
		else {
			rv = INVLOC_HAND2;
			pi = &p->Hand2Item;
		}
	}
	else if (r >= 19 && r <= 24) {
		rv = INVLOC_BODY;
		pi = &p->BodyItem;
	}
	else if (r >= 25 && r <= 64) {
		// is there an inventory item in this slot?
		if (0 == (r = abs(p->InvGrid[r - 25]))) return -1;

		// make zero based
		r--;

		// convert to global inventory slot number
		rv = r + NUM_INVLOC;

		// get item
		pi = &p->InvList[r];
	}
	else if (r >= 65) {
		drawsbarflag = TRUE;
		r -= 65;
		pi = &p->SpdList[r];
		if (pi->_itype == -1) return -1;

		// convert to global inventory slot number
		rv = r + MAXINV + NUM_INVLOC;
	}

	// Do I have an item (I should)
	app_assert(pi);

	// Is it still there?
	if (pi->_itype == -1) return -1;

	if (pi->_itype == IT_GOLD) {
		int nGold = pi->_ivalue;
		const char * get_pieces_str(int nGold);
		sprintf(infostr,"%i gold %s",nGold,get_pieces_str(nGold));
	}
	else {
		if (pi->_iMagical == IMAGIC_MAGIC) infoclr = ICOLOR_BLUE;
		else if (pi->_iMagical == IMAGIC_UNIQUE) infoclr = ICOLOR_GOLD;
		strcpy(infostr, pi->_iName);
		if (pi->_iIdentified) {
			strcpy(infostr, pi->_iIName);
			PrintItemDetails(pi);
		} else {
			PrintItemDur(pi);
		}
	}

	return rv;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RemoveScroll(int pnum)
{
	int i;

	for (i = 0; i < plr[pnum]._pNumInv; i++) {
		if ((plr[pnum].InvList[i]._itype != -1) &&
			((plr[pnum].InvList[i]._iMiscId == IMID_SCROLL) ||
			(plr[pnum].InvList[i]._iMiscId == IMID_TSCROLL)) &&
			(plr[pnum].InvList[i]._iSpell == plr[pnum]._pSpell)) {
				RemoveInvItem(pnum, i);
				CalcPlrScrolls(pnum);
				return;
		}
	}
	for (i = 0; i < MAXSPD; i++) {
		if ((plr[pnum].SpdList[i]._itype != -1) &&
			((plr[pnum].SpdList[i]._iMiscId == IMID_SCROLL) ||
			(plr[pnum].SpdList[i]._iMiscId == IMID_TSCROLL)) &&
			(plr[pnum].SpdList[i]._iSpell == plr[pnum]._pSpell)) {
				RemoveSpdBarItem(pnum, i);
				CalcPlrScrolls(pnum);
				return;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL UseScroll()
{
	int i;

	if (curs != GLOVE_CURS) return(FALSE);
	// rjs if (leveltype == 0) return(FALSE);
	if (leveltype == 0 && spelldata[plr[myplr]._pRSpell].sTownSpell == FALSE) return(FALSE);
	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if ((plr[myplr].InvList[i]._itype != -1) &&
			((plr[myplr].InvList[i]._iMiscId == IMID_SCROLL) ||
			(plr[myplr].InvList[i]._iMiscId == IMID_TSCROLL)) &&
			(plr[myplr].InvList[i]._iSpell == plr[myplr]._pRSpell)) {
				return(TRUE);
		}
	}
	for (i = 0; i < MAXSPD; i++) {
		if ((plr[myplr].SpdList[i]._itype != -1) &&
			((plr[myplr].SpdList[i]._iMiscId == IMID_SCROLL) ||
			(plr[myplr].SpdList[i]._iMiscId == IMID_TSCROLL)) &&
			(plr[myplr].SpdList[i]._iSpell == plr[myplr]._pRSpell)) {
				return(TRUE);
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void UseStaffCharge(int pnum)
{
	if ((plr[pnum].Hand1Item._itype != -1) &&
		(plr[pnum].Hand1Item._iMiscId == IMID_STAFF ||
		 plr[myplr].Hand1Item._iMiscId == IMID_UNIQUE) &&
		(plr[pnum].Hand1Item._iSpell == plr[pnum]._pRSpell) &&
		(plr[pnum].Hand1Item._iCharges > 0)) {
			plr[pnum].Hand1Item._iCharges--;
			CalcPlrStaff(pnum);
			return;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL UseStaff()
{
	if (curs != GLOVE_CURS) return(FALSE);
	if ((plr[myplr].Hand1Item._itype != -1) &&
		(plr[myplr].Hand1Item._iMiscId == IMID_STAFF ||
		 plr[myplr].Hand1Item._iMiscId == IMID_UNIQUE) &&
		(plr[myplr].Hand1Item._iSpell == plr[myplr]._pRSpell) &&
		(plr[myplr].Hand1Item._iCharges > 0)) {
			return(TRUE);
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL UseStaffSBook(int spl)
{
	if (curs != GLOVE_CURS) return(FALSE);
	if ((plr[myplr].Hand1Item._itype != -1) &&
		(plr[myplr].Hand1Item._iMiscId == IMID_STAFF ||
		 plr[myplr].Hand1Item._iMiscId == IMID_UNIQUE) &&
		(plr[myplr].Hand1Item._iSpell == spl) &&
		(plr[myplr].Hand1Item._iCharges > 0)) {
			return(TRUE);
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL CheckUsable(int pnum, int cii)
{
	int c, idata;
	BOOL rv;

	// Can't use items on my body, just in my inv
	rv = FALSE;
	if (cii <= 5) return(rv);
	if (cii <= 46) {
		c = cii - 7;
		idata = plr[pnum].InvList[c].IDidx;
		if (AllItemsList[idata].iUsable) rv = TRUE;
	} else {
		c = cii - 47;
		idata = plr[pnum].SpdList[c].IDidx;
		if (AllItemsList[idata].iUsable) rv = TRUE;
	}
	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

extern int	dropGoldValue;

void StartGoldDrop()
{
	// Save the index of the gold pile
	initialDropGoldIndex = cursinvitem;

	if (cursinvitem <= 46) // Gold item is in InvList
		// Save the value of the gold pile
		initialDropGoldValue = plr[myplr].InvList[cursinvitem-7]._ivalue;
	else // Gold item is in SpdList
		// Save the value of the gold pile
		initialDropGoldValue = plr[myplr].SpdList[cursinvitem-47]._ivalue;
						
	// Set flag so drop gold functionality is enabled
	dropGoldFlag = TRUE;
	// Initialize the drop gold value when right click gold pile
	dropGoldValue = 0;

	if (talkflag) TalkEnd();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL UseInvItem(int pnum, int cii)
{
	int c, idata, it;
	ItemStruct *Item;
	BOOL speedlist;

	// Don't use while dead
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) return TRUE;
	// Don't use unless glove curs
	if (curs != GLOVE_CURS) return TRUE;
	// Don't use during stores
	if (stextflag != STORE_NONE) return TRUE;

	// Can't use items on my body, just in my inv
	if (cii <= 5) return FALSE;
	if (cii <= 46) {
		c = cii - 7;
		Item = &plr[pnum].InvList[c];
		speedlist = FALSE;
	} else {
		if (talkflag) return TRUE;
		c = cii - 47;
		Item = &plr[pnum].SpdList[c];
		speedlist = TRUE;
	}

	idata = Item->IDidx;
	switch (idata)
	{
	case IDI_MUSHROOM:
		sfxdelay = 10;
		#if !IS_VERSION(SHAREWARE)
		if (plr[pnum]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR95;
		else if (plr[pnum]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE95;
		else if (plr[pnum]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE95;
		else if (plr[pnum]._pClass == CLASS_MONK) sfxdnum = PS_MONK95;
		else if (plr[pnum]._pClass == CLASS_BARD) sfxdnum = PS_BARD95;
		else if (plr[pnum]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN95;
		#endif
		return TRUE;
	case IDI_FUNGALTM:
		PlaySFX(IS_IBOOK);
		sfxdelay = 10;
		if (plr[pnum]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR29;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[pnum]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE29;
		else if (plr[pnum]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE29;
		else if (plr[pnum]._pClass == CLASS_MONK) sfxdnum = PS_MONK29;
		else if (plr[pnum]._pClass == CLASS_BARD) sfxdnum = PS_BARD29;
		else if (plr[pnum]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN29;
		#endif
		return TRUE;
	}
	if (!AllItemsList[idata].iUsable)
		return FALSE;
	if (!Item->_iStatFlag) {
		if (plr[pnum]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR13);
		#if !IS_VERSION(SHAREWARE)
		else if (plr[pnum]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE13);
		else if (plr[pnum]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE13);
		else if (plr[pnum]._pClass == CLASS_MONK) PlaySFX(PS_MONK13);
		else if (plr[pnum]._pClass == CLASS_BARD) PlaySFX(PS_BARD13);
		else if (plr[pnum]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN13);
		#endif
		return TRUE;
	} else {
		// Was the item chosen a pile of gold
		if ((Item->_iMiscId == IMID_NONE) && (Item->_itype == IT_GOLD)) 
		{
			StartGoldDrop();
			return TRUE;;
		}
		if (dropGoldFlag) {
			dropGoldFlag = FALSE;
			dropGoldValue = 0;
		}
		
		if ((Item->_iMiscId == IMID_SCROLL) && (currlevel == 0) &&
			(spelldata[Item->_iSpell].sTownSpell == FALSE)) return TRUE;
		if ((Item->_iMiscId == IMID_TSCROLL) && (currlevel == 0) &&
			(spelldata[Item->_iSpell].sTownSpell == FALSE)) return TRUE;
		if ((Item->_iMiscId > IMID_FIRSTRUNE) &&
			(Item->_iMiscId < IMID_LASTRUNE) &&
			(currlevel == 0)) return TRUE;
		it = ItemCAnimTbl[Item->_iCurs];
		if (Item->_iMiscId == IMID_BOOK) PlaySFX(IS_RBOOK);
		else if (pnum == myplr) PlaySFX(ItemInvSnds[it]);
		UseItem(pnum, Item->_iMiscId, Item->_iSpell);

		if (speedlist)
		{
			if (plr[pnum].SpdList[c]._iMiscId == IMID_FULLNOTE)
			{
				InitQTextMsg(TXT_SKULLJRNL7);
				invflag = FALSE;
//				PlaySFX(HSFX_SKULLJRNL7);
				return TRUE;
			}
			RemoveSpdBarItem(pnum, c);
		}
		else
		{
			// Don't delete map of doom when used
			if (plr[pnum].InvList[c]._iMiscId == IMID_MAPOFDOOM) return TRUE;
			if (plr[pnum].InvList[c]._iMiscId == IMID_FULLNOTE)
			{
				InitQTextMsg(TXT_SKULLJRNL7);
				invflag = FALSE;
//				PlaySFX(HSFX_SKULLJRNL7);
				return TRUE;
			}
			RemoveInvItem(pnum, c);
		}
	}
	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoTelekinesis()
{
	if (cursobj != -1) {
		NetSendCmdParam1(TRUE, CMD_OPOBJT, cursobj);
	}
	if (cursitem != -1) {
		NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr, myplr, cursitem);
	}
	BOOL M_Talker (int);
	if ((cursmonst != -1) && (!M_Talker(cursmonst)) && (monster[cursmonst].mtalkmsg == 0)) {
		NetSendCmdParam1(TRUE, CMD_KNOCKBACK, cursmonst);
	}
 	
	NewCursor(GLOVE_CURS);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

long CalculateGold(int pnum)
{
	int		i;
	long	gold;

	// Initialize gold
	gold = 0;
	// Find the gold in the speedbar list
	for (i = 0; i < MAXSPD; i++) {
		if (plr[pnum].SpdList[i]._itype == IT_GOLD) {
			gold += plr[pnum].SpdList[i]._ivalue;
			force_redraw = FULLDRAW;
		} 
	} 
	// Find the gold in the inventory list
	for (i = 0; i < plr[pnum]._pNumInv; i++) {
		if (plr[pnum].InvList[i]._itype == IT_GOLD) 
			gold += plr[pnum].InvList[i]._ivalue;
	} 
	
	return(gold);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL DropItemBeforeTrig()
/*-----------------------------------------------------------------------**
** DESCRIPTION: Drops the item the player has in hand before talking to a 
**              towner or entering a level.
** INPUT: None
** RETURN: TRUE = The item was dropped successfully
**         FALSE = The item could not be dropped.
/*-----------------------------------------------------------------------*/
{
	if (TryInvPut()) {
		NetSendCmdPItem(TRUE,CMD_PUTITEM,cursmx,cursmy);
		NewCursor(GLOVE_CURS);
		return TRUE;
	} 
	return FALSE;
}

int GetHighRingValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_RING
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_RING
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

int GetHighBowValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_BOW
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_BOW
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

int GetHighStaffValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_STAFF
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_STAFF
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

int GetHighSwordValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_SWORD
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_SWORD
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

int GetHighHelmValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_HELM
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_HELM
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

int GetHighShieldValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_SHIELD
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_SHIELD
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

int GetHighArmorValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& (plr[myPlr].InvBody[i]._itype == IT_ARMOR
			|| plr[myPlr].InvBody[i]._itype == IT_MARMOR
			|| plr[myPlr].InvBody[i]._itype == IT_HARMOR )
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& (plr[myPlr].InvList[i]._itype == IT_ARMOR
			|| plr[myPlr].InvList[i]._itype == IT_MARMOR
			|| plr[myPlr].InvList[i]._itype == IT_HARMOR)
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

int GetHighMaceValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_MACE
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_MACE
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	return itemValue;
}

int GetHighAmuletValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_AMULET
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_AMULET
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	return itemValue;
}

int GetHighAxeValue(int myPlr)
{
	int itemValue = 0;
	int i;
	
	for (i =0; i < NUM_INVLOC; ++i)
	{
		if (plr[myPlr].InvBody[i]._iClass != -1
			&& plr[myPlr].InvBody[i]._itype == IT_AXE
		    && itemValue < plr[myPlr].InvBody[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvBody[i]._iIvalue;
		}
	}
	for (i =0; i < MAXINV; ++i)
	{
		if (plr[myPlr].InvList[i]._iClass != -1
			&& plr[myPlr].InvList[i]._itype == IT_AXE
		    && itemValue < plr[myPlr].InvList[i]._iIvalue)
		{
			itemValue = plr[myPlr].InvList[i]._iIvalue;
		}
	}
	
	return itemValue;
}

