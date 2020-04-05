/*-----------------------------------------------------------------------**
** 								Diablo
**
**							Scrolling file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SCROLLRT.CPP 3     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
** CheckForScroll
** DrawAndBlit
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"storm.h"
#include	"sound.h"
#include	"engine.h"
#include	"scrollrt.h"
#include	"scrlasm.h"
#include	"gendung.h"
#include	"debug.h"
#include	"inv.h"
#include	"multi.h"
#include	"lighting.h"
#include	"control.h"
#include	"gamemenu.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"monstint.h"
#include	"dead.h"
#include	"objects.h"
#include	"missiles.h"
#include	"misdat.h"
#include	"spells.h"
#include	"cursor.h"
#include	"quests.h"
#include	"town.h"
#include	"minitext.h"
#include	"automap.h"
#include	"help.h"
#include	"error.h"
#include	"doom.h"


/*-----------------------------------------------------------------------**
** debugging
**-----------------------------------------------------------------------*/
#define LOCK_WAIT	1	// 1 in final
#define LOCK_SLEEP	1	// 1 in final
#define MAX_FRAMES	50	// arbitrary limit to check for bogus numbers of frames
const char sgszUnknownAction[] = "unknown action";
#define GRACEFUL_EXIT		// this needs to be undefined later to get assert turned back on. JKE

/*-----------------------------------------------------------------------**
** Registration info
**-----------------------------------------------------------------------*/
#include "regconst.h"
char sgszRegSig6[REG_LEN] = "REGISTRATION_BLOCK";


/*-----------------------------------------------------------------------**
** Function stubs
**-----------------------------------------------------------------------*/
#if RLE_DRAW
void DrawUnit(long xp,long yp,BYTE *pCelBuff,long nCel,long nCelW,long ostart,long oend);
void DrawUnitOutline(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend);
void DrawInfraUnit(long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend, char loff);
void DrawLitUnit(long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend);

void DrawUnitClipped(long xp,long yp,BYTE *pCelBuff,long nCel,long nCelW,long ostart,long oend);
void DrawUnitOutlineClipped(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend);
void DrawInfraUnitClipped(long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend, char loff);
void DrawLitUnitClipped(long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend);
#endif

void DrawHTLXsub (BYTE *pTo, int sx, int sy, int xp, int yp, BOOL chflag);
void DrawHTLXsub2 (BYTE *pTo, int sx, int sy, int sv, int sv2, int xp, int yp, BOOL chflag);
void DrawHTLXsub3 (BYTE *pTo, int sx, int sy, int ev, int ev2, int xp, int yp, BOOL chflag);
void plrmsg_draw();

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#define MOUTC	233
#define IOUTC	181		//130
#define OOUTC	194		//148
#define POUTC	165

extern "C" {
	BOOL nTrans;
	long nLVal;
	int gnPieceNum;
	char gbPartialTrans;
	DWORD gdwPNum;
	long glClipY;
}
int gnMI;
long nBuffWTbl[1024];


// frame counter
#ifndef NDEBUG
static DWORD sgnLastFrame;
static BYTE sgfFrameCounterEnabled = FALSE;
#endif

bool HighLightAllItems = false;

//******************************************************************
// savecrsr variables
//******************************************************************
static BYTE sgSaveBack[8*1024];
static DWORD sgdwCursX,sgdwCursY,sgdwCursWdt,sgdwCursHgt;
static DWORD sgdwOldX,sgdwOldY,sgdwOldWdt,sgdwOldHgt;


//******************************************************************
//******************************************************************
void savecrsr_reset() {
	// inhibit cursor redraw
	sgdwCursWdt = 0;
	sgdwOldWdt = 0;
}


//******************************************************************
//******************************************************************
static void savecrsr_hide() {
	if (! sgdwCursWdt) return;

	// copy from save cursor buffer to draw buffer
	app_assert(gpBuffer);
	BYTE * pbDst = gpBuffer + (sgdwCursY + 160) * 768 + sgdwCursX + 64;
	const BYTE * pbSrc = sgSaveBack;
	for (DWORD hgt = sgdwCursHgt; hgt--; ) {
		memcpy(pbDst,pbSrc,sgdwCursWdt);
		pbSrc += sgdwCursWdt;
		pbDst += 768;
	}

	sgdwOldX = sgdwCursX;
	sgdwOldY = sgdwCursY;
	sgdwOldWdt = sgdwCursWdt;
	sgdwOldHgt = sgdwCursHgt;
	sgdwCursWdt = 0;
}


