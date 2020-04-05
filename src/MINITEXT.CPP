/*-----------------------------------------------------------------------**
** 								Diablo
**
**						  Miniquest Text file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"minitext.h"
#include	"textdat.h"
#include	"engine.h"
#include	"scrollrt.h"
#include	"quests.h"
#include	"effects.h"
#include	"items.h"
#include	"gendung.h"

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define KERNSPACE	2

static const BYTE qfonttrans[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// 0-15
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// 16-31
	0, 37, 49, 38, 0, 39, 40, 47, 42, 43, 41, 45, 52, 44, 53, 55,	// 32-47
	36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 51, 50, 48, 46, 49, 54,	// 48-63
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,			// 64-79
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 0, 43, 0, 0,	// 80-95
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,			// 96-111
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 48, 0, 49, 0, 0 };	// 112-127

/*char qfontkern[56] = { 8,													// Space/Invalid
	24, 16, 19, 20, 15, 15, 19, 18, 8, 8, 18, 14, 24, 20, 23, 15,			// a-p
	24, 20, 16, 21, 24, 25, 32, 24, 25, 18,									// q-z
	8, 16, 17, 17, 16, 16, 17, 16, 16, 24,									// 1-0
	8, 15, 26, 23, 13, 9, 8, 10, 13, 12, 8, 15, 15, 8, 8, 8, 8, 16, 17 };	// misc */

static const BYTE qfontkern[56] = { 5,													// Space/Invalid
	15, 10, 13, 14, 10, 9, 13, 11, 5, 5, 11, 10, 16, 13, 16, 10,			// a-p
	15, 12, 10, 14, 17, 17, 22, 17, 16, 11,									// q-z
	5, 11, 11, 11, 10, 11, 11, 11, 11, 15,									// 1-0
	5, 10, 18, 15, 8, 6, 6, 7, 10, 9, 6, 10, 10, 5, 5, 5, 5, 11, 12 };		// misc

BYTE qtextflag;
static BYTE *pMedTextCels;
static BYTE *pTextBoxCels;
static const char *qtextptr;
static int qtexty;
int qtextSpd;
static int qtextDelay;
static DWORD sgLastScroll;

