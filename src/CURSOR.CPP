/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  Cursor file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/CURSOR.CPP 4     2/12/97 10:48a Dbrevik2 $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"engine.h"
#include	"gendung.h"
#include	"control.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"objects.h"
#include	"cursor.h"
#include	"debug.h"
#include	"scrollrt.h"
#include	"inv.h"
#include	"trigs.h"
#include	"lighting.h"
#include	"missiles.h"
#include	"town.h"
#include	"towners.h"
#include	"quests.h"
#include	"doom.h"

/*-----------------------------------------------------------------------**
** externs
**-----------------------------------------------------------------------*/
BOOL IsTracking();
void savecrsr_reset();


/*-----------------------------------------------------------------------**
** Global variables
**-----------------------------------------------------------------------*/
int curs;
int cursW, cursH;
int icursW, icursH;
int icursW28, icursH28;
int cursmx, cursmy;
int cursmonst;
char cursobj;
char cursitem;
char cursinvitem;
char cursplr;
BYTE *pCursCels;
BYTE *pCursCels2;
static int oldcursmonst;


const int CursorWidth [ITEM_LAST_ID+12] = {
	// Null, Glove Pointer, Identify, Repair, Recharge, Disarm, Oil, Telekenesis, Resurrect, Target, Heal Other, Watch
	0, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 23,
	// Inv 1x1
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	// Inv 1x2
	28, 28, 28, 28, 28, 28,
	// Inv 1x3
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28,
	// Inv 2x2
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56,
	// Inv 2x3
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56,
	
	// New 1x1
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	
	// New 2x2
	56, 56,
	
	// New 1x3
	28, 28, 28,
	
	// New 2x3
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 56, 56,
	
	};