//******************************************************************
//******************************************************************
static void savecrsr_show() {
	// make sure cursor was already erased before
	app_assert(! sgdwCursWdt);

	if (curs <= 0) return;
	if (! cursW || ! cursH) return;

	// add one pixel in each direction for outline drawn around cursor
	int cursX = MouseX - 1;
	if (cursX < 0) cursX = 0;
	else if (cursX > 639) return;
	int cursY = MouseY - 1;
	if (cursY < 0) cursY = 0;
	else if (cursY > 479) return;

	// calculate area behind cursor -- X/WDT rounded to DWORD boundary/size
	sgdwCursX = cursX;							// get X1 coord
	sgdwCursWdt = cursX + cursW + 2 - 1;		// get X2 coord
	if (sgdwCursWdt > 639) sgdwCursWdt = 639;	// clip X
	sgdwCursX &= ~3;							// X1 % 4 == 0
	sgdwCursWdt |= 3;							// X2 % 4 == 3
	sgdwCursWdt += 1 - sgdwCursX;				// WDT = X2 - X1 + 1

	sgdwCursY = cursY;							// get X1 coord
	sgdwCursHgt = cursY + cursH + 2 - 1;		// get X2 coord
	if (sgdwCursHgt > 479) sgdwCursHgt = 479;	// clip Y
	sgdwCursHgt += 1 - sgdwCursY;				// HGT = Y2 - Y1 + 1
	app_assert(sgdwCursWdt * sgdwCursHgt < sizeof sgSaveBack);

	// copy from draw buffer to save cursor buffer
	app_assert(gpBuffer);
	const BYTE * pbSrc = gpBuffer + (sgdwCursY + 160) * 768 + sgdwCursX + 64;
	BYTE * pbDst = sgSaveBack;
	for (DWORD hgt = sgdwCursHgt; hgt--; ) {
		memcpy(pbDst,pbSrc,sgdwCursWdt);
		pbDst += sgdwCursWdt;
		pbSrc += 768;
	}

	// remove one pixel in each direction for outline drawn around cursor
	cursX += 1;
	cursY += 1;
	glClipY = (long) gpBuffer + nBuffWTbl[640] - (cursW + 2);
	if (curs >= ICSTART) {
		int oc = 197;
		if (plr[myplr].HoldItem._iMagical) oc = 181;
		if (!plr[myplr].HoldItem._iStatFlag) oc = 229;
		if (curs <= ICLAST)
		{
			COutlineSlabCel(oc, cursX+64, cursY+160+cursH-1, pCursCels, curs, cursW, 0, 8);
			if (oc != 229) CDrawSlabCel(cursX+64, cursY+160+cursH-1, pCursCels, curs, cursW, 0, 8);
			else CDrawSlabCelI(cursX+64, cursY+160+cursH-1, pCursCels, curs, cursW, 0, 8, LIGHT_INFRA);
		}
		else
		{
			COutlineSlabCel(oc, cursX+64, cursY+160+cursH-1, pCursCels2, curs - ICLAST, cursW, 0, 8);
			if (oc != 229) CDrawSlabCel(cursX+64, cursY+160+cursH-1, pCursCels2, curs - ICLAST, cursW, 0, 8);
			else CDrawSlabCelI(cursX+64, cursY+160+cursH-1, pCursCels2, curs - ICLAST, cursW, 0, 8, LIGHT_INFRA);
		}
	}
	else {
		CDrawSlabCel(cursX+64, cursY+160+cursH-1, pCursCels, curs, cursW, 0, 8);
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
void DrawMissile(int sx, int sy, int xp, int yp, int ds, int de, BOOL pre)
{
	int		i, gnMI, mx, my;

	if (dMissile[sx][sy] != -1) {
		gnMI = dMissile[sx][sy] - 1;
		if(missile[gnMI]._miPreFlag == pre) {
			mx = missile[gnMI]._mixoff + xp - missile[gnMI]._miAnimWidth2;
			my = missile[gnMI]._miyoff + yp;
			if (missile[gnMI]._miUniqTrans) {
				DrawSlabCelI(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de, LIGHT_U + missile[gnMI]._miUniqTrans - 1);
			}
			else {
				if (missile[gnMI]._miLightFlag) {
					DrawSlabCelL(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
				}
				else {
					app_assert(missile[gnMI]._miAnimData);
					DrawSlabCel(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
				}
			}
		}
	} else {
		for (i = 0; i < nummissiles; ++i) {
			gnMI = missileactive[i];
			if ((missile[gnMI]._mix == sx) && (missile[gnMI]._miy == sy)
				&& (missile[gnMI]._miPreFlag == pre) && (missile[gnMI]._miDrawFlag == TRUE))
			{
				mx = missile[gnMI]._mixoff + xp - missile[gnMI]._miAnimWidth2;
				my = missile[gnMI]._miyoff + yp;
				if (missile[gnMI]._miUniqTrans) {
					DrawSlabCelI(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de, LIGHT_U + missile[gnMI]._miUniqTrans - 1);
				}
				else {
					if (missile[gnMI]._miLightFlag) {
						DrawSlabCelL(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
					}
					else {
						app_assert(missile[gnMI]._miAnimData);
						DrawSlabCel(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
					}
				}
			}
		}
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
void CDrawMissile(int sx, int sy, int xp, int yp, int ds, int de, BOOL pre)
{
	int		i, gnMI, mx, my;

	if (dMissile[sx][sy] != -1) {
		gnMI = dMissile[sx][sy] - 1;
		if (missile[gnMI]._miPreFlag == pre) {
			mx = missile[gnMI]._mixoff + xp - missile[gnMI]._miAnimWidth2;
			my = missile[gnMI]._miyoff + yp;
			if (missile[gnMI]._miUniqTrans)
				CDrawSlabCelI(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de, LIGHT_U + missile[gnMI]._miUniqTrans - 1);
			else {
				if (missile[gnMI]._miLightFlag) CDrawSlabCelL(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
				else CDrawSlabCel(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
			}
		}
	} else {
		for (i = 0; i < nummissiles; ++i) {
			gnMI = missileactive[i];
			if ((missile[gnMI]._mix == sx) && (missile[gnMI]._miy == sy)
				&& (missile[gnMI]._miPreFlag == pre) && (missile[gnMI]._miDrawFlag == TRUE))
			{
				mx = missile[gnMI]._mixoff + xp - missile[gnMI]._miAnimWidth2;
				my = missile[gnMI]._miyoff + yp;
				if (missile[gnMI]._miUniqTrans)
					CDrawSlabCelI(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de, LIGHT_U + missile[gnMI]._miUniqTrans - 1);
				else {
					if (missile[gnMI]._miLightFlag) CDrawSlabCelL(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
					else CDrawSlabCel(mx, my, missile[gnMI]._miAnimData, missile[gnMI]._miAnimFrame, missile[gnMI]._miAnimWidth, ds, de);
				}
			}
		}
	}
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if RLE_DRAW
void DrawMissile(int sx, int sy, int xp, int yp, int ds, int de, BOOL pre)
{
	int i, mx, my;
	MissileStruct * pMiss;

	if (dMissile[sx][sy] == -1) {
		for (i = 0; i < nummissiles; ++i) {
			app_assert(missileactive[i] < MAXMISSILES && missileactive[i] >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (missileactive[i] >= MAXMISSILES || missileactive[i] < 0 )
				return;
			#endif
			// jcm.patch1.end.1/14/97
			pMiss = &missile[missileactive[i]];
			if ((pMiss->_mix != sx) || (pMiss->_miy != sy) || (pMiss->_miPreFlag != pre) || !pMiss->_miDrawFlag)
				continue;
			if (pMiss->_miAnimData == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				return;
				#else
				app_fatal("Draw Missile type %d: NULL Cel Buffer",pMiss->_mitype);
				#endif
				// jcm.patch1.end.1/14/97
			if (pMiss->_miAnimFrame < 1 || *(DWORD*)pMiss->_miAnimData > MAX_FRAMES || 
				pMiss->_miAnimFrame > *(long*)pMiss->_miAnimData) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				return;
				#else
				app_fatal(
					"Draw Missile: frame %d of %d, missile type==%d",
					pMiss->_miAnimFrame,
					*(long*)pMiss->_miAnimData,
					pMiss->_mitype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			mx = pMiss->_mixoff + xp - pMiss->_miAnimWidth2;
			my = pMiss->_miyoff + yp;
			if (pMiss->_miUniqTrans) {
				DrawInfraUnit(
					mx, my,
					pMiss->_miAnimData,
					pMiss->_miAnimFrame,
					pMiss->_miAnimWidth,
					ds, de,
					LIGHT_U + pMiss->_miUniqTrans - 1
				);
			} else if (pMiss->_miLightFlag) {
				DrawLitUnit(mx, my, pMiss->_miAnimData, pMiss->_miAnimFrame, pMiss->_miAnimWidth, ds, de);
			} else {
				DrawUnit(mx, my, pMiss->_miAnimData, pMiss->_miAnimFrame, pMiss->_miAnimWidth, ds, de);
			}
		}
		return;
	}
	pMiss = &missile[dMissile[sx][sy] - 1];
	if (pMiss->_miPreFlag != pre)
		return;
	if (pMiss->_miAnimData == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Missile 2 type %d: NULL Cel Buffer",pMiss->_mitype);
		#endif
		// jcm.patch1.end.1/14/97
	if (pMiss->_miAnimFrame < 1 || *(DWORD*)pMiss->_miAnimData > MAX_FRAMES
		|| pMiss->_miAnimFrame > *(long*)pMiss->_miAnimData) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Draw Missile 2: frame %d of %d, missile type==%d",
			pMiss->_miAnimFrame,
			*(long*)pMiss->_miAnimData,
			pMiss->_mitype
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	mx = pMiss->_mixoff + xp - pMiss->_miAnimWidth2;
	my = pMiss->_miyoff + yp;
	if (pMiss->_miUniqTrans) {
		DrawInfraUnit(
			mx, my,
			pMiss->_miAnimData,
			pMiss->_miAnimFrame,
			pMiss->_miAnimWidth,
			ds, de,
			LIGHT_U + pMiss->_miUniqTrans - 1
		);
	} else if (pMiss->_miLightFlag) {
		DrawLitUnit(mx, my, pMiss->_miAnimData, pMiss->_miAnimFrame, pMiss->_miAnimWidth, ds, de);
	} else {
		DrawUnit(mx, my, pMiss->_miAnimData, pMiss->_miAnimFrame, pMiss->_miAnimWidth, ds, de);
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if RLE_DRAW
void CDrawMissile(int sx, int sy, int xp, int yp, int ds, int de, BOOL pre)
{
	int i, mx, my;
	MissileStruct * pMiss;

	if (dMissile[sx][sy] == -1) {
		for (i = 0; i < nummissiles; ++i) {
			app_assert(missileactive[i] < MAXMISSILES && missileactive[i] >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (missileactive[i] >= MAXMISSILES || missileactive[i] < 0)
				return;
			#endif
			// jcm.patch1.end.1/14/97
			pMiss = &missile[missileactive[i]];
			if ((pMiss->_mix != sx) || (pMiss->_miy != sy) || (pMiss->_miPreFlag != pre) || !pMiss->_miDrawFlag)
				continue;
			if (pMiss->_miAnimData == NULL)
				// jcm.patch1.start.1/14/97
				//#ifdef GRACEFUL_EXIT
				return;
				//#else
				//app_fatal("Draw Missile type %d Clipped: NULL Cel Buffer",pMiss->_mitype);
				//#endif
				// jcm.patch1.end.1/14/97
			if (pMiss->_miAnimFrame < 1 || *reinterpret_cast<DWORD*>(pMiss->_miAnimData) > MAX_FRAMES
				|| pMiss->_miAnimFrame > *reinterpret_cast<long*>(pMiss->_miAnimData)) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				return;
				#else
				app_fatal(
					"Draw Clipped Missile: frame %d of %d, missile type==%d",
					pMiss->_miAnimFrame,
					*reinterpret_cast<long*>(pMiss->_miAnimData),
					pMiss->_mitype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			mx = pMiss->_mixoff + xp - pMiss->_miAnimWidth2;
			my = pMiss->_miyoff + yp;
			if (pMiss->_miUniqTrans) {
				DrawInfraUnitClipped(
					mx, my,
					pMiss->_miAnimData,
					pMiss->_miAnimFrame,
					pMiss->_miAnimWidth,
					ds, de,
					LIGHT_U + pMiss->_miUniqTrans - 1
				);
			} else if (pMiss->_miLightFlag) {
				DrawLitUnitClipped(mx,my,pMiss->_miAnimData,pMiss->_miAnimFrame,pMiss->_miAnimWidth,ds,de);
			} else {
				DrawUnitClipped(mx,my,pMiss->_miAnimData,pMiss->_miAnimFrame,pMiss->_miAnimWidth,ds,de);
			}
		}
		return;
	}
	pMiss = &missile[dMissile[sx][sy] - 1];
	if (pMiss->_miPreFlag != pre)
		return;
	if (pMiss->_miAnimData == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Missile 2 type %d Clipped: NULL Cel Buffer",pMiss->_mitype);
		#endif
		// jcm.patch1.end.1/14/97
	if (pMiss->_miAnimFrame < 1 || *(DWORD*)pMiss->_miAnimData > MAX_FRAMES
		|| pMiss->_miAnimFrame > *(long*)pMiss->_miAnimData) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Draw Clipped Missile 2: frame %d of %d, missile type==%d",
			pMiss->_miAnimFrame,
			*reinterpret_cast<long*>(pMiss->_miAnimData),
			pMiss->_mitype
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	mx = pMiss->_mixoff + xp - pMiss->_miAnimWidth2;
	my = pMiss->_miyoff + yp;
	if (pMiss->_miUniqTrans) {
		DrawInfraUnitClipped(
			mx, my,
			pMiss->_miAnimData,
			pMiss->_miAnimFrame,
			pMiss->_miAnimWidth,
			ds, de,
			LIGHT_U + pMiss->_miUniqTrans - 1
		);
	} else if (pMiss->_miLightFlag) {
		DrawLitUnitClipped(mx,my,pMiss->_miAnimData,pMiss->_miAnimFrame,pMiss->_miAnimWidth,ds,de);
	} else {
		DrawUnitClipped(mx,my,pMiss->_miAnimData,pMiss->_miAnimFrame,pMiss->_miAnimWidth,ds,de);
	}
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
static void DrawMSlabCelL (int sx, int sy, int xp, int yp, int gnMI, int ostart, int oend)
{
	char	dinfra;

	if (!(dFlags[sx][sy] & BFLAG_VISIBLE))
		DrawSlabCelI(xp, yp, monster[gnMI]._mAnimData, monster[gnMI]._mAnimFrame, monster[gnMI].MType->mAnimWidth, ostart, oend, LIGHT_INFRA);
	else {
		dinfra = LIGHT_NORM;
		if (monster[gnMI]._uniqtype != 0) dinfra = LIGHT_U + monster[gnMI]._uniqtrans;
		if (monster[gnMI]._mmode == MM_STONE) dinfra = LIGHT_STONE;
		if ((plr[myplr]._pInfraFlag) && (nLVal > 8)) dinfra = LIGHT_INFRA;
		if (dinfra != LIGHT_NORM)
			DrawSlabCelI(xp, yp, monster[gnMI]._mAnimData, monster[gnMI]._mAnimFrame, monster[gnMI].MType->mAnimWidth, ostart, oend, dinfra);
		else
			DrawSlabCelL(xp, yp, monster[gnMI]._mAnimData, monster[gnMI]._mAnimFrame, monster[gnMI].MType->mAnimWidth, ostart, oend);
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
static void CDrawMSlabCelL (int sx, int sy, int xp, int yp, int gnMI, int ostart, int oend)
{
	char	dinfra;

	if (!(dFlags[sx][sy] & BFLAG_VISIBLE))
		CDrawSlabCelI(xp, yp, monster[gnMI]._mAnimData, monster[gnMI]._mAnimFrame, monster[gnMI].MType->mAnimWidth, ostart, oend, LIGHT_INFRA);
	else {
		dinfra = LIGHT_NORM;
		if (monster[gnMI]._uniqtype != 0) dinfra = LIGHT_U + monster[gnMI]._uniqtrans;
		if (monster[gnMI]._mmode == MM_STONE) dinfra = LIGHT_STONE;
		if ((plr[myplr]._pInfraFlag) && (nLVal > 8)) dinfra = LIGHT_INFRA;
		if (dinfra != LIGHT_NORM)
			CDrawSlabCelI(xp, yp, monster[gnMI]._mAnimData, monster[gnMI]._mAnimFrame, monster[gnMI].MType->mAnimWidth, ostart, oend, dinfra);
		else
			CDrawSlabCelL(xp, yp, monster[gnMI]._mAnimData, monster[gnMI]._mAnimFrame, monster[gnMI].MType->mAnimWidth, ostart, oend);
	}
}
#endif


//***************************************************************************
//***************************************************************************
	// These strings should match the MM_xxx constants defined in monstint.h
	const char * sgszMonsterAction[] = {
		"standing",
		"walking (1)",
		"walking (2)",
		"walking (3)",
		"attacking",
		"getting hit",
		"dying",
		"attacking (special)",
		"fading in",
		"fading out",
		"attacking (ranged)",
		"standing (special)",
		"attacking (special ranged)",
		"delaying",
		"charging",
		"stoned",
		"healing",
		"talking"
	};
inline const char * MonsterActionString(DWORD dwAction) {
	if (dwAction <= MM_TALK)
		return sgszMonsterAction[dwAction];
	return sgszUnknownAction;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if RLE_DRAW
static void DrawMSlabCelL (int sx, int sy, int xp, int yp, int gnMI, int ostart, int oend)
{
	if ((DWORD)gnMI >= MAXMONSTERS)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Monster: tried to draw illegal monster %d",gnMI);
		#endif
		// jcm.patch1.end.1/14/97
	if (monster[gnMI]._mAnimData == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Monster \"%s\": NULL Cel Buffer",monster[gnMI].mName);
		#endif
		// jcm.patch1.end.1/14/97
	if (monster[gnMI]._mAnimFrame < 1 || *reinterpret_cast<DWORD*>(monster[gnMI]._mAnimData) > MAX_FRAMES
		|| monster[gnMI]._mAnimFrame > *reinterpret_cast<long*>(monster[gnMI]._mAnimData)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Draw Monster \"%s\" %s: facing %d, frame %d of %d",
			monster[gnMI].mName,
			MonsterActionString(monster[gnMI]._mmode),
			monster[gnMI]._mdir,
			monster[gnMI]._mAnimFrame,
			*reinterpret_cast<long*>(monster[gnMI]._mAnimData)
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (!(dFlags[sx][sy] & BFLAG_VISIBLE)) {
		DrawInfraUnit(
			xp, yp,
			monster[gnMI]._mAnimData,
			monster[gnMI]._mAnimFrame,
			monster[gnMI].MType->mAnimWidth,
			ostart,
			oend,
			LIGHT_INFRA
		);
		return;
	}
	char dinfra = LIGHT_NORM;
	if (monster[gnMI]._uniqtype)
		dinfra = LIGHT_U + monster[gnMI]._uniqtrans;
	if (monster[gnMI]._mmode == MM_STONE)
		dinfra = LIGHT_STONE;
	if (plr[myplr]._pInfraFlag && (nLVal > 8))
		dinfra = LIGHT_INFRA;
	if (dinfra != LIGHT_NORM)
		DrawInfraUnit(
			xp, yp,
			monster[gnMI]._mAnimData,
			monster[gnMI]._mAnimFrame,
			monster[gnMI].MType->mAnimWidth,
			ostart,
			oend,
			dinfra
		);
	else
		DrawLitUnit(
			xp, yp,
			monster[gnMI]._mAnimData,
			monster[gnMI]._mAnimFrame,
			monster[gnMI].MType->mAnimWidth,
			ostart,
			oend
		);
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if RLE_DRAW
static void CDrawMSlabCelL (int sx, int sy, int xp, int yp, int gnMI, int ostart, int oend)
{
	if ((DWORD)gnMI >= MAXMONSTERS)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Monster Clipped: tried to draw illegal monster %d",gnMI);
		#endif
		// jcm.patch1.end.1/14/97
	if (monster[gnMI]._mAnimData == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Monster \"%s\" Clipped: NULL Cel Buffer",monster[gnMI].mName);
		#endif
		// jcm.patch1.end.1/14/97
	if (monster[gnMI]._mAnimFrame < 1 || *reinterpret_cast<DWORD*>(monster[gnMI]._mAnimData) > MAX_FRAMES
		|| monster[gnMI]._mAnimFrame > *reinterpret_cast<long*>(monster[gnMI]._mAnimData)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Draw Monster \"%s\" %s Clipped: facing %d, frame %d of %d",
			monster[gnMI].mName,
			MonsterActionString(monster[gnMI]._mmode),
			monster[gnMI]._mdir,
			monster[gnMI]._mAnimFrame,
			*reinterpret_cast<long*>(monster[gnMI]._mAnimData)
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (!(dFlags[sx][sy] & BFLAG_VISIBLE)) {
		DrawInfraUnitClipped(
			xp, yp,
			monster[gnMI]._mAnimData,
			monster[gnMI]._mAnimFrame,
			monster[gnMI].MType->mAnimWidth,
			ostart,
			oend,
			LIGHT_INFRA
		);
		return;
	}
	char dinfra = LIGHT_NORM;
	if (monster[gnMI]._uniqtype)
		dinfra = LIGHT_U + monster[gnMI]._uniqtrans;
	if (monster[gnMI]._mmode == MM_STONE)
		dinfra = LIGHT_STONE;
	if (plr[myplr]._pInfraFlag && (nLVal > 8))
		dinfra = LIGHT_INFRA;
	if (dinfra != LIGHT_NORM)
		DrawInfraUnitClipped(
			xp, yp,
			monster[gnMI]._mAnimData,
			monster[gnMI]._mAnimFrame,
			monster[gnMI].MType->mAnimWidth,
			ostart,
			oend,
			dinfra
		);
	else
		DrawLitUnitClipped(
			xp, yp,
			monster[gnMI]._mAnimData,
			monster[gnMI]._mAnimFrame,
			monster[gnMI].MType->mAnimWidth,
			ostart,
			oend
		);
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
static void DrawPSlabCelL (int gnPlayer, int sx, int sy, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	if (dFlags[sx][sy] & BFLAG_VISIBLE || plr[myplr]._pInfraFlag) {
		if (gnPlayer == cursplr) {
			OutlineSlabCel(
				POUTC,
				xp, yp,
				pCelBuff, nCel,	nCelW,
				ostart,	oend
			);
		}

		if (gnPlayer == myplr) {
			app_assert(pCelBuff);
			DrawSlabCel(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
		}
		else if (!(dFlags[sx][sy] & BFLAG_VISIBLE)
				 || plr[myplr]._pInfraFlag && (nLVal > 8)) {
			DrawSlabCelI(xp, yp, pCelBuff, nCel, nCelW, ostart, oend, LIGHT_INFRA);
		}
		else {
			// Draw other players slightly brighter than monsters
			int templval = nLVal;
			if (nLVal >= 5)
				nLVal -= 5;
			else
				nLVal = 0;
			DrawSlabCelL(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
			nLVal = templval;
		}
	}
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
static void CDrawPSlabCelL (int gnPlayer, int sx, int sy, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	if (dFlags[sx][sy] & BFLAG_VISIBLE || plr[myplr]._pInfraFlag
		|| (!setlevel && currlevel == 0)) {
		if (gnPlayer == cursplr)
			COutlineSlabCel(
				POUTC,
				xp, yp,
				pCelBuff, nCel,	nCelW,
				ostart,	oend
			);
		if (gnPlayer == myplr)
			CDrawSlabCel(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
		else if (!(dFlags[sx][sy] & BFLAG_VISIBLE)
				 || plr[myplr]._pInfraFlag && (nLVal > 8))
			CDrawSlabCelI(xp, yp, pCelBuff, nCel, nCelW, ostart, oend, LIGHT_INFRA);
		else 
		{
			// Draw other players slightly brighter than monsters
			int templval = nLVal;
			if (nLVal >= 5)
				nLVal -= 5;
			else
				nLVal = 0;
			CDrawSlabCelL(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
			nLVal = templval;
		}
	}
}
#endif


//***************************************************************************
//***************************************************************************
	// These strings should match the PM_xxx constants defined in player.h
	const char * sgszPlayerAction[] = {
		"standing",
		"walking (1)",
		"walking (2)",
		"walking (3)",
		"attacking (melee)",
		"attacking (ranged)",
		"blocking",
		"getting hit",
		"dying",
		"casting a spell",
		"changing levels",
		"quitting"
	};
inline const char * PlayerActionString(DWORD dwAction) {
	if (dwAction <= PM_QUIT)
		return sgszPlayerAction[dwAction];
	return sgszUnknownAction;
}



/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if RLE_DRAW
static void DrawPSlabCelL (
	int nPlayer, int sx, int sy, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	if (!(dFlags[sx][sy] & BFLAG_VISIBLE) && !plr[myplr]._pInfraFlag && (setlevel || currlevel))
		return;
	if (pCelBuff == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Drawing player %d \"%s\": NULL Cel Buffer",nPlayer,plr[nPlayer]._pName);
		#endif
		// jcm.patch1.end.1/14/97
	if (nCel < 1 || *(DWORD*)pCelBuff > MAX_FRAMES || nCel > *reinterpret_cast<long*>(pCelBuff)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Drawing player %d \"%s\" %s: facing %d, frame %d of %d",
			nPlayer,
			plr[nPlayer]._pName,
			PlayerActionString(plr[nPlayer]._pmode),
			plr[nPlayer]._pdir,
			nCel,
			*reinterpret_cast<long*>(pCelBuff)
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (nPlayer == cursplr)
		DrawUnitOutline(POUTC,xp,yp,pCelBuff,nCel,nCelW,ostart,oend);

	if (nPlayer == myplr) {
		DrawUnit(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
	}
	else if (!(dFlags[sx][sy] & BFLAG_VISIBLE)
			 || plr[myplr]._pInfraFlag && (nLVal > 8)) {
		DrawInfraUnit(xp, yp, pCelBuff, nCel, nCelW, ostart, oend, LIGHT_INFRA);
	}
	else {
		// Draw other players slightly brighter than monsters
		int templval = nLVal;
		nLVal = (nLVal < 5) ? 0 : nLVal - 5;
		DrawLitUnit(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
		nLVal = templval;
	}
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if RLE_DRAW
static void CDrawPSlabCelL (
	int nPlayer, int sx, int sy, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	if (!(dFlags[sx][sy] & BFLAG_VISIBLE) && !plr[myplr]._pInfraFlag)
		return;
	if (pCelBuff == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Drawing player %d \"%s\" clipped: NULL Cel Buffer",nPlayer,plr[nPlayer]._pName);
		#endif
		// jcm.patch1.end.1/14/97
	if (nCel < 1 || *(DWORD*)pCelBuff > MAX_FRAMES || nCel > *reinterpret_cast<long*>(pCelBuff)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Drawing player %d \"%s\" %s clipped: facing %d, frame %d of %d",
			nPlayer,
			plr[nPlayer]._pName,
			PlayerActionString(plr[nPlayer]._pmode),
			plr[nPlayer]._pdir,
			nCel,
			*reinterpret_cast<long*>(pCelBuff)
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (nPlayer == cursplr)
		DrawUnitOutlineClipped(POUTC,xp,yp,pCelBuff,nCel,nCelW,ostart,oend);
	if (nPlayer == myplr) {
		DrawUnitClipped(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
	} else if (!(dFlags[sx][sy] & BFLAG_VISIBLE)
			 || plr[myplr]._pInfraFlag && (nLVal > 8)) {
		DrawInfraUnitClipped(xp, yp, pCelBuff, nCel, nCelW, ostart, oend, LIGHT_INFRA);
	} else {
		// Draw other players slightly brighter than monsters
		int templval = nLVal;
		nLVal = (nLVal < 5) ? 0 : nLVal - 5;
		DrawLitUnitClipped(xp, yp, pCelBuff, nCel, nCelW, ostart, oend);
		nLVal = templval;
	}
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawDeadPlr(int sx, int sy, int xp, int yp, int ostart, int oend, BOOL clip)
{
	int pnum;
	static void (*PlayerDraw)(int, int, int, long, long, BYTE *, long, long, long, long);
	PlayerStruct * pPlayer;

	PlayerDraw = clip ? CDrawPSlabCelL : DrawPSlabCelL;

	dFlags[sx][sy] &= ~BFLAG_DEADPLR;
	for (pnum = 0; pnum < MAX_PLRS; ++pnum)
	{
		pPlayer = &plr[pnum];
		if (!pPlayer->plractive || pPlayer->_pHitPoints || pPlayer->plrlevel != currlevel
			|| pPlayer->_px != sx || pPlayer->_py != sy)
			continue;
		if (pPlayer->_pAnimData == NULL)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			return;
			#else
			app_fatal("Drawing dead player %d \"%s\": NULL Cel Buffer", pnum, pPlayer->_pName);
			#endif
			// jcm.patch1.end.1/14/97
		if (pPlayer->_pAnimFrame < 1 || *reinterpret_cast<DWORD*>(pPlayer->_pAnimData) > MAX_FRAMES
			|| pPlayer->_pAnimFrame > *reinterpret_cast<long*>(pPlayer->_pAnimData)) {
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			return;
			#else
			app_fatal(
				"Drawing dead player %d \"%s\": facing %d, frame %d of %d",
				pnum,
				pPlayer->_pName,
				pPlayer->_pdir,
				pPlayer->_pAnimFrame,
				*reinterpret_cast<long*>(pPlayer->_pAnimData)
			);
			#endif
			// jcm.patch1.end.1/14/97
		}
		dFlags[sx][sy] |= BFLAG_DEADPLR;
		PlayerDraw(pnum, sx, sy,
					pPlayer->_pxoff + xp - pPlayer->_pAnimWidth2,
					pPlayer->_pyoff + yp,
					pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth,
					ostart, oend);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawObjCel(int sx, int sy, int xp, int yp, BOOL pre, int ostart, int oend)
{
	char bv;
	int pxp, pyp;
	int odx, ody;

	if (dObject[sx][sy] > 0) {
		bv = dObject[sx][sy] - 1;
		if (object[bv]._oPreFlag != pre) return;
		pxp = xp - object[bv]._oAnimWidth2;
		pyp = yp;
	} else {
		bv = -(dObject[sx][sy]+1);
		if (object[bv]._oPreFlag != pre) return;
		odx = (object[bv]._ox - sx);
		ody = (object[bv]._oy - sy);
		pxp = xp - object[bv]._oAnimWidth2 + (odx << 5) - (ody << 5);
		pyp = yp + (odx << 4) + (ody << 4);
		ostart = 0;
		oend = 8;
	}
	app_assert(bv < MAXOBJECTS && bv >= 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (bv >= MAXOBJECTS || bv < 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	if (object[bv]._oAnimData == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Object type %d: NULL Cel Buffer",object[bv]._otype);
		#endif
		// jcm.patch1.end.1/14/97
	if (object[bv]._oAnimFrame < 1 || *reinterpret_cast<DWORD*>(object[bv]._oAnimData) > MAX_FRAMES
		|| object[bv]._oAnimFrame > *reinterpret_cast<long*>(object[bv]._oAnimData)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Draw Object: frame %d of %d, object type==%d",
			object[bv]._oAnimFrame,
			*reinterpret_cast<long*>(object[bv]._oAnimData),
			object[bv]._otype
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bv == cursobj) OutlineSlabCel(OOUTC, pxp, pyp, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, ostart, oend);
	if (object[bv]._oLight) {
		DrawSlabCelL(pxp, pyp, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, ostart, oend);
	}
	else {
		app_assert(object[bv]._oAnimData);
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		if (object[bv]._oAnimData == NULL)
			return;
		#endif
		// jcm.patch1.end.1/14/97
		DrawSlabCel(pxp, pyp, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, ostart, oend);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void CDrawObjCel(int sx, int sy, int xp, int yp, BOOL pre, int ostart, int oend)
{
	char bv;
	int pxp, pyp;
	int odx, ody;

	if (dObject[sx][sy] > 0) {
		bv = dObject[sx][sy] - 1;
		if (object[bv]._oPreFlag != pre) return;
		pxp = xp - object[bv]._oAnimWidth2;
		pyp = yp;
	} else {
		bv = -(dObject[sx][sy]+1);
		if (object[bv]._oPreFlag != pre) return;
		odx = (object[bv]._ox - sx);
		ody = (object[bv]._oy - sy);
		pxp = xp - object[bv]._oAnimWidth2 + (odx << 5) - (ody << 5);
		pyp = yp + (odx << 4) + (ody << 4);
		ostart = 0;
		oend = 8;
	}
	app_assert(bv < MAXOBJECTS && bv >= 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (bv >= MAXOBJECTS)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	if (object[bv]._oAnimData == NULL)
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal("Draw Object type %d Clipped: NULL Cel Buffer",object[bv]._otype);
		#endif
		// jcm.patch1.end.1/14/97
	if (object[bv]._oAnimFrame < 1 || *reinterpret_cast<DWORD*>(object[bv]._oAnimData) > MAX_FRAMES
		|| object[bv]._oAnimFrame > *reinterpret_cast<long*>(object[bv]._oAnimData)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		return;
		#else
		app_fatal(
			"Draw Clipped Object: frame %d of %d, object type==%d",
			object[bv]._oAnimFrame,
			*reinterpret_cast<long*>(object[bv]._oAnimData),
			object[bv]._otype
		);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bv == cursobj) COutlineSlabCel(OOUTC, pxp, pyp, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, ostart, oend);
	if (object[bv]._oLight) CDrawSlabCelL(pxp, pyp, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, ostart, oend);
	else CDrawSlabCel(pxp, pyp, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, ostart, oend);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawEFlag1(BYTE *pTo2, int sx, int sy, int xp, int yp)
{
	BYTE *pTo;
	long oldnLVal;
	BOOL oldnTrans;
	int oldPieceNum;
	WORD *mt;

	oldnLVal = nLVal;
	oldnTrans = nTrans;
	oldPieceNum = gnPieceNum;
	gnPieceNum = dPiece[sx][sy];
	nLVal = dLight[sx][sy];
	nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
	mt = &dMT2[CalcRot(sx,sy)].mt[0];

	gbPartialTrans = PART_TRANS_LEFT;
	if (gdwPNum = mt[0])
		DrawMTileClipBottom (pTo2);
	gbPartialTrans = PART_TRANS_RIGHT;
	if (gdwPNum = mt[1])
		DrawMTileClipBottom (pTo2+32);
	gbPartialTrans = PART_TRANS_NONE;
	pTo = pTo2;
	for(int t = 2; t < MicroTileLen; t += 2)
	{
		pTo -= NBUFFWSL5;
		if (gdwPNum = mt[t])
			DrawMTileClipBottom (pTo);
		if (gdwPNum = mt[t+1])
			DrawMTileClipBottom (pTo+32);
	}

	DrawHTLXsub (pTo2, sx, sy, xp, yp, FALSE);

	nLVal = oldnLVal;
	nTrans = oldnTrans;
	gnPieceNum = oldPieceNum;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawHTLXsub (BYTE *pTo, int sx, int sy, int xp, int yp, BOOL chflag)
{
	char bFlags, bDead, bObject, bItem, bPlayer, bSpecial, bPlayerAbove, bTransVal;
	int nMonster, nMonsterAbove;
	int pxp,pyp;

	app_assert(sx < MAXDUNX);
	app_assert(sy < MAXDUNY);
	bFlags =	dFlags[sx][sy];
	bDead =		dDead[sx][sy];
	bObject =	dObject[sx][sy];
	bItem =		dItem[sx][sy];
	bPlayer =	dPlayer[sx][sy];
	bSpecial =	dSpecial[sx][sy];
	bTransVal =	dTransVal[sx][sy];
	nMonster =	dMonster[sx][sy];

	app_assert((sy-1) < MAXDUNY);
	bPlayerAbove =	dPlayer[sx][sy-1];
	nMonsterAbove =	dMonster[sx][sy-1];

	if ((visiondebug) && (bFlags & BFLAG_VISIBLE))
		CDrawSlabCelP(pTo, pSquareCel, 1, 64, 0, 8);
	if (MissilePreFlag && (bFlags & BFLAG_MISSILE))
		CDrawMissile(sx, sy, xp, yp, 0, 8, TRUE);
	if (nLVal < lightmax) {
		if (bDead) {
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			do {
			#endif
			// jcm.patch1.end.1/14/97
			DeadStruct * pDeadGuy = &dead[(bDead & 0x1f) - 1];
			char dd = (bDead & 0xe0) >> 5;
			pxp = xp - pDeadGuy->_deadWidth2;
			app_assert(pDeadGuy->_deadData[dd] != NULL);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (pDeadGuy->_deadData[dd] == NULL)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pDeadGuy->_deadFrame < 1 || *reinterpret_cast<DWORD*>(pDeadGuy->_deadData[dd]) > MAX_FRAMES
				|| pDeadGuy->_deadFrame > *reinterpret_cast<long*>(pDeadGuy->_deadData[dd])) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Clipped dead sub: frame %d of %d, deadnum==%d",
					pDeadGuy->_deadFrame,
					*reinterpret_cast<long*>(pDeadGuy->_deadData[dd]),
					(bDead & 0x1f) - 1
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			#if RLE_DRAW
			if (pDeadGuy->_deadtrans)
				DrawInfraUnitClipped(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					0,
					8,
					pDeadGuy->_deadtrans
				);
			else
				DrawLitUnitClipped(
				pxp, 
				yp,
				pDeadGuy->_deadData[dd],
				pDeadGuy->_deadFrame,
				pDeadGuy->_deadWidth,
				0,
				8
			);
			#else
			if (pDeadGuy->_deadtrans)
				CDrawSlabCelI(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					0,
					8,
					pDeadGuy->_deadtrans
				);
			else
				CDrawSlabCelL(
				pxp, 
				yp,
				pDeadGuy->_deadData[dd],
				pDeadGuy->_deadFrame,
				pDeadGuy->_deadWidth,
				0,
				8
			);
			#endif
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			} while (0);
			#endif
			// jcm.patch1.end.1/14/97
		}
		if (bObject)
			CDrawObjCel(sx, sy, xp, yp, TRUE, 0, 8);
	}
	if (bItem) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		ItemStruct * pItem = &item[bItem - 1];
		if (!pItem->_iPostDraw) {
			app_assert(bItem <= MAXITEMS && bItem >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (bItem > MAXITEMS || bItem < 0)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pItem->_iAnimData == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Item \"%s\" Clipped 1: NULL Cel Buffer",pItem->_iIName);
				#endif
				// jcm.patch1.end.1/14/97
			if (pItem->_iAnimFrame < 1 || *reinterpret_cast<DWORD*>(pItem->_iAnimData) > MAX_FRAMES
				|| pItem->_iAnimFrame > *reinterpret_cast<long*>(pItem->_iAnimData)) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Draw Clipped \"%s\" Item: frame %d of %d, item type==%d",
					pItem->_iIName,
					pItem->_iAnimFrame,
					*reinterpret_cast<long*>(pItem->_iAnimData),
					pItem->_itype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			pxp = xp - pItem->_iAnimWidth2;
			if ((bItem - 1) == cursitem 
				|| HighLightAllItems == true)
				COutlineSlabCel(
					IOUTC,
					pxp,
					yp,
					pItem->_iAnimData,
					pItem->_iAnimFrame,
					pItem->_iAnimWidth,
					0,
					8
				);
			CDrawSlabCelL(
				pxp,
				yp,
				pItem->_iAnimData,
				pItem->_iAnimFrame,
				pItem->_iAnimWidth,
				0,
				8
			);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_PLRLR) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		int nPlayer = -(bPlayerAbove + 1);
		if ((DWORD)nPlayer >= MAX_PLRS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("draw player clipped: tried to draw illegal player %d",nPlayer);
			#endif
			// jcm.patch1.end.1/14/97
		PlayerStruct * pPlayer = &plr[nPlayer];
		pxp = pPlayer->_pxoff + xp - pPlayer->_pAnimWidth2;
		pyp = pPlayer->_pyoff + yp;
		CDrawPSlabCelL(
			-(bPlayerAbove + 1),
			sx, sy-1,
			pxp,
			pyp,
			pPlayer->_pAnimData,
			pPlayer->_pAnimFrame,
			pPlayer->_pAnimWidth,
			0,
			8
		);
		if (chflag && pPlayer->_peflag)	{
			if (pPlayer->_peflag == 2)
				DrawEFlag1(pTo-(NBUFFWSL4+96), sx-2, sy+1, xp-96, yp-16);
			DrawEFlag1(pTo-64, sx-1, sy+1, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while(0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if ((bFlags & BFLAG_MONSTLR) && ((bFlags & BFLAG_VISIBLE) || plr[myplr]._pInfraFlag) && (nMonsterAbove < 0)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		gnMI = -(nMonsterAbove + 1);
		if ((DWORD)gnMI >= MAXMONSTERS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("Draw Monster Clipped: tried to draw illegal monster %d",gnMI);
			#endif
			// jcm.patch1.end.1/14/97
		MonsterStruct * pMonster = &monster[gnMI];
 		if (!(pMonster->_mFlags & MFLAG_INVISIBLE)) {
			if (pMonster->MType == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster",pMonster->mName);
				#endif
				// jcm.patch1.end.1/14/97
			pxp = pMonster->_mxoff + xp - pMonster->MType->mAnimWidth2;
			pyp = pMonster->_myoff + yp;
			#if RLE_DRAW
			if (gnMI == cursmonst)
				DrawUnitOutlineClipped(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					0, 
					8
				);
			#else
			if (gnMI == cursmonst)
				COutlineSlabCel(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					0, 
					8
				);
			#endif
			CDrawMSlabCelL(sx, sy, pxp, pyp, gnMI, 0, 8);
			if (chflag && pMonster->_meflag)
				DrawEFlag1(pTo-64, sx-1, sy+1, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_DEADPLR)
		DrawDeadPlr(sx, sy, xp, yp, 0, 8, TRUE);
	if (bPlayer > 0) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		int nPlayer = bPlayer - 1;
		if ((DWORD)nPlayer >= MAX_PLRS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("draw player clipped: tried to draw illegal player %d",nPlayer);
			#endif
			// jcm.patch1.end.1/14/97
		PlayerStruct * pPlayer = &plr[nPlayer];
		pxp = pPlayer->_pxoff + xp - pPlayer->_pAnimWidth2;
		pyp = pPlayer->_pyoff + yp;
		CDrawPSlabCelL(
			bPlayer - 1,
			sx, sy,
			pxp,
			pyp,
			pPlayer->_pAnimData,
			pPlayer->_pAnimFrame,
			pPlayer->_pAnimWidth,
			0,
			8
		);
		if (chflag && pPlayer->_peflag) {
			if (pPlayer->_peflag == 2)
				DrawEFlag1(pTo-(NBUFFWSL4+96), sx-2, sy+1, xp-96, yp-16);
			DrawEFlag1(pTo-64, sx-1, sy+1, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if ((nMonster > 0) && ((bFlags & BFLAG_VISIBLE) || plr[myplr]._pInfraFlag)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		gnMI = nMonster - 1;
		if ((DWORD)gnMI >= MAXMONSTERS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("Draw Monster Clipped: tried to draw illegal monster %d",gnMI);
			#endif
			// jcm.patch1.end.1/14/97
		MonsterStruct * pMonster = &monster[gnMI];
 		if (!(pMonster->_mFlags & MFLAG_INVISIBLE)) {
			if (pMonster->MType == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster",pMonster->mName);
				#endif
				// jcm.patch1.end.1/14/97
			pxp = pMonster->_mxoff + xp - pMonster->MType->mAnimWidth2;
			pyp = pMonster->_myoff + yp;
			#if RLE_DRAW
			if (gnMI == cursmonst)
				DrawUnitOutlineClipped(
				MOUTC, 
				pxp,
				pyp,
				pMonster->_mAnimData,
				pMonster->_mAnimFrame,
				pMonster->MType->mAnimWidth,
				0,
				8
			);
			#else
			if (gnMI == cursmonst)
				COutlineSlabCel(
				MOUTC, 
				pxp,
				pyp,
				pMonster->_mAnimData,
				pMonster->_mAnimFrame,
				pMonster->MType->mAnimWidth,
				0,
				8
			);
			#endif
			CDrawMSlabCelL(sx, sy, pxp, pyp, gnMI, 0, 8);
			if (chflag && pMonster->_meflag)
				DrawEFlag1(pTo-64, sx-1, sy+1, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_MISSILE)
		CDrawMissile(sx, sy, xp, yp, 0, 8, FALSE);
	if (bObject && (nLVal < lightmax)) 
		CDrawObjCel(sx, sy, xp, yp, FALSE, 0, 8);
	if (bItem) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		ItemStruct * pItem = &item[bItem - 1];
		if (pItem->_iPostDraw) {
			app_assert(bItem <= MAXITEMS && bItem >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (bItem > MAXITEMS || bItem < 0)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pItem->_iAnimData == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Item \"%s\" Clipped 2: NULL Cel Buffer",pItem->_iIName);
				#endif
				// jcm.patch1.end.1/14/97
			if (pItem->_iAnimFrame < 1 || *reinterpret_cast<DWORD*>(pItem->_iAnimData) > MAX_FRAMES
				|| pItem->_iAnimFrame > *reinterpret_cast<long*>(pItem->_iAnimData)) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Draw Clipped \"%s\" Item 2: frame %d of %d, item type==%d",
					pItem->_iIName,
					pItem->_iAnimFrame,
					*reinterpret_cast<long*>(pItem->_iAnimData),
					pItem->_itype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			pxp = xp - pItem->_iAnimWidth2;
			if ((bItem - 1) == cursitem
				|| HighLightAllItems == true)
				COutlineSlabCel(
				IOUTC,
				pxp,
				yp,
				pItem->_iAnimData,
				pItem->_iAnimFrame,
				pItem->_iAnimWidth,
				0,
				8
			);
			CDrawSlabCelL(
				pxp,
				yp,
				pItem->_iAnimData,
				pItem->_iAnimFrame,
				pItem->_iAnimWidth,
				0,
				8
			);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bSpecial) {
		nTrans = TransList[bTransVal];
		TCDrawSlabCelPL(pTo, pSpecialCels, bSpecial, 64, 0, 8);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawHTileLineX (int sx, int sy, int xp, int yp, int nd, int halfflag)
{
	int	i;
	BYTE *pTo;
	int t;
	MICROS *pmt;
	WORD *mt;

	app_assert(gpBuffer);
	pmt = &dMT2[CalcRot(sx,sy)];
	if (halfflag) {
		if (((unsigned)sy < DMAXY) && ((unsigned)sx < DMAXX)) {
			gnPieceNum = dPiece[sx][sy];
			nLVal = dLight[sx][sy];
			if (gnPieceNum) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp + 32;
				nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];

				mt = &pmt->mt[0];

				gbPartialTrans = PART_TRANS_RIGHT;
				if (gdwPNum = mt[1])
					DrawMTileClipBottom (pTo);
				gbPartialTrans = PART_TRANS_NONE;

				pTo -= NBUFFWSL5;
				if (gdwPNum = mt[3])
					DrawMTileClipBottom (pTo);
				
				pTo -= NBUFFWSL5;
				if (gdwPNum = mt[5])
					DrawMTileClipBottom (pTo);

				pTo -= NBUFFWSL5;
				if (gdwPNum = mt[7])
					DrawMTileClipBottom (pTo);
				
				pTo -= NBUFFWSL5;
				if (gdwPNum = mt[9])
					DrawMTileClipBottom (pTo);
				
				pTo -= NBUFFWSL5;
				if ((gdwPNum = mt[11]) && (leveltype == 4))
					DrawMTileClipBottom (pTo);

				DrawHTLXsub (gpBuffer + nBuffWTbl[yp] + xp, sx, sy, xp, yp, FALSE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				DrawBlankMTile(pTo);
			}
		}
		++sx;
		--sy;
		xp += 64;
		++pmt;
		--nd;
	}

	for (i = nd; i-- && sy >= 0 && sx < DMAXX; ++sx, --sy, xp += 64, ++pmt) {
		if (sy >= DMAXY || sx < 0)
			continue;
		gnPieceNum = dPiece[sx][sy];
		nLVal = dLight[sx][sy];
		if (!gnPieceNum) {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			DrawBlankMTile(pTo);
			continue;
		}
		pTo = gpBuffer + nBuffWTbl[yp] + xp;
		nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];

		mt = &pmt->mt[0];

		gbPartialTrans = PART_TRANS_LEFT;
		if (gdwPNum = mt[0]) DrawMTileClipBottom (pTo);
		gbPartialTrans = PART_TRANS_RIGHT;
		if (gdwPNum = mt[1]) DrawMTileClipBottom (pTo+32);
		gbPartialTrans = PART_TRANS_NONE;
		for(t = 2; t < MicroTileLen; t += 2)
		{
			pTo -= NBUFFWSL5;
			if (gdwPNum = mt[t])
				DrawMTileClipBottom (pTo);
			if (gdwPNum = mt[t+1])
				DrawMTileClipBottom (pTo+32);
		}

		DrawHTLXsub (gpBuffer + nBuffWTbl[yp] + xp, sx, sy, xp, yp, TRUE);
	}

	if (!halfflag || !((unsigned)sy < DMAXY && (unsigned)sx < DMAXX))
		return;
	gnPieceNum = dPiece[sx][sy];
	nLVal = dLight[sx][sy];
	if (!gnPieceNum) {
		pTo = gpBuffer + nBuffWTbl[yp] + xp;
		DrawBlankMTile(pTo);
		return;
	}
	pTo = gpBuffer + nBuffWTbl[yp] + xp;
	nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
	mt = &pmt->mt[0];
	gbPartialTrans = PART_TRANS_LEFT;
	if (gdwPNum = mt[0])
		DrawMTileClipBottom (pTo);
	gbPartialTrans = PART_TRANS_NONE;
	pTo -= NBUFFWSL5;
	if (gdwPNum = mt[2])
		DrawMTileClipBottom (pTo);
	pTo -= NBUFFWSL5;
	if (gdwPNum = mt[4])
		DrawMTileClipBottom (pTo);
	pTo -= NBUFFWSL5;
	if (gdwPNum = mt[6])
		DrawMTileClipBottom (pTo);
	pTo -= NBUFFWSL5;
	if (gdwPNum = mt[8])
		DrawMTileClipBottom (pTo);
	pTo -= NBUFFWSL5;
	if ((gdwPNum = mt[10]) && (leveltype == 4))
		DrawMTileClipBottom (pTo);
	DrawHTLXsub (gpBuffer + nBuffWTbl[yp] + xp, sx, sy, xp, yp, FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawEFlag2(BYTE *pTo2, int sx, int sy, int sv, int sv2, int xp, int yp)
{
	BYTE *pTo;
	long oldnLVal;
	BOOL oldnTrans;
	int oldPieceNum;
	WORD *mt;

	oldnLVal = nLVal;
	oldnTrans = nTrans;
	oldPieceNum = gnPieceNum;
	gnPieceNum = dPiece[sx][sy];
	nLVal = dLight[sx][sy];
	pTo = pTo2 + (NBUFFWSL5 * sv);
	nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
	mt = &dMT2[CalcRot(sx,sy)].mt[0];

	switch (sv) {
	case 0:
		if (gdwPNum = mt[2]) DrawMTileClipBottom (pTo);
		if (gdwPNum = mt[3]) DrawMTileClipBottom (pTo+32);
	case 1:
		pTo -= NBUFFWSL5;
		if (gdwPNum = mt[4]) DrawMTileClipBottom (pTo);
		if (gdwPNum = mt[5]) DrawMTileClipBottom (pTo+32);
	case 2:
		pTo -= NBUFFWSL5;
		if (gdwPNum = mt[6]) DrawMTileClipBottom (pTo);
		if (gdwPNum = mt[7]) DrawMTileClipBottom (pTo+32);
	case 3:
		pTo -= NBUFFWSL5;
		if (gdwPNum = mt[8]) DrawMTileClipBottom (pTo);
		if (gdwPNum = mt[9]) DrawMTileClipBottom (pTo+32);
//	case 4:
//		pTo -= NBUFFWSL5;
//		if (leveltype == 4) {
//			if (gdwPNum = mt[10]) DrawMTileClipBottom (pTo);
//			if (gdwPNum = mt[11]) DrawMTileClipBottom (pTo+32);
//		}
	}

	if (sv2 < 8)
		DrawHTLXsub2 (pTo2, sx, sy, sv, sv2, xp, yp, FALSE);
	nLVal = oldnLVal;
	nTrans = oldnTrans;
	gnPieceNum = oldPieceNum;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawHTLXsub2 (BYTE *pTo, int sx, int sy, int sv, int sv2, int xp, int yp, BOOL chflag)
{
	char bFlags, bDead, bObject, bItem, bPlayer, bSpecial, bPlayerAbove, bTransVal;
	int nMonster, nMonsterAbove;
	int pxp,pyp;

	app_assert(sx < MAXDUNX);
	app_assert(sy < MAXDUNY);
	bFlags =	dFlags[sx][sy];
	bDead =		dDead[sx][sy];
	bObject =	dObject[sx][sy];
	bItem =		dItem[sx][sy];
	bPlayer =	dPlayer[sx][sy];
	bSpecial =	dSpecial[sx][sy];
	bTransVal =	dTransVal[sx][sy];
	nMonster =	dMonster[sx][sy];

	app_assert((sy-1) < MAXDUNY);
	bPlayerAbove =	dPlayer[sx][sy-1];
	nMonsterAbove =	dMonster[sx][sy-1];

	if (visiondebug && (bFlags & BFLAG_VISIBLE))
		CDrawSlabCelP(pTo, pSquareCel, 1, 64, sv2, 8);
	if (MissilePreFlag && (bFlags & BFLAG_MISSILE))
		CDrawMissile(sx, sy, xp, yp, sv2, 8, TRUE);
	if (nLVal < lightmax) {
		if (bDead) {
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			do {
			#endif
			// jcm.patch1.end.1/14/97
			DeadStruct * pDeadGuy = &dead[(bDead & 0x1f) - 1];
			char dd = (bDead & 0xe0) >> 5;
			pxp = xp - pDeadGuy->_deadWidth2;
			app_assert(pDeadGuy->_deadData[dd] != NULL);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (pDeadGuy->_deadData[dd] == NULL)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pDeadGuy->_deadFrame < 1 || *reinterpret_cast<DWORD*>(pDeadGuy->_deadData[dd]) > MAX_FRAMES
				|| pDeadGuy->_deadFrame > *reinterpret_cast<long*>(pDeadGuy->_deadData[dd])) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Clipped dead sub2: frame %d of %d, deadnum==%d",
					pDeadGuy->_deadFrame,
					*reinterpret_cast<long*>(pDeadGuy->_deadData[dd]),
					(bDead & 0x1f) - 1
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			#if RLE_DRAW
			if (pDeadGuy->_deadtrans)
				DrawInfraUnitClipped(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					sv2,
					8,
					pDeadGuy->_deadtrans
				);
			else
				DrawLitUnitClipped(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					sv2,
					8
				);
			#else
			if (pDeadGuy->_deadtrans)
				CDrawSlabCelI(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					sv2,
					8,
					pDeadGuy->_deadtrans
				);
			else
				CDrawSlabCelL(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					sv2,
					8
				);
			#endif
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			} while (0);
			#endif
			// jcm.patch1.end.1/14/97
		}
		if (bObject)
			CDrawObjCel(sx, sy, xp, yp, TRUE, sv2, 8);
	}
	if (bItem) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		ItemStruct * pItem = &item[bItem - 1];
		if (!pItem->_iPostDraw) {
			app_assert(bItem <= MAXITEMS && bItem >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (bItem > MAXITEMS || bItem < 0)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pItem->_iAnimData == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Item \"%s\" Clipped 3: NULL Cel Buffer",pItem->_iIName);
				#endif
				// jcm.patch1.end.1/14/97
			if (pItem->_iAnimFrame < 1 || *reinterpret_cast<DWORD*>(pItem->_iAnimData) > MAX_FRAMES
				|| pItem->_iAnimFrame > *reinterpret_cast<long*>(pItem->_iAnimData)) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Draw Clipped \"%s\" Item 3: frame %d of %d, item type==%d",
					pItem->_iIName,
					pItem->_iAnimFrame,
					*reinterpret_cast<long*>(pItem->_iAnimData),
					pItem->_itype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			pxp = xp - pItem->_iAnimWidth2;
			if ((bItem - 1) == cursitem
				|| HighLightAllItems == true)
				COutlineSlabCel(
					IOUTC,
					pxp,
					yp,
					pItem->_iAnimData,
					pItem->_iAnimFrame,
					pItem->_iAnimWidth,
					sv2,
					8
				);
			CDrawSlabCelL(
				pxp,
				yp,
				pItem->_iAnimData,
				pItem->_iAnimFrame,
				pItem->_iAnimWidth,
				sv2,
				8
			);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_PLRLR) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		int nPlayer = -(bPlayerAbove + 1);
		if (nPlayer >= MAX_PLRS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("draw player clipped: tried to draw illegal player %d",nPlayer);
			#endif
			// jcm.patch1.end.1/14/97
		PlayerStruct * pPlayer = &plr[nPlayer];
		pxp = pPlayer->_pxoff + xp - pPlayer->_pAnimWidth2;
		pyp = pPlayer->_pyoff + yp;
		CDrawPSlabCelL(
			-(bPlayerAbove + 1),
			sx, sy-1,
			pxp,
			pyp,
			pPlayer->_pAnimData,
			pPlayer->_pAnimFrame,
			pPlayer->_pAnimWidth,
			sv2,
			8
		);
		if (chflag && pPlayer->_peflag) {
			if (pPlayer->_peflag == 2)
				DrawEFlag2(pTo-(NBUFFWSL4+96), sx-2, sy+1, sv, sv2, xp-96, yp-16);
			DrawEFlag2(pTo-64, sx-1, sy+1, sv, sv2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if ((bFlags & BFLAG_MONSTLR) && ((bFlags & BFLAG_VISIBLE) || (plr[myplr]._pInfraFlag)) && (nMonsterAbove < 0)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		gnMI = -(nMonsterAbove + 1);
		if (gnMI >= MAXMONSTERS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("Draw Monster Clipped: tried to draw illegal monster %d",gnMI);
			#endif
			// jcm.patch1.end.1/14/97
		MonsterStruct * pMonster = &monster[gnMI];
 		if (!(pMonster->_mFlags & MFLAG_INVISIBLE)) {
			if (pMonster->MType == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster",pMonster->mName);
				#endif
				// jcm.patch1.end.1/14/97
			pxp = pMonster->_mxoff + xp - pMonster->MType->mAnimWidth2;
			pyp = pMonster->_myoff + yp;
			#if RLE_DRAW
			if (gnMI == cursmonst)
				DrawUnitOutlineClipped(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					sv2,
					8
				);
			#else
			if (gnMI == cursmonst)
				COutlineSlabCel(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					sv2,
					8
				);
			#endif
			CDrawMSlabCelL(sx, sy, pxp, pyp, gnMI, sv2, 8);
			if (chflag && !pMonster->_meflag)
				DrawEFlag2(pTo-64, sx-1, sy+1, sv, sv2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_DEADPLR)
		DrawDeadPlr(sx, sy, xp, yp, sv2, 8, TRUE);
	if (bPlayer > 0) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		int nPlayer = bPlayer - 1;
		if ((DWORD)nPlayer >= MAX_PLRS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("draw player clipped: tried to draw illegal player %d",nPlayer);
			#endif
			// jcm.patch1.end.1/14/97
		PlayerStruct * pPlayer = &plr[nPlayer];
		pxp = pPlayer->_pxoff + xp - pPlayer->_pAnimWidth2;
		pyp = pPlayer->_pyoff + yp;
		CDrawPSlabCelL(
			bPlayer - 1,
			sx, sy,
			pxp,
			pyp,
			pPlayer->_pAnimData,
			pPlayer->_pAnimFrame,
			pPlayer->_pAnimWidth,
			sv2,
			8
		);
		if (chflag && pPlayer->_peflag) {
			if (pPlayer->_peflag == 2)
				DrawEFlag2(pTo-(NBUFFWSL4+96), sx-2, sy+1, sv, sv2, xp-96, yp-16);
			DrawEFlag2(pTo-64, sx-1, sy+1, sv, sv2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if ((nMonster > 0) && ((bFlags & BFLAG_VISIBLE) || (plr[myplr]._pInfraFlag))) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		gnMI = nMonster - 1;
		if ((DWORD)gnMI >= MAXMONSTERS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("Draw Monster Clipped: tried to draw illegal monster %d",gnMI);
			#endif
			// jcm.patch1.end.1/14/97
		MonsterStruct * pMonster = &monster[gnMI];
 		if (!(pMonster->_mFlags & MFLAG_INVISIBLE)) {
			if (pMonster->MType == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster",pMonster->mName);
				#endif
				// jcm.patch1.end.1/14/97
			pxp = pMonster->_mxoff + xp - pMonster->MType->mAnimWidth2;
			pyp = pMonster->_myoff + yp;
			#if RLE_DRAW
			if (gnMI == cursmonst)
				DrawUnitOutlineClipped(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					sv2,
					8
				);
			#else
			if (gnMI == cursmonst)
				COutlineSlabCel(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					sv2,
					8
				);
			#endif
			CDrawMSlabCelL(sx, sy, pxp, pyp, gnMI, sv2, 8);
			if (chflag && !pMonster->_meflag)
				DrawEFlag2(pTo-64, sx-1, sy+1, sv, sv2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_MISSILE)
		CDrawMissile(sx, sy, xp, yp, sv2, 8, FALSE);
	if (bObject && (nLVal < lightmax))
		CDrawObjCel(sx, sy, xp, yp, FALSE, sv2, 8);
	if (bItem) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		ItemStruct * pItem = &item[bItem - 1];
		if (pItem->_iPostDraw) {
			app_assert(bItem <= MAXITEMS && bItem >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (bItem > MAXITEMS || bItem < 0)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pItem->_iAnimData == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Item \"%s\" Clipped 4: NULL Cel Buffer",pItem->_iIName);
				#endif
				// jcm.patch1.end.1/14/97
			if (pItem->_iAnimFrame < 1 || *reinterpret_cast<DWORD*>(pItem->_iAnimData) > MAX_FRAMES
				|| pItem->_iAnimFrame > *reinterpret_cast<long*>(pItem->_iAnimData)) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Draw Clipped \"%s\" Item 4: frame %d of %d, item type==%d",
					pItem->_iIName,
					pItem->_iAnimFrame,
					*reinterpret_cast<long*>(pItem->_iAnimData),
					pItem->_itype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			pxp = xp - pItem->_iAnimWidth2;
			if ((bItem - 1) == cursitem
				|| HighLightAllItems == true)
				COutlineSlabCel(
					IOUTC,
					pxp,
					yp,
					pItem->_iAnimData,
					pItem->_iAnimFrame,
					pItem->_iAnimWidth,
					sv2,
					8
				);
			CDrawSlabCelL(
				pxp,
				yp,
				pItem->_iAnimData,
				pItem->_iAnimFrame,
				pItem->_iAnimWidth,
				sv2,
				8
			);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bSpecial) {
		nTrans = TransList[bTransVal];
		TCDrawSlabCelPL(pTo, pSpecialCels, bSpecial, 64, sv2, 8);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawHTLX2 (int sx, int sy, int xp, int yp, int nd, int sv, int halfflag)
{
	int	i;
	BYTE *pTo;
	int sv2;
	int t;
	MICROS *pmt;
	WORD *mt;

	app_assert(gpBuffer);
	pmt = &dMT2[CalcRot(sx,sy)];
	sv2 = (sv + 1) << 1;
	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gnPieceNum = dPiece[sx][sy]; 
			nLVal = dLight[sx][sy];
			if (gnPieceNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp - NBUFFWSL5 + 32;
				nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
				mt = &pmt->mt[0];

				for(t = 0; t < ((MicroTileLen>>1)-1); ++t)
				{
					if ((sv <= t) && (gdwPNum = mt[2*t+3])) DrawMTileClipBottom (pTo);
					pTo -= NBUFFWSL5;
				}
				if (sv2 < 8) {
					pTo = gpBuffer + nBuffWTbl[yp] + xp - (NBUFFWSL4 * sv2);
					DrawHTLXsub2 (pTo, sx, sy, sv, sv2, xp, yp, FALSE);
				}
			}
		}
		++sx;
		--sy;
		xp += 64;
		--nd;
		++pmt;
	}

	for (i = nd; i-- && sx < DMAXX && sy >= 0; ++sx, --sy, xp += 64, ++pmt) {
		if (sy >= DMAXY || sx < 0)
			continue;
		gnPieceNum = dPiece[sx][sy]; 
		nLVal = dLight[sx][sy];
		if (!gnPieceNum)
			continue;
		pTo = gpBuffer + nBuffWTbl[yp] + xp - NBUFFWSL5;
		nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
		mt = &pmt->mt[0];
		for(t = 0; t < ((MicroTileLen>>1)-1); ++t, pTo -= NBUFFWSL5)
		{
			if (sv > t)
				continue;
			if (gdwPNum = mt[2*t + 2])
				DrawMTileClipBottom (pTo);
			if (gdwPNum = mt[2*t + 3])
				DrawMTileClipBottom (pTo+32);
		}

		if (sv2 >= 8)
			continue;
		pTo = gpBuffer + nBuffWTbl[yp] + xp - (NBUFFWSL5 * (sv + 1));
		DrawHTLXsub2 (pTo, sx, sy, sv, sv2, xp, yp, TRUE);
	}

	if (!halfflag || !(static_cast<unsigned>(sy) < DMAXY && static_cast<unsigned>(sx) < DMAXX))
		return;

	gnPieceNum = dPiece[sx][sy]; 
	nLVal = dLight[sx][sy];

	if (!gnPieceNum)
		return;
	pTo = gpBuffer + nBuffWTbl[yp] + xp - NBUFFWSL5;
	nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
	mt = &pmt->mt[0];

	for(t = 0; t < ((MicroTileLen>>1)-1); ++t)
	{
		if ((sv <= t) && (gdwPNum = mt[2*t+2])) DrawMTileClipBottom (pTo);
		pTo -= NBUFFWSL5;
	}

	if (sv2 < 8) {
		pTo = gpBuffer + nBuffWTbl[yp] + xp - (NBUFFWSL4 * sv2);
		DrawHTLXsub2 (pTo, sx, sy, sv, sv2, xp, yp, FALSE);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawEFlag3(BYTE *pTo2, int sx, int sy, int ev, int ev2, int xp, int yp)
{
	BYTE *pTo;
	long oldnLVal;
	BOOL oldnTrans;
	int oldPieceNum;
	int t;
	WORD *mt;

	oldnLVal = nLVal;
	oldnTrans = nTrans;
	oldPieceNum = gnPieceNum;
	gnPieceNum = dPiece[sx][sy];
	nLVal = dLight[sx][sy];
	pTo = pTo2;
	nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
	mt = &dMT2[CalcRot(sx,sy)].mt[0];

	gbPartialTrans = PART_TRANS_LEFT;
	if (gdwPNum = mt[0]) DrawMTileClipTop (pTo);
	gbPartialTrans = PART_TRANS_RIGHT;
	if (gdwPNum = mt[1]) DrawMTileClipTop (pTo+32);
	gbPartialTrans = PART_TRANS_NONE;
	for(t = 1; t < ((MicroTileLen>>1)-1); ++t)
	{
		pTo -= NBUFFWSL5;
		if (ev >= t) {
			if (gdwPNum = mt[2*t]) DrawMTileClipTop (pTo);
			if (gdwPNum = mt[2*t+1]) DrawMTileClipTop (pTo+32);
		}
	}

	DrawHTLXsub3 (pTo2, sx, sy, ev, ev2, xp, yp, FALSE);

	nLVal = oldnLVal;
	nTrans = oldnTrans;
	gnPieceNum = oldPieceNum;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawHTLXsub3 (BYTE *pTo, int sx, int sy, int ev, int ev2, int xp, int yp, BOOL chflag)
{
	char bFlags, bDead, bObject, bItem, bPlayer, bSpecial, bPlayerAbove, bTransVal;
	int nMonster, nMonsterAbove;
	int pxp,pyp;

	app_assert(sx < MAXDUNX);
	app_assert(sy < MAXDUNY);
	bFlags =	dFlags[sx][sy];
	bDead =		dDead[sx][sy];
	bObject =	dObject[sx][sy];
	bItem =		dItem[sx][sy];
	bPlayer =	dPlayer[sx][sy];
	bSpecial =	dSpecial[sx][sy];
	bTransVal =	dTransVal[sx][sy];
	nMonster =	dMonster[sx][sy];

	app_assert((sy-1) < MAXDUNY);
	bPlayerAbove =	dPlayer[sx][sy-1];
	nMonsterAbove =	dMonster[sx][sy-1];

	if (visiondebug && (bFlags & BFLAG_VISIBLE))
		DrawSlabCelP(pTo, pSquareCel, 1, 64, 0, ev2);
	if (MissilePreFlag && (bFlags & BFLAG_MISSILE))
		DrawMissile(sx, sy, xp, yp, 0, ev2, TRUE);
	if (nLVal < lightmax) {
		if (bDead) {
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			do {
			#endif
			// jcm.patch1.end.1/14/97
			DeadStruct * pDeadGuy = &dead[(bDead & 0x1f) - 1];
			char dd = (bDead & 0xe0) >> 5;
			pxp = xp - pDeadGuy->_deadWidth2;
			app_assert(pDeadGuy->_deadData[dd] != NULL);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (pDeadGuy->_deadData[dd] == NULL)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pDeadGuy->_deadFrame < 1 || *reinterpret_cast<DWORD*>(pDeadGuy->_deadData[dd]) > MAX_FRAMES
				|| pDeadGuy->_deadFrame > *reinterpret_cast<long*>(pDeadGuy->_deadData[dd])) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Unclipped dead: frame %d of %d, deadnum==%d",
					pDeadGuy->_deadFrame,
					*reinterpret_cast<long*>(pDeadGuy->_deadData[dd]),
					(bDead & 0x1f) - 1
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			#if RLE_DRAW
			if (pDeadGuy->_deadtrans)
				DrawInfraUnit(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					0,
					ev2,//diff
					pDeadGuy->_deadtrans
				);
			else
				DrawLitUnit(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					0,
					ev2//diff
				);
			#else
			if (pDeadGuy->_deadtrans)
				DrawSlabCelI(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					0,
					ev2,//diff
					pDeadGuy->_deadtrans
				);
			else
				DrawSlabCelL(
					pxp,
					yp,
					pDeadGuy->_deadData[dd],
					pDeadGuy->_deadFrame,
					pDeadGuy->_deadWidth,
					0,
					ev2//diff
				);
			#endif
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			} while (0);
			#endif
			// jcm.patch1.end.1/14/97
		}
		if (bObject)
			DrawObjCel(sx, sy, xp, yp, TRUE, 0, ev2);//diff
	}
	if (bItem) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		ItemStruct * pItem = &item[bItem - 1];
		if (!pItem->_iPostDraw) {
			app_assert(bItem <= MAXITEMS && bItem >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (bItem > MAXITEMS || bItem < 0)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pItem->_iAnimData == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Item \"%s\" 1: NULL Cel Buffer",pItem->_iIName);
				#endif
				// jcm.patch1.end.1/14/97
			if (pItem->_iAnimFrame < 1 || *reinterpret_cast<DWORD*>(pItem->_iAnimData) > MAX_FRAMES
				|| pItem->_iAnimFrame > *reinterpret_cast<long*>(pItem->_iAnimData)) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Draw \"%s\" Item 1: frame %d of %d, item type==%d",
					pItem->_iIName,
					pItem->_iAnimFrame,
					*reinterpret_cast<long*>(pItem->_iAnimData),
					pItem->_itype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			pxp = xp - pItem->_iAnimWidth2;
			if ((bItem - 1) == cursitem
				|| HighLightAllItems == true)
				OutlineSlabCel(
					IOUTC,
					pxp,
					yp,
					pItem->_iAnimData,
					pItem->_iAnimFrame,
					pItem->_iAnimWidth,
					0,
					ev2//diff
				);
			DrawSlabCelL(
				pxp,
				yp,
				pItem->_iAnimData,
				pItem->_iAnimFrame,
				pItem->_iAnimWidth,
				0,
				ev2//diff
			);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_PLRLR) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		int nPlayer = -(bPlayerAbove + 1);
		if (nPlayer >= MAX_PLRS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("draw player: tried to draw illegal player %d",nPlayer);
			#endif
			// jcm.patch1.end.1/14/97
		PlayerStruct * pPlayer = &plr[nPlayer];
		pxp = pPlayer->_pxoff + xp - pPlayer->_pAnimWidth2;
		pyp = pPlayer->_pyoff + yp;
		DrawPSlabCelL(
			-(bPlayerAbove + 1),
			sx, sy-1,
			pxp,
			pyp,
			pPlayer->_pAnimData,
			pPlayer->_pAnimFrame,
			pPlayer->_pAnimWidth,
			0,
			ev2//diff
		);
		if (chflag && pPlayer->_peflag) {
			if (pPlayer->_peflag == 2)
				DrawEFlag3(pTo-(NBUFFWSL4+96), sx-2, sy+1, ev, ev2, xp-96, yp-16);
			DrawEFlag3(pTo-64, sx-1, sy+1, ev, ev2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if ((bFlags & BFLAG_MONSTLR) && ((bFlags & BFLAG_VISIBLE) || plr[myplr]._pInfraFlag) && (nMonsterAbove < 0)) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		gnMI = -(nMonsterAbove + 1);
		if (gnMI >= MAXMONSTERS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("Draw Monster: tried to draw illegal monster %d",gnMI);
			#endif	
			// jcm.patch1.end.1/14/97
		MonsterStruct * pMonster = &monster[gnMI];
 		if (!(pMonster->_mFlags & MFLAG_INVISIBLE)) {
			if (pMonster->MType == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Monster \"%s\": uninitialized monster",pMonster->mName);
				#endif
				// jcm.patch1.end.1/14/97
			pxp = pMonster->_mxoff + xp - pMonster->MType->mAnimWidth2;
			pyp = pMonster->_myoff + yp;
			#if RLE_DRAW
			if (gnMI == cursmonst)
				DrawUnitOutline(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					0,
					ev2
				);
			#else
			if (gnMI == cursmonst)
				OutlineSlabCel(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					0,
					ev2
				);
			#endif
			DrawMSlabCelL(sx, sy, pxp, pyp, gnMI, 0, ev2);
			if (chflag && !pMonster->_meflag)
				DrawEFlag3(pTo-64, sx-1, sy+1, ev, ev2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_DEADPLR)
		DrawDeadPlr(sx, sy, xp, yp, 0, ev2, FALSE);
	if (bPlayer > 0) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		int nPlayer = bPlayer - 1;
		if (nPlayer >= MAX_PLRS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("draw player: tried to draw illegal player %d",nPlayer);
			#endif
			// jcm.patch1.end.1/14/97
		PlayerStruct * pPlayer = &plr[nPlayer];
		pxp = pPlayer->_pxoff + xp - pPlayer->_pAnimWidth2;
		pyp = pPlayer->_pyoff + yp;
		DrawPSlabCelL(
			bPlayer - 1,
			sx, sy,
			pxp,
			pyp,
			pPlayer->_pAnimData,
			pPlayer->_pAnimFrame,
			pPlayer->_pAnimWidth,
			0,
			ev2
		);
		if (chflag && pPlayer->_peflag) {
			if (pPlayer->_peflag == 2)
				DrawEFlag3(pTo-(NBUFFWSL4+96), sx-2, sy+1, ev, ev2, xp-96, yp-16);
			DrawEFlag3(pTo-64, sx-1, sy+1, ev, ev2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if ((nMonster > 0) && ((bFlags & BFLAG_VISIBLE) || (plr[myplr]._pInfraFlag))) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		gnMI = nMonster - 1;
		if ((DWORD)gnMI >= MAXMONSTERS)
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			break;
			#else
			app_fatal("Draw Monster: tried to draw illegal monster %d",gnMI);
			#endif
			// jcm.patch1.end.1/14/97
		MonsterStruct * pMonster = &monster[gnMI];
 		if (!(pMonster->_mFlags & MFLAG_INVISIBLE)) {
			if (pMonster->MType == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Monster \"%s\": uninitialized monster",pMonster->mName);
				#endif
				// jcm.patch1.end.1/14/97
			pxp = pMonster->_mxoff + xp - pMonster->MType->mAnimWidth2;
			pyp = pMonster->_myoff + yp;
			#if RLE_DRAW
			if (gnMI == cursmonst)
				DrawUnitOutline(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					0,
					ev2
				);
			#else
			if (gnMI == cursmonst)
				OutlineSlabCel(
					MOUTC,
					pxp,
					pyp,
					pMonster->_mAnimData,
					pMonster->_mAnimFrame,
					pMonster->MType->mAnimWidth,
					0,
					ev2
				);
			#endif
			DrawMSlabCelL(sx, sy, pxp, pyp, gnMI, 0, ev2);
			if (chflag && !pMonster->_meflag)
				DrawEFlag3(pTo-64, sx-1, sy+1, ev, ev2, xp-64, yp);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bFlags & BFLAG_MISSILE)
		DrawMissile(sx, sy, xp, yp, 0, ev2, FALSE);
	if (bObject && (nLVal < lightmax))
		DrawObjCel(sx, sy, xp, yp, FALSE, 0, ev2);
	if (bItem) {
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		do {
		#endif
		// jcm.patch1.end.1/14/97
		ItemStruct * pItem = &item[bItem - 1];
		if (pItem->_iPostDraw) {
			app_assert(bItem <= MAXITEMS && bItem >= 0);
			// jcm.patch1.start.1/14/97
			#ifdef GRACEFUL_EXIT
			if (bItem > MAXITEMS || bItem < 0)
				break;
			#endif
			// jcm.patch1.end.1/14/97
			if (pItem->_iAnimData == NULL)
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal("Draw Item \"%s\" 2: NULL Cel Buffer",pItem->_iIName);
				#endif
				// jcm.patch1.end.1/14/97
			if (pItem->_iAnimFrame < 1 || *reinterpret_cast<DWORD*>(pItem->_iAnimData) > MAX_FRAMES
				|| pItem->_iAnimFrame > *reinterpret_cast<long*>(pItem->_iAnimData)) {
				// jcm.patch1.start.1/14/97
				#ifdef GRACEFUL_EXIT
				break;
				#else
				app_fatal(
					"Draw \"%s\" Item 2: frame %d of %d, item type==%d",
					pItem->_iIName,
					pItem->_iAnimFrame,
					*reinterpret_cast<long*>(pItem->_iAnimData),
					pItem->_itype
				);
				#endif
				// jcm.patch1.end.1/14/97
			}
			pxp = xp - pItem->_iAnimWidth2;
			if ((bItem - 1) == cursitem
				|| HighLightAllItems == true)
				OutlineSlabCel(
					IOUTC,
					pxp,
					yp,
					pItem->_iAnimData,
					pItem->_iAnimFrame,
					pItem->_iAnimWidth,
					0,
					ev2
				);
			DrawSlabCelL(
				pxp,
				yp,
				pItem->_iAnimData,
				pItem->_iAnimFrame,
				pItem->_iAnimWidth,
				0,
				ev2
			);
		}
		// jcm.patch1.start.1/14/97
		#ifdef GRACEFUL_EXIT
		} while (0);
		#endif
		// jcm.patch1.end.1/14/97
	}
	if (bSpecial) {
		nTrans = TransList[bTransVal];
		TDrawSlabCelPL(pTo, pSpecialCels, bSpecial, 64, 0, ev2);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawHTLX3 (int sx, int sy, int xp, int yp, int nd, int ev, int halfflag)
{
	int	i;
	BYTE *pTo;
	int ev2;
	int t;
	MICROS *pmt;
	WORD *mt;

	app_assert(gpBuffer);
	pmt = &dMT2[CalcRot(sx,sy)];
	ev2 = (ev + 1) << 1;
	if (ev2 > 8) ev2 = 8;
	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gnPieceNum = dPiece[sx][sy]; 
			nLVal = dLight[sx][sy];
			if (gnPieceNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp + 32;
				nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
				mt = &pmt->mt[0];
				if ((ev >= 0) && (gdwPNum = mt[1])) {
					gbPartialTrans = PART_TRANS_RIGHT;
					DrawMTileClipTop (pTo);
					gbPartialTrans = PART_TRANS_NONE;
				}
				pTo -= NBUFFWSL5;
				if ((ev >= 1) && (gdwPNum = mt[3]))
					DrawMTileClipTop (pTo);
				pTo -= NBUFFWSL5;
				if ((ev >= 2) && (gdwPNum = mt[5]))
					DrawMTileClipTop (pTo);
				pTo -= NBUFFWSL5;
				if ((ev >= 3) && (gdwPNum = mt[7]))
					DrawMTileClipTop (pTo);
				//pTo -= NBUFFWSL5;
				//if ((ev >= 4) && (gdwPNum = mt[9]) && (leveltype == 4)) DrawMTileClipTop (pTo);

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				DrawHTLXsub3 (pTo, sx, sy, ev, ev2, xp, yp, FALSE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				DrawBlankMTile(pTo);
			}
		}
		++sx;
		--sy;
		xp += 64;
		--nd;
		++pmt;
	}

	for (i = 0; i < nd; ++i) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gnPieceNum = dPiece[sx][sy]; 
			nLVal = dLight[sx][sy];
			if (gnPieceNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
				mt = &pmt->mt[0];

				gbPartialTrans = PART_TRANS_LEFT;
				if (gdwPNum = mt[0]) DrawMTileClipTop (pTo);
				gbPartialTrans = PART_TRANS_RIGHT;
				if (gdwPNum = mt[1]) DrawMTileClipTop (pTo+32);
				gbPartialTrans = PART_TRANS_NONE;
				for(t = 1; t < ((MicroTileLen>>1)-1); ++t)
				{
					pTo -= NBUFFWSL5;
					if (ev >= t) {
						if (gdwPNum = mt[2*t]) DrawMTileClipTop (pTo);
						if (gdwPNum = mt[2*t+1]) DrawMTileClipTop (pTo+32);
					}
				}

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				DrawHTLXsub3 (pTo, sx, sy, ev, ev2, xp, yp, TRUE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				DrawBlankMTile(pTo);
			}
		}
		++sx;
		--sy;
		xp += 64;
		++pmt;
	}

	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gnPieceNum = dPiece[sx][sy]; 
			nLVal = dLight[sx][sy];
			if (gnPieceNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				nTrans = TransList[dTransVal[sx][sy]] & nTransTable[gnPieceNum];
				mt = &pmt->mt[0];

				gbPartialTrans = PART_TRANS_LEFT;
				if ((ev >= 0) && (gdwPNum = mt[0])) DrawMTileClipTop (pTo);
				gbPartialTrans = PART_TRANS_NONE;
				pTo -= NBUFFWSL5;
				if ((ev >= 1) && (gdwPNum = mt[2])) DrawMTileClipTop (pTo);
				pTo -= NBUFFWSL5;
				if ((ev >= 2) && (gdwPNum = mt[4])) DrawMTileClipTop (pTo);
				pTo -= NBUFFWSL5;
				if ((ev >= 3) && (gdwPNum = mt[6])) DrawMTileClipTop (pTo);
				//pTo -= NBUFFWSL5;
				//if ((ev >= 4) && (gdwPNum = mt[8]) && (leveltype == 4)) DrawMTileClipTop (pTo);

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				DrawHTLXsub3 (pTo, sx, sy, ev, ev2, xp, yp, FALSE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				DrawBlankMTile(pTo);
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void SVGADrawView(int StartX, int StartY)
{
	int	xpos, ypos;
	int	i;
	int width,height;

	ViewDX = 640;
	ViewDY = 352;
	ViewBX = 10;
	ViewBY = 11;

	xpos = 64 + ScrollInfo._sxoff;
	ypos = 175 + ScrollInfo._syoff;
	StartX -= 10;
	StartY -= 1;
	width = 10;
	height = 8;

	if (chrflag || questlog) {
		StartX += 2;
		StartY -= 2;
		xpos += 288;
		width = 6;
	}
	if (invflag || sbookflag) {
		StartX += 2;
		StartY -= 2;
		xpos -= 32;
		width = 6;
	}

	switch (ScrollInfo._sdir) {
		case SCRL_UR :
			++width;
		case SCRL_U :
			ypos -= 32;
			--StartX;
			--StartY;
			height;
			break;
		case SCRL_DR :
			++height;
		case SCRL_R :
			++width;
			break;
		case SCRL_D :
			++height;
			break;
		case SCRL_DL :
			++height;
		case SCRL_L :
			xpos -= 64;
			--StartX;
			++StartY;
			++width;
			break;
		case SCRL_UL :
			xpos -= 64;
			ypos -= 32;
			StartX -= 2;
			++width;
			++height;
			break;
//		case SCRL_NONE :
	}

	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[160];
	for (i = 0; i < 4; ++i) {
		DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 0);
		++StartY;
		xpos -= 32;
		ypos += 16;
		DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 1);
		++StartX;
		xpos += 32;
		ypos += 16;
	}
	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[512];
	for (i = 0; i < height; ++i) {
		DrawHTileLineX(StartX, StartY, xpos, ypos, width, 0);
		++StartY;
		xpos -= 32;
		ypos += 16;
		DrawHTileLineX(StartX, StartY, xpos, ypos, width, 1);
		++StartX;
		xpos += 32;
		ypos += 16;
	}
	gbPartialTrans = PART_TRANS_NONE;
	for (i = 0; i < 4; ++i) {
		DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 0);
		++StartY;
		xpos -= 32;
		ypos += 16;
		DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 1);
		++StartX;
		xpos += 32;
		ypos += 16;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void VGADrawView (int StartX, int StartY)
{
	int	xpos, ypos;
	int	i;
	int width,height;
	long csrc, cdest, cw;

	ViewDX = 384;
	ViewDY = 192;
	ViewBX = 6;
	ViewBY = 6;

	xpos = 64 + ScrollInfo._sxoff;
	ypos = 143 + ScrollInfo._syoff;
	StartX -= 6;
	StartY -= 1;
	width = 6;
	height = 3;

	switch (ScrollInfo._sdir) {
		case SCRL_UR :
			++width;
		case SCRL_U :
			ypos -= 32;
			--StartX;
			--StartY;
			++height;
			break;
		case SCRL_DR :
			++height;
		case SCRL_R :
			++width;
			break;
		case SCRL_D :
			++height;
			break;
		case SCRL_DL :
			++height;
		case SCRL_L :
			xpos -= 64;
			--StartX;
			++StartY;
			++width;
			break;
		case SCRL_UL :
			xpos -= 64;
			ypos -= 32;
			StartX -= 2;
			++width;
			++height;
//		case SCRL_NONE :
	}

	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[143];
	for (i = 0; i < 4; ++i) {
		DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 0);
		++StartY;
		xpos -= 32;
		ypos += 16;
		DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 1);
		++StartX;
		xpos += 32;
		ypos += 16;
	}
	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[320];
	for (i = 0; i < height; ++i) {
		DrawHTileLineX(StartX, StartY, xpos, ypos, width, 0);
		++StartY;
		xpos -= 32;
		ypos += 16;
		DrawHTileLineX(StartX, StartY, xpos, ypos, width, 1);
		++StartX;
		xpos += 32;
		ypos += 16;
	}
	gbPartialTrans = PART_TRANS_NONE;
	for (i = 0; i < 4; ++i) {
		DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 0);
		++StartY;
		xpos -= 32;
		ypos += 16;
		DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 1);
		++StartX;
		xpos += 32;
		ypos += 16;
	}

	if (chrflag || questlog) {
		csrc = 245168;
		cdest = 392064;
		cw = 160;
	} else {
		if (invflag || sbookflag) {
			csrc = 245168;
			cdest = 391744;
			cw = 160;
		} else {
			csrc = 245088;
			cdest = 391744;
			cw = 320;
		}
	}
	// Double res copy
	app_assert(gpBuffer);
	__asm {
		mov		esi,[gpBuffer]
		mov		edx,[cdest]
		mov		edi,esi
		mov		ecx,[csrc]
		add		edi,edx
		add		esi,ecx
		mov		ebx,edi
		add		ebx,768

		mov		edx,176
_YLp:
		mov		ecx,[cw]
_XLp:
		mov		al,[esi]
		inc		esi
		mov		ah,al
		mov		[edi],ax
		mov		[ebx],ax
		add		edi,2
		add		ebx,2
		dec		ecx
		jnz		_XLp
		mov		eax,768
		add		eax,[cw]
		sub		esi,eax
		add		eax,eax
		sub		ebx,eax
		sub		edi,eax
		dec		edx
		jnz		_YLp
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawView(int StartX, int StartY)
{
	app_assert(gpBuffer);
/*	__asm {
		mov		edi,dword ptr [gpBuffer]
		add		edi,122944

		mov		edx,352
		mov	eax,092929292h	// yellow
		//xor		eax,eax			// black
_YLp:	mov		ecx,160
		rep stosd
		add		edi,128
		dec		edx
		jnz		_YLp
	}*/

	if (svgamode) SVGADrawView (StartX, StartY);
	else VGADrawView (StartX, StartY);
	if (automapflag) DrawAutomap();

	if (invflag) DrawInv();
	else if (sbookflag) DrawSpellBook();

	DrawDurIcon();

	if (chrflag) DrawChr();
	else if (questlog) DrawQuestLog();
	else if ((plr[myplr]._pStatPts != 0) && (!spselflag)) DrawLevelUpIcon();

	if (uitemflag) DrawUniqueInfo();
	if (qtextflag) DrawQText();
	if (spselflag) DrawSpellList();
	if (dropGoldFlag) DrawGoldBox(dropGoldValue);
	if (helpflag) DrawHelp();
	if (msgflag) DrawDiabloMsg();
	if (deathflag) RedBack();
	else if (PauseMode) DrawPause();

	plrmsg_draw();

	gmenu_draw();

	DrawMapOfDoom();

	DrawInfoBox();
	DrawHealthTop();
	DrawManaTop();

	/* TEMP! ONLY ENABLE FOR TESTING
	if (automapflag) {
		if (leveltype == 1)
			DrawDungMiniMap(22);
		if (leveltype == 2)
			DrawDungMiniMap(12);
		if (leveltype == 3)
			DrawDungMiniMap(8); 
		if (leveltype == 4) 
			DrawDungMiniMap(30); 
	}
	TEMP! ONLY ENABLE FOR TESTING */
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ClrDraw()
{
	// Clear drawing area with solid color to detect transparency problems
	lock_buf(3);
	app_assert(gpBuffer);
	__asm {
		mov		edi,dword ptr [gpBuffer]
		add		edi,122944

		mov		edx,480
		xor		eax,eax			// black
_YLp:	mov		ecx,160
		rep stosd
		add		edi,128
		dec		edx
		jnz		_YLp
	}
	unlock_buf(3);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckForScroll ()
{
	if (curs >= ICSTART) return;

	BOOL bScrollUpdate = FALSE;
	if (MouseX < 20) {
		if ((ViewY < (dmaxy - 1)) && (ViewX > dminx)) {
			++ViewY;
			--ViewX;
			bScrollUpdate = TRUE;
		} else {
			if (ViewY < (dmaxy - 1)) {
				++ViewY;
				bScrollUpdate = TRUE;
			}
			if (ViewX > dminx) {
				--ViewX;
				bScrollUpdate = TRUE;
			}
		}
	}

	if (MouseX > 620) {
		if ((ViewX < (dmaxx - 1)) && (ViewY > dminy)) {
			--ViewY;
			++ViewX;
			bScrollUpdate = TRUE;
		} else {
			if (ViewX < (dmaxx - 1)) {
				++ViewX;
				bScrollUpdate = TRUE;
			}
			if (ViewY > dminy) {
				ViewY--;
				bScrollUpdate = TRUE;
			}
		}
	}

	if (MouseY < 20) {
		if ((ViewY > dminy) && (ViewX > dminx)) {
			--ViewX;
			--ViewY;
			bScrollUpdate = TRUE;
		} else {
			if (ViewY > dminy) {
				--ViewY;
				bScrollUpdate = TRUE;
			}
			if (ViewX > dminx) {
				--ViewX;
				bScrollUpdate = TRUE;
			}
		}
	}

	if (MouseY > 460) {
		if ((ViewY < (dmaxy - 1)) && (ViewX < (dmaxx - 1))) {
			++ViewX;
			++ViewY;
			bScrollUpdate = TRUE;
		} else {
			if (ViewY < (dmaxy - 1)) {
				++ViewY;
				bScrollUpdate = TRUE;
			}
			if (ViewX < (dmaxx - 1)) {
				++ViewX;
				bScrollUpdate = TRUE;
			}
		}
	}
	
	if (bScrollUpdate)
		ScrollInfo._sdir = SCRL_NONE;
}


//*******************************************************************
//*******************************************************************
#ifndef NDEBUG
void toggle_frame_counter() {
	sgfFrameCounterEnabled = !sgfFrameCounterEnabled;
	sgnLastFrame = GetTickCount();
}
#endif


//*******************************************************************
//*******************************************************************
#ifndef NDEBUG
static void draw_frame_rate() {
	if (!sgfFrameCounterEnabled)
		return;
	// don't write stuff if we're not the active application
	if (! bActive)
		return;

	static int snFrameCount = 0;
	static int nFrameRate = 0;
	int nTimeSpent;
	char szFrameRate[10];
	DWORD nThisFrame;
	HDC hDC;

	++snFrameCount;
	nThisFrame = GetTickCount();
	//give a 1 second average frame rate
	if (nThisFrame - sgnLastFrame >= 1000) {
		nTimeSpent = nThisFrame - sgnLastFrame;
		sgnLastFrame = nThisFrame;
		nFrameRate = (snFrameCount*1000)/nTimeSpent;
		snFrameCount = 0;
	}

	if (nFrameRate > 99) nFrameRate = 99;
	wsprintf(szFrameRate, "%2d", nFrameRate);
	HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
	if (ddr != DD_OK) return;
	TextOut(hDC,0,400,szFrameRate,strlen(szFrameRate));
	lpDDSPrimary->ReleaseDC(hDC);
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define BUFFER_STARTX	64
#define BUFFER_STARTY	160
static DDSURFACEDESC sgDDSD;
extern LPDIRECTDRAWSURFACE lpDDSBackBuf;
static void DDBlit(DWORD dwX,DWORD dwY,DWORD dwWdt,DWORD dwHgt) {

	app_assert(! (dwX   & 3));
	app_assert(! (dwWdt & 3));

	// if we have a back buffer, it is because we cannot lock the
	// primary surface.  Use BltFast to avoid locking problem
	if (lpDDSBackBuf) {
		HRESULT ddrval;
		RECT r;
		r.left = dwX + BUFFER_STARTX;
		r.top = dwY + BUFFER_STARTY;
		r.right = r.left + dwWdt - 1;
		r.bottom = r.top + dwHgt - 1;

		// make sure nobody is holding a lock on back surface
		// because we wouldn't be able to BltFast
		app_assert(! gpBuffer);

		DWORD dwStartTime = GetTickCount();
		while (1) {
			// perform draw
			#if LOCK_WAIT
			ddrval = lpDDSPrimary->BltFast(dwX,dwY,lpDDSBackBuf,&r,DDBLTFAST_WAIT);
			#else
			ddrval = lpDDSPrimary->BltFast(dwX,dwY,lpDDSBackBuf,&r,0);
			#endif
			if (ddrval == DD_OK) break;

			// fatal if we've waited a long time for surface
			if (dwStartTime - GetTickCount() > 5*1000) break;

			#if LOCK_SLEEP
			Sleep(LOCK_SLEEP);
			#endif

			// hey, we just checked a second ago, and we still
			// had our surface -- try to get it next time
			if (ddrval == DDERR_SURFACELOST) return;

			// handle errors related to another thread locking surface
			if (ddrval == DDERR_WASSTILLDRAWING) continue;
			if (ddrval == DDERR_SURFACEBUSY) continue;

			// other errors -- fatal
			break;
		}

		if (ddrval == DDERR_SURFACELOST) return;
		if (ddrval == DDERR_WASSTILLDRAWING) return;
		if (ddrval == DDERR_SURFACEBUSY) return;
		ddraw_assert(ddrval);
		return;
	}

	LONG lSrcOff = (dwY + BUFFER_STARTY) * BUFFERX + dwX + BUFFER_STARTX;
	LONG lDstOff = dwY * sgDDSD.lPitch + dwX;
	LONG lSrcMod = BUFFERX - dwWdt;
	LONG lDstMod = sgDDSD.lPitch - dwWdt;
	dwWdt >>= 2;
	lock_buf(6);
	app_assert(gpBuffer);
	__asm {
		mov		esi, [gpBuffer]
		mov		edi, [sgDDSD.lpSurface]
		add		esi, [lSrcOff]
		add		edi, [lDstOff]
		mov		eax, [lSrcMod]
		mov		ebx, [lDstMod]
		mov		edx, [dwHgt]
	lpdword:
		mov		ecx, [dwWdt]
		rep		movsd
		add		esi, eax
		add		edi, ebx
		dec		edx
		jnz		lpdword
	}
	unlock_buf(6);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DirectDrawBlt(
	long ysize,
	BOOL tbox,
	BOOL hp,
	BOOL mana,
	BOOL spdbar,
	BOOL btns
) {
    HRESULT ddrval;

	// don't redraw if we aren't the active application
    if (! bActive) return;

	// don't redraw if we don't have a direct draw buffer
	if (! lpDDSPrimary) return;

	// make sure primary surface is still in video memory
	if (lpDDSPrimary->IsLost() == DDERR_SURFACELOST) {
		ddrval = lpDDSPrimary->Restore();
		if (ddrval != DD_OK) return;
		void ResetPal();
		ResetPal();

		// force redraw of everything
		ysize = TOTALY;
	}

	// lock the surface if we have to manually blit
	if (! lpDDSBackBuf) {
		BOOL bReinit = TRUE;
		DWORD dwStartTime = GetTickCount();
		while (1) {
			// try to lock the surface
			sgDDSD.dwSize = sizeof(sgDDSD);
			#if LOCK_WAIT
			ddrval = lpDDSPrimary->Lock(NULL,&sgDDSD,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
			#else
			ddrval = lpDDSPrimary->Lock(NULL,&sgDDSD,DDLOCK_WRITEONLY,NULL);
			#endif
			if (ddrval == DD_OK) break;

			// fatal if we've waited a long time for surface
			if (dwStartTime - GetTickCount() > 5*1000) break;

			#if LOCK_SLEEP
			Sleep(LOCK_SLEEP);
			#endif

			// hey, we just checked a second ago, and we still
			// had our surface -- try to get it next time
			if (ddrval == DDERR_SURFACELOST) return;

			// handle errors related to another thread locking surface
			if (ddrval == DDERR_WASSTILLDRAWING) continue;
			if (ddrval == DDERR_SURFACEBUSY) continue;

			// this is to fix a bug when user switches to a different
			// screen mode in another app.  In windows NT, a DOS app
			// switching to Mode 0x13 will cause a DDERR_GENERIC in
			// this app when it is restored
			if (bReinit && ddrval == DDERR_GENERIC) {
				bReinit = FALSE;
				void ddraw_reinit();
				ddraw_reinit();
				ysize = TOTALY;
				dwStartTime = GetTickCount();
				continue;
			}

			// other errors -- fatal
			break;
		}

		// handle errors related to another thread locking surface
		if (ddrval == DDERR_SURFACELOST) return;
		if (ddrval == DDERR_WASSTILLDRAWING) return;
		if (ddrval == DDERR_SURFACEBUSY) return;

		ddraw_assert(ddrval);
	}

	app_assert(ysize >= 0 && ysize <= 480);

	if (ysize > 0) {
		// blit main game area
		DDBlit(0,0,160*4,ysize);		// X is divisible by 4
	}

	if (ysize < TOTALY) {
		if (spdbar) {
			// speed bar
			// DDBlit(205,357,58*4,28);
			DDBlit(204,357,58*4,28);	// X is divisible by 4
		}
	
		if (tbox) {
			// text box
			// DDBlit(177,398,72*4,60);
			DDBlit(176,398,72*4,60);	// X is divisible by 4
		}

		if (mana) {
			// mana bar
			// DDBlit(461,352,22*4,72);
			DDBlit(460,352,22*4,72);	// X is divisible by 4

			// spell icon
			// DDBlit(565,416,14*4,56);
			DDBlit(564,416,14*4,56);	// X is divisible by 4
		}

		if (hp) {
			// hitpoint bar
			DDBlit(96,352,22*4,72);		// X is divisible by 4
		}

		if (btns) {
			// buttons on left side
			// DDBlit(11,357,18*4,119);
			DDBlit(8,357,18*4,119);		// X is divisible by 4

			// buttons on right side
			// DDBlit(558,357,18*4,42);
			DDBlit(556,357,18*4,48);	// X is divisible by 4

			if (gbMaxPlayers > 1) {
				// "chat" button
				// DDBlit(87,443,9*4,32);
				DDBlit(84,443,9*4,32);	// X is divisible by 4

				// "friendly" button
				// DDBlit(527,443,9*4,32);
				DDBlit(524,443,9*4,32);	// X is divisible by 4
			}
		}

		if (sgdwOldWdt) {
			DDBlit(sgdwOldX,sgdwOldY,sgdwOldWdt,sgdwOldHgt);
		}
		if (sgdwCursWdt) {
			DDBlit(sgdwCursX,sgdwCursY,sgdwCursWdt,sgdwCursHgt);
		}

	}

	if (! lpDDSBackBuf) {
		// in NT, it is possible for us to lose the surface
		// even when it is locked!
		ddrval = lpDDSPrimary->Unlock(NULL);
		if (ddrval != DDERR_SURFACELOST)
			ddraw_assert(ddrval);
	}

	#ifndef NDEBUG
	draw_frame_rate();
	#endif
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void FullBlit(BOOL bArrow) {
	int y;
	if (force_redraw == FULLDRAW) {
		force_redraw = NODRAW;
		y = TOTALY;
	}
	else {
		y = 0;
	}

	if (bArrow) {
		lock_buf(0);
		savecrsr_show();
		unlock_buf(0);
	}

	DirectDrawBlt(y, FALSE, FALSE, FALSE, FALSE, FALSE);

	if (bArrow) {
		lock_buf(0);
		savecrsr_hide();
		unlock_buf(0);
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
extern BOOL gbRunGame;

void DrawAndBlit(void) {
	BOOL tBox;
	BOOL ctrlPan;
	long ysize;

	if (! gbRunGame) return;

	if (force_redraw == FULLDRAW) {
		drawhpflag = TRUE;
		drawmanaflag = TRUE;
		drawbtnflag = TRUE;
		drawsbarflag = TRUE;
		tBox = FALSE;
		ctrlPan = TRUE;
		ysize = TOTALY;
	}
	else if (force_redraw == VIEWDRAW) {
		tBox = TRUE;
		ctrlPan = FALSE;
		ysize = GAMEY;
	}
	else {
		return;
	}
	force_redraw = NODRAW;

	lock_buf(0);
	if (leveltype) DrawView(ViewX, ViewY);
	else T_DrawView(ViewX, ViewY);
	if (ctrlPan) DrawCtrlPan();
	if (drawhpflag) DrawHealthBar();
	if (drawmanaflag) DrawManaBar();
	if (drawbtnflag) DrawButtons();
	if (drawsbarflag) DrawSpdBar();
	if (talkflag) {
		DrawTalkBox();
		ysize = TOTALY;
	}
	savecrsr_show();
	unlock_buf(0);

	DirectDrawBlt(ysize, tBox, drawhpflag, drawmanaflag, drawsbarflag, drawbtnflag);

	lock_buf(0);
	savecrsr_hide();
	unlock_buf(0);

	drawhpflag = FALSE;
	drawmanaflag = FALSE;
	drawbtnflag = FALSE;
	drawsbarflag = FALSE;
}