//Quest text scrolling rate 1 = slowest, 5 = normal, 9 = fastest
int qtextDelaySpd[10] = { 2, 4, 6, 8, 0, -1, -2, -3, -4 };

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#define MQTEXTX1	112
#define MQTEXTY1	241
#define MQTEXTW		543
#define MQTEXTY2	469
#define MQTEXTNL	38

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FreeQuestText() {
	DiabloFreePtr(pMedTextCels);
	DiabloFreePtr(pTextBoxCels);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitQuestText() {
	app_assert(! pMedTextCels);
	pMedTextCels = LoadFileInMemSig("Data\\MedTextS.CEL",NULL,'MINI');
	pTextBoxCels = LoadFileInMemSig("Data\\TextBox.CEL",NULL,'MINI');
	qtextflag = FALSE;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitQTextMsg(int m) {
	app_assert((DWORD) m < gdwAllTextEntries);
	if (alltext[m].scrlltxt) 
	{
		questlog = FALSE;
		qtextflag = TRUE;
		qtextptr = alltext[m].txtstr;
		qtexty = MQTEXTY2 + 31;
		qtextSpd = qtextDelaySpd[alltext[m].txtspd-1]; 
		qtextDelay = qtextSpd;
		sgLastScroll = GetTickCount();
		#if CHEATS
		if ((currlevel == 0) && (davecheat)) {
			qtextSpd = qtextDelaySpd[tstQMsgSpd-1];
			qtextDelay = qtextSpd;
		}
		#endif
	}
	PlaySFX(alltext[m].sfxnr);

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DrawQTextBack()
{
	DrawCel(88, 487, pTextBoxCels, 1, 591);
	app_assert(gpBuffer);
	__asm {
		mov		edi,dword ptr [gpBuffer]
		add		edi,371803

		xor		eax,eax
		mov		edx,148
_YLp:	mov		ecx,292
_XLp1:	stosb
		inc		edi
		loop	_XLp1
		stosb
		sub		edi,1353
		mov		ecx,292
_XLp2:	inc		edi
		stosb
		loop	_XLp2
		sub		edi,1352
		dec		edx
		jnz		_YLp
		mov		ecx,292
_XLp3:	stosb
		inc		edi
		loop	_XLp3
		stosb
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawQTextCel (long xp, long yp, BYTE *pCelBuff, long nCel)
{
	BYTE *pTo, *pY1, *pY2;
	long RLELen;

	app_assert(gpBuffer);
	pTo = gpBuffer + nBuffWTbl[yp] + xp;
	pY1 = gpBuffer + nBuffWTbl[MQTEXTY1-32];
	pY2 = gpBuffer + nBuffWTbl[MQTEXTY2];
	__asm {
		mov		ebx,dword ptr [pCelBuff]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax
		mov		eax,dword ptr [ebx+4]
		sub		eax,dword ptr [ebx]
		mov		dword ptr [RLELen],eax

		mov		esi,dword ptr [pCelBuff]
		add		esi,dword ptr [ebx]

		mov		edi,dword ptr [pTo]		// Dest

		mov		ebx,dword ptr [RLELen]
		add		ebx,esi

_T1Lp1:	mov		edx,22

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [pY1]
		jb		_T1C
		cmp		edi,dword ptr [pY2]
		ja		_T1C
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
		jmp		_T1x
_T1C:	add		esi,eax
		add		edi,eax
_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,790
		cmp		ebx,esi
		jnz		_T1Lp1
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawQText()
{
	const char *p, *pt, *pnl;
	char tempstr[128];
	int tx, ty;
	int l, i;
	BOOL doneflag;

	DrawQTextBack();

	p = qtextptr;
	pnl = NULL;
	tx = MQTEXTX1;
	ty = qtexty;

	doneflag = FALSE;
	while (!doneflag) {
		l = 0;
		pt = p;
		for (i = 0; (*pt != '\n') && (*pt != '|') && (l < MQTEXTW); i++) {
			BYTE c = char2print(*pt++);
			if (c != 0) {
				tempstr[i] = c;
				c = qfonttrans[c];
				l += qfontkern[c] + KERNSPACE;
			} else i--;
		}
		tempstr[i] = 0;
		if (*pt == '|') {
			tempstr[i] = 0;
			doneflag = TRUE;
		} else if (*pt == '\n') {
			pt++;
		} else while ((tempstr[i] != ' ') && (i > 0)) {
			tempstr[i] = 0;
			i--;
		}
		for (i = 0; tempstr[i] != 0; i++) {
			BYTE c = char2print(tempstr[i]);
			c = qfonttrans[c];
			// while (*p == 0) p++;  <<< this look awfully dangerous -- pat 
			p++;
			if (*p == '\n') p++;
			if (c != 0) DrawQTextCel(tx, ty, pMedTextCels, c);
			tx += qfontkern[c] + KERNSPACE;
		}
		if (pnl == NULL) pnl = p;
		tx = MQTEXTX1;
		ty += MQTEXTNL;
		if (ty > (MQTEXTY2+32)) doneflag = TRUE;
	}

	//Delay for text scrolling
	DWORD currTime = GetTickCount();
	do {
		if (qtextSpd <= 0) { //Go faster
			qtexty--;
			qtexty += qtextSpd;
		} else { //Go slower
			qtextDelay--;
			if (qtextDelay != 0) qtexty--;
		}
		if (qtextDelay == 0) qtextDelay = qtextSpd;
		
		if (qtexty <= (MQTEXTY1-32)) {
			qtexty += MQTEXTNL;
			qtextptr = pnl;
			if (*qtextptr == '|') qtextflag = FALSE;		// done?
			break;
		}
		sgLastScroll += 1000/GAME_FRAMES_PER_SECOND;
	} while (currTime-sgLastScroll < 0x7FFFFFFF);
}