const int CursorHeight[ITEM_LAST_ID+12] = {
	// Null, Glove Pointer, Identify, Repair, Recharge, Disarm, Oil, Telekenesis, Resurrect, Target, Heal Other, Watch
	0, 29, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35,
	// Inv 1x1
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	// Inv 1x2
	56, 56, 56, 56, 56, 56,
	// Inv 1x3
	84, 84, 84, 84,	84, 84, 84, 84, 84, 84,
	84, 84, 84, 84, 84, 84, 84, 84, 84,
	// Inv 2x2
	56, 56,	56, 56, 56, 56, 56, 56, 56, 56,
	56, 56,	56, 56, 56, 56, 56, 56, 56, 56,
	56, 56,	56, 56, 56,
	// Inv 2x3
	84, 84, 84, 84, 84, 84, 84, 84, 84, 84,
	84, 84, 84, 84, 84, 84, 84, 84, 84, 84,
	84, 84, 84, 84, 84, 84, 84, 84, 84, 84,
	84, 84, 84, 84, 84, 84, 84, 84, 84, 84,
	84, 84, 84, 84, 84, 84, 84, 84, 84, 84,
	84, 84, 84, 84, 84, 84, 84, 84, 84, 84,
	84, 84, 84, 84, 84, 84, 84, 84,
	
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	
	56, 56,
	
	84, 84, 84,
	
	84, 84, 84, 84,	84, 84, 84, 84, 84, 84,
	84, 84, 84, 84,	84, 84, 84, 84, 84, 84,
	84, 84, 84, 84,
	
	};


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitCursor() {
	app_assert(! pCursCels);
	pCursCels = LoadFileInMemSig("Data\\Inv\\Objcurs.CEL",NULL,'CRSR');
	pCursCels2 = LoadFileInMemSig("Data\\Inv\\Objcurs2.CEL",NULL,'CRSR');
	savecrsr_reset();
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FreeCursor() {
	DiabloFreePtr(pCursCels);
	DiabloFreePtr(pCursCels2);
	savecrsr_reset();
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetICursor(int i) {
	icursW = CursorWidth[i];
	icursH = CursorHeight[i];
	icursW28 = icursW / 28;
	icursH28 = icursH / 28;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetCursor(int i) {
	curs = i;
	cursW = CursorWidth[curs];
	cursH = CursorHeight[curs];
	SetICursor(i);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void NewCursor(int i) {
	SetCursor(i);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitLevelCursor() {
	SetCursor(GLOVE_CURS);
	cursmx = ViewX;
	cursmy = ViewY;
	oldcursmonst = -1;
	cursmonst = -1;
	cursobj = -1;
	cursitem = -1;
	cursplr = -1;
	savecrsr_reset();
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CheckTown() {
	for (int i = 0; i < nummissiles; i++) {
		int mx = missileactive[i];
		if (missile[mx]._mitype == MIT_TOWN) {
			if (((cursmx == missile[mx]._mix - 1) && (cursmy == missile[mx]._miy)) ||
				((cursmx == missile[mx]._mix) && (cursmy == missile[mx]._miy - 1)) ||
				((cursmx == missile[mx]._mix - 1) && (cursmy == missile[mx]._miy - 1)) ||
				((cursmx == missile[mx]._mix - 2) && (cursmy == missile[mx]._miy - 1)) ||
				((cursmx == missile[mx]._mix - 2) && (cursmy == missile[mx]._miy - 2)) ||
				((cursmx == missile[mx]._mix - 1) && (cursmy == missile[mx]._miy - 2)) ||
				((cursmx == missile[mx]._mix) && (cursmy == missile[mx]._miy))) {
				trigflag = TRUE;
				ClearPanel();
				strcpy(infostr, "Town Portal");
				sprintf(tempstr, "from %s", plr[missile[mx]._misource]._pName);
				AddPanelString(tempstr, TEXT_CENTER);
				cursmx = missile[mx]._mix;
				cursmy = missile[mx]._miy;
			}
		}
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckRportal() {
	for (int i = 0; i < nummissiles; i++) {
		int mx = missileactive[i];
		if (missile[mx]._mitype == MIT_RPORTAL) {
			if (((cursmx == missile[mx]._mix - 1) && (cursmy == missile[mx]._miy)) ||
				((cursmx == missile[mx]._mix) && (cursmy == missile[mx]._miy - 1)) ||
				((cursmx == missile[mx]._mix - 1) && (cursmy == missile[mx]._miy - 1)) ||
				((cursmx == missile[mx]._mix - 2) && (cursmy == missile[mx]._miy - 1)) ||
				((cursmx == missile[mx]._mix - 2) && (cursmy == missile[mx]._miy - 2)) ||
				((cursmx == missile[mx]._mix - 1) && (cursmy == missile[mx]._miy - 2)) ||
				((cursmx == missile[mx]._mix) && (cursmy == missile[mx]._miy))) {
				trigflag = TRUE;
				ClearPanel();
				strcpy(infostr, "Portal to");
				if (!(setlevel)) strcpy(tempstr, "The Unholy Altar");
				else strcpy(tempstr, "level 15");
				AddPanelString(tempstr, TEXT_CENTER);
				cursmx = missile[mx]._mix;
				cursmy = missile[mx]._miy;
			}
		}
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckCursMove() {
	int	mx,my;
	int offsetx,offsety,gridx,gridy;
	char co,ci,cp;
	int cm;
	BOOL minusy, plusx;
	BOOL lefthalf;
	int newMouseX;
	int newMouseY;

	int pvar6tmp, pvar7tmp;
	long xo, yo;

	newMouseX = MouseX;
	newMouseY = MouseY;

	// Convert screen point to map position
	if (chrflag || questlog) {
		if (newMouseX >= TOTALX/4) newMouseX -= TOTALX/4;
		else newMouseX = 0;
	}
	else if (invflag || sbookflag) {
		if (newMouseX <= TOTALX/2) newMouseX += TOTALX/4;
		else newMouseX = 0;
	}

	// if the mouse outside the gamemap area, but 
	// the mouse is in tracking mode, then pretend
	// that the mouse is actually in the gamemap
	if (newMouseY > 351 && IsTracking())
		newMouseY = 351;

	if (!svgamode)
	{
		newMouseX >>= 1;
		newMouseY >>= 1;
	}

	// Offset mouse position by the scroll offset, to align it with tile grid
	newMouseX -= ScrollInfo._sxoff;
	newMouseY -= ScrollInfo._syoff;

	// THIS IS A HACK
	// Here, we predict the next scroll increment, so the mouse position is
	// where it would be during the next frame.
	// This is a fix -- without this, the following sometimes happens:
	// The user holds down the mouse button, and the walk path alternates
	// between two different directions, because the cursor is sampled just before
	// player reaches his next square.
	xo = plr[myplr]._pVar6 >> 8;
	yo = plr[myplr]._pVar7 >> 8;
	pvar6tmp = plr[myplr]._pVar6 + plr[myplr]._pxvel;
	pvar7tmp = plr[myplr]._pVar7 + plr[myplr]._pyvel;
	xo -= (pvar6tmp >> 8);
	yo -= (pvar7tmp >> 8);
	if ((myplr == myplr) && (ScrollInfo._sdir != SCRL_NONE)) {
		newMouseX -= xo;
		newMouseY -= yo;
	}

	if (newMouseX < 0)
		newMouseX = 0;
	if (newMouseX >= TOTALX)
		newMouseX = TOTALX;
	if (newMouseY < 0)
		newMouseY = 0;
	if (newMouseY >= TOTALY)
		newMouseY = TOTALY;

	// Calculate position in square grid
	gridx = newMouseX >> 6;
	gridy = newMouseY >> 5;

	// Calculate offset in that square
	offsetx = newMouseX & 63;
	offsety = newMouseY & 31;

	// Convert from square grid to diamond grid
	mx = gridx + gridy + ViewX - (svgamode ? 10:5);
	my = gridy - gridx + ViewY;

	if (minusy = (offsety < (offsetx >> 1))) {
		my--;
	}
	if (plusx = (offsety >= (32 - (offsetx >> 1)))) {
		mx++;
	}

	if (mx < 0) mx = 0;
	if (mx >= DMAXX) mx = DMAXX - 1;
	if (my < 0) my = 0;
	if (my >= DMAXY) my = DMAXY - 1;

	lefthalf = (minusy && plusx) || ((minusy || plusx) && offsetx < 32);

	oldcursmonst = cursmonst;
	cursmonst = -1;
	cursobj = -1;
	cursitem = -1;
	if (cursinvitem != -1)
		drawsbarflag = TRUE;
	cursinvitem = -1;
	cursplr = -1;
	uitemflag = FALSE;
	panelflag = FALSE;
	trigflag = FALSE;

	if (plr[myplr]._pInvincible) return;		// Dead?

	// Skip if I have an item
	if ((curs >= ICSTART) || (spselflag)) {
		cursmx = mx;
		cursmy = my;
		return;
	}

	if (MouseY > 352) {
		CheckPanelInfo();
		return;
	}

	if (drawmapofdoom) return;

	if ((invflag) && (MouseX > 320)) {
		cursinvitem = CheckInvHLight();
		return;
	}
	if (sbookflag && (MouseX > 320))
		return;

	if ((chrflag || questlog) && (MouseX < 320)) return;

	if (leveltype != 0) {
		if (oldcursmonst != -1) {
			if (!lefthalf && (dMonster[mx+2][my+1] != 0) && (dFlags[mx+2][my+1] & BFLAG_VISIBLE)) {
				if (dMonster[mx+2][my+1] > 0) cm = dMonster[mx+2][my+1] - 1;
				else cm = -(dMonster[mx+2][my+1] + 1);
				if (cm == oldcursmonst) {
					if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_TOP)) {
						cursmx = mx + 1;
						cursmy = my + 2;
						cursmonst = cm;
					}
				}
			}
			if (lefthalf && (dMonster[mx+1][my+2] != 0) && (dFlags[mx+1][my+2] & BFLAG_VISIBLE)) {
				if (dMonster[mx+1][my+2] > 0) cm = dMonster[mx+1][my+2] - 1;
				else cm = -(dMonster[mx+1][my+2] + 1);
				if (cm == oldcursmonst) {
					if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_TOP)) {
						cursmx = mx + 1;
						cursmy = my + 2;
						cursmonst = cm;
					}
				}
			}
			if ((dMonster[mx+2][my+2] != 0) && (dFlags[mx+2][my+2] & BFLAG_VISIBLE)) {
				if (dMonster[mx+2][my+2] > 0) cm = dMonster[mx+2][my+2] - 1;
				else cm = -(dMonster[mx+2][my+2] + 1);
				if (cm == oldcursmonst) {
					if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_TOP)) {
						cursmx = mx + 2;
						cursmy = my + 2;
						cursmonst = cm;
					}
				}
			}
			if (!lefthalf && (dMonster[mx+1][my] != 0) && (dFlags[mx+1][my] & BFLAG_VISIBLE)) {
				if (dMonster[mx+1][my] > 0) cm = dMonster[mx+1][my] - 1;
				else cm = -(dMonster[mx+1][my] + 1);
				if (cm == oldcursmonst) {
					if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_MID)) {
						cursmx = mx + 1;
						cursmy = my;
						cursmonst = cm;
					}
				}
			}
			if (lefthalf && (dMonster[mx][my+1] != 0) && (dFlags[mx][my+1] & BFLAG_VISIBLE)) {
				if (dMonster[mx][my+1] > 0) cm = dMonster[mx][my+1] - 1;
				else cm = -(dMonster[mx][my+1] + 1);
				if (cm == oldcursmonst) {
					if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_MID)) {
						cursmx = mx;
						cursmy = my + 1;
						cursmonst = cm;
					}
				}
			}
			if ((dMonster[mx][my] != 0) && (dFlags[mx][my] & BFLAG_VISIBLE)) {
				if (dMonster[mx][my] > 0) cm = dMonster[mx][my] - 1;
				else cm = -(dMonster[mx][my] + 1);
				if (cm == oldcursmonst) {
					if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_FLR)) {
						cursmx = mx;
						cursmy = my;
						cursmonst = cm;
					}
				}
			}
			if ((dMonster[mx+1][my+1] != 0) && (dFlags[mx+1][my+1] & BFLAG_VISIBLE)) {
				if (dMonster[mx+1][my+1] > 0) cm = dMonster[mx+1][my+1] - 1;
				else cm = -(dMonster[mx+1][my+1] + 1);
				if (cm == oldcursmonst) {
					if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_MID)) {
						cursmx = mx + 1;
						cursmy = my + 1;
						cursmonst = cm;
					}
				}
			}
			if (cursmonst != -1) {
				if (monster[cursmonst]._mFlags & MFLAG_INVISIBLE) {
					cursmonst = -1;
					cursmx = mx;
					cursmy = my;
				}
			}
			if ((cursmonst != -1)
				&& ((monster[cursmonst]._mFlags & MFLAG_MKILLER) != 0)
				&& ((monster[cursmonst]._mFlags & MFLAG_BERSERK) == 0)
				) cursmonst = -1;
			
			if (cursmonst != -1) return;
		}
		if (!lefthalf && (dMonster[mx+2][my+1] != 0) && (dFlags[mx+2][my+1] & BFLAG_VISIBLE)) {
			if (dMonster[mx+2][my+1] > 0) cm = dMonster[mx+2][my+1] - 1;
			else cm = -(dMonster[mx+2][my+1] + 1);
			if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_TOP)) {
				cursmx = mx + 2;
				cursmy = my + 1;
				cursmonst = cm;
			}
		}
		if (lefthalf && (dMonster[mx+1][my+2] != 0) && (dFlags[mx+1][my+2] & BFLAG_VISIBLE)) {
			if (dMonster[mx+1][my+2] > 0) cm = dMonster[mx+1][my+2] - 1;
			else cm = -(dMonster[mx+1][my+2] + 1);
			if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_TOP)) {
				cursmx = mx + 1;
				cursmy = my + 2;
				cursmonst = cm;
			}
		}
		if ((dMonster[mx+2][my+2] != 0) && (dFlags[mx+2][my+2] & BFLAG_VISIBLE)) {
			if (dMonster[mx+2][my+2] > 0) cm = dMonster[mx+2][my+2] - 1;
			else cm = -(dMonster[mx+2][my+2] + 1);
			if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_TOP)) {
				cursmx = mx + 2;
				cursmy = my + 2;
				cursmonst = cm;
			}
		}
		if (!lefthalf && (dMonster[mx+1][my] != 0) && (dFlags[mx+1][my] & BFLAG_VISIBLE)) {
			if (dMonster[mx+1][my] > 0) cm = dMonster[mx+1][my] - 1;
			else cm = -(dMonster[mx+1][my] + 1);
			if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_MID)) {
				cursmx = mx + 1;
				cursmy = my;
				cursmonst = cm;
			}
		}
		if (lefthalf && (dMonster[mx][my+1] != 0) && (dFlags[mx][my+1] & BFLAG_VISIBLE)) {
			if (dMonster[mx][my+1] > 0) cm = dMonster[mx][my+1] - 1;
			else cm = -(dMonster[mx][my+1] + 1);
			if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_MID)) {
				cursmx = mx;
				cursmy = my + 1;
				cursmonst = cm;
			}
		}
		if ((dMonster[mx][my] != 0) && (dFlags[mx][my] & BFLAG_VISIBLE)) {
			if (dMonster[mx][my] > 0) cm = dMonster[mx][my] - 1;
			else cm = -(dMonster[mx][my] + 1);
			if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_FLR)) {
				cursmx = mx;
				cursmy = my;
				cursmonst = cm;
			}
		}
		if ((dMonster[mx+1][my+1] != 0) && (dFlags[mx+1][my+1] & BFLAG_VISIBLE)) {
			if (dMonster[mx+1][my+1] > 0) cm = dMonster[mx+1][my+1] - 1;
			else cm = -(dMonster[mx+1][my+1] + 1);
			if (((monster[cm]._mhitpoints >> HP_SHIFT) > 0) && (monster[cm].MData->mSelFlag & MSEL_MID)) {
				cursmx = mx + 1;
				cursmy = my + 1;
				cursmonst = cm;
			}
		}
		if (cursmonst != -1) {
				if (monster[cursmonst]._mFlags & MFLAG_INVISIBLE) {
				cursmonst = -1;
				cursmx = mx;
				cursmy = my;
			}
		}
		if ((cursmonst != -1) 
			&& ((monster[cursmonst]._mFlags & MFLAG_MKILLER) != 0)
			&& ((monster[cursmonst]._mFlags & MFLAG_BERSERK) == 0)
			) cursmonst = -1;
	} else {
		if (!lefthalf && (dMonster[mx+1][my] > 0)) {
			cursmonst = dMonster[mx+1][my] - 1;
			cursmx = mx + 1;
			cursmy = my;
		}
		if (lefthalf && (dMonster[mx][my+1] > 0)) {
			cursmonst = dMonster[mx][my+1] - 1;
			cursmx = mx;
			cursmy = my + 1;
		}
		if (dMonster[mx][my] > 0) {
			cursmonst = dMonster[mx][my] - 1;
			cursmx = mx;
			cursmy = my;
		}
		if (dMonster[mx+1][my+1] > 0) {
			cursmonst = dMonster[mx+1][my+1] - 1;
			cursmx = mx + 1;
			cursmy = my + 1;
		}
		if (!towner[cursmonst]._tSelFlag) cursmonst = -1;
	}

	if (cursmonst == -1) {
		if (!lefthalf && (dPlayer[mx+1][my] != 0)) {
			if (dPlayer[mx+1][my] > 0) cp = dPlayer[mx+1][my] - 1;
			else cp = -(dPlayer[mx+1][my] + 1);
			if ((cp != myplr) && (plr[cp]._pHitPoints != 0)) {
				cursmx = mx + 1;
				cursmy = my;
				cursplr = cp;
			}
		}
		if (lefthalf && (dPlayer[mx][my+1] != 0)) {
			if (dPlayer[mx][my+1] > 0) cp = dPlayer[mx][my+1] - 1;
			else cp = -(dPlayer[mx][my+1] + 1);
			if ((cp != myplr) && (plr[cp]._pHitPoints != 0)) {
				cursmx = mx;
				cursmy = my + 1;
				cursplr = cp;
			}
		}
		if (dPlayer[mx][my] != 0) {
			if (dPlayer[mx][my] > 0) cp = dPlayer[mx][my] - 1;
			else cp = -(dPlayer[mx][my] + 1);
			if (cp != myplr) {
				cursmx = mx;
				cursmy = my;
				cursplr = cp;
			}
		}
		if (dFlags[mx][my] & BFLAG_DEADPLR) {
			for (int i = 0; i < MAX_PLRS; i++) {
				if ((plr[i]._px == mx) && (plr[i]._py == my) && (i != myplr)) {
					cursmx = mx;
					cursmy = my;
					cursplr = i;
				}
			}
		}
		
		if (curs == RESURRECT_CURS) {
			for (int j = -1; j < 2; j++) {
				for (int k = -1; k < 2; k++) {
					if (dFlags[mx+j][my+k] & BFLAG_DEADPLR) {
						for (int i = 0; i < MAX_PLRS; i++) {
							if ((plr[i]._px == mx+j) && (plr[i]._py == my+k) && (i != myplr)) {
								cursmx = mx+j;
								cursmy = my+k;
								cursplr = i;
							}
						}
					}
				}
			}
		}

		if (dPlayer[mx+1][my+1] != 0) {
			if (dPlayer[mx+1][my+1] > 0) cp = dPlayer[mx+1][my+1] - 1;
			else cp = -(dPlayer[mx+1][my+1] + 1);
			if ((cp != myplr) && (plr[cp]._pHitPoints != 0)) {
				cursmx = mx + 1;
				cursmy = my + 1;
				cursplr = cp;
			}
		}
	}

	if ((cursmonst == -1) && (cursplr == -1)) {
		if (!lefthalf && (dObject[mx+1][my] != 0)) {
			if (dObject[mx+1][my] > 0) co = dObject[mx+1][my] - 1;
			else co = -(dObject[mx+1][my] + 1);
			if (object[co]._oSelFlag >= OSEL_TOP) {
				cursmx = mx + 1;
				cursmy = my;
				cursobj = co;
			}
		}
		if (lefthalf && (dObject[mx][my+1] != 0)) {
			if (dObject[mx][my+1] > 0) co = dObject[mx][my+1] - 1;
			else co = -(dObject[mx][my+1] + 1);
			if (object[co]._oSelFlag >= OSEL_TOP) {
				cursmx = mx;
				cursmy = my + 1;
				cursobj = co;
			}
		}
		if (dObject[mx][my] != 0) {
			if (dObject[mx][my] > 0) co = dObject[mx][my] - 1;
			else co = -(dObject[mx][my] + 1);
			if ((object[co]._oSelFlag == OSEL_FLR) || (object[co]._oSelFlag == OSEL_ALL)) {
				cursmx = mx;
				cursmy = my;
				cursobj = co;
			}
		}
		if (dObject[mx+1][my+1] != 0) {
			if (dObject[mx+1][my+1] > 0) co = dObject[mx+1][my+1] - 1;
			else co = -(dObject[mx+1][my+1] + 1);
			if (object[co]._oSelFlag >= OSEL_TOP) {
				cursmx = mx + 1;
				cursmy = my + 1;
				cursobj = co;
			}
		}
	}

	if ((cursplr == -1) && (cursobj == -1) && (cursmonst == -1)) {
		if (!lefthalf && (dItem[mx+1][my] > 0)) {
			ci = dItem[mx+1][my] - 1;
			if (item[ci]._iSelFlag >= ISEL_TOP) {
				cursmx = mx + 1;
				cursmy = my;
				cursitem = ci;
			}
		}
		if (lefthalf && (dItem[mx][my+1] > 0)) {
			ci = dItem[mx][my+1] - 1;
			if (item[ci]._iSelFlag >= ISEL_TOP) {
				cursmx = mx;
				cursmy = my + 1;
				cursitem = ci;
			}
		}
		if (dItem[mx][my] > 0) {
			ci = dItem[mx][my] - 1;
			if ((item[ci]._iSelFlag == ISEL_FLR) || (item[ci]._iSelFlag == ISEL_ALL)) {
				cursmx = mx;
				cursmy = my;
				cursitem = ci;
			}
		}
		if (dItem[mx+1][my+1] > 0) {
			ci = dItem[mx+1][my+1] - 1;
			if (item[ci]._iSelFlag >= ISEL_TOP) {
				cursmx = mx + 1;
				cursmy = my + 1;
				cursitem = ci;
			}
		}
		if (cursitem == -1) {
			cursmx = mx;
			cursmy = my;
			CheckTrigForce();
			CheckTown();
			CheckRportal();
		}
	}
	if (curs == IDENTIFY_CURS) {
		cursobj = -1;
		cursmonst = -1;
		cursitem = -1;
		cursmx = mx;
		cursmy = my;
	}

	if ((cursmonst != -1)
		&& ((monster[cursmonst]._mFlags & MFLAG_MKILLER) != 0)
		&& ((monster[cursmonst]._mFlags & MFLAG_BERSERK) == 0)
		 ) cursmonst = -1;
}
