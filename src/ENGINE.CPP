/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  Engine file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/ENGINE.CPP 2     1/22/97 2:32p Dgartner $
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"resource.h"
#include	"storm.h"
#include	"engine.h"
#include	"lighting.h"
#include	"scrollrt.h"
#include	"debug.h"
#include	"gendung.h"
#include	"palette.h"

//******************************************************************
// extern
//******************************************************************
void ErrorDlg(int nDlgId,DWORD dwErr,const char * pszFile,int nLine);


//******************************************************************
// Registration info
//******************************************************************
#include "regconst.h"
char sgszRegSig1[REG_LEN] = "REGISTRATION_BLOCK";


//******************************************************************
// random numbers
//******************************************************************
	static long sglGameSeed;


/*-----------------------------------------------------------------------**
** Decode RLE data without lighting
**-----------------------------------------------------------------------*/
void DecodeFullCel (BYTE *pDecodeTo, BYTE *pRLEBytes, long lRLECount, long nWidth)
{
	long	nBufferW;
	app_assert(pDecodeTo != NULL);
	app_assert(pRLEBytes != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pDecodeTo == NULL || pRLEBytes == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	__asm {
		mov		esi,dword ptr [pRLEBytes]		// Source
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,768							// Increase width
		add		eax,dword ptr [nWidth]
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [lRLECount]
		add		ebx,esi

_T1Lp1:	mov		edx,dword ptr [nWidth]

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
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
	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Draws a NORMAL cel in a cel file RAM buffer. NOTE: non-dungeon 5 offset cel
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
**-----------------------------------------------------------------------*/
void DrawCel (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	pTo = gpBuffer + nBuffWTbl[yp] + xp;
	DecodeFullCel (pTo, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Draws a NORMAL cel in a cel file RAM buffer. NOTE: non-dungeon 5 offset cel
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
**-----------------------------------------------------------------------*/
void DrawCelP (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW)
{
	BYTE *pFrom;
	long RLELen;

	app_assert(pCelBuff != NULL);
	app_assert(pBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL || pBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	DecodeFullCel (pBuff, pFrom, RLELen, nCelW);
}

/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
**-----------------------------------------------------------------------*/
void DrawSlabCel (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
	DecodeFullCel(pTo, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
**-----------------------------------------------------------------------*/
void DrawSlabCelP (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	app_assert(pBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL || pBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	DecodeFullCel(pBuff, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Decode RLE data with light translation
**-----------------------------------------------------------------------*/
void DecodeFullCelL (BYTE *pDecodeTo, BYTE *pRLEBytes, long lRLECount, long nWidth)
{
	long nBufferW, nL;
	app_assert(pDecodeTo != NULL);
	app_assert(pRLEBytes != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pDecodeTo == NULL || pRLEBytes == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	__asm {
		mov		eax,dword ptr [nLVal];
		shl		eax,8
		add		eax,dword ptr [pLightTbl];
		mov		dword ptr [nL],eax

		mov		esi,dword ptr [pRLEBytes]		// Source
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,768							// Increase width
		add		eax,dword ptr [nWidth]
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [lRLECount]
		add		ebx,esi

_T1Lp1:	mov		edx,dword ptr [nWidth]

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		push	ebx
		mov		ebx,dword ptr [nL]
		sub		edx,eax
		mov		ecx,eax

		push edx
		call xbytes
		pop edx

		pop		ebx
		or		edx,edx
		jnz		_T1Lp2
		jmp		_T1Nxt

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,dword ptr [nBufferW]
		cmp		ebx,esi
		jnz		_T1Lp1

		jmp _done
xbytes:

		shr cl,1
		jnc xwords

		mov dl, [esi]
		mov dl,	[ebx+edx]
		mov [edi],dl

		add	esi,1
		add edi,1

xwords:
		shr	cl,1
		jnc	xquads

		mov dl, [esi]
		mov	ch, [ebx+edx]

		mov [edi],ch
		mov dl, [esi+1]

		mov	ch, [ebx+edx]
		mov [edi+1],ch

		add esi,2
		add edi,2

xquads:
		test cl,cl
		jz	xend

xnext:
		mov eax, [esi]
		add	esi,4

		mov dl,al
		mov ch,[ebx+edx]

		mov dl,ah
		ror eax,16
		mov	[edi],ch

		mov ch,[ebx+edx]

		mov dl,al
		mov	[edi+1],ch

		mov ch,[ebx+edx]

		mov dl,ah
		mov	[edi+2],ch

		mov ch,[ebx+edx]
		mov	[edi+3],ch

		add	edi,4
		
		dec cl
		jnz xnext

xend:
		ret

_done:
	}		// end of asm block
}

/*-----------------------------------------------------------------------**
** Decode RLE data with light translation and transparency
**-----------------------------------------------------------------------*/
void TDecodeFullCelL (BYTE *pDecodeTo, BYTE *pRLEBytes, long lRLECount, long nWidth)
{
	long nBufferW, nL, LineVal;
	app_assert(pDecodeTo != NULL);
	app_assert(pRLEBytes != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pDecodeTo == NULL || pRLEBytes == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	__asm {
		mov		eax,dword ptr [nLVal];
		shl		eax,8
		add		eax,dword ptr [pLightTbl];
		mov		dword ptr [nL],eax

		mov		esi,dword ptr [pRLEBytes]		// Source
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,768							// Increase width
		add		eax,dword ptr [nWidth]
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [lRLECount]
		add		ebx,esi

		mov		eax,edi
		and		eax,1
		mov		dword ptr [LineVal],eax

_T1Lp1:	mov		edx,dword ptr [nWidth]

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		push	ebx
		mov		ebx,dword ptr [nL]
		sub		edx,eax
		mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [LineVal]
		jnz		_T1Od
		shr		ecx,1
		jnc		_T1w
		inc		esi
		inc		edi
		jecxz	_T1x
		jmp		_T1w2
_T1w:	shr		ecx,1
		jnc		_T1Lp3
		inc		esi
		inc		edi
		lodsb
		xlatb
		stosb
		jecxz	_T1x
_T1Lp3:	lodsd
		inc		edi
		ror		eax,8
		xlatb
		stosb
		ror		eax,16
		inc		edi
		xlatb
		stosb
		loop	_T1Lp3
		jmp		_T1x

_T1Od:	shr		ecx,1
		jnc		_T1w2
		lodsb
		xlatb
		stosb
		jecxz	_T1x
		jmp		_T1w
_T1w2:	shr		ecx,1
		jnc		_T1Lp4
		lodsb
		xlatb
		stosb
		inc		esi
		inc		edi
		jecxz	_T1x
_T1Lp4:	lodsd
		xlatb
		stosb
		inc		edi
		ror		eax,16
		xlatb
		stosb
		inc		edi
		loop	_T1Lp4

_T1x:	pop		ebx
		or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,dword ptr [nBufferW]
		mov		eax,dword ptr [LineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [LineVal],eax
		cmp		ebx,esi
		jnz		_T1Lp1
	}		// end of asm block
}

/*-----------------------------------------------------------------------**
** Draws a NORMAL cel in a cel file RAM buffer. NOTE: non-dungeon 5 offset cel
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void DrawCelL (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	pTo = gpBuffer + nBuffWTbl[yp] + xp;
	if (nLVal)
		DecodeFullCelL (pTo, pFrom, RLELen, nCelW);
	else
		DecodeFullCel (pTo, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Draws a NORMAL cel in a cel file RAM buffer. NOTE: non-dungeon 5 offset cel
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
/*
void DrawCelPL (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW)
{
	BYTE *pFrom;
	long RLELen;

	app_assert(pCelBuff != NULL);
	app_assert(pBuff != NULL);
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	if (nLVal)
		DecodeFullCelL (pBuff, pFrom, RLELen, nCelW);
	else
		DecodeFullCel (pBuff, pFrom, RLELen, nCelW);
}
*/

/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void DrawSlabCelL (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
	if (nLVal)
		DecodeFullCelL (pTo, pFrom, RLELen, nCelW);
	else
		DecodeFullCel (pTo, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
/*
void DrawSlabCelPL (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	app_assert(pBuff != NULL);
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	if (nLVal)
		DecodeFullCelL (pBuff, pFrom, RLELen, nCelW);
	else
		DecodeFullCel (pBuff, pFrom, RLELen, nCelW);
}
*/

/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer. with transparency
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void TDrawSlabCelPL (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	app_assert(pBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL || pBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	if (nTrans)
		TDecodeFullCelL (pBuff, pFrom, RLELen, nCelW);
	else if (nLVal)
		DecodeFullCelL (pBuff, pFrom, RLELen, nCelW);
	else
		DecodeFullCel (pBuff, pFrom, RLELen, nCelW);
}

/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer. With infared vision
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void DrawSlabCelI (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend, char loff)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;
	long nBufferW, nL, ltaboff;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
	ltaboff = light4flag ? 1024 : 4096;
	if (loff == LIGHT_STONE)
		ltaboff += 256;
	if (loff >= LIGHT_U)
		ltaboff += ((loff - LIGHT_U) << 8) + 768;
	__asm {
		mov		eax,dword ptr [pLightTbl]
		add		eax,dword ptr [ltaboff]
		mov		dword ptr [nL],eax

		mov		esi,dword ptr [pFrom]		// Source
		mov		edi,dword ptr [pTo]			// Dest

		mov		eax,768							// Increase width
		add		eax,dword ptr [nCelW]
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [RLELen]
		add		ebx,esi
_T1Lp1:
		mov		edx,dword ptr [nCelW]
_T1Lp2:
		xor		eax,eax				// Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		_T1J

		push	ebx
		mov		ebx,dword ptr [nL]
		sub		edx,eax
		mov		ecx,eax
Bytes:
		mov		al,[esi]
		inc		esi
		mov		al,[ebx+eax]
		mov		[edi],al
		dec		ecx
		lea		edi,[edi+1]
		jnz		Bytes

		pop		ebx
		test	edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2
_T1J:
		neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:
		sub		edi,dword ptr [nBufferW]
		cmp		ebx,esi
		jnz		_T1Lp1
	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Decode RLE data without lighting, with clipping
**-----------------------------------------------------------------------*/
void CDecodeFullCel (BYTE *pDecodeTo, BYTE *pRLEBytes, long lRLECount, long nWidth)
{
	long	nBufferW;
	app_assert(pDecodeTo != NULL);
	app_assert(pRLEBytes != NULL);
	app_assert(gpBuffer);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pDecodeTo == NULL || pRLEBytes == NULL || gpBuffer == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	__asm {
		mov		esi,dword ptr [pRLEBytes]		// Source
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,768							// Increase width
		add		eax,dword ptr [nWidth]
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [lRLECount]
		add		ebx,esi

_T1Lp1:	mov		edx,dword ptr [nWidth]

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
	}		// end of asm block
}

/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
**-----------------------------------------------------------------------*/
void CDrawSlabCel (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
	CDecodeFullCel(pTo, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
**-----------------------------------------------------------------------*/
void CDrawSlabCelP (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	app_assert(pBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL || pBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = *(WORD*)(pFrom + oend);
	if (oend == 8)
		offval2 = 0;
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	CDecodeFullCel (pBuff, pFrom, RLELen, nCelW);
}

/*-----------------------------------------------------------------------**
** Decode RLE data with light translation and with clipping
**-----------------------------------------------------------------------*/
void CDecodeFullCelL (BYTE *pDecodeTo, BYTE *pRLEBytes, long lRLECount, long nWidth)
{
	long nBufferW, nL;
	app_assert(pDecodeTo != NULL);
	app_assert(pRLEBytes != NULL);

	app_assert(gpBuffer);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pDecodeTo == NULL || pRLEBytes == NULL || gpBuffer == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	__asm {
		mov		eax,dword ptr [nLVal];
		shl		eax,8
		add		eax,dword ptr [pLightTbl];
		mov		dword ptr [nL],eax

		mov		esi,dword ptr [pRLEBytes]		// Source
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,768							// Increase width
		add		eax,dword ptr [nWidth]
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [lRLECount]
		add		ebx,esi

_T1Lp1:	mov		edx,dword ptr [nWidth]

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		push	ebx
		mov		ebx,dword ptr [nL]
		sub		edx,eax
		cmp		edi,dword ptr [glClipY]
		jb		_T1C1
		add		esi,eax
		add		edi,eax
		jmp		_T1x
_T1C1:	mov		ecx,eax

		push edx
		call xbytes
		pop edx

_T1x:	pop		ebx
		or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,dword ptr [nBufferW]
		cmp		ebx,esi
		jnz		_T1Lp1

		jmp _done

xbytes:

		shr cl,1
		jnc xwords

		mov dl, [esi]
		mov dl,	[ebx+edx]
		mov [edi],dl

		add	esi,1
		add edi,1

xwords:
		shr	cl,1
		jnc	xquads

		mov dl, [esi]
		mov	ch, [ebx+edx]

		mov [edi],ch
		mov dl, [esi+1]

		mov	ch, [ebx+edx]
		mov [edi+1],ch

		add esi,2
		add edi,2

xquads:
		test cl,cl
		jz	xend

xnext:
		mov eax, [esi]
		add	esi,4

		mov dl,al
		mov ch,[ebx+edx]

		mov dl,ah
		ror eax,16
		mov	[edi],ch

		mov ch,[ebx+edx]

		mov dl,al
		mov	[edi+1],ch

		mov ch,[ebx+edx]

		mov dl,ah
		mov	[edi+2],ch

		mov ch,[ebx+edx]
		mov	[edi+3],ch

		add	edi,4
		
		dec cl
		jnz xnext

xend:
		ret
_done:
	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Decode RLE data with light translation and with clipping and transparency
**-----------------------------------------------------------------------*/
void TCDecodeFullCelL (BYTE *pDecodeTo, BYTE *pRLEBytes, long lRLECount, long nWidth)
{
	long nBufferW, nL, LineVal;
	app_assert(pDecodeTo != NULL);
	app_assert(pRLEBytes != NULL);

	app_assert(gpBuffer);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pDecodeTo == NULL || pRLEBytes == NULL || gpBuffer == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	__asm {
		mov		eax,dword ptr [nLVal];
		shl		eax,8
		add		eax,dword ptr [pLightTbl];
		mov		dword ptr [nL],eax

		mov		esi,dword ptr [pRLEBytes]		// Source
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,768							// Increase width
		add		eax,dword ptr [nWidth]
		mov		dword ptr [nBufferW],eax

		mov		ebx,dword ptr [lRLECount]
		add		ebx,esi

		mov		eax,edi
		and		eax,1
		mov		dword ptr [LineVal],eax

_T1Lp1:	mov		edx,dword ptr [nWidth]

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		push	ebx
		mov		ebx,dword ptr [nL]
		sub		edx,eax
		cmp		edi,dword ptr [glClipY]
		jb		_T1C1
		add		esi,eax
		add		edi,eax
		jmp		_T1x
_T1C1:	mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [LineVal]
		jnz		_T1Od
		shr		ecx,1
		jnc		_T1w
		inc		esi
		inc		edi
		jecxz	_T1x
		jmp		_T1w2
_T1w:	shr		ecx,1
		jnc		_T1Lp3
		inc		esi
		inc		edi
		lodsb
		xlatb
		stosb
		jecxz	_T1x
_T1Lp3:	lodsd
		inc		edi
		ror		eax,8
		xlatb
		stosb
		ror		eax,16
		inc		edi
		xlatb
		stosb
		loop	_T1Lp3
		jmp		_T1x

_T1Od:	shr		ecx,1
		jnc		_T1w2
		lodsb
		xlatb
		stosb
		jecxz	_T1x
		jmp		_T1w
_T1w2:	shr		ecx,1
		jnc		_T1Lp4
		lodsb
		xlatb
		stosb
		inc		esi
		inc		edi
		jecxz	_T1x
_T1Lp4:	lodsd
		xlatb
		stosb
		inc		edi
		ror		eax,16
		xlatb
		stosb
		inc		edi
		loop	_T1Lp4

_T1x:	pop		ebx
		or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,dword ptr [nBufferW]
		mov		eax,dword ptr [LineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [LineVal],eax
		cmp		ebx,esi
		jnz		_T1Lp1
	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void CDrawSlabCelL (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = *(WORD*)(pFrom + oend);
	if (oend == 8)
		offval2 = 0;
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
	if (nLVal)
		CDecodeFullCelL(pTo, pFrom, RLELen, nCelW);
	else
		CDecodeFullCel(pTo, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
/*
void CDrawSlabCelPL (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = *(WORD*)(pFrom + oend);
	if (oend == 8)
		offval2 = 0;
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	if (nLVal)
		CDecodeFullCelL (pBuff, pFrom, RLELen, nCelW);
	else
		CDecodeFullCel (pBuff, pFrom, RLELen, nCelW);
}
*/

/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer.
**
** *pBuff = pointer to destination x,y in offscreen buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void TCDrawSlabCelPL (BYTE *pBuff, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pFrom;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = *(WORD*)(pFrom + oend);
	if (oend == 8)
		offval2 = 0;
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	if (nTrans)
		TCDecodeFullCelL (pBuff, pFrom, RLELen, nCelW);
	else if (nLVal)
		CDecodeFullCelL (pBuff, pFrom, RLELen, nCelW);
	else
		CDecodeFullCel (pBuff, pFrom, RLELen, nCelW);
}


/*-----------------------------------------------------------------------**
** Draws a 5 offset slab cel in a cel file RAM buffer. With infared vision and clipping
**
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void CDrawSlabCelI (long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend, char loff)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;
	long nBufferW, nL, ltaboff;

	app_assert(gpBuffer);
	app_assert(pCelBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97
	pFrom = pCelBuff + *((DWORD*)pCelBuff + nCel);
	offval = *(WORD*)(pFrom + ostart);
	if (!offval)
		return;
	RLELen = *((DWORD*)pCelBuff + nCel+1) - *((DWORD*)pCelBuff + nCel);
	offval2 = (oend == 8) ? 0 : *(WORD*)(pFrom + oend);
	RLELen = offval2 ? offval2 - offval : RLELen - offval;
	pFrom += offval;
	pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
	ltaboff = light4flag ? 1024 : 4096;
	if (loff == LIGHT_STONE)
		ltaboff += 256;
	if (loff >= LIGHT_U)
		ltaboff += ((loff - LIGHT_U) << 8) + 768;
	nL = (long)(pLightTbl + ltaboff);
	nBufferW = 768 + nCelW;			// Increase width
	__asm {
		mov		esi,[pFrom]			// Source
		mov		edi,[pTo]			// Dest

		mov		ecx,[RLELen]
		add		ecx,esi
_T1Lp1:
		push	ecx
		mov		edx,[nCelW]
		xor		ecx,ecx
_T1Lp2:
		xor		eax,eax				// Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		_T1J

		mov		ebx,[nL]
		sub		edx,eax
		cmp		edi,[glClipY]
		jb		_T1C1
		add		esi,eax
		add		edi,eax
		jmp		_T1x
_T1C1:
		mov		cl,[esi]
		inc		esi
		mov		cl,[ebx+ecx]
		mov		[edi],cl
		dec		eax
		lea		edi,[edi+1]
		jnz		_T1C1
_T1x:
		test	edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2
_T1J:
		neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:
		pop		ecx
		sub		edi,dword ptr [nBufferW]
		cmp		ecx,esi
		jnz		_T1Lp1
	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Draws a NORMAL cel in a cel file RAM buffer. NOTE: non-dungeon 5 offset cel
**
** pBuff = Buffer to draw in
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** nBuffW = Width of the buffer
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
**-----------------------------------------------------------------------*/
void DrawBuffCel(BYTE *pBuff, long xp, long yp, long nBuffW, BYTE *pCelBuff, long nCel, long nCelW)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen;
	app_assert(pCelBuff != NULL);
	app_assert(pBuff != NULL);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL || pBuff == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	__asm {
		mov		ebx,dword ptr [pCelBuff]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax
		mov		eax,dword ptr [ebx+4]
		sub		eax,dword ptr [ebx]
		mov		dword ptr [RLELen],eax
		mov		eax,dword ptr [pCelBuff]
		add		eax,dword ptr [ebx]
		mov		dword ptr [pFrom],eax
	}
	pTo = pBuff + (nBuffW * yp) + xp;
	__asm {
		mov		esi,dword ptr [pFrom]		// Source
		mov		edi,dword ptr [pTo]		// Dest

		mov		eax,dword ptr [nBuffW]			// Increase width
		add		eax,dword ptr [nCelW]
		mov		dword ptr [nBuffW],eax

		mov		ebx,dword ptr [RLELen]
		add		ebx,esi

_T1Lp1:	mov		edx,dword ptr [nCelW]

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
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
_T1Nxt:	sub		edi,dword ptr [nBuffW]
		cmp		ebx,esi
		jnz		_T1Lp1
	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Makes a mask outline of a 5 offset slab cel in a cel file RAM buffer
**
** ocolor = Color to outline object with
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void OutlineSlabCel(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;
	long nBufferW;
	app_assert(pCelBuff != NULL);
	app_assert(gpBuffer);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL || gpBuffer == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	__asm {
		mov		ebx,dword ptr [pCelBuff]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax
		mov		eax,dword ptr [ebx+4]
		sub		eax,dword ptr [ebx]
		mov		dword ptr [RLELen],eax
		mov		edx,dword ptr [pCelBuff]
		add		edx,dword ptr [ebx]
		mov		dword ptr [pFrom],edx
		add		edx,dword ptr [ostart]
		xor		eax,eax
		mov		ax,word ptr [edx]
		mov		dword ptr [offval],eax
		mov		edx,dword ptr [pFrom]
		add		edx,dword ptr [oend]
		mov		ax,word ptr [edx]
		mov		dword ptr [offval2],eax
	}
	if (offval != 0) {
		if (oend == 8) offval2 = 0;
		if (offval2 != 0) RLELen = offval2 - offval;
		else RLELen -= offval;
		pFrom += offval;
		pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
		__asm {
			mov		esi,dword ptr [pFrom]		// Source
			mov		edi,dword ptr [pTo]			// Dest

			mov		eax,768							// Increase width
			add		eax,dword ptr [nCelW]
			mov		dword ptr [nBufferW],eax

			mov		ebx,dword ptr [RLELen]
			add		ebx,esi

_T1Lp1:		mov		edx,dword ptr [nCelW]

_T1Lp2:		xor		eax,eax				// Load control byte
			lodsb
			or		al,al
			js		_T1J

			sub		edx,eax
			mov		ecx,eax
			mov		ah,byte ptr [ocolor]
_T1Lp3:		lodsb
			or		al,al
			jz		_T1Skip
			mov		byte ptr [edi-768],ah
			mov		byte ptr [edi-1],ah
			mov		byte ptr [edi+1],ah
			mov		byte ptr [edi+768],ah
_T1Skip:	inc		edi
			loop	_T1Lp3
			or		edx,edx
			jz		_T1Nxt
			jmp		_T1Lp2

_T1J:		neg		al					// Do jump
			add		edi,eax
			sub		edx,eax
			jnz		_T1Lp2
_T1Nxt:		sub		edi,dword ptr [nBufferW]
			cmp		ebx,esi
			jnz		_T1Lp1
		}		// end of asm block
	}
}

/*-----------------------------------------------------------------------**
** Makes a mask outline of a 5 offset slab cel in a cel file RAM buffer. With clipping
**
** ocolor = Color to outline object with
** xp = Left X pixel position in offscreen buffer to draw to
** yp = Bottom Y pixel position in offscreen buffer to draw to
** *pCelBuff = pointer to cel file RAM buffer
** nCel = Cel number to draw in cel file (start with 1!!!!)
** nCelW = width of cel to draw
** nLVal = light value
**-----------------------------------------------------------------------*/
void COutlineSlabCel(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE *pTo;
	BYTE *pFrom;
	long RLELen, offval, offval2;
	long nBufferW;
	app_assert(pCelBuff != NULL);
	app_assert(gpBuffer);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (pCelBuff == NULL || gpBuffer == NULL)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	__asm {
		mov		ebx,dword ptr [pCelBuff]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax
		mov		eax,dword ptr [ebx+4]
		sub		eax,dword ptr [ebx]
		mov		dword ptr [RLELen],eax
		mov		edx,dword ptr [pCelBuff]
		add		edx,dword ptr [ebx]
		mov		dword ptr [pFrom],edx
		add		edx,dword ptr [ostart]
		xor		eax,eax
		mov		ax,word ptr [edx]
		mov		dword ptr [offval],eax
		mov		edx,dword ptr [pFrom]
		add		edx,dword ptr [oend]
		mov		ax,word ptr [edx]
		mov		dword ptr [offval2],eax
	}
	if (offval != 0) {
		if (oend == 8) offval2 = 0;
		if (offval2 != 0) RLELen = offval2 - offval;
		else RLELen -= offval;
		pFrom += offval;
		pTo = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
		__asm {
			mov		esi,dword ptr [pFrom]		// Source
			mov		edi,dword ptr [pTo]			// Dest

			mov		eax,768							// Increase width
			add		eax,dword ptr [nCelW]
			mov		dword ptr [nBufferW],eax

			mov		ebx,dword ptr [RLELen]
			add		ebx,esi

_T1Lp1:		mov		edx,dword ptr [nCelW]

_T1Lp2:		xor		eax,eax				// Load control byte
			lodsb
			or		al,al
			js		_T1J

			sub		edx,eax
			mov		ecx,dword ptr [glClipY]
			cmp		edi,ecx
			jb		_T1C1
			add		esi,eax
			add		edi,eax
			jmp		_T1x
_T1C1:		sub		ecx,768
			cmp		edi,ecx
			jae		_T1C2
			mov		ecx,eax
			mov		ah,byte ptr [ocolor]
_T1Lp3:		lodsb
			or		al,al
			jz		_T1Skip
			mov		byte ptr [edi-768],ah
			mov		byte ptr [edi-1],ah
			mov		byte ptr [edi+1],ah
			mov		byte ptr [edi+768],ah
_T1Skip:	inc		edi
			loop	_T1Lp3
			jmp		_T1x

_T1C2:		mov		ecx,eax
			mov		ah,byte ptr [ocolor]
_T1Lp4:		lodsb
			or		al,al
			jz		_T1Skip2
			mov		byte ptr [edi-768],ah
			mov		byte ptr [edi-1],ah
			mov		byte ptr [edi+1],ah
_T1Skip2:	inc		edi
			loop	_T1Lp4

_T1x:		or		edx,edx
			jz		_T1Nxt
			jmp		_T1Lp2

_T1J:		neg		al					// Do jump
			add		edi,eax
			sub		edx,eax
			jnz		_T1Lp2
_T1Nxt:		sub		edi,dword ptr [nBufferW]
			cmp		ebx,esi
			jnz		_T1Lp1
		}		// end of asm block
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
void TranslateCels(byte *p, byte *ttbl, int nf)

{
	int		j;
	app_assert(p != NULL);
	app_assert(ttbl != NULL);

	for (j = 1; j <= nf; j++) {
		__asm {
			mov		ebx,dword ptr [p]
			mov		eax,dword ptr [j]
			shl		eax,2
			add		ebx,eax
			mov		esi,dword ptr [p]
			add		esi,dword ptr [ebx]
			add		esi,10							// Source

			mov		edx,dword ptr [ebx+4]			// Size
			sub		edx,dword ptr [ebx]
			sub		edx,10

			mov		edi,esi							// Dest

			mov		ebx,dword ptr [ttbl]			// Color conversion table

_TLp1:		xor		eax,eax				// Load control byte
			lodsb
			stosb
			dec		edx
			jz		_TNxt
			or		al,al
			js		_TLp1

			sub		edx,eax
			mov		ecx,eax
_TLp2:		lodsb
			xlatb
			stosb
			loop	_TLp2
			or		edx,edx
			jnz		_TLp1

_TNxt:		nop
		}
	}
}
#endif


/*-----------------------------------------------------------------------**
** Plot a point
**-----------------------------------------------------------------------*/
void DrawPoint(int x, int y, byte c)
{
	BYTE *pTo;

	app_assert(gpBuffer);
	if ((y < 0) || (y >= 640)) return;
	if ((x < 64) || (x >= 704)) return;
	pTo = gpBuffer + nBuffWTbl[y] + x;
	__asm {
		mov		edi, dword ptr [pTo];
		cmp		edi, dword ptr [glClipY]
		jae		_NoD
		mov		al, byte ptr [c]
		mov		byte ptr [edi], al
_NoD:
	}
}

/*-----------------------------------------------------------------------**
** For line drawing
**-----------------------------------------------------------------------*/
#define dlswap(a,b)			{ a^=b; b^=a; a^=b; }
#define absolute(i,j,k)		( (i-j)*(k = ( (i-j)<0 ? -1 : 1)))

int dlreverse;
byte dlcolor;
BOOL dlclipflag;


/*-----------------------------------------------------------------------**
** plot for line drawing (temp probably, will write in asm)
**-----------------------------------------------------------------------*/
void plot(int x, int y)
{
	BYTE *pTo;

	app_assert(gpBuffer);
	if (dlreverse) {
		if (dlclipflag) {
			if ((x < 0) || (x >= 640)) return;
			if ((y < 64) || (y >= 704)) return;
		}
		pTo = gpBuffer + nBuffWTbl[x] + y;
	} else {
		if (dlclipflag) {
			if ((y < 0) || (y >= 640)) return;
			if ((x < 64) || (x >= 704)) return;
		}
		pTo = gpBuffer + nBuffWTbl[y] + x;
	}
	__asm {
		mov		edi, dword ptr [pTo];
		cmp		edi,dword ptr [glClipY]
		jae		_NoD
		mov		al,byte ptr [dlcolor]
		mov		byte ptr [edi],al
_NoD:
	}
}


/*-----------------------------------------------------------------------**
** line drawing ctrl (temp probably, will write in asm)
**-----------------------------------------------------------------------*/
void DrawLine(int a1, int b1, int a2, int b2, byte clr)
{
	int		dx, dy, incr1, incr2, D, x, y, xend, c, pixels_left;
	int		x1, y1;
	int		sign_x, sign_y, step, i;

	dlcolor = clr;

	// Test clipping bounds to see if we even need to test on the pixel level
	dlclipflag = FALSE;
	if ((a1 < 64) || (a1 >= 704)) dlclipflag = TRUE;
	if ((a2 < 64) || (a2 >= 704)) dlclipflag = TRUE;
	if ((b1 < 160) || (b1 >= 512)) dlclipflag = TRUE;
	if ((b2 < 160) || (b2 >= 512)) dlclipflag = TRUE;

	dx = absolute(a2, a1, sign_x);
	dy = absolute(b2, b1, sign_y);

	if (sign_x == sign_y)
		step = 1;
	else
		step = -1;

	if (dy > dx) {
		dlswap(a1, b1);
		dlswap(a2, b2);
		dlswap(dx, dy);
		dlreverse = 1;
	} else
		dlreverse = 0;

	if (a1 > a2) {
		x = a2;
		y = b2;
		x1 = a1;
		y1 = b1;
	} else {
		x = a1;
		y = b1;
		x1 = a2;
		y1 = b2;
	}

	xend = (dx - 1) / 4;
	pixels_left = (dx - 1) % 4;

	plot(x, y);
	plot(x1, y1);
	incr2 = 4 * dy - 2 * dx;
	if (incr2 < 0) {
		c = 2 * dy;
		incr1 = 2 * c;
		D = incr1 - dx;

		for (i = 0; i < xend; i++) {
			++x;
			--x1;
			if (D < 0) {
				plot(x, y);
				plot(++x, y);
				plot(x1, y1);
				plot(--x1, y1);
				D += incr1;
			} else {
				if (D < c) {
					plot(x, y);
					plot(++x, y += step);
					plot(x1, y1);
					plot(--x1, y1 -= step);
				} else {
					plot(x, y += step);
					plot(++x, y);
					plot(x1, y1 -= step);
					plot(--x1, y1);
				}
				D += incr2;
			}
		}

		if (pixels_left) {
			if (D < 0) {
				plot(++x, y);
				if (pixels_left > 1)
					plot(++x, y);
				if (pixels_left > 2)
					plot(--x1, y1);
			} else {
				if (D < c) {
					plot(++x, y);
					if (pixels_left > 1)
						plot(++x, y += step);
					if (pixels_left > 2)
						plot(--x1, y1);
				} else {
					plot(++x, y += step);
					if (pixels_left > 1)
						plot(++x, y);
					if (pixels_left > 2)
						plot(--x1, y1 -= step);
				}
			}
		}
	} else {
		c = 2 * (dy - dx);
		incr1 = 2 * c;
		D = incr1 + dx;
		for (i = 0; i < xend; i++) {
			++x;
			--x1;
			if (D > 0) {
				plot(x, y += step);
				plot(++x, y += step);
				plot(x1, y1 -= step);
				plot(--x1, y1 -= step);
				D += incr1;
			} else {
				if (D < c) {
					plot(x, y);
					plot(++x, y += step);
					plot(x1, y1);
					plot(--x1, y1 -= step);
				} else {
					plot(x, y += step);
					plot(++x, y);
					plot(x1, y1 -= step);
					plot(--x1, y1);
				}
				D += incr2;
			}
		}

		if (pixels_left) {
			if (D > 0) {
				plot(++x, y += step);
				if (pixels_left > 1)
					plot(++x, y += step);
				if (pixels_left > 2)
					plot(--x1, y1 -= step);
			} else {
				if (D < c) {
					plot(++x, y);
					if (pixels_left > 1)
						plot(++x, y += step);
					if (pixels_left > 2)
						plot(--x1, y1);
				} else {
					plot(++x, y += step);
					if (pixels_left > 1)
						plot(++x, y);
					if (pixels_left > 2) {
						if (D > c)
							plot(--x1, y1 -= step);
						else
							plot(--x1, y1);
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
int GetDirection(int x1, int y1, int x2, int y2)
{
	int mx, my, md;

	mx = x2-x1;
	my = y2-y1;
	if (mx >= 0) {
		if (my >= 0) {
			md = 0;
			if ((mx << 1) < my) md = 1;
			if ((my << 1) < mx) md = 7;
		} else {
			md = 6;
			my = -my;
			if ((mx << 1) < my) md = 5;
			if ((my << 1) < mx) md = 7;
		}
	} else {
		if (my >= 0) {
			md = 2;
			mx = -mx;
			if ((mx << 1) < my) md = 1;
			if ((my << 1) < mx) md = 3;
		} else {
			md = 4;
			mx = -mx;
			my = -my;
			if ((mx << 1) < my) md = 5;
			if ((my << 1) < mx) md = 3;
		}
	}
	return (md);
}

int SeedCount;
long orgseed;

//***************************************************************************
//***************************************************************************
void SetRndSeed(long s) {
	sglGameSeed = s;
	orgseed = s;		//debug
	SeedCount = 0;
}

//***************************************************************************
//***************************************************************************
long GetRndSeed() {
	SeedCount++;
	static const DWORD INCREMENT = 1;
	static const DWORD MULTIPLIER = 0x015a4e35L;
	sglGameSeed = MULTIPLIER * sglGameSeed + INCREMENT;
	return abs(sglGameSeed);
}


//***************************************************************************
//***************************************************************************
long random(byte idx,long v) {
	if (v <= 0) return 0;

	// high order bits are more "random" than low order bits
	if (v < 0x0ffff) return (GetRndSeed() >> 16) % v;

	return GetRndSeed() % v;
}


//******************************************************************
// memory manager vars
//******************************************************************
#define TRACK_MEM_BLOCKS	1	// 0 in final
#if DEBUG_MEM
#define DUMP_MEM_BLOCKS		1	// 0 in final
#else
#define DUMP_MEM_BLOCKS		0	// 0 in final
#endif
#ifdef NDEBUG
#undef TRACK_MEM_BLOCKS
#undef DUMP_MEM_BLOCKS
#define TRACK_MEM_BLOCKS 0
#define DUMP_MEM_BLOCKS 0
#endif

#if DEBUG_MEM && TRACK_MEM_BLOCKS
typedef struct TMemType {
	struct TMemType * pNext;
	DWORD dwSig;

	// current amount of memory allocated with this signature
	DWORD dwCurrAmount;

	// maximum amount ever allocated for this signature
	DWORD dwMostEver;

	// amount of memory allocated to this signature when the
	// program had its maximum memory allocated
	DWORD dwAmountAtMax;
} TMemType;

typedef union TPrintSig {
	DWORD dwSig;
	char szSig[sizeof(DWORD) + 1];
} TPrintSig;

static DWORD sgdwCurrAllocated = 0;
static DWORD sgdwHighestAllocated = 0;
static TMemType * sgpMemTypeHead = NULL;

typedef struct TMemBlockHdr {
	DWORD dwBytes;
	DWORD dwSig;
	DWORD dwCheck1;
	DWORD dwCheck2;
} TMemBlockHdr;
typedef struct TMemBlockFtr {
	DWORD dwCheck3;
	DWORD dwCheck4;
} TMemBlockFtr;

#define MEM_CHECK1	0x497208ab
#define MEM_CHECK2	0x127834dc
#define MEM_CHECK3	0x023481a9
#define MEM_CHECK4	0xfcb87147

#endif

// serialize access to memory manager -- according to Mike O'Brien,
// malloc does not serialize access in all cases in NT 4.0 even
// when using LIBCMT.LIB.  Therefore, don't trust *any* memory munger
static CCritSect sgMemCrit;


//******************************************************************
//******************************************************************
#if DEBUG_MEM && TRACK_MEM_BLOCKS
static const char * sig_to_string(DWORD dwSig,TPrintSig * pSig) {
	__asm	mov		eax, dwSig
	__asm	bswap	eax
	__asm	mov		dwSig, eax
	pSig->dwSig = dwSig;
	pSig->szSig[sizeof(DWORD)] = 0;
	return pSig->szSig;

}
#endif


//******************************************************************
//******************************************************************
void mem_cleanup(BOOL bNormalExit) {
#if DEBUG_MEM && TRACK_MEM_BLOCKS
	// serialize access to memory manager -- according to Mike O'Brien,
	// malloc does not serialize access in all cases in NT 4.0 even
	// when using LIBCMT.LIB.  Therefore, don't trust *any* memory munger
	sgMemCrit.Enter();

	FILE * f = NULL;

	#if DUMP_MEM_BLOCKS
	if (bNormalExit) f = fopen("c:\\memdump.txt","wb");
	#endif

	if (f) {
		fprintf(
			f,
			"sig       most ever    amount at max    at end of game\r\n"
			"------------------------------------------------------\r\n"
		);
	}

	TMemType * pNext = NULL;
	for (TMemType * pType = sgpMemTypeHead; pType; pType = pNext) {
		if (f) {
			TPrintSig sig;
			fprintf(
				f,
				"%4s     0x%08x      0x%08x     %8dk    %8d\r\n",
				sig_to_string(pType->dwSig,&sig),
				pType->dwMostEver,
				pType->dwAmountAtMax,
				pType->dwAmountAtMax / 1024,
				pType->dwCurrAmount
			);
		}

		pNext = pType->pNext;
		SMemFree(pType,__FILE__,__LINE__);
	}

	if (f) {
		fprintf(
			f,
			"------------------------------------------------\r\n"
			"max allocated            0x%08x    %8dk\r\n",
			sgdwHighestAllocated,
			sgdwHighestAllocated / 1024
		);
		fclose(f);
	}

	sgMemCrit.Leave();
#endif
}


//******************************************************************
//******************************************************************
#if DEBUG_MEM && TRACK_MEM_BLOCKS
static TMemType * find_mem_type_by_sig(DWORD dwSig) {
	// find existing block with signature
	for (TMemType * pType = sgpMemTypeHead; pType; pType = pType->pNext)
		if (pType->dwSig == dwSig) return pType;

	// create a new signature block
	pType = (TMemType *) SMemAlloc(sizeof(TMemType),__FILE__,__LINE__);
	ZeroMemory(pType,sizeof TMemType);
	pType->dwSig = dwSig;

	// link to list
	pType->pNext = sgpMemTypeHead;
	sgpMemTypeHead = pType;

	// return new signature block
	return pType;
}
#endif


//******************************************************************
//******************************************************************
#if DEBUG_MEM && TRACK_MEM_BLOCKS
static void mem_addto_type(TMemType * pType,DWORD dwBytes) {
	// fixup memory for this type
	pType->dwCurrAmount += dwBytes;
	if (pType->dwMostEver < pType->dwCurrAmount)
		pType->dwMostEver = pType->dwCurrAmount;

	// fixup memory for all types
	sgdwCurrAllocated += dwBytes;
	if (sgdwHighestAllocated < sgdwCurrAllocated) {
		sgdwHighestAllocated = sgdwCurrAllocated;
		for (TMemType * pHigh = sgpMemTypeHead; pHigh; pHigh = pHigh->pNext)
			pHigh->dwAmountAtMax = pHigh->dwCurrAmount;
	}
}
#endif


//******************************************************************
//******************************************************************
#if DEBUG_MEM && TRACK_MEM_BLOCKS
static const char * mem_freefrom_type(TMemType * pType,DWORD dwBytes) {
	// fixup memory for this type
	DWORD dwTemp = pType->dwCurrAmount;
	pType->dwCurrAmount -= dwBytes;
	if (pType->dwCurrAmount > dwTemp)
		return "memory block signature underflow: %s";

	// fixup global memory indicator
	dwTemp = sgdwCurrAllocated;
	sgdwCurrAllocated -= dwBytes;
	if (sgdwCurrAllocated > dwTemp)
		return "memory free underflow";

	return NULL;
}
#endif


//******************************************************************
//******************************************************************
#if DEBUG_MEM
void mem_use_sig(DWORD dwSig,DWORD dwBytes) {
#if TRACK_MEM_BLOCKS
	TMemType * pType = find_mem_type_by_sig(dwSig);
	if (! pType) ErrorDlg(IDD_MEM_ERR,GetLastError(),__FILE__,__LINE__);

	mem_addto_type(pType,dwBytes);
#endif
}


//******************************************************************
//******************************************************************
void mem_unuse_sig(DWORD dwSig,DWORD dwBytes) {
#if TRACK_MEM_BLOCKS
	const char * pszErr_s;
	TMemType * pType = find_mem_type_by_sig(dwSig);
	if (pType)
		pszErr_s = mem_freefrom_type(pType,dwBytes);
	else
		pszErr_s = "Bad signature unused: %s";
		
	TPrintSig sig;
	if (pszErr_s) app_fatal(pszErr_s,sig_to_string(dwSig,&sig));
#endif
}
#endif


//******************************************************************
//******************************************************************
#if DEBUG_MEM
BYTE * mem_malloc_dbg(DWORD dwBytes,DWORD dwSig,DWORD dwLine,const TCHAR * pszFile)
#else
BYTE * DiabloAllocPtr(DWORD dwBytes)
#endif
{
	// serialize access to memory manager -- according to Mike O'Brien,
	// malloc does not serialize access in all cases in NT 4.0 even
	// when using LIBCMT.LIB.  Therefore, don't trust *any* memory munger
	sgMemCrit.Enter();

	// allocate memory block
	#if DEBUG_MEM && TRACK_MEM_BLOCKS
	DWORD dwAlloc = dwBytes + sizeof(TMemBlockHdr) + sizeof(TMemBlockFtr);

	BYTE * rv = (BYTE *) SMemAlloc(dwAlloc,pszFile,dwLine);
	#elif DEBUG_MEM
	BYTE * rv = (BYTE *) SMemAlloc(dwBytes,pszFile,dwLine);
	#else
	BYTE * rv = (BYTE *) SMemAlloc(dwBytes,__FILE__,__LINE__);
	#endif

	#if DEBUG_MEM && TRACK_MEM_BLOCKS
	if (rv) {
		TMemType * pType = find_mem_type_by_sig(dwSig);
		if (pType) {
			mem_addto_type(pType,dwBytes);

			TMemBlockHdr * pHdr = (TMemBlockHdr *) rv;
			rv += sizeof(TMemBlockHdr);
			TMemBlockFtr * pFtr = (TMemBlockFtr *) (rv + dwBytes);

			pHdr->dwSig = dwSig;
			pHdr->dwBytes = dwBytes;
			pHdr->dwCheck1 = MEM_CHECK1;
			pHdr->dwCheck2 = MEM_CHECK2;

			pFtr->dwCheck3 = MEM_CHECK3;
			pFtr->dwCheck4 = MEM_CHECK4;
		}
		else {
			SMemFree(rv,__FILE__,__LINE__);
			rv = NULL;
		}
	}
	#endif

	sgMemCrit.Leave();

	#if DEBUG_MEM
	if (! rv) ErrorDlg(IDD_MEM_ERR,GetLastError(),pszFile,dwLine);
	#else
	if (! rv) ErrorDlg(IDD_MEM_ERR,GetLastError(),__FILE__,__LINE__);
	#endif

	return rv;
}


//******************************************************************
//******************************************************************
#if DEBUG_MEM
void mem_free_dbg(void * p,DWORD dwLine,const TCHAR * pszFile)
#else
void mem_free_dbg(void * p)
#endif
{
	if (! p) return;

	// serialize access to memory manager -- according to Mike O'Brien,
	// malloc does not serialize access in all cases in NT 4.0 even
	// when using LIBCMT.LIB.  Therefore, don't trust *any* memory munger
	sgMemCrit.Enter();

	// free memory block
#if DEBUG_MEM && TRACK_MEM_BLOCKS
	const char * pszErr_s = NULL;
	TMemBlockHdr * pHdr = (TMemBlockHdr *) ((BYTE *) p - sizeof(TMemBlockHdr));
	DWORD dwSig = pHdr->dwSig;	// assume signature is valid
	if (pHdr->dwCheck1 != MEM_CHECK1 || pHdr->dwCheck2 != MEM_CHECK2)
		pszErr_s = "Memory block header corruption: %s";
	else {
		// Now change it so if there is duplicate free we'll catch it.
		pHdr->dwCheck1 = 0xDEADC0DE;
		pHdr->dwCheck2 = 0xDEADC0DE;
	}

	if (! pszErr_s) {
		TMemBlockFtr * pFtr = (TMemBlockFtr *) ((BYTE *) p + pHdr->dwBytes);
		if (pFtr->dwCheck3 != MEM_CHECK3 || pFtr->dwCheck4 != MEM_CHECK4)
			pszErr_s = "Memory block footer corruption: %s";
		else {
			// Now change it so if there is duplicate free we'll catch it.
			pFtr->dwCheck3 = 0xDEADC0DE;
			pFtr->dwCheck4 = 0xDEADC0DE;
		}
	}
	if (! pszErr_s) {
		TMemType * pType = find_mem_type_by_sig(dwSig);
		if (pType)
			pszErr_s = mem_freefrom_type(pType,pHdr->dwBytes);
		else
			pszErr_s = "Attempt to free memory with unknown signature %s";
	}

	p = (void *) pHdr;
	SMemFree(p,pszFile,dwLine);
#elif DEBUG_MEM
	SMemFree(p,pszFile,dwLine);
#else
	SMemFree(p,__FILE__,__LINE__);
#endif

	sgMemCrit.Leave();

	// display any error which occurred -- outside critical section
	#if DEBUG_MEM && TRACK_MEM_BLOCKS
	if (pszErr_s) {
		TPrintSig sig;
		app_fatal(pszErr_s,sig_to_string(dwSig,&sig));
	}
	#endif
}


//******************************************************************
//******************************************************************
#if DEBUG_MEM
BYTE * load_file_dbg(const char * pszName,DWORD * pdwFileLen,DWORD dwSig,DWORD dwLine,const TCHAR * pszFile)
#else
BYTE * LoadFileInMem(const char * pszName,DWORD * pdwFileLen)
#endif
{
	HSFILE hFile;

	#if DEBUG_MEM
	if (! pszName) app_fatal("LoadFileInMem: %s:%d",pszFile,dwLine);
	#else
	app_assert(pszName);
	#endif

	patSFileOpenFile(pszName,&hFile);
	DWORD dwFileLen = patSFileGetFileSize(hFile,NULL);
	if (pdwFileLen) *pdwFileLen = dwFileLen;
	if (! dwFileLen) app_fatal("Zero length SFILE:\n%s",pszName);

	#if DEBUG_MEM
	BYTE * pbMem = mem_malloc_dbg(dwFileLen,dwSig,dwLine,pszFile);
	#else
	BYTE * pbMem = DiabloAllocPtr(dwFileLen);
	#endif

	patSFileReadFile(hFile,pbMem,dwFileLen);
	patSFileCloseFile(hFile);
	return pbMem;
}


//******************************************************************
//******************************************************************
DWORD LoadFileWithMem(const char * pszName,BYTE * pbMem) {
	HSFILE hFile;

	app_assert(pszName);
	if (! pbMem) app_fatal("LoadFileWithMem(NULL):\n%s",pszName);

	patSFileOpenFile(pszName,&hFile);
	DWORD dwFileLen = patSFileGetFileSize(hFile,NULL);
	if (! dwFileLen) app_fatal("Zero length SFILE:\n%s",pszName);
	patSFileReadFile(hFile,pbMem,dwFileLen);
	patSFileCloseFile(hFile);
	return dwFileLen;
}


//*************************************************************
//	RLE Unitdraw Code
//*************************************************************
#if RLE_DRAW
	#define MAX_RLE_COPY	65
	static int sgnWidth;		//used in both RLEDrawLitUnit f'cns
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void TranslateCels(BYTE *p, BYTE *ttbl, int nFrames)
{
	app_assert(p != NULL);
	app_assert(ttbl != NULL);

	int nDataSize;
	BYTE * pImage;
	char bBlock;
	for (int j = 1; j <= nFrames; j++) {
		pImage = p + ((DWORD*)p)[j] + sizeof(WORD)*5;
		nDataSize = ((DWORD*)p)[j+1] - ((DWORD*)p)[j] - sizeof(WORD)*5;
		while (nDataSize) {
			bBlock = *pImage++;
			nDataSize--;
			app_assert(nDataSize >= 0);
			//check for skip
			if (bBlock >= 0)
				continue;
			bBlock = -bBlock;
			//check for run
			if (bBlock > MAX_RLE_COPY) {
				bBlock -= MAX_RLE_COPY;
				nDataSize--;
				app_assert(nDataSize >= 0);
				*pImage = ttbl[*pImage++];
				continue;
			}
			//do copy
			nDataSize -= bBlock;
			app_assert(nDataSize >= 0);
			while (bBlock--)
				*pImage = ttbl[*pImage++];
		}
	}
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
static void RLEDrawUnit(BYTE * pDst, BYTE * pRLEData, int nDataSize, int nWidth) {
	_asm {
		//**** Assumes _fastcall convention!! ****
		push	ebx
		push	esi
		push	edi

		//eax - copy/run/skip block size
		//ebx - pixels left to draw this line
		//ecx - bytes of RLE data left
		//edx - scratch
		//esi - source RLE data
		//edi - destination buffer

		mov		esi, edx
		mov		edi, ecx

		xor		eax, eax
		mov		ebx, [nWidth]
		mov		ecx, [nDataSize]

Loop1:
		mov		al,[esi]
		inc		esi
		dec		ecx

		//negative packets are copy/runs, nonnegative are skips
		test	al,al
		jns		SkipLoop

		neg		al
		//packets larger than MAX_RLE_COPY are runs, rest are copies
		cmp		al,MAX_RLE_COPY
		jle		Copy

		//get size of run
		sub		al,MAX_RLE_COPY
		dec		ecx

		//write out run packet
		mov		dl,[esi]
		inc		esi
		sub		ebx,eax
RunMemset:
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		RunMemset
		jmp		EOLTest

Copy:
		//write out copy packet
		sub		ecx,eax
		sub		ebx,eax
CopyMemcpy:
		mov		dl,[esi]
		inc		esi
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		CopyMemcpy

EOLTest:
		//if at end of line, move dst and reset linecounter
		test	ebx,ebx
		jnz		NextBlock
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
		jmp		NextBlock

SkipLoop:
		//does skip extend past end of this line?
		cmp		eax,ebx
		jle		SingleLineSkip
		mov		edx,ebx
		add		edi,ebx		
		sub		eax,ebx
		jmp		SkipMem
SingleLineSkip:
		mov		edx,eax
		add		edi,eax		
		xor		eax,eax
SkipMem:
		//if at end of line, move dst and reset linecounter
		sub		ebx,edx
		jnz		SkipNotEOL
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
SkipNotEOL:
		//have we finished off this block? (skips may span multiple lines)
		test	eax,eax
		jnz		SkipLoop
				
NextBlock:
		test	ecx,ecx
		jnz		Loop1

		pop		edi
		pop		esi
		pop		ebx
	}
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
static void RLEDrawOutline(BYTE * pDst, BYTE * pRLEData, int nDataSize, int nWidth, BYTE bOutLineColor) {
	_asm {
		//**** Assumes _fastcall convention!! ****
		push	ebx
		push	esi
		push	edi

		//eax - copy/run/skip block size
		//ebx - pixels left to draw this line
		//ecx - bytes of RLE data left
		//dl  - outline color
		//dh  - scratch
		//esi - source RLE data
		//edi - destination buffer

		mov		esi,edx
		mov		edi,ecx

		xor		eax,eax
		mov		ebx,[nWidth]
		xor		edx,edx
		mov		ecx,[nDataSize]
		mov		dl, [bOutLineColor]

Loop1:
		mov		al,[esi]
		inc		esi
		dec		ecx

		//negative packets are copy/runs, nonnegative are skips
		test	al,al
		jns		SkipLoop

		neg		al
		//packets larger than MAX_RLE_COPY are runs, rest are copies
		cmp		al,MAX_RLE_COPY
		jle		Copy

		//get size of run
		sub		al,MAX_RLE_COPY
		dec		ecx

		//no outline for color zero (shadows)
		mov		dh,[esi]
		inc		esi
		test	dh,dh
		jz		SkipLoop

		//write outline for run packet
		mov		[edi-1],dl		//outline pixel left
		sub		ebx,eax
		mov		[edi+eax],dl	//and right of run
RunMemset:
		mov		[edi-BUFFERX],dl	//outline above
		mov		[edi+BUFFERX],dl	//and below run
		dec		eax
		lea		edi,[edi+1]
		jnz		RunMemset
		jmp		EOLCheck

Copy:
		sub		ecx,eax
		sub		ebx,eax
CopyMemcpy:
		mov		dh,[esi]
		inc		esi
		//don't draw outline for shadow pixels
		test	dh,dh
		jz		SkipPixel

		mov		[edi-1],dl		//outline pixel left
		mov		[edi+1],dl	//and right of copy
		mov		[edi-BUFFERX],dl	//outline above
		mov		[edi+BUFFERX],dl	//and below copy
SkipPixel:
		dec		eax
		lea		edi,[edi+1]
		jnz		CopyMemcpy

EOLCheck:
		//if at end of line, move dst and reset linecounter
		test	ebx,ebx
		jnz		NextBlock
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
		jmp		NextBlock

SkipLoop:
		cmp		eax,ebx
		jle		SingleLineSkip
		mov		edx,ebx
		add		edi,ebx		
		sub		eax,ebx
		jmp		SkipMem
SingleLineSkip:
		mov		edx,eax
		add		edi,eax		
		xor		eax,eax
SkipMem:
		//if at end of line, move dst and reset linecounter
		sub		ebx,edx
		jnz		SkipNotEOL
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
SkipNotEOL:
		//have we finished off this block?
		test	eax,eax
		jnz		SkipLoop
		mov		dl, [bOutLineColor]
				
NextBlock:
		test	ecx,ecx
		jnz		Loop1

		pop		edi
		pop		esi
		pop		ebx
	}
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
static void RLEDrawLitUnit(BYTE * pDst, BYTE * pRLEData, int nDataSize, int nWidth, BYTE * pLightTable) {
	_asm {
		//**** Assumes _fastcall convention!! ****
		push	ebx
		push	esi
		push	edi

		//eax - copy/run/skip block size
		//ebx - pixels left to draw this line
		//ecx - bytes of RLE data left
		//edx - scratch
		//ebp - light table pointer
		//esi - source RLE data
		//edi - destination buffer

		mov		esi, edx
		mov		edi, ecx

		mov		ebx, [nWidth]
		mov		ecx, [nDataSize]
		mov		edx, [pLightTable]
		push	ebp
		mov		[sgnWidth],ebx		//cheesy way to avoid using ebp
		mov		ebp,edx
		xor		eax,eax
		xor		edx,edx

Loop1:
		mov		al,[esi]
		inc		esi
		dec		ecx

		//negative packets are copy/runs, nonnegative are skips
		test	al,al
		jns		SkipLoop

		neg		al
		//packets larger than MAX_RLE_COPY are runs, rest are copies
		cmp		al,MAX_RLE_COPY
		jle		Copy

		//get size of run
		sub		al,MAX_RLE_COPY
		dec		ecx

		//write run packet
		sub		ebx,eax
		mov		dl,[esi]
		inc		esi
		mov		dl,[ebp+edx]
RunMemset:
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		RunMemset
		jmp		EOLCheck

Copy:
		//write copy packet
		sub		ecx,eax
		sub		ebx,eax
CopyMemcpy:
		mov		dl,[esi]
		inc		esi
		mov		dl,[ebp+edx]
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		CopyMemcpy

EOLCheck:
		//if at end of line, move dst and reset linecounter
		test	ebx,ebx
		jnz		NextBlock
		mov		ebx,[sgnWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
		jmp		NextBlock

SkipLoop:
		cmp		eax,ebx
		jle		SingleLineSkip
		mov		edx,ebx
		add		edi,ebx		
		sub		eax,ebx
		jmp		SkipMem
SingleLineSkip:
		mov		edx,eax
		add		edi,eax		
		xor		eax,eax
SkipMem:
		//if at end of line, move dst and reset linecounter
		sub		ebx,edx
		jnz		SkipNotEOL
		mov		ebx,[sgnWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
SkipNotEOL:
		//have we finished off this block?
		test	eax,eax
		jnz		SkipLoop
				
NextBlock:
		test	ecx,ecx
		jnz		Loop1

		pop		ebp

		pop		edi
		pop		esi
		pop		ebx
	}
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
static void RLEDrawUnitClipped(BYTE * pDst, BYTE * pRLEData, int nDataSize, int nWidth) {
	_asm {
		//**** Assumes _fastcall convention!! ****
		push	ebx
		push	esi
		push	edi

		//eax - copy/run/skip block size
		//ebx - pixels left to draw this line
		//ecx - bytes of RLE data left
		//edx - scratch
		//esi - source RLE data
		//edi - destination buffer

		mov		esi, edx
		mov		edi, ecx

		xor		eax, eax
		mov		ebx, [nWidth]
		mov		ecx, [nDataSize]

Loop1:
		mov		al,[esi]
		inc		esi
		dec		ecx

		//negative packets are copy/runs, nonnegative are skips
		test	al,al
		jns		SkipLoop

		neg		al
		//packets larger than MAX_RLE_COPY are runs, rest are copies
		cmp		al,MAX_RLE_COPY
		jle		Copy

		//get size of run
		sub		al,MAX_RLE_COPY
		dec		ecx

		//prepare to write out run packet
		mov		dl,[esi]
		inc		esi

		//off end of buffer?
		cmp		edi,[glClipY]
		jge		SkipLoop

		sub		ebx,eax
RunMemset:
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		RunMemset
		jmp		EOLTest

Copy:
		//prepare to write out copy packet
		sub		ecx,eax

		//off end of buffer?
		cmp		edi,[glClipY]
		jl		PreCopyMemcpy
		add		esi,eax
		jmp		SkipLoop

PreCopyMemcpy:
		sub		ebx,eax
CopyMemcpy:
		mov		dl,[esi]
		inc		esi
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		CopyMemcpy

EOLTest:
		//if at end of line, move dst and reset linecounter
		test	ebx,ebx
		jnz		NextBlock
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
		jmp		NextBlock

SkipLoop:
		//does skip extend past end of this line?
		cmp		eax,ebx
		jle		SingleLineSkip
		mov		edx,ebx
		add		edi,ebx		
		sub		eax,ebx
		jmp		SkipMem
SingleLineSkip:
		mov		edx,eax
		add		edi,eax		
		xor		eax,eax
SkipMem:
		//if at end of line, move dst and reset linecounter
		sub		ebx,edx
		jnz		SkipNotEOL
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
SkipNotEOL:
		//have we finished off this block? (skips may span multiple lines)
		test	eax,eax
		jnz		SkipLoop
				
NextBlock:
		test	ecx,ecx
		jnz		Loop1

		pop		edi
		pop		esi
		pop		ebx
	}
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
static void RLEDrawOutlineClipped(BYTE * pDst, BYTE * pRLEData, int nDataSize, int nWidth, BYTE bOutLineColor) {
	_asm {
		//**** Assumes _fastcall convention!! ****
		push	ebx
		push	esi
		push	edi

		//eax - copy/run/skip block size
		//ebx - pixels left to draw this line
		//ecx - bytes of RLE data left
		//dl  - outline color
		//dh  - scratch
		//esi - source RLE data
		//edi - destination buffer

		mov		esi,edx
		mov		edi,ecx

		xor		eax,eax
		mov		ebx,[nWidth]
		xor		edx,edx
		mov		ecx,[nDataSize]
		mov		dl, [bOutLineColor]

Loop1:
		mov		al,[esi]
		inc		esi
		dec		ecx

		//negative packets are copy/runs, nonnegative are skips
		test	al,al
		jns		SkipLoop

		neg		al
		//packets larger than MAX_RLE_COPY are runs, rest are copies
		cmp		al,MAX_RLE_COPY
		jle		Copy

		//get size of run
		sub		al,MAX_RLE_COPY
		dec		ecx

		//no outline for color zero (shadows)
		mov		dh,[esi]
		inc		esi
		test	dh,dh
		jz		SkipLoop

		//off end of buffer?
		cmp		edi,[glClipY]
		jge		SkipLoop

		//write outline for run packet
		mov		[edi-1],dl		//outline pixel left
		sub		ebx,eax
		mov		[edi+eax],dl	//and right of run
RunMemset:
		mov		[edi-BUFFERX],dl	//outline above
		mov		[edi+BUFFERX],dl	//and below run
		dec		eax
		lea		edi,[edi+1]
		jnz		RunMemset
		jmp		EOLCheck

Copy:
		sub		ecx,eax

		//off end of buffer?
		cmp		edi,[glClipY]
		jl		PreCopyMemcpy
		add		esi,eax
		jmp		SkipLoop

PreCopyMemcpy:
		sub		ebx,eax
CopyMemcpy:
		mov		dh,[esi]
		inc		esi
		//don't draw outline for shadow pixels
		test	dh,dh
		jz		SkipPixel

		mov		[edi-1],dl		//outline pixel left
		mov		[edi+1],dl	//and right of copy
		mov		[edi-BUFFERX],dl	//outline above
		mov		[edi+BUFFERX],dl	//and below copy
SkipPixel:
		dec		eax
		lea		edi,[edi+1]
		jnz		CopyMemcpy

EOLCheck:
		//if at end of line, move dst and reset linecounter
		test	ebx,ebx
		jnz		NextBlock
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
		jmp		NextBlock

SkipLoop:
		cmp		eax,ebx
		jle		SingleLineSkip
		mov		edx,ebx
		add		edi,ebx		
		sub		eax,ebx
		jmp		SkipMem
SingleLineSkip:
		mov		edx,eax
		add		edi,eax		
		xor		eax,eax
SkipMem:
		//if at end of line, move dst and reset linecounter
		sub		ebx,edx
		jnz		SkipNotEOL
		mov		ebx,[nWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
SkipNotEOL:
		//have we finished off this block?
		test	eax,eax
		jnz		SkipLoop
		mov		dl, [bOutLineColor]
				
NextBlock:
		test	ecx,ecx
		jnz		Loop1

		pop		edi
		pop		esi
		pop		ebx
	}
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
static void RLEDrawLitUnitClipped(BYTE * pDst, BYTE * pRLEData, int nDataSize, int nWidth, BYTE * pLightTable) {
	_asm {
		//**** Assumes _fastcall convention!! ****
		push	ebx
		push	esi
		push	edi

		//eax - copy/run/skip block size
		//ebx - pixels left to draw this line
		//ecx - bytes of RLE data left
		//edx - scratch
		//ebp - light table pointer
		//esi - source RLE data
		//edi - destination buffer

		mov		esi, edx
		mov		edi, ecx

		mov		ebx, [nWidth]
		mov		ecx, [nDataSize]
		mov		edx, [pLightTable]
		push	ebp
		mov		[sgnWidth],ebx		//cheesy way to avoid using ebp
		mov		ebp,edx
		xor		eax,eax
		xor		edx,edx

Loop1:
		mov		al,[esi]
		inc		esi
		dec		ecx

		//negative packets are copy/runs, nonnegative are skips
		test	al,al
		jns		SkipLoop

		neg		al
		//packets larger than MAX_RLE_COPY are runs, rest are copies
		cmp		al,MAX_RLE_COPY
		jle		Copy

		//get size of run
		sub		al,MAX_RLE_COPY
		dec		ecx

		//prepare to write run packet
		mov		dl,[esi]
		inc		esi
		mov		dl,[ebp+edx]

		//off end of buffer?
		cmp		edi,[glClipY]
		jge		SkipLoop

		sub		ebx,eax
RunMemset:
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		RunMemset
		jmp		EOLCheck

Copy:
		//write copy packet
		sub		ecx,eax

		//off end of buffer?
		cmp		edi,[glClipY]
		jl		PreCopyMemcpy
		add		esi,eax
		jmp		SkipLoop

PreCopyMemcpy:
		sub		ebx,eax
CopyMemcpy:
		mov		dl,[esi]
		inc		esi
		mov		dl,[ebp+edx]
		mov		[edi],dl
		dec		eax
		lea		edi,[edi+1]
		jnz		CopyMemcpy

EOLCheck:
		//if at end of line, move dst and reset linecounter
		test	ebx,ebx
		jnz		NextBlock
		mov		ebx,[sgnWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
		jmp		NextBlock

SkipLoop:
		cmp		eax,ebx
		jle		SingleLineSkip
		mov		edx,ebx
		add		edi,ebx		
		sub		eax,ebx
		jmp		SkipMem
SingleLineSkip:
		mov		edx,eax
		add		edi,eax		
		xor		eax,eax
SkipMem:
		//if at end of line, move dst and reset linecounter
		sub		ebx,edx
		jnz		SkipNotEOL
		mov		ebx,[sgnWidth]
		sub		edi,BUFFERX
		sub		edi,ebx
SkipNotEOL:
		//have we finished off this block?
		test	eax,eax
		jnz		SkipLoop
				
NextBlock:
		test	ecx,ecx
		jnz		Loop1

		pop		ebp

		pop		edi
		pop		esi
		pop		ebx
	}
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawUnit (long xp,long yp,BYTE *pCelBuff,long nCel,long nCelW,long ostart,long oend)
{
	BYTE *pDst, *pSrc;
	DWORD * pFrameTable;
	long RLELen, nFrameStart, nFrameEnd;

	app_assert(gpBuffer != NULL);
	app_assert(pCelBuff != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	nFrameStart = *(WORD*)(pSrc + ostart);
	if (!nFrameStart)
		return;
	nFrameEnd = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (nFrameEnd ? nFrameEnd : pFrameTable[nCel+1] - pFrameTable[nCel]) - nFrameStart;
	pSrc += nFrameStart;
	pDst = gpBuffer + nBuffWTbl[yp - ostart*16] + xp;
	RLEDrawUnit(pDst, pSrc, RLELen, nCelW);
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawUnitOutline(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE * pDst, * pSrc;
	DWORD * pFrameTable;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	app_assert(gpBuffer != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	offval = *(WORD*)(pSrc + ostart);
	if (!offval)
		return;
	offval2 = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (offval2 ? offval2 : pFrameTable[nCel+1] - pFrameTable[nCel]) - offval;
	pSrc += offval;
	pDst = gpBuffer + nBuffWTbl[yp - ostart*16] + xp;
	RLEDrawOutline(pDst,pSrc,RLELen,nCelW,ocolor);
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawInfraUnit(long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend, char loff)
{
	BYTE * pDst, * pSrc;
	DWORD * pFrameTable;
	long RLELen, offval, offval2, ltaboff;

	app_assert(gpBuffer != NULL);
	app_assert(pCelBuff != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	offval = *(WORD*)(pSrc + ostart);
	if (!offval)
		return;
	offval2 = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (offval2 ? offval2 : pFrameTable[nCel+1] - pFrameTable[nCel]) - offval;
	pSrc += offval;
	pDst = gpBuffer + nBuffWTbl[yp - ostart*16] + xp;
	ltaboff = light4flag ? 1024 : 4096;
	if (loff == LIGHT_STONE)
		ltaboff += 256;
	if (loff >= LIGHT_U)
		ltaboff += ((loff - LIGHT_U) << 8) + 768;
	RLEDrawLitUnit(pDst,pSrc,RLELen,nCelW,pLightTbl+ltaboff);
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawLitUnit(long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE * pDst, * pSrc;
	DWORD * pFrameTable;
	long RLELen, offval, offval2;

	app_assert(gpBuffer != NULL);
	app_assert(pCelBuff != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	offval = *(WORD*)(pSrc + ostart);
	if (!offval)
		return;
	offval2 = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (offval2 ? offval2 : pFrameTable[nCel+1] - pFrameTable[nCel]) - offval;
	pSrc += offval;
	pDst = gpBuffer + nBuffWTbl[yp - ostart*16] + xp;
	if (nLVal)
		RLEDrawLitUnit(pDst,pSrc,RLELen,nCelW,pLightTbl + (nLVal << 8));
	else
		RLEDrawUnit(pDst,pSrc,RLELen,nCelW);
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawUnitClipped (long xp,long yp,BYTE *pCelBuff,long nCel,long nCelW,long ostart,long oend)
{
	BYTE * pDst, * pSrc;
	DWORD * pFrameTable;
	long RLELen, offval, offval2;

	app_assert(gpBuffer != NULL);
	app_assert(pCelBuff != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	offval = *(WORD*)(pSrc + ostart);
	if (!offval)
		return;
	offval2 = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (offval2 ? offval2 : pFrameTable[nCel+1] - pFrameTable[nCel]) - offval;
	pSrc += offval;
	pDst = gpBuffer + nBuffWTbl[yp - ostart*16] + xp;
	RLEDrawUnitClipped(pDst, pSrc, RLELen, nCelW);
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawUnitOutlineClipped(byte ocolor, long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE * pDst, * pSrc;
	DWORD * pFrameTable;
	long RLELen, offval, offval2;

	app_assert(pCelBuff != NULL);
	app_assert(gpBuffer != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	offval = *(WORD*)(pSrc + ostart);
	if (!offval)
		return;
	offval2 = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (offval2 ? offval2 : pFrameTable[nCel+1] - pFrameTable[nCel]) - offval;
	pSrc += offval;
	pDst = gpBuffer + nBuffWTbl[yp - ostart*16] + xp;
	glClipY -= BUFFERX;
	RLEDrawOutlineClipped(pDst,pSrc,RLELen,nCelW,ocolor);
	glClipY += BUFFERX;
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawInfraUnitClipped(
	long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend, char loff)
{
	BYTE * pDst, * pSrc;
	DWORD * pFrameTable;
	long RLELen, offval, offval2, ltaboff;

	app_assert(gpBuffer != NULL);
	app_assert(pCelBuff != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	offval = *(WORD*)(pSrc + ostart);
	if (!offval)
		return;
	offval2 = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (offval2 ? offval2 : pFrameTable[nCel+1] - pFrameTable[nCel]) - offval;
	pSrc += offval;
	pDst = gpBuffer + nBuffWTbl[yp - ostart*16] + xp;
	ltaboff = light4flag ? 1024 : 4096;
	if (loff == LIGHT_STONE)
		ltaboff += 256;
	if (loff >= LIGHT_U)
		ltaboff += ((loff - LIGHT_U) << 8) + 768;
	RLEDrawLitUnitClipped(pDst,pSrc,RLELen,nCelW,pLightTbl + ltaboff);
}
#endif


//*************************************************************************
//*************************************************************************
#if RLE_DRAW
void DrawLitUnitClipped(long xp, long yp, BYTE *pCelBuff, long nCel, long nCelW, long ostart, long oend)
{
	BYTE * pDst, * pSrc;
	DWORD * pFrameTable;
	long RLELen, offval, offval2;

	app_assert(gpBuffer != NULL);
	app_assert(pCelBuff != NULL);
	app_assert(nCel > 0);
	// jcm.patch1.start.1/14/97
	#ifdef GRACEFUL_EXIT
	if (gpBuffer == NULL || pCelBuff == NULL || nCel <= 0)
		return;
	#endif
	// jcm.patch1.end.1/14/97

	pFrameTable = (DWORD*)pCelBuff;
	app_assert(nCel <= (int) pFrameTable[0]);
	pSrc = pCelBuff + pFrameTable[nCel];
	offval = *(WORD*)(pSrc + ostart);
	if (!offval)
		return;
	offval2 = (oend == 8) ? 0 : *(WORD*)(pSrc + oend);
	RLELen = (offval2 ? offval2 : pFrameTable[nCel+1] - pFrameTable[nCel]) - offval;
	pSrc += offval;
	pDst = gpBuffer + nBuffWTbl[yp - (ostart << 4)] + xp;
	if (nLVal)
		RLEDrawLitUnitClipped(pDst,pSrc,RLELen,nCelW,pLightTbl + (nLVal << 8));
	else
		RLEDrawUnitClipped(pDst,pSrc,RLELen,nCelW);
}
#endif


//*************************************************************************
//*************************************************************************
void play_movie(const char * pszMovie,BOOL bAllowCancel);
void PlayInGameMovie(const char * pszMovie) {
	PaletteFadeOut(FADE_FAST);
	play_movie(pszMovie,FALSE);
	ClrDraw();
	force_redraw = FULLDRAW;
	FullBlit(TRUE);
	PaletteFadeIn(FADE_FAST);
	force_redraw = FULLDRAW;
}
