/*-----------------------------------------------------------------------**
** 								Diablo
**
**							   Town file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/TOWN.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"engine.h"
#include	"scrollrt.h"
#include	"scrlasm.h"
#include	"gendung.h"

#include	"items.h"
#include	"cursor.h"
#include	"trigs.h"
#include	"control.h"
#include	"gamemenu.h"

#include	"player.h"
#include	"monster.h"

#include	"town.h"
#include	"towners.h"
#include	"inv.h"
#include	"quests.h"
#include	"minitext.h"
#include	"stores.h"
#include	"effects.h"

#include	"automap.h"
#include	"help.h"
#include	"error.h"
#include	"doom.h"
#include	"multi.h"

#define T_POUTC	165

// #define TESTINGCRYPT		// JKE set this to keep the crypt open

#if RLE_DRAW
void DrawUnit (long xp,long yp,BYTE *pCelBuff,long nCel,long nCelW,long ostart,long oend);
void DrawUnitClipped (long xp,long yp,BYTE *pCelBuff,long nCel,long nCelW,long ostart,long oend);
void DrawUnitOutline(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend);
void DrawUnitOutlineClipped(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend);
#endif

static void T_DrawHTLXsub (BYTE *pTo, int sx, int sy, int xp, int yp, BOOL chflag);
static void T_DrawHTLXsub2 (BYTE *pTo, int sx, int sy, int sv, int sv2, int xp, int yp, BOOL chflag);
static void T_DrawHTLXsub3 (BYTE *pTo, int sx, int sy, int ev, int ev2, int xp, int yp, BOOL chflag);
void plrmsg_draw();

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TDrawBlankMini(BYTE *pDecodeTo)
{
	app_assert(gpBuffer);
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		edx,30
		mov		ebx,1
		xor		eax,eax
		//mov		eax,0d3d3d3d3h
_BLp1:	cmp		edi,dword ptr [glClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,ebx
		rep stosd
		add		edi,edx
		sub		edi,NBUFFW64
		or		edx,edx
		jz		_Bb
		sub		edx,2
		inc		ebx
		jmp		_BLp1

_Bb:	mov		edx,2
		mov		ebx,15
_BLp2:	cmp		edi,dword ptr [glClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,ebx
		rep stosd
		add		edi,edx
		sub		edi,NBUFFW64
		dec		ebx
		add		edx,2
		cmp		edx,32
		jnz		_BLp2
_Done:	nop
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TCDrawBlankMini(BYTE *pDecodeTo)
{
	app_assert(gpBuffer);
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		edx,30
		mov		ebx,1
		xor		eax,eax
		//mov		eax,0d3d3d3d3h
_BLp1:	cmp		edi,dword ptr [glClipY]
		jb		_C1
		add		edi,64
		jmp		_C2
_C1:	add		edi,edx
		mov		ecx,ebx
		rep stosd
		add		edi,edx
_C2:	sub		edi,NBUFFW64
		or		edx,edx
		jz		_Bb
		sub		edx,2
		inc		ebx
		jmp		_BLp1

_Bb:	mov		edx,2
		mov		ebx,15
_BLp2:	cmp		edi,dword ptr [glClipY]
		jb		_C3
		add		edi,64
		jmp		_C4
_C3:	add		edi,edx
		mov		ecx,ebx
		rep stosd
		add		edi,edx
_C4:	sub		edi,NBUFFW64
		dec		ebx
		add		edx,2
		cmp		edx,32
		jnz		_BLp2
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_CDrawSpecial(BYTE *pTo, long nCel)
{
/*
	long RLELen;
	long nBufferW;
	app_assert(gpBuffer);
	__asm {
		mov		ebx,dword ptr [pSpecialCels]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax
		mov		eax,dword ptr [ebx+4]
		sub		eax,dword ptr [ebx]
		mov		dword ptr [RLELen],eax
		mov		esi,dword ptr [pSpecialCels]
		add		esi,dword ptr [ebx]

		mov		edi,dword ptr [pTo]				// Dest

		mov		eax,832							// Increase width
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [RLELen]
		add		ebx,esi

_T1Lp1:	mov		edx,64

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [glClipY]
		jb		_T1C1
		add		esi,eax
		add		edi,eax
		jmp		_T1x
_T1C1:	mov		ecx,eax
		shr		ecx,1
		jnc		_T1w
		movsb
		jecxz	_T1x
_T1w:	shr		ecx,1
		jnc		_T1Lp3
		movsw
		jecxz	_T1x
_T1Lp3:	rep movsd
_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,dword ptr [nBufferW]
		cmp		ebx,esi
		jnz		_T1Lp1
	}
*/
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawSpecial(BYTE *pTo, int nCel)
{
/*
	long RLELen;
	long nBufferW;
	app_assert(gpBuffer);
	__asm {
		mov		ebx,dword ptr [pSpecialCels]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax
		mov		eax,dword ptr [ebx+4]
		sub		eax,dword ptr [ebx]
		mov		dword ptr [RLELen],eax
		mov		esi,dword ptr [pSpecialCels]
		add		esi,dword ptr [ebx]

		mov		edi,dword ptr [pTo]				// Dest

		mov		eax,832							// Increase width
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [RLELen]
		add		ebx,esi

_T1Lp1:	mov		edx,64

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [glClipY]
		jb		_Done
		mov		ecx,eax
		shr		ecx,1
		jnc		_T1w
		movsb
		jecxz	_T1x
_T1w:	shr		ecx,1
		jnc		_T1Lp3
		movsw
		jecxz	_T1x
_T1Lp3:	rep movsd
_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,dword ptr [nBufferW]
		cmp		ebx,esi
		jnz		_T1Lp1
_Done:	nop
	}
*/
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawEFlag1(BYTE *pTo2, int sx, int sy, int xp, int yp)
{
	BYTE *pTo;
	int t;
	WORD *mt;

	pTo = pTo2;
	mt = &dMT2[CalcRot(sx,sy)].mt[0];
	for(t = 0; t < 12; t += 2)
	{
		if (gdwPNum = mt[t]) DrawMTileClipBottom (pTo);
		if (gdwPNum = mt[t+1]) DrawMTileClipBottom (pTo+32);
		pTo -= NBUFFWSL5;
	}

	T_DrawHTLXsub (pTo2, sx, sy, xp, yp, FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void T_DrawHTLXsub (BYTE *pTo, int sx, int sy, int xp, int yp, BOOL chflag)
{
	int pxp,pyp;
	char bv;
	int mi;

	app_assert(gpBuffer);
	pTo = gpBuffer + nBuffWTbl[yp] + xp;
	if (dItem[sx][sy] != 0) {
		bv = dItem[sx][sy] - 1;
		pxp = xp - item[bv]._iAnimWidth2;
		if (bv == cursitem) COutlineSlabCel(181, pxp, yp, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, 8);
		CDrawSlabCel(pxp, yp, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, 8);
	}
	if ((dFlags[sx][sy] & BFLAG_MONSTLR) != 0) {
		mi = -(dMonster[sx][sy-1] + 1);
		pxp = xp - towner[mi]._tAnimWidth2;
		pyp = yp;
		if (mi == cursmonst) COutlineSlabCel(166, pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
		CDrawSlabCel(pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
	}
	if (dMonster[sx][sy] > 0) {
		mi = dMonster[sx][sy] - 1;
		pxp = xp - towner[mi]._tAnimWidth2;
		pyp = yp;
		if (mi == cursmonst) COutlineSlabCel(166, pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
		CDrawSlabCel(pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
	}
	if ((dFlags[sx][sy] & BFLAG_PLRLR) != 0) {
		bv = -(dPlayer[sx][sy-1] + 1);
		pxp = plr[bv]._pxoff + xp - plr[bv]._pAnimWidth2;
		pyp = plr[bv]._pyoff + yp;

		#if RLE_DRAW
		if (bv == cursplr) DrawUnitOutlineClipped(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		DrawUnitClipped(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		#else
		if (bv == cursplr) COutlineSlabCel(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		CDrawSlabCel(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		#endif

		if (chflag)
		{
//			if (plr[bv]._peflag == 2) T_DrawEFlag1(pTo-NBUFFWSL4+96, sx-2, sy+1, xp-96, yp-16);
			if (plr[bv]._peflag != 0) T_DrawEFlag1(pTo-64, sx-1, sy+1, xp-64, yp);
		}
	}
	if (dFlags[sx][sy] & BFLAG_DEADPLR)
		DrawDeadPlr(sx, sy, xp, yp, 0, 8, TRUE);
	if (dPlayer[sx][sy] > 0) {
		bv = dPlayer[sx][sy] - 1;
		pxp = plr[bv]._pxoff + xp - plr[bv]._pAnimWidth2;
		pyp = plr[bv]._pyoff + yp;

		#if RLE_DRAW
		if (bv == cursplr) DrawUnitOutlineClipped(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		DrawUnitClipped(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		#else
		if (bv == cursplr) COutlineSlabCel(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		CDrawSlabCel(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		#endif

		if (chflag)
		{
//			if (plr[bv]._peflag == 2) T_DrawEFlag1(pTo-NBUFFWSL4+96, sx-2, sy+1, xp-96, yp-16);
			if (plr[bv]._peflag != 0) T_DrawEFlag1(pTo-64, sx-1, sy+1, xp-64, yp);
		}
	}
	if ((dFlags[sx][sy] & BFLAG_MISSILE) != 0) CDrawMissile(sx, sy, xp, yp, 0, 8, FALSE);
	if (dSpecial[sx][sy]) T_CDrawSpecial(pTo, dSpecial[sx][sy]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawHTileLineX (int sx, int sy, int xp, int yp, int nd, int halfflag)
{
	int	i;
	BYTE *pTo;
	int t;
	WORD *mt;

	app_assert(gpBuffer);
	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			if (gdwPNum = dPiece[sx][sy]) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp + 32;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];

				for(t = 1; t < 17; t += 2)
				{
					if (gdwPNum = mt[t]) DrawMTileClipBottom (pTo);
					pTo -= NBUFFWSL5;
				}

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				T_DrawHTLXsub (pTo, sx, sy, xp, yp, FALSE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TCDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TCDrawBlankMini(pTo);
		}
		sx++;
		sy--;
		xp += 64;
	}

	nd -= halfflag;
	for (i = 0; i < nd; i++) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gdwPNum = dPiece[sx][sy];
			if (gdwPNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];

				for(t = 0; t < 16; t+= 2)
				{
					if (gdwPNum = mt[t]) DrawMTileClipBottom (pTo);
					if (gdwPNum = mt[t+1]) DrawMTileClipBottom (pTo+32);
					pTo -= NBUFFWSL5;
				}

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				T_DrawHTLXsub (pTo, sx, sy, xp, yp, TRUE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TCDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TCDrawBlankMini(pTo);
		}
		sx++;
		sy--;
		xp += 64;
	}
	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gdwPNum = dPiece[sx][sy];
			if (gdwPNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];

				for(t = 0; t < 16; t+= 2)
				{
					if (gdwPNum = mt[t]) DrawMTileClipBottom (pTo);
					pTo -= NBUFFWSL5;
				}

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				T_DrawHTLXsub (pTo, sx, sy, xp, yp, FALSE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TCDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TCDrawBlankMini(pTo);
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawEFlag2(BYTE *pTo2, int sx, int sy, int sv, int sv2, int xp, int yp)
{
	BYTE *pTo;
	int t;
	WORD *mt;

	if (sv == 0) pTo = pTo2;
	else
		pTo = pTo2 + (NBUFFWSL5 * sv);
	mt = &dMT2[CalcRot(sx,sy)].mt[0];

	for(t = 0; t < 6; t++)
	{
		if (sv <= t) {
			if (gdwPNum = mt[2*t+2]) DrawMTileClipBottom (pTo);
			if (gdwPNum = mt[2*t+3]) DrawMTileClipBottom (pTo+32);
		}
		pTo -= NBUFFWSL5;
	}

	if (sv2 < 8) {
		T_DrawHTLXsub2 (pTo2, sx, sy, sv, sv2, xp, yp, FALSE);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void T_DrawHTLXsub2 (BYTE *pTo, int sx, int sy, int sv, int sv2, int xp, int yp, BOOL chflag)
{
	int pxp,pyp;
	char bv;
	int mi;

	if (dItem[sx][sy] != 0) {
		bv = dItem[sx][sy] - 1;
		pxp = xp - item[bv]._iAnimWidth2;
		if (bv == cursitem) COutlineSlabCel(181, pxp, yp, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, sv2, 8);
		CDrawSlabCel(pxp, yp, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, sv2, 8);
	}
	if ((dFlags[sx][sy] & BFLAG_MONSTLR) != 0) {
		mi = -(dMonster[sx][sy-1] + 1);
		pxp = xp - towner[mi]._tAnimWidth2;
		pyp = yp;
		if (mi == cursmonst) COutlineSlabCel(166, pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, sv2, 8);
		CDrawSlabCel(pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, sv2, 8);
	}
	if (dMonster[sx][sy] > 0) {
		mi = dMonster[sx][sy] - 1;
		pxp = xp - towner[mi]._tAnimWidth2;
		pyp = yp;
		if (mi == cursmonst) COutlineSlabCel(166, pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, sv2, 8);
		CDrawSlabCel(pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, sv2, 8);
	}
	if ((dFlags[sx][sy] & BFLAG_PLRLR) != 0) {
		bv = -(dPlayer[sx][sy-1] + 1);
		pxp = plr[bv]._pxoff + xp - plr[bv]._pAnimWidth2;
		pyp = plr[bv]._pyoff + yp;

		#if RLE_DRAW
		if (bv == cursplr) DrawUnitOutlineClipped(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		DrawUnitClipped(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		#else
		if (bv == cursplr) COutlineSlabCel(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		CDrawSlabCel(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		#endif

		if (chflag)
		{
//			if (plr[bv]._peflag == 2) T_DrawEFlag2(pTo-NBUFFWSL4+96, sx-2, sy+1, sv, sv2, xp-96, yp-16);
			if (plr[bv]._peflag != 0) T_DrawEFlag2(pTo-64, sx-1, sy+1, sv, sv2, xp-64, yp);
		}
	}
	if (dFlags[sx][sy] & BFLAG_DEADPLR)
		DrawDeadPlr(sx, sy, xp, yp, sv2, 8, TRUE);
	if (dPlayer[sx][sy] > 0) {
		bv = dPlayer[sx][sy] - 1;
		pxp = plr[bv]._pxoff + xp - plr[bv]._pAnimWidth2;
		pyp = plr[bv]._pyoff + yp;

		#if RLE_DRAW
		if (bv == cursplr) DrawUnitOutlineClipped(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		DrawUnitClipped(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		#else
		if (bv == cursplr) COutlineSlabCel(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		CDrawSlabCel(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, sv2, 8);
		#endif

		if (chflag)
		{
//			if (plr[bv]._peflag == 2) T_DrawEFlag2(pTo-NBUFFWSL4+96, sx-2, sy+1, sv, sv2, xp-96, yp-16);
			if (plr[bv]._peflag != 0) T_DrawEFlag2(pTo-64, sx-1, sy+1, sv, sv2, xp-64, yp);
		}
	}
	if ((dFlags[sx][sy] & BFLAG_MISSILE) != 0) CDrawMissile(sx, sy, xp, yp, sv2, 8, FALSE);
	if (dSpecial[sx][sy]) T_CDrawSpecial(pTo+nBuffWTbl[sv2<<4], dSpecial[sx][sy]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawHTLX2 (int sx, int sy, int xp, int yp, int nd, int sv, int halfflag)
{
	int	i;
	BYTE *pTo;
	int	sv2;
	int t;
	WORD *mt;

	app_assert(gpBuffer);
	sv2 = (sv + 1) << 1;
	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			if (gdwPNum = dPiece[sx][sy]) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp - NBUFFWSL5 + 32;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];

				for(t = 0; t < 7; t++)
				{
					if ((sv <= t) && (gdwPNum = mt[2*t+3])) DrawMTileClipBottom (pTo);
					pTo -= NBUFFWSL5;
				}

				if (sv2 < 8) {
					pTo = gpBuffer + nBuffWTbl[yp] + xp;
					T_DrawHTLXsub2 (pTo, sx, sy, sv, sv2, xp, yp, FALSE);
				}
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TCDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TCDrawBlankMini(pTo);
		}
		sx++;
		sy--;
		xp += 64;
	}

	nd -= halfflag;
	for (i = 0; i < nd; i++) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gdwPNum = dPiece[sx][sy]; 
			if (gdwPNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp - NBUFFWSL5;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];
				for(t = 0; t < 7; t++)
				{
					if (sv <= t) {
						if (gdwPNum = mt[2*t+2]) DrawMTileClipBottom (pTo);
						if (gdwPNum = mt[2*t+3]) DrawMTileClipBottom (pTo+32);
					}
					pTo -= NBUFFWSL5;
				}

				if (sv2 < 8) {
					pTo = gpBuffer + nBuffWTbl[yp] + xp - (sv2 * NBUFFWSL4);
					T_DrawHTLXsub2 (pTo, sx, sy, sv, sv2, xp, yp, TRUE);
				}																						   
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TCDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TCDrawBlankMini(pTo);
		}
		sx++;
		sy--;
		xp += 64;
	}

	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gdwPNum = dPiece[sx][sy]; 
			if (gdwPNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp - NBUFFWSL5;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];

				for(t = 0; t < 7; t++)
				{
					if ((sv <= t) && (gdwPNum = mt[2*t+2])) DrawMTileClipBottom (pTo);
					pTo -= NBUFFWSL5;
				}

				if (sv2 < 8) {
					pTo = gpBuffer + nBuffWTbl[yp] + xp;
					T_DrawHTLXsub2 (pTo, sx, sy, sv, sv2, xp, yp, FALSE);
				}
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TCDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TCDrawBlankMini(pTo);
		}
	}

}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawEFlag3(BYTE *pTo2, int sx, int sy, int ev, int ev2, int xp, int yp)
{
	BYTE *pTo;
	int t;
	WORD *mt;

	pTo = pTo2;
	mt = &dMT2[CalcRot(sx,sy)].mt[0];

	for(t = 0; t < 7; t++)
	{
		if (ev >= t) {
			if (gdwPNum = mt[2*t]) DrawMTileClipTop (pTo);
			if (gdwPNum = mt[2*t+1]) DrawMTileClipTop (pTo+32);
		}
		pTo -= NBUFFWSL5;
	}

	T_DrawHTLXsub3 (pTo2, sx, sy, ev, ev2, xp, yp, FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void T_DrawHTLXsub3 (BYTE *pTo, int sx, int sy, int ev, int ev2, int xp, int yp, BOOL chflag)
{
	int pxp,pyp;
	char bv;
	int mi;

	if (dItem[sx][sy] != 0) {
		bv = dItem[sx][sy] - 1;
		pxp = xp - item[bv]._iAnimWidth2;
		if (bv == cursitem) OutlineSlabCel(181, pxp, yp, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, ev2);
		app_assert(item[bv]._iAnimData);
		DrawSlabCel(pxp, yp, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, ev2);
	}
	if ((dFlags[sx][sy] & BFLAG_MONSTLR) != 0) {
		mi = -(dMonster[sx][sy-1] + 1);
		pxp = xp - towner[mi]._tAnimWidth2;
		pyp = yp;
		if (mi == cursmonst) OutlineSlabCel(166, pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, ev2);
		app_assert(towner[mi]._tAnimData);
		DrawSlabCel(pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, ev2);
	}
	if (dMonster[sx][sy] > 0) {
		mi = dMonster[sx][sy] - 1;
		pxp = xp - towner[mi]._tAnimWidth2;
		pyp = yp;
		if (mi == cursmonst) OutlineSlabCel(166, pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, ev2);
		app_assert(towner[mi]._tAnimData);
		DrawSlabCel(pxp, pyp, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, ev2);
	}
	if ((dFlags[sx][sy] & BFLAG_PLRLR) != 0) {
		bv = -(dPlayer[sx][sy-1] + 1);
		pxp = plr[bv]._pxoff + xp - plr[bv]._pAnimWidth2;
		pyp = plr[bv]._pyoff + yp;

		#if RLE_DRAW
		if (bv == cursplr) DrawUnitOutline(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		app_assert(plr[bv]._pAnimData);
		DrawUnit(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		#else
		if (bv == cursplr) OutlineSlabCel(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		app_assert(plr[bv]._pAnimData);
		DrawSlabCel(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		#endif

		if (chflag)
		{
//			if (plr[bv]._peflag == 2) T_DrawEFlag3(pTo-NBUFFWSL4-96, sx-2, sy+1, ev, ev2, xp-96, yp-16);
			if (plr[bv]._peflag != 0) T_DrawEFlag3(pTo-64, sx-1, sy+1, ev, ev2, xp-64, yp);
		}
	}
	if (dFlags[sx][sy] & BFLAG_DEADPLR)
		DrawDeadPlr(sx, sy, xp, yp, 0, ev2, FALSE);
	if (dPlayer[sx][sy] > 0) {
		bv = dPlayer[sx][sy] - 1;
		pxp = plr[bv]._pxoff + xp - plr[bv]._pAnimWidth2;
		pyp = plr[bv]._pyoff + yp;

		#if RLE_DRAW
		if (bv == cursplr) DrawUnitOutline(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		app_assert(plr[bv]._pAnimData);
		DrawUnit(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		#else
		if (bv == cursplr) OutlineSlabCel(T_POUTC, pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		app_assert(plr[bv]._pAnimData);
		DrawSlabCel(pxp, pyp, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, ev2);
		#endif

		if (chflag)
		{
//			if (plr[bv]._peflag == 2) T_DrawEFlag3(pTo-NBUFFWSL4-96, sx-2, sy+1, ev, ev2, xp-96, yp-16);
			if (plr[bv]._peflag != 0) T_DrawEFlag3(pTo-64, sx-1, sy+1, ev, ev2, xp-64, yp);
		}
	}
	if ((dFlags[sx][sy] & BFLAG_MISSILE) != 0) DrawMissile(sx, sy, xp, yp, 0, ev2, FALSE);
	if (dSpecial[sx][sy]) T_DrawSpecial(pTo, dSpecial[sx][sy]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawHTLX3 (int sx, int sy, int xp, int yp, int nd, int ev, int halfflag)
{
	int	i;
	BYTE *pTo;
	int	ev2;
	int t;
	WORD *mt;

	app_assert(gpBuffer);
	ev2 = (ev + 1) << 1;
	if (ev2 > 8) ev2 = 8;
	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			if (gdwPNum = dPiece[sx][sy]) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp + 32;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];

				for(t = 0; t < 7; t++)
				{
					if ((ev >= t) && (gdwPNum = mt[2*t+1])) DrawMTileClipTop (pTo);
					pTo -= NBUFFWSL5;
				}

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				T_DrawHTLXsub3 (pTo, sx, sy, ev, ev2, xp, yp, FALSE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TDrawBlankMini(pTo);
		}
		sx++;
		sy--;
		xp += 64;
	}

	nd -= halfflag;
	for (i = 0; i < nd; i++) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gdwPNum = dPiece[sx][sy]; 
			if (gdwPNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];
				for(t = 0; t < 7; t++)
				{
					if (ev >= t) {
						if (gdwPNum = mt[2*t]) DrawMTileClipTop (pTo);
						if (gdwPNum = mt[2*t+1]) DrawMTileClipTop (pTo+32);
					}
					pTo -= NBUFFWSL5;
				}

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				T_DrawHTLXsub3 (pTo, sx, sy, ev, ev2, xp, yp, TRUE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TDrawBlankMini(pTo);
		}
		sx++;
		sy--;
		xp += 64;
	}

	if (halfflag != 0) {
		if ((sy >= 0) && (sy < DMAXY) && (sx >= 0) && (sx < DMAXX)) {
			gdwPNum = dPiece[sx][sy]; 
			if (gdwPNum != 0) {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				mt = &dMT2[CalcRot(sx,sy)].mt[0];

				for(t = 0; t < 7; t++)
				{
					if ((ev >= t) && (gdwPNum = mt[2*t])) DrawMTileClipTop (pTo);
					pTo -= NBUFFWSL5;
				}

				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				T_DrawHTLXsub3 (pTo, sx, sy, ev, ev2, xp, yp, FALSE);
			} else {
				pTo = gpBuffer + nBuffWTbl[yp] + xp;
				TDrawBlankMini(pTo);
			}
		} else {
			pTo = gpBuffer + nBuffWTbl[yp] + xp;
			TDrawBlankMini(pTo);
		}
	}

}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_SVGADrawView (int StartX, int StartY)
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
	height = 5;

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
		case SCRL_NONE :
			break;
		case SCRL_U :
			ypos -= 32;
			StartX--;
			StartY--;
			height++;
			break;
		case SCRL_UR :
			ypos -= 32;
			StartX--;
			StartY--;
			width++;
			height++;
			break;
		case SCRL_R :
			width++;
			break;
		case SCRL_DR :
			width++;
			height++;
			break;
		case SCRL_D :
			height++;
			break;
		case SCRL_DL :
			xpos -= 64;
			StartX--;
			StartY++;
			width++;
			height++;
			break;
		case SCRL_L :
			xpos -= 64;
			StartX--;
			StartY++;
			width++;
			break;
		case SCRL_UL :
			xpos -= 64;
			ypos -= 32;
			StartX -= 2;
			width++;
			height++;
			break;
	}

	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[160];
	for (i = 0; i < 7; i++) {
		T_DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 0);
		StartY++;
		xpos -= 32;
		ypos += 16;
		T_DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 1);
		StartX++;
		xpos += 32;
		ypos += 16;
	}
	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[512];
	for (i = 0; i < height; i++) {
		T_DrawHTileLineX(StartX, StartY, xpos, ypos, width, 0);
		StartY++;
		xpos -= 32;
		ypos += 16;
		T_DrawHTileLineX(StartX, StartY, xpos, ypos, width, 1);
		StartX++;
		xpos += 32;
		ypos += 16;
	}
	for (i = 0; i < 7; i++) {
		T_DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 0);
		StartY++;
		xpos -= 32;
		ypos += 16;
		T_DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 1);
		StartX++;
		xpos += 32;
		ypos += 16;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_VGADrawView (int StartX, int StartY)
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
	height = 0;

	switch (ScrollInfo._sdir) {
		case SCRL_NONE :
			break;
		case SCRL_U :
			ypos -= 32;
			StartX--;
			StartY--;
			height++;
			break;
		case SCRL_UR :
			ypos -= 32;
			StartX--;
			StartY--;
			width++;
			height++;
			break;
		case SCRL_R :
			width++;
			break;
		case SCRL_DR :
			width++;
			height++;
			break;
		case SCRL_D :
			height++;
			break;
		case SCRL_DL :
			xpos -= 64;
			StartX--;
			StartY++;
			width++;
			height++;
			break;
		case SCRL_L :
			xpos -= 64;
			StartX--;
			StartY++;
			width++;
			break;
		case SCRL_UL :
			xpos -= 64;
			ypos -= 32;
			StartX -= 2;
			width++;
			height++;
			break;
	}

	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[143];
	for (i = 0; i < 7; i++) {
		T_DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 0);
		StartY++;
		xpos -= 32;
		ypos += 16;
		T_DrawHTLX3(StartX, StartY, xpos, ypos, width, i, 1);
		StartX++;
		xpos += 32;
		ypos += 16;
	}
	app_assert(gpBuffer);
	glClipY = (long)gpBuffer + nBuffWTbl[320];
	for (i = 0; i < height; i++) {
		T_DrawHTileLineX(StartX, StartY, xpos, ypos, width, 0);
		StartY++;
		xpos -= 32;
		ypos += 16;
		T_DrawHTileLineX(StartX, StartY, xpos, ypos, width, 1);
		StartX++;
		xpos += 32;
		ypos += 16;
	}
	for (i = 0; i < 7; i++) {
		T_DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 0);
		StartY++;
		xpos -= 32;
		ypos += 16;
		T_DrawHTLX2(StartX, StartY, xpos, ypos, width, i, 1);
		StartX++;
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
/*	// Double res copy (Old)
	app_assert(gpBuffer);
	__asm {
		mov		esi,dword ptr [gpBuffer]
		mov		edi,esi
		add		esi,dword ptr [csrc]
		add		edi,dword ptr [cdest]
		mov		ebx,edi
		add		ebx,768

		mov		edx,176
_YLp:	mov		ecx,dword ptr [cw]
_XLp:	lodsb
		mov		ah,al
		stosw
		mov		word ptr [ebx],ax
		add		ebx,2
		loop	_XLp
		mov		eax,768
		add		eax,dword ptr [cw]
		sub		esi,eax
		add		eax,eax
		sub		ebx,eax
		sub		edi,eax
		dec		edx
		jnz		_YLp
	}*/
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_DrawView (int StartX, int StartY)
{
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

	// Non-town draw routines use nLVal and nTrans as global params,
	// but in town all tiles are fully lit, and there are no transparent
	// walls, so we set them here once for the whole screen
	nLVal = 0;
	nTrans = 0;

	if (svgamode) T_SVGADrawView (StartX, StartY);
	else T_VGADrawView (StartX, StartY);
	if (automapflag) DrawAutomap();
	if ((stextflag) && (!qtextflag)) DrawSText();
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
	if (PauseMode && !deathflag) DrawPause();

	plrmsg_draw();

	gmenu_draw();

	DrawMapOfDoom();

	DrawInfoBox();
	DrawHealthTop();
	DrawManaTop();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_SetDungeonMicros()
{
	int	wv;
	int i,j;
	int t;
	WORD *mtsource;
	WORD *mt;

	// Init the light values for each piece
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			wv = dPiece[i][j];
			mt = &dMT2[CalcRot(i,j)].mt[0];
			if (wv != 0) {
				wv--;
				mtsource = (WORD *)(pMiniTiles + 32*wv);
				for(t = 0; t < 16; t++)
					// MiniTiles array uses opposite y direction
					// hence wierd index on next line
					mt[t] = mtsource[14-(t&0xe)+(t&1)];
			} else {
				for(t = 0; t < 16; t++)
					mt[t] = 0;
			}
		}
	}

#if 0
	int xmp,ymp,nPNum;
	int s;
	WORD tempmt[16];

	// Zero micro tile values for each piece
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			mt = &dMT2[CalcRot(i,j)].mt[0];
			for(t = 0; t < 16; t++)
				tempmt[t] = 0;

			nPNum = mt[0];
			if (((nPNum & 0x7000) == 0x4000) && (nPNum)) tempmt[0] = 1;
			nPNum = mt[1];
			if (((nPNum & 0x7000) == 0x5000) && (nPNum != 0)) tempmt[1] = 1;

			for(t = 2; t < 16; t++)
			{
				nPNum = mt[t];
				if (((nPNum & 0x7000) == 0) && nPNum)
				{
					tempmt[t] = 1;
					xmp = i;
					ymp = j;
					for(s = t-2; s >= 0 && xmp > 0 && ymp > 0; s -= 2)
					{
						xmp--;
						ymp--;
						dMT2[CalcRot(xmp,ymp)].mt[s] = 0;
					}
				}
			}

			for(t = 0; t < 13; t += 2)
			{
				if(tempmt[t] && tempmt[t+2])
				{
					xmp = i-1;
					ymp = j;
					for(s = t+1; s >= 0 && xmp >= 0 && ymp >= 0; s -= 2)
					{
						dMT2[CalcRot(xmp,ymp)].mt[s] = 0;
						xmp--;
						ymp--;
					}
				}
			}
			for(t = 1; t < 14; t += 2)
			{
				if(tempmt[t] && tempmt[t+2])
				{
					xmp = i;
					ymp = j-1;
					for(s = t+1; s >= 0 && xmp >= 0 && ymp >= 0; s -= 2)
					{
						dMT2[CalcRot(xmp,ymp)].mt[s] = 0;
						xmp--;
						ymp--;
					}
				}
			}

		}
	}
#endif

	if (svgamode) {
		ViewDX = 640;
		ViewDY = 352;
		ViewBX = 10;
		ViewBY = 11;
	} else {
		ViewDX = 384;
		ViewDY = 224;
		ViewBX = 6;
		ViewBY = 7;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_FillSector (BYTE *P3Tiles, BYTE *pSector, int xi, int yi, int w, int h)
{
	int	i,j,xx,yy;
	long v1,v2,v3,v4,ii;

	// Convert dungeon mega tiles to mini tiles
	ii = 4;
	yy = yi;
	for (j = 0; j < h; j++) {
		xx = xi;
		for (i = 0; i < w; i++) {
			__asm {
				mov		esi,dword ptr [pSector]
				mov		eax,dword ptr [ii]
				add		esi,eax
				xor		eax,eax
				lodsw
				or		eax,eax
				jz		_Zero
				dec		eax
				mov		esi,dword ptr [P3Tiles]
				shl		eax,3
				add		esi,eax
				xor		eax,eax
				lodsw
				inc		eax
				mov		dword ptr [v1],eax
				lodsw
				inc		eax
				mov		dword ptr [v2],eax
				lodsw
				inc		eax
				mov		dword ptr [v3],eax
				lodsw
				inc		eax
				mov		dword ptr [v4],eax
				jmp		_Done
_Zero:			mov		dword ptr [v1],eax
				mov		dword ptr [v2],eax
				mov		dword ptr [v3],eax
				mov		dword ptr [v4],eax
_Done:			nop
			}
			dPiece[xx][yy] = (int) v1;
			dPiece[xx+1][yy] = (int) v2;
			dPiece[xx][yy+1] = (int) v3;
			dPiece[xx+1][yy+1] = (int) v4;
			xx += 2;
			ii += 2;
		}
		yy += 2;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void T_FillTile(BYTE *P3Tiles, int xx, int yy, int t)
{
	long v1,v2,v3,v4;

	// Convert dungeon mega tiles to mini tiles
	__asm {
		mov		eax,dword ptr [t]
		dec		eax
		mov		esi,dword ptr [P3Tiles]
		shl		eax,3
		add		esi,eax
		xor		eax,eax
		lodsw
		inc		eax
		mov		dword ptr [v1],eax
		lodsw
		inc		eax
		mov		dword ptr [v2],eax
		lodsw
		inc		eax
		mov		dword ptr [v3],eax
		lodsw
		inc		eax
		mov		dword ptr [v4],eax
		jmp		_Done
		mov		dword ptr [v1],eax
		mov		dword ptr [v2],eax
		mov		dword ptr [v3],eax
		mov		dword ptr [v4],eax
_Done:	nop
	}
	dPiece[xx][yy] = (int) v1;
	dPiece[xx+1][yy] = (int) v2;
	dPiece[xx][yy+1] = (int) v3;
	dPiece[xx+1][yy+1] = (int) v4;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void OpenNest()
{
		dPiece[78][60] = 1162;	// shadow at fork	
		dPiece[79][60] = 1163;	
		dPiece[78][61] = 1164;	
		dPiece[79][61] = 1294;
		
		dPiece[78][62] = 1262;	// bridge
		dPiece[78][63] = 1264;
		dPiece[79][62] = 1296;
		dPiece[79][63] = 1297;

		dPiece[79][64] = 1298;	// second shadow
		dPiece[78][64] = 282;
		dPiece[78][65] = 284;
		dPiece[79][65] = 285;

		dPiece[80][60] = 1299;	// start tile of thing
		dPiece[80][61] = 1301;
		dPiece[81][61] = 1302;

		dPiece[82][60] = 1303;	// second tile of thing
		dPiece[83][60] = 1304;
		dPiece[82][61] = 1305;
		dPiece[83][61] = 1306;

		dPiece[80][62] = 1307;	// third tile of thing
		dPiece[81][62] = 1308;
		dPiece[80][63] = 1309;
		dPiece[81][63] = 1310;

		dPiece[80][64] = 1311;	// forth tile of thing
		dPiece[81][64] = 1312;
		dPiece[80][65] = 1313;
		dPiece[81][65] = 1314;

		dPiece[82][64] = 1319;	// fifth tile of thing
		dPiece[83][64] = 1320;
		dPiece[82][65] = 1321;
		dPiece[83][65] = 1322;

		dPiece[82][62] = 1315;	// sixth tile of thing
		dPiece[83][62] = 1316;
		dPiece[82][63] = 1317;
		dPiece[83][63] = 1318;

		dPiece[84][61] = 280;	// remove tree
		dPiece[84][62] = 280;
		dPiece[84][63] = 280;
//		dPiece[85][59] = 11;
		dPiece[85][60] = 280;
		dPiece[85][61] = 280;
		dPiece[85][62] = 8;
		dPiece[85][63] = 8;
		dPiece[85][64] = 8;
		dPiece[86][60] = 217;
		dPiece[86][61] = 24;
		dPiece[85][62] = 19;
//		dPiece[86][62] = 11;
//		dPiece[86][63] = 11;
//		dPiece[86][64] = 11;
		dPiece[84][64] = 280;
//		dPiece[86][65] = 11;
		T_SetDungeonMicros();
}
void CloseNest()
{
		dPiece[78][60] = 1162;	// shadow at fork	
		dPiece[79][60] = 1259;	
		dPiece[78][61] = 1260;	
		dPiece[79][61] = 1261;
		
		dPiece[78][62] = 1262;	// bridge
		dPiece[79][62] = 1263;
		dPiece[78][63] = 1264;
		dPiece[79][63] = 1265;

		dPiece[78][64] = 1266;	// second shadow
		dPiece[79][64] = 1267;
		dPiece[78][65] = 1268;

		dPiece[80][60] = 1269;	// start tile of thing
		dPiece[81][60] = 1270;
		dPiece[80][61] = 1271;
		dPiece[81][61] = 1272;

		dPiece[82][60] = 1273;	// second tile of thing
		dPiece[83][60] = 1274;
		dPiece[82][61] = 1275;
		dPiece[83][61] = 1276;

		dPiece[80][62] = 1277;	// third tile of thing
		dPiece[81][62] = 1278;
		dPiece[80][63] = 1279;
		dPiece[81][63] = 1280;

		dPiece[80][64] = 1281;	// forth tile of thing
		dPiece[81][64] = 1282;
		dPiece[80][65] = 1283;
		dPiece[81][65] = 1284;

		dPiece[82][64] = 1289;	// fifth tile of thing
		dPiece[83][64] = 1290;
		dPiece[82][65] = 1291;
		dPiece[83][65] = 1292;

		dPiece[82][62] = 1285;	// sixth tile of thing
		dPiece[83][62] = 1286;
		dPiece[82][63] = 1287;
		dPiece[83][63] = 1288;

		dPiece[84][61] = 280;	// remove tree
		dPiece[84][62] = 280;
		dPiece[84][63] = 280;
//		dPiece[85][59] = 11;
		dPiece[85][60] = 280;
		dPiece[85][61] = 280;
		dPiece[85][62] = 8;
		dPiece[85][63] = 8;
		dPiece[85][64] = 8;
		dPiece[86][60] = 217;
		dPiece[86][61] = 24;
		dPiece[85][62] = 19;
//		dPiece[86][62] = 11;
//		dPiece[86][63] = 11;
//		dPiece[86][64] = 11;
		dPiece[84][64] = 280;
//		dPiece[86][65] = 11;
		T_SetDungeonMicros();		
}

void CloseCrypt()
{
	dPiece[36][21] = 1323;
	dPiece[37][21] = 1324;
	dPiece[36][22] = 1325;
	dPiece[37][22] = 1326;

	dPiece[36][23] = 1327;
	dPiece[37][23] = 1328;
	dPiece[36][24] = 1329;
	dPiece[37][24] = 1330;

	dPiece[35][21] = 1339;
	dPiece[34][21] = 1340;
//	dPiece[34][22] = 1341;
//	dPiece[35][22] = 1342;


	T_SetDungeonMicros();
}

void OpenCrypt()
{
	dPiece[36][21] = 1331;
	dPiece[37][21] = 1332;
	dPiece[36][22] = 1333;
	dPiece[37][22] = 1334;

	dPiece[36][23] = 1335;
	dPiece[37][23] = 1336;
	dPiece[36][24] = 1337;
	dPiece[37][24] = 1338;

	dPiece[35][21] = 1339;
	dPiece[34][21] = 1340;
//	dPiece[34][22] = 1341;
//	dPiece[35][22] = 1342;


	T_SetDungeonMicros();
}

void T_Pass3()
{
	BYTE *P3Tiles;
	BYTE *pSector;
	int	xx,yy;

	// Init Dungeon to blank
	for (yy = 0; yy < DMAXY; yy+=2) {
		for (xx = 0; xx < DMAXX; xx+=2) {
			dPiece[xx][yy] = 0;
			dPiece[xx+1][yy] = 0;
			dPiece[xx][yy+1] = 0;
			dPiece[xx+1][yy+1] = 0;
		}
	}

	// Load convertion tiles
	P3Tiles = LoadFileInMemSig("Levels\\TownData\\Town.TIL",NULL,'TOWN');

	// Load map sector
	pSector = LoadFileInMemSig("Levels\\TownData\\Sector1s.DUN",NULL,'TOWN');
	T_FillSector(P3Tiles, pSector, 46, 46, 25, 25);
	DiabloFreePtr(pSector);

	pSector = LoadFileInMemSig("Levels\\TownData\\Sector2s.DUN",NULL,'TOWN');
	T_FillSector(P3Tiles, pSector, 46, 0, 25, 23);
	DiabloFreePtr(pSector);

	pSector = LoadFileInMemSig("Levels\\TownData\\Sector3s.DUN",NULL,'TOWN');
	T_FillSector(P3Tiles, pSector, 0, 46, 23, 25);
	DiabloFreePtr(pSector);

	pSector = LoadFileInMemSig("Levels\\TownData\\Sector4s.DUN",NULL,'TOWN');
	T_FillSector(P3Tiles, pSector, 0, 0, 23, 23);
	DiabloFreePtr(pSector);

#ifdef TESTINGCRYPT
		quests[Q_CRYPTMAP]._qactive = QUEST_DONE;
#endif


#if IS_VERSION(SHAREWARE)
	// Close Maus shortcut
	T_FillTile(P3Tiles, 48, 20, 320);
	// Close cave shortcut
	T_FillTile(P3Tiles, 16, 68, 332);
	T_FillTile(P3Tiles, 16, 70, 331);
	// Close crev shortcut
	for (xx = 36; xx < 46; xx++) T_FillTile(P3Tiles, xx, 78, random(0,4)+1);
#else
	if (gbMaxPlayers == 1) {
		if ((quests[Q_FARMER]._qactive == QUEST_DONE) || (quests[Q_FARMER]._qactive == QUEST_REALLYDONE)
				|| (quests[Q_COWSUIT]._qactive == QUEST_DONE) || (quests[Q_COWSUIT]._qactive == QUEST_REALLYDONE))
		{
			OpenNest();
		}
		else
		{
			CloseNest();
		}

		if ((quests[Q_CRYPTMAP]._qactive != QUEST_DONE) && (!plr[myplr]._pLvlVisited[CRYPTSTART]))
			CloseCrypt();
		else
			OpenCrypt();

		
		// Close Maus shortcut
		if (!((plr[myplr].pTownWarps & 0x01) || plr[myplr]._pLevel >= 10))
			T_FillTile(P3Tiles, 48, 20, 320);
		// Close cave shortcut
		if (!((plr[myplr].pTownWarps & 0x02) || plr[myplr]._pLevel >= 15))
		{
			T_FillTile(P3Tiles, 16, 68, 332);
			T_FillTile(P3Tiles, 16, 70, 331);
		}
		// Close crev shortcut or open or close or whatever is asked today
		if (!((plr[myplr].pTownWarps & 0x04) || plr[myplr]._pLevel >= 20))
//		if (!(plr[myplr].pTownWarps & 0x04))
		{
			for (xx = 36; xx < 46; xx++)
				T_FillTile(P3Tiles, xx, 78, random(0,4)+1);
		}
	}
	else
	{
		if ((quests[Q_FARMER]._qactive == QUEST_DONE) || (quests[Q_FARMER]._qactive == QUEST_REALLYDONE)
		|| (quests[Q_COWSUIT]._qactive == QUEST_DONE) || (quests[Q_COWSUIT]._qactive == QUEST_REALLYDONE))
		{
			OpenNest();
		}
		else
		{
			CloseNest();
		}

		if ((quests[Q_CRYPTMAP]._qactive != QUEST_DONE) && (!plr[myplr]._pLvlVisited[CRYPTSTART]))
			CloseCrypt();
		else
			OpenCrypt();
	}

#endif

	if ((quests[Q_PWATER]._qactive == QUEST_DONE) || (quests[Q_PWATER]._qactive == QUEST_NOTAVAIL))
		T_FillTile(P3Tiles, 60, 70, 71);
	else 
		T_FillTile(P3Tiles, 60, 70, 342);
	
	DiabloFreePtr(P3Tiles);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
extern int TWarpFrom;

void CreateTown(int entry)
{
	int		i,j;

	dminx = 10;
	dminy = 10;
	dmaxx = 84;
	dmaxy = 84;

	if (entry == LVL_DOWN) {
		ViewX = 75;		// Your house
		ViewY = 68;
	} else {
		if (entry == LVL_UP) {
			ViewX = 25;		// Near church
			ViewY = 31;
		} else {
			if (entry == LVL_TWARPUP) {
				if (TWarpFrom == 5) {
					ViewX = 49;		// Maus warp
					ViewY = 22;
				}
				if (TWarpFrom == 9) {
					ViewX = 18;		// Cave warp
					ViewY = 69;
				}
				if (TWarpFrom == 13) {
					ViewX = 41;		// Hell warp
					ViewY = 81;
				}// JKE add level up here.
				if (TWarpFrom == CRYPTSTART) {
					ViewX = 36;
					ViewY = 25;
				}
				if (TWarpFrom == HIVESTART) {
					ViewX = 79;
					ViewY = 62;
				}
			}
		}
	}

	T_Pass3();

	// Init the light values for each piece
	ZeroMemory(dLight,sizeof(dLight));
	ZeroMemory(dFlags,sizeof(dFlags));
	ZeroMemory(dPlayer,sizeof(dPlayer));
	ZeroMemory(dMonster,sizeof(dMonster));
	ZeroMemory(dObject,sizeof(dObject));
	ZeroMemory(dItem,sizeof(dItem));
	ZeroMemory(dSpecial,sizeof(dSpecial));

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			int nPiece = dPiece[i][j];
			if (nPiece == 360) dSpecial[i][j] = 1;
			else if (nPiece == 358) dSpecial[i][j] = 2;
			else if (nPiece == 129) dSpecial[i][j] = 6;
			else if (nPiece == 130) dSpecial[i][j] = 7;
			else if (nPiece == 128) dSpecial[i][j] = 8;
			else if (nPiece == 117) dSpecial[i][j] = 9;
			else if (nPiece == 157) dSpecial[i][j] = 10;
			else if (nPiece == 158) dSpecial[i][j] = 11;
			else if (nPiece == 156) dSpecial[i][j] = 12;
			else if (nPiece == 162) dSpecial[i][j] = 13;
			else if (nPiece == 160) dSpecial[i][j] = 14;
			else if (nPiece == 214) dSpecial[i][j] = 15;
			else if (nPiece == 212) dSpecial[i][j] = 16;
			else if (nPiece == 217) dSpecial[i][j] = 17;
			else if (nPiece == 216) dSpecial[i][j] = 18;
		}
	}

	T_SetDungeonMicros();
}
