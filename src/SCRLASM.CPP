/*-----------------------------------------------------------------------**
** 								Diablo
**
**							Scrolling file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SCRLASM.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
** CheckForScroll
** DrawAndBlit
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"scrlasm.h"
#include	"gendung.h"
#include	"lighting.h"

	//*************************************************************************
	// Externs from scrollrt.cpp
	//*************************************************************************
	extern long nLVal;
	extern DWORD gdwPNum;
	extern long ClipY;
	extern int gnPieceNum;
	extern BOOL nTrans;
	extern char gbPartialTrans;
	extern char lightmax;


	//*************************************************************************
	// Private
	//*************************************************************************
	static long	sgLineVal, cm;
	static byte wt;
	static long	*t;
	static long sgRightMask[32] = {
		0xeaaaaaaa,		// 1110 1010 1010 1010 1010 1010 1010 1010
		0xf5555555,		// 1111 0101 0101 0101 0101 0101 0101 0101
		0xfeaaaaaa,		// 1111 1110 1010 1010 1010 1010 1010 1010
		0xff555555,		// 1111 1111 0101 0101 0101 0101 0101 0101
		0xffeaaaaa,		// 1111 1111 1110 1010 1010 1010 1010 1010
		0xfff55555,		// 1111 1111 1111 0101 0101 0101 0101 0101
		0xfffeaaaa,		// 1111 1111 1111 1110 1010 1010 1010 1010
		0xffff5555,		// 1111 1111 1111 1111 0101 0101 0101 0101
		0xffffeaaa,		// 1111 1111 1111 1111 1110 1010 1010 1010
		0xfffff555,		// 1111 1111 1111 1111 1111 0101 0101 0101
		0xfffffeaa,		// 1111 1111 1111 1111 1111 1110 1010 1010
		0xffffff55,		// 1111 1111 1111 1111 1111 1111 0101 0101
		0xffffffea,		// 1111 1111 1111 1111 1111 1111 1110 1010
		0xfffffff5,		// 1111 1111 1111 1111 1111 1111 1111 0101
		0xfffffffe,		// 1111 1111 1111 1111 1111 1111 1111 1110
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111

		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff };	// 1111 1111 1111 1111 1111 1111 1111 1111

	static long sgLeftMask[32] = {
		0xaaaaaaab,		// 1010 1010 1010 1010 1010 1010 1010 1011
		0x5555555f,		// 0101 0101 0101 0101 0101 0101 0101 1111
		0xaaaaaabf,		// 1010 1010 1010 1010 1010 1010 1011 1111
		0x555555ff,		// 0101 0101 0101 0101 0101 0101 1111 1111
		0xaaaaabff,		// 1010 1010 1010 1010 1010 1011 1111 1111
		0x55555fff,		// 0101 0101 0101 0101 0101 1111 1111 1111
		0xaaaabfff,		// 1010 1010 1010 1010 1011 1111 1111 1111
		0x5555ffff,		// 0101 0101 0101 0101 1111 1111 1111 1111
		0xaaabffff,		// 1010 1010 1010 1011 1111 1111 1111 1111
		0x555fffff,		// 0101 0101 0101 1111 1111 1111 1111 1111
		0xaabfffff,		// 1010 1010 1011 1111 1111 1111 1111 1111
		0x55ffffff,		// 0101 0101 1111 1111 1111 1111 1111 1111
		0xabffffff,		// 1010 1011 1111 1111 1111 1111 1111 1111
		0x5fffffff,		// 0101 1111 1111 1111 1111 1111 1111 1111
		0xbfffffff,		// 1011 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111

		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff,		// 1111 1111 1111 1111 1111 1111 1111 1111
		0xffffffff };	// 1111 1111 1111 1111 1111 1111 1111 1111

	static long sgFullMask[32] = {
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101

		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555,		// 0101 0101 0101 0101 0101 0101 0101 0101
		0xaaaaaaaa,		// 1010 1010 1010 1010 1010 1010 1010 1010
		0x55555555 };	// 0101 0101 0101 0101 0101 0101 0101 0101


/*-----------------------------------------------------------------------**
** Now with Y clipping (top clip)
**-----------------------------------------------------------------------*/

void TDecodeMicroTile (BYTE *pDecodeTo)
{
	t = &microoffset[0][0];
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,dword ptr [nLVal]
		or		al,al
		jz		_NoLt
		cmp		al,byte ptr [lightmax]
		jz		_Black

		mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jnz		_Speed

		mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		ebx,dword ptr [nLVal];			// Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [pLightTbl];

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		_Type0							// Solid 32x32 block
		cmp		ax,1
		jz		_Type1							// 32x32 block with '0' holes
		cmp		ax,2
		jz		_Type2							// Left Triangle
		cmp		ax,3
		jz		_Type3							// Right Triangle
		cmp		ax,4
		jz		_Type4							// Left Triangle to wall
		jmp		_Type5							// Right Triangle to wall

_Speed:	mov		esi,dword ptr [t]
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [nLVal]
		shl		eax,2
		add		esi,eax							// Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
_SCmp:	cmp		ax,8
		jz		_Type8							// PreTrans Solid 32x32 block
		cmp		ax,9
		jz		_Type9							// PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		_TypeA							// PreTrans Left Triangle
		cmp		ax,11
		jz		_TypeB							// PreTrans Right Triangle
		cmp		ax,12
		jz		_TypeC							// PreTrans Left Triangle to wall
		jmp		_TypeD							// PreTrans Right Triangle to wall

_NoLt:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RNoLt

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RNoLt:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		_SCmp

_Black:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RBlk

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RBlk:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		_TypeG							// Black Trans Solid 32x32 block
		cmp		ax,1
		jz		_TypeH							// Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		_TypeI							// Black Trans Left Triangle
		cmp		ax,3
		jz		_TypeJ							// Black Trans Right Triangle
		cmp		ax,4
		jz		_TypeK							// Black Trans Left Triangle to wall
		jmp		_TypeL							// Black Trans Right Triangle to wall

/*-----------------------------------------------------------------------*/

_Type0:	mov		edx,16
_T0Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push edx
		call xquad2
		pop edx

		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push edx
		call xquad1
		pop edx

		sub		edi,NBUFFW32
		dec		edx
		jnz		_T0Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type1:	mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32

_T1Lp1:	push	ecx
		mov		edx,32

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		_T1Od

		push edx
		call xbyte1
		pop edx
		jmp _T1x

_T1Od:
		push edx
		call xbyte2
		pop edx

_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2

_T1Nxt:	pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_T1Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type2:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T2Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T2Od

		push edx
		call xbyte1
		pop edx
		jmp	_T2x

_T2Od:
		push edx
		call xbyte2
		pop edx

_T2x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T2b
		sub		edx,2
		jmp		_T2Lp1

_T2b:	mov		edx,2
_T2Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T2Od2

		push edx
		call xbyte1
		pop edx
		jmp	_T2x2

_T2Od2:
		push edx
		call xbyte2
		pop edx

_T2x2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_T2Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type3:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T3Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T3Od

		push edx
		call xbyte1
		pop edx
		jmp	_T3x

_T3Od:
		push edx
		call xbyte2
		pop edx

_T3x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T3b
		add		edi,edx
		sub		edx,2
		jmp		_T3Lp1

_T3b:	mov		edx,2
_T3Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T3Od2

		push edx
		call xbyte1
		pop edx
		jmp	_T3x2

_T3Od2:
		push edx
		call xbyte2
		pop edx

_T3x2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_T3Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type4:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T4Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T4Od

		push edx
		call xbyte1
		pop edx
		jmp	_T4x

_T4Od:
		push edx
		call xbyte2
		pop edx

_T4x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T4b
		sub		edx,2
		jmp		_T4Lp1

_T4b:	mov		edx,8
_T4Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push edx
		call xquad2
		pop edx

		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push edx
		call xquad1
		pop edx

		sub		edi,NBUFFW32
		dec		edx
		jnz		_T4Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type5:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T5Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T5Od

		push edx
		call xbyte1
		pop edx
		jmp	_T5x

_T5Od:
		push edx
		call xbyte2
		pop edx

_T5x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T5b
		add		edi,edx
		sub		edx,2
		jmp		_T5Lp1

_T5b:	mov		edx,8
_T5Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push edx
		call xquad2
		pop edx

		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push edx
		call xquad1
		pop edx

		sub		edi,NBUFFW32
		dec		edx
		jnz		_T5Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type8:	mov		edx,16
_T8Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_T8Lp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_T8Lp2
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_T8Lp3:	lodsd
		stosb
		inc		edi
		ror		eax,16
		stosb
		inc		edi
		loop	_T8Lp3
		sub		edi,NBUFFW32
		dec		edx
		jnz		_T8Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type9:	mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32

_T9Lp1:	push	ecx
		mov		edx,32

_T9Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T9J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		_T9Od
		shr		ecx,1
		jnc		_T9w
		inc		esi
		inc		edi
		jecxz	_T9x
		jmp		_T9w2
_T9w:	shr		ecx,1
		jnc		_T9Lp3
		inc		esi
		inc		edi
		movsb
		jecxz	_T9x
_T9Lp3:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_T9Lp3
		jmp		_T9x

_T9Od:	shr		ecx,1
		jnc		_T9w2
		movsb
		jecxz	_T9x
		jmp		_T9w
_T9w2:	shr		ecx,1
		jnc		_T9Lp4
		movsb
		inc		esi
		inc		edi
		jecxz	_T9x
_T9Lp4:	lodsd
		stosb
		inc		edi
		ror		eax,16
		stosb
		inc		edi
		loop	_T9Lp4

_T9x:	or		edx,edx
		jz		_T9Nxt
		jmp		_T9Lp2

_T9J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T9Lp2

_T9Nxt:	pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_T9Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeA:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TALp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TAOd
		shr		ecx,2
		jnc		_TALp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TAx
_TALp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TALp2
		jmp		_TAx

_TAOd:	shr		ecx,2
		jnc		_TALp3
		lodsw
		stosb
		inc		edi
		jecxz	_TAx
_TALp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TALp3

_TAx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TAb
		sub		edx,2
		jmp		_TALp1

_TAb:	mov		edx,2
_TALp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TAOd2
		shr		ecx,2
		jnc		_TALp5
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TAx2
_TALp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TALp5
		jmp		_TAx2

_TAOd2:	shr		ecx,2
		jnc		_TALp6
		lodsw
		stosb
		inc		edi
		jecxz	_TAx2
_TALp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TALp6

_TAx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TALp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeB:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TBLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TBOd
		shr		ecx,2
		jnc		_TBLp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TBx
_TBLp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TBLp2
		jmp		_TBx

_TBOd:	shr		ecx,2
		jnc		_TBLp3
		lodsw
		stosb
		inc		edi
		jecxz	_TBx
_TBLp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TBLp3

_TBx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TBb
		add		edi,edx
		sub		edx,2
		jmp		_TBLp1

_TBb:	mov		edx,2
_TBLp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TBOd2
		shr		ecx,2
		jnc		_TBLp5
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TBx2
_TBLp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TBLp5
		jmp		_TBx2

_TBOd2:	shr		ecx,2
		jnc		_TBLp6
		lodsw
		stosb
		inc		edi
		jecxz	_TBx2
_TBLp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TBLp6

_TBx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TBLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeC:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TCLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TCOd
		shr		ecx,2
		jnc		_TCLp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TCx
_TCLp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TCLp2
		jmp		_TCx

_TCOd:	shr		ecx,2
		jnc		_TCLp3
		lodsw
		stosb
		inc		edi
		jecxz	_TCx
_TCLp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TCLp3

_TCx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TCb
		sub		edx,2
		jmp		_TCLp1

_TCb:	mov		edx,8
_TCLp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TCLp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TCLp5
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TCLp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TCLp6
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TCLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeD:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TDLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TDOd
		shr		ecx,2
		jnc		_TDLp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TDx
_TDLp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TDLp2
		jmp		_TDx

_TDOd:	shr		ecx,2
		jnc		_TDLp3
		lodsw
		stosb
		inc		edi
		jecxz	_TDx
_TDLp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TDLp3

_TDx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TDb
		add		edi,edx
		sub		edx,2
		jmp		_TDLp1

_TDb:	mov		edx,8
_TDLp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TDLp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TDLp5
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TDLp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TDLp6
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TDLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeG:	mov		edx,16
		xor		eax,eax
_TGLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TGLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TGLp2
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TGLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TGLp3
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TGLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeH:	mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32

_THLp1:	push	ecx
		mov		edx,32

_THLp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_THJ

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		mov		ecx,eax
		add		esi,ecx
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		_THOd
		xor		eax,eax
		shr		ecx,1
		jnc		_THw
		inc		edi
		jecxz	_THx
		jmp		_THw2
_THw:	shr		ecx,1
		jnc		_THLp3
		inc		edi
		stosb
		jecxz	_THx
_THLp3:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_THLp3
		jmp		_THx

_THOd:	xor		eax,eax
		shr		ecx,1
		jnc		_THw2
		stosb
		jecxz	_THx
		jmp		_THw
_THw2:	shr		ecx,1
		jnc		_THLp4
		stosb
		inc		edi
		jecxz	_THx
_THLp4:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_THLp4

_THx:	or		edx,edx
		jz		_THNxt
		jmp		_THLp2

_THJ:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_THLp2

_THNxt:	pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_THLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeI:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TILp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TIOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TILp2
		inc		edi
		stosb
		jecxz	_TIx
_TILp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TILp2
		jmp		_TIx

_TIOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TILp3
		stosb
		inc		edi
		jecxz	_TIx
_TILp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TILp3

_TIx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TIb
		sub		edx,2
		jmp		_TILp1

_TIb:	mov		edx,2
_TILp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TIOd2
		xor		eax,eax
		shr		ecx,2
		jnc		_TILp5
		inc		edi
		stosb
		jecxz	_TIx2
_TILp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TILp5
		jmp		_TIx2

_TIOd2:	xor		eax,eax
		shr		ecx,2
		jnc		_TILp6
		stosb
		inc		edi
		jecxz	_TIx2
_TILp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TILp6

_TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TILp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeJ:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TJLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TJOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TJLp2
		inc		edi
		stosb
		jecxz	_TJx
_TJLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TJLp2
		jmp		_TJx

_TJOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TJLp3
		stosb
		inc		edi
		jecxz	_TJx
_TJLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TJLp3

_TJx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TJb
		add		edi,edx
		sub		edx,2
		jmp		_TJLp1

_TJb:	mov		edx,2
_TJLp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TJOd2
		xor		eax,eax
		shr		ecx,2
		jnc		_TJLp5
		inc		edi
		stosb
		jecxz	_TJx2
_TJLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TJLp5
		jmp		_TJx2

_TJOd2:	xor		eax,eax
		shr		ecx,2
		jnc		_TJLp6
		stosb
		inc		edi
		jecxz	_TJx2
_TJLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TJLp6

_TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TJLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeK:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TKLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TKOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TKLp2
		inc		edi
		stosb
		jecxz	_TKx
_TKLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TKLp2
		jmp		_TKx

_TKOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TKLp3
		stosb
		inc		edi
		jecxz	_TKx
_TKLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TKLp3

_TKx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TKb
		sub		edx,2
		jmp		_TKLp1

_TKb:	mov		edx,8
		xor		eax,eax
_TKLp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TKLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TKLp5
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TKLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TKLp6
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TKLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeL:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TLLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TLOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TLLp2
		inc		edi
		stosb
		jecxz	_TLx
_TLLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TLLp2
		jmp		_TLx

_TLOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TLLp3
		stosb
		inc		edi
		jecxz	_TLx
_TLLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TLLp3

_TLx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TLb
		add		edi,edx
		sub		edx,2
		jmp		_TLLp1

_TLb:	mov		edx,8
		xor		eax,eax
_TLLp4:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TLLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TLLp5
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
_TLLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TLLp6
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TLLp4
		jmp		_Done

xbyte1:				// blank first

		shr ecx,1
		jnc xword2
		inc esi
		inc edi

xword1:				// xlat first
		shr ecx,1
		jnc xquadt1
		mov dl,[esi]
		mov dl,[ebx+edx]
		add esi,2
		mov [edi],dl
		add edi,2

xquadt1:
		test cl,cl
		jz xend

xquad1:				// xlat first
/* debug
		shl ecx,2
		add esi,ecx
		add edi,ecx
		ret
*/

		mov eax,[esi]
		add esi,4
		mov dl,al
		shr eax,16
		mov dl,[ebx+edx]
		mov [edi],dl
		mov dl,al
		add edi,4
		mov dl,[ebx+edx]
		dec ecx
		mov [edi-2],dl
		jnz xquad1

		ret


xbyte2:				// xlat first

		shr ecx,1
		jnc xword1
		mov dl,[esi]
		mov dl,[ebx+edx]
		inc esi
		mov	[edi],dl
		inc edi

xword2:				// blank first
		shr ecx,1
		jnc xquadt2
		mov dl,[esi+1]
		mov dl,[ebx+edx]
		add esi,2
		mov [edi+1],dl
		add edi,2

xquadt2:
		test cl,cl
		jz xend

xquad2:				// blank first
/* debug
		shl ecx,2
		add esi,ecx
		add edi,ecx
		ret
*/

		
		mov eax,[esi]
		add esi,4
		mov dl,ah
		shr eax,16
		mov dl,[ebx+edx]
		mov [edi+1],dl
		mov dl,ah
		add edi,4
		mov dl,[ebx+edx]
		dec ecx
		mov [edi-1],dl
		jnz xquad2

xend:
		ret

_PDone:	pop		eax
_Done:	nop


	}		// end of asm block
}

/*-----------------------------------------------------------------------**
** Now with Y clipping and Masking Transparency
**-----------------------------------------------------------------------*/

void TDecodeM12Tile (BYTE *pDecodeTo, long *mask)
{
	t = &microoffset[0][0];
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,dword ptr [nLVal]
		or		al,al
		jz		_NoLt
		cmp		al,byte ptr [lightmax]
		jz		_Black

		mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jnz		_Speed

		mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		ebx,dword ptr [nLVal];			// Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [pLightTbl];

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		_Type0							// Solid 32x32 block
		cmp		ax,1
		jz		_Type1							// 32x32 block with '0' holes
		cmp		ax,2
		jz		_Type2							// Left Triangle
		cmp		ax,3
		jz		_Type3							// Right Triangle
		cmp		ax,4
		jz		_Type4							// Left Triangle to wall
		jmp		_Type5							// Right Triangle to wall

_Speed:	mov		esi,dword ptr [t]
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [nLVal]
		shl		eax,2
		add		esi,eax							// Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
_SCmp:	cmp		ax,8
		jz		_Type8							// PreTrans Solid 32x32 block
		cmp		ax,9
		jz		_Type9							// PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		_TypeA							// PreTrans Left Triangle
		cmp		ax,11
		jz		_TypeB							// PreTrans Right Triangle
		cmp		ax,12
		jz		_TypeC							// PreTrans Left Triangle to wall
		jmp		_TypeD							// PreTrans Right Triangle to wall

_NoLt:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RNoLt

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RNoLt:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		_SCmp

_Black:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RBlk

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RBlk:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		_TypeG							// Black Trans Solid 32x32 block
		cmp		ax,1
		jz		_TypeH							// Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		_TypeI							// Black Trans Left Triangle
		cmp		ax,3
		jz		_TypeJ							// Black Trans Right Triangle
		cmp		ax,4
		jz		_TypeK							// Black Trans Left Triangle to wall
		jmp		_TypeL							// Black Trans Right Triangle to wall

/*-----------------------------------------------------------------------*/

_Type0:	mov		edx,32
_T0Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T0Lp2:	lodsb
		shl		edx,1
		jnc		_T0S1
		xlatb
		mov		byte ptr [edi],al
_T0S1:	inc		edi
		loop	_T0Lp2
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T0Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type1:	mov		ecx,32

_T1Lp1:	push	ecx
		mov		eax,dword ptr [mask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [cm],eax

		mov		edx,32

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		mov		ecx,eax
		push	edx
		mov		edx,dword ptr [cm]
_T1Lp3:	lodsb
		shl		edx,1
		jnc		_T1S1
		xlatb
		mov		byte ptr [edi],al
_T1S1:	inc		edi
		loop	_T1Lp3
		mov		dword ptr [cm],edx
		pop		edx
		or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		_T1S2
		push	eax
		mov		eax,dword ptr [cm]
		shl		eax,cl
		mov		dword ptr [cm],eax
		pop		eax
_T1S2:	sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		ecx
		jnz		_T1Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type2:	mov		edx,30
_T2Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T2Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T2x
_T2Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T2Lp2
_T2x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T2b
		sub		edx,2
		jmp		_T2Lp1

_T2b:	mov		edx,2
_T2Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T2Lp4
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T2x2
_T2Lp4:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T2Lp4
_T2x2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_T2Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type3:	mov		edx,30
_T3Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T3Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T3x
_T3Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T3Lp2
_T3x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T3b
		add		edi,edx
		sub		edx,2
		jmp		_T3Lp1

_T3b:	mov		edx,2
_T3Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T3Lp4
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T3x2
_T3Lp4:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T3Lp4
_T3x2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_T3Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type4:	mov		edx,30
_T4Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T4Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T4x
_T4Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T4Lp2
_T4x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T4b
		sub		edx,2
		jmp		_T4Lp1

_T4b:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_T4Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T4Lp4:	lodsb
		shl		edx,1
		jnc		_T4S1
		xlatb
		mov		byte ptr [edi],al
_T4S1:	inc		edi
		loop	_T4Lp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T4Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type5:	mov		edx,30
_T5Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T5Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T5x
_T5Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T5Lp2
_T5x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T5b
		add		edi,edx
		sub		edx,2
		jmp		_T5Lp1

_T5b:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_T5Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T5Lp4:	lodsb
		shl		edx,1
		jnc		_T5S1
		xlatb
		mov		byte ptr [edi],al
_T5S1:	inc		edi
		loop	_T5Lp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T5Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type8:	mov		edx,32
_T8Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T8Lp2:	lodsb
		shl		edx,1
		jnc		_T8S1
		mov		byte ptr [edi],al
_T8S1:	inc		edi
		loop	_T8Lp2
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T8Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type9:	mov		ecx,32

_T9Lp1:	push	ecx
		mov		eax,dword ptr [mask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [cm],eax

		mov		edx,32

_T9Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T9J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		push	edx
		mov		edx,dword ptr [cm]
		mov		ecx,eax
_T9Lp3:	lodsb
		shl		edx,1
		jnc		_T9S1
		mov		byte ptr [edi],al
_T9S1:	inc		edi
		loop	_T9Lp3
		mov		dword ptr [cm],edx
		pop		edx
		or		edx,edx
		jz		_T9Nxt
		jmp		_T9Lp2

_T9J:	neg		al					// Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		_T9S2
		mov		ebx,dword ptr [cm]
		shl		ebx,cl
		mov		dword ptr [cm],ebx
_T9S2:	sub		edx,eax
		jnz		_T9Lp2
_T9Nxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		ecx
		jnz		_T9Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeA:	mov		edx,30
_TALp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp2
		movsw
		jecxz	_TAx
_TALp2:	rep movsd
_TAx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TAb
		sub		edx,2
		jmp		_TALp1

_TAb:	mov		edx,2
_TALp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp4
		movsw
		jecxz	_TAx2
_TALp4:	rep movsd
_TAx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TALp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeB:	mov		edx,30
_TBLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp2
		movsw
		jecxz	_TBx
_TBLp2:	rep movsd
_TBx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TBb
		add		edi,edx
		sub		edx,2
		jmp		_TBLp1

_TBb:	mov		edx,2
_TBLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp4
		movsw
		jecxz	_TBx2
_TBLp4:	rep movsd
_TBx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TBLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeC:	mov		edx,30
_TCLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TCLp2
		movsw
		jecxz	_TCx
_TCLp2:	rep movsd
_TCx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TCb
		sub		edx,2
		jmp		_TCLp1

_TCb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TCLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_TCLp4:	lodsb
		shl		edx,1
		jnc		_TCS1
		mov		byte ptr [edi],al
_TCS1:	inc		edi
		loop	_TCLp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TCLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeD:	mov		edx,30
_TDLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TDLp2
		movsw
		jecxz	_TDx
_TDLp2:	rep movsd
_TDx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TDb
		add		edi,edx
		sub		edx,2
		jmp		_TDLp1

_TDb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TDLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_TDLp4:	lodsb
		shl		edx,1
		jnc		_TDS1
		mov		byte ptr [edi],al
_TDS1:	inc		edi
		loop	_TDLp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TDLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeG:	mov		edx,32
_TGLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
_TGLp2:	shl		edx,1
		jnc		_TGS1
		mov		byte ptr [edi],al
_TGS1:	inc		edi
		loop	_TGLp2
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TGLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeH:	mov		ecx,32

_THLp1:	push	ecx
		mov		eax,dword ptr [mask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [cm],eax

		mov		edx,32

_THLp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_THJ

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		push	edx
		mov		edx,dword ptr [cm]
		mov		ecx,eax
		add		esi,ecx
		xor		eax,eax
_THLp3:	shl		edx,1
		jnc		_THS1
		mov		byte ptr [edi],al
_THS1:	inc		edi
		loop	_THLp3
		mov		dword ptr [cm],edx
		pop		edx
		or		edx,edx
		jz		_THNxt
		jmp		_THLp2

_THJ:	neg		al					// Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		_THS2
		mov		ebx,dword ptr [cm]
		shl		ebx,cl
		mov		dword ptr [cm],ebx
_THS2:	sub		edx,eax
		jnz		_THLp2
_THNxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		ecx
		jnz		_THLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeI:	mov		edx,30
		xor		eax,eax
_TILp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TILp2
		stosw
		jecxz	_TIx
_TILp2:	rep stosd
_TIx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TIb
		sub		edx,2
		jmp		_TILp1

_TIb:	mov		edx,2
_TILp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx

		shr		ecx,2
		jnc		_TILp4
		stosw
		jecxz	_TIx2
_TILp4:	rep stosd
_TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TILp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeJ:	mov		edx,30
		xor		eax,eax
_TJLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp2
		stosw
		jecxz	_TJx
_TJLp2:	rep stosd
_TJx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TJb
		add		edi,edx
		sub		edx,2
		jmp		_TJLp1

_TJb:	mov		edx,2
_TJLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp4
		stosw
		jecxz	_TJx2
_TJLp4:	rep stosd
_TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TJLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeK:	mov		edx,30
		xor		eax,eax
_TKLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TKLp2
		stosw
		jecxz	_TKx
_TKLp2:	rep stosd
_TKx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TKb
		sub		edx,2
		jmp		_TKLp1

_TKb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TKLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
_TKLp4:	shl		edx,1
		jnc		_TKS1
		mov		byte ptr [edi],al
_TKS1:	inc		edi
		loop	_TKLp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TKLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeL:	mov		edx,30
		xor		eax,eax
_TLLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TLLp2
		stosw
		jecxz	_TLx
_TLLp2:	rep stosd
_TLx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TLb
		add		edi,edx
		sub		edx,2
		jmp		_TLLp1

_TLb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TLLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
_TLLp4:	shl		edx,1
		jnc		_TLS1
		mov		byte ptr [edi],al
_TLS1:	inc		edi
		loop	_TLLp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TLLp3
		jmp		_Done

_PDone:	pop		eax
_Done:	nop

	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Now with Y clipping (top clip)
**-----------------------------------------------------------------------*/

void DecodeMicroTile (BYTE *pDecodeTo)
{
	if (nTrans) {
		if (gbPartialTrans == PART_TRANS_NONE) {
			TDecodeMicroTile(pDecodeTo);
			return;
		} else {
			if (gbPartialTrans == PART_TRANS_LEFT) {
				wt = nWTypeTable[gnPieceNum];
				if ((wt == WTYPE_LEFT) || (wt == WTYPE_ULC)) {
					TDecodeM12Tile(pDecodeTo, &sgLeftMask[31]);
					return;
				}
				if (wt == WTYPE_LRC) {
					TDecodeM12Tile(pDecodeTo, &sgRightMask[31]);
					return;
				}
			}
			if (gbPartialTrans == PART_TRANS_RIGHT) {
				wt = nWTypeTable[gnPieceNum];
				if ((wt == WTYPE_RIGHT) || (wt == WTYPE_ULC)) {
					TDecodeM12Tile(pDecodeTo, &sgRightMask[31]);
					return;
				}
				if (wt == WTYPE_LRC) {
					TDecodeM12Tile(pDecodeTo, &sgLeftMask[31]);
					return;
				}
			}
		}
	}
	t = &microoffset[0][0];
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,dword ptr [nLVal]
		or		al,al
		jz		_NoLt
		cmp		al,byte ptr [lightmax]
		jz		_Black

		mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jnz		_Speed

		mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		ebx,dword ptr [nLVal]			// Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [pLightTbl]

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		_Type0							// Solid 32x32 block
		cmp		ax,1
		jz		_Type1							// 32x32 block with '0' holes
		cmp		ax,2
		jz		_Type2							// Left Triangle
		cmp		ax,3
		jz		_Type3							// Right Triangle
		cmp		ax,4
		jz		_Type4							// Left Triangle to wall
		jmp		_Type5							// Right Triangle to wall

_Speed:	mov		esi,dword ptr [t]
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [nLVal]
		shl		eax,2
		add		esi,eax							// Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
_SCmp:	cmp		ax,8
		jz		_Type8							// PreTrans Solid 32x32 block
		cmp		ax,9
		jz		_Type9							// PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		_TypeA							// PreTrans Left Triangle
		cmp		ax,11
		jz		_TypeB							// PreTrans Right Triangle
		cmp		ax,12
		jz		_TypeC							// PreTrans Left Triangle to wall
		jmp		_TypeD							// PreTrans Right Triangle to wall

_NoLt:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RNoLt

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RNoLt:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		_SCmp

_Black:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RBlk

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RBlk:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		_TypeG							// Black Trans Solid 32x32 block
		cmp		ax,1
		jz		_TypeH							// Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		_TypeI							// Black Trans Left Triangle
		cmp		ax,3
		jz		_TypeJ							// Black Trans Right Triangle
		cmp		ax,4
		jz		_TypeK							// Black Trans Left Triangle to wall
		jmp		_TypeL							// Black Trans Right Triangle to wall

/*-----------------------------------------------------------------------*/

_Type0:	mov		edx,32
_T0Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push	edx
		call	xquads
		pop		edx

		sub		edi,NBUFFW32
		dec		edx
		jnz		_T0Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type1:	mov		ecx,32

_T1Lp1:	push	ecx
		mov		edx,32

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		mov		ecx,eax

		push	edx
		call	xbytes
		pop		edx

		or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	pop		ecx
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_T1Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type2:	mov		edx,30
_T2Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

		sub		edi,NBUFFW32
		or		edx,edx
		jz		_T2b
		sub		edx,2
		jmp		_T2Lp1

_T2b:	mov		edx,2
_T2Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

		sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_T2Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type3:	mov		edx,30
_T3Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

		sub		edi,NBUFFW32
		or		edx,edx
		jz		_T3b
		add		edi,edx
		sub		edx,2
		jmp		_T3Lp1

_T3b:	mov		edx,2
_T3Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_T3Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type4:	mov		edx,30
_T4Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

		sub		edi,NBUFFW32
		or		edx,edx
		jz		_T4b
		sub		edx,2
		jmp		_T4Lp1

_T4b:	mov		edx,16
_T4Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push	edx
		call	xquads
		pop		edx

		sub		edi,NBUFFW32
		dec		edx
		jnz		_T4Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type5:	mov		edx,30
_T5Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

		sub		edi,NBUFFW32
		or		edx,edx
		jz		_T5b
		add		edi,edx
		sub		edx,2
		jmp		_T5Lp1

_T5b:	mov		edx,16
_T5Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8

		push	edx
		call	xquads
		pop		edx

		sub		edi,NBUFFW32
		dec		edx
		jnz		_T5Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type8:	mov		edx,32
_T8Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
		rep movsd
		sub		edi,NBUFFW32
		dec		edx
		jnz		_T8Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type9:	mov		ecx,32

_T9Lp1:	push	ecx
		mov		edx,32

_T9Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T9J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		mov		ecx,eax
		shr		ecx,1
		jnc		_T9w
		movsb
		jecxz	_T9x
_T9w:	shr		ecx,1
		jnc		_T9Lp3
		movsw
		jecxz	_T9x
_T9Lp3:	rep movsd
_T9x:	or		edx,edx
		jz		_T9Nxt
		jmp		_T9Lp2

_T9J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T9Lp2
_T9Nxt:	pop		ecx
		sub		edi,NBUFFW32
		loop	_T9Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeA:	mov		edx,30
_TALp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp2
		movsw
		jecxz	_TAx
_TALp2:	rep movsd
_TAx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TAb
		sub		edx,2
		jmp		_TALp1

_TAb:	mov		edx,2
_TALp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp4
		movsw
		jecxz	_TAx2
_TALp4:	rep movsd
_TAx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TALp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeB:	mov		edx,30
_TBLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp2
		movsw
		jecxz	_TBx
_TBLp2:	rep movsd
_TBx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TBb
		add		edi,edx
		sub		edx,2
		jmp		_TBLp1

_TBb:	mov		edx,2
_TBLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp4
		movsw
		jecxz	_TBx2
_TBLp4:	rep movsd
_TBx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TBLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeC:	mov		edx,30
_TCLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TCLp2
		movsw
		jecxz	_TCx
_TCLp2:	rep movsd
_TCx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TCb
		sub		edx,2
		jmp		_TCLp1

_TCb:	mov		edx,16
_TCLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
		rep movsd
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TCLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeD:	mov		edx,30
_TDLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TDLp2
		movsw
		jecxz	_TDx
_TDLp2:	rep movsd
_TDx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TDb
		add		edi,edx
		sub		edx,2
		jmp		_TDLp1

_TDb:	mov		edx,16
_TDLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
		rep movsd
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TDLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeG:	mov		edx,32
		xor		eax,eax
_TGLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
		rep stosd
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TGLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeH:	mov		ecx,32

_THLp1:	push	ecx
		mov		edx,32

_THLp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_THJ

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_PDone
		mov		ecx,eax
		add		esi,ecx
		xor		eax,eax
		shr		ecx,1
		jnc		_THw
		stosb
		jecxz	_THx
_THw:	shr		ecx,1
		jnc		_THLp3
		stosw
		jecxz	_THx
_THLp3:	rep stosd
_THx:	or		edx,edx
		jz		_THNxt
		jmp		_THLp2

_THJ:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_THLp2
_THNxt:	pop		ecx
		sub		edi,NBUFFW32
		loop	_THLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeI:	mov		edx,30
		xor		eax,eax
_TILp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TILp2
		stosw
		jecxz	_TIx
_TILp2:	rep stosd
_TIx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TIb
		sub		edx,2
		jmp		_TILp1

_TIb:	mov		edx,2
_TILp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TILp4
		stosw
		jecxz	_TIx2
_TILp4:	rep stosd
_TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TILp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeJ:	mov		edx,30
		xor		eax,eax
_TJLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp2
		stosw
		jecxz	_TJx
_TJLp2:	rep stosd
_TJx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TJb
		add		edi,edx
		sub		edx,2
		jmp		_TJLp1

_TJb:	mov		edx,2
_TJLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp4
		stosw
		jecxz	_TJx2
_TJLp4:	rep stosd
_TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TJLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeK:	mov		edx,30
		xor		eax,eax
_TKLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TKLp2
		stosw
		jecxz	_TKx
_TKLp2:	rep stosd
_TKx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TKb
		sub		edx,2
		jmp		_TKLp1

_TKb:	mov		edx,16
_TKLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
		rep stosd
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TKLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeL:	mov		edx,30
		xor		eax,eax
_TLLp1:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TLLp2
		stosw
		jecxz	_TLx
_TLLp2:	rep stosd
_TLx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TLb
		add		edi,edx
		sub		edx,2
		jmp		_TLLp1

_TLb:	mov		edx,16
_TLLp3:	cmp		edi,dword ptr [ClipY]
		jb		_Done
		mov		ecx,8
		rep stosd
		sub		edi,NBUFFW32
		dec		edx
		jnz		_TLLp3
		jmp		_Done

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

_PDone:	pop		eax
_Done:	nop

	}		// end of asm block
}

/*-----------------------------------------------------------------------**
** Now with Y clipping
**-----------------------------------------------------------------------*/

void TCDecodeMicroTile (BYTE *pDecodeTo)
{
	t = &microoffset[0][0];
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,dword ptr [nLVal]
		or		al,al
		jz		_NoLt
		cmp		al,byte ptr [lightmax]
		jz		_Black

		mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jnz		_Speed

		mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		ebx,dword ptr [nLVal];			// Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [pLightTbl];

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		_Type0							// Solid 32x32 block
		cmp		ax,1
		jz		_Type1							// 32x32 block with '0' holes
		cmp		ax,2
		jz		_Type2							// Left Triangle
		cmp		ax,3
		jz		_Type3							// Right Triangle
		cmp		ax,4
		jz		_Type4							// Left Triangle to wall
		jmp		_Type5							// Right Triangle to wall

_Speed:	mov		esi,dword ptr [t]
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [nLVal]
		shl		eax,2
		add		esi,eax							// Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
_SCmp:	cmp		ax,8
		jz		_Type8							// PreTrans Solid 32x32 block
		cmp		ax,9
		jz		_Type9							// PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		_TypeA							// PreTrans Left Triangle
		cmp		ax,11
		jz		_TypeB							// PreTrans Right Triangle
		cmp		ax,12
		jz		_TypeC							// PreTrans Left Triangle to wall
		jmp		_TypeD							// PreTrans Right Triangle to wall

_NoLt:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RNoLt

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RNoLt:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		_SCmp

_Black:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RBlk

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RBlk:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		_TypeG							// Black Trans Solid 32x32 block
		cmp		ax,1
		jz		_TypeH							// Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		_TypeI							// Black Trans Left Triangle
		cmp		ax,3
		jz		_TypeJ							// Black Trans Right Triangle
		cmp		ax,4
		jz		_TypeK							// Black Trans Left Triangle to wall
		jmp		_TypeL							// Black Trans Right Triangle to wall

/*-----------------------------------------------------------------------*/

_Type0:	mov		edx,16
_T0Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T0C1
		add		esi,32
		add		edi,32
		jmp		_T0C2
_T0C1:	mov		ecx,8

		push edx
		call xquad2
		pop edx

_T0C2:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_T0C3
		add		esi,32
		add		edi,32
		jmp		_T0C4
_T0C3:	mov		ecx,8

		push edx
		call xquad1
		pop edx

_T0C4:	sub		edi,NBUFFW32
		dec		edx
		jnz		_T0Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type1:	mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32

_T1Lp1:	push	ecx
		mov		edx,32

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_T1C1
		add		esi,eax
		add		edi,eax
		jmp		_T1x
_T1C1:	mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		_T1Od

		push edx
		call xbyte1
		pop edx
		jmp _T1x

_T1Od:
		push edx
		call xbyte2
		pop edx

_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_T1Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type2:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T2Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T2C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_T2x
_T2C1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T2Od

		push edx
		call xbyte1
		pop edx
		jmp _T2x

_T2Od:
		push edx
		call xbyte2
		pop edx

_T2x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T2b
		sub		edx,2
		jmp		_T2Lp1

_T2b:	mov		edx,2
_T2Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_T2C2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_T2x2
_T2C2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T2Od2

		push edx
		call xbyte1
		pop edx
		jmp _T2x2

_T2Od2:
		push edx
		call xbyte2
		pop edx

_T2x2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_T2Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type3:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T3Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T3C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T3x
_T3C1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T3Od

		push edx
		call xbyte1
		pop edx
		jmp _T3x

_T3Od:
		push edx
		call xbyte2
		pop edx

_T3x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T3b
		add		edi,edx
		sub		edx,2
		jmp		_T3Lp1

_T3b:	mov		edx,2
_T3Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_T3C2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T3x2
_T3C2:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T3Od2

		push edx
		call xbyte1
		pop edx
		jmp _T3x2

_T3Od2:
		push edx
		call xbyte2
		pop edx

_T3x2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_T3Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type4:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T4Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T4C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_T4x
_T4C1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T4Od

		push edx
		call xbyte1
		pop edx
		jmp _T4x

_T4Od:
		push edx
		call xbyte2
		pop edx

_T4x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T4b
		sub		edx,2
		jmp		_T4Lp1

_T4b:	mov		edx,8
_T4Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_T4C2
		add		esi,32
		add		edi,32
		jmp		_T4C3
_T4C2:	mov		ecx,8

		push edx
		call xquad2
		pop edx

_T4C3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_T4C4
		add		esi,32
		add		edi,32
		jmp		_T4C5
_T4C4:	mov		ecx,8

		push edx
		call xquad1
		pop edx

_T4C5:	sub		edi,NBUFFW32
		dec		edx
		jnz		_T4Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type5:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_T5Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T5C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T5x
_T5C1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_T5Od

		push edx
		call xbyte1
		pop edx
		jmp _T5x

_T5Od:
		push edx
		call xbyte2
		pop edx

_T5x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T5b
		add		edi,edx
		sub		edx,2
		jmp		_T5Lp1

_T5b:	mov		edx,8
_T5Lp4:	cmp		edi,dword ptr [ClipY]
		jb		_T5C2
		add		esi,32
		add		edi,32
		jmp		_T5C3
_T5C2:	mov		ecx,8

		push edx
		call xquad2
		pop edx

_T5C3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_T5C4
		add		esi,32
		add		edi,32
		jmp		_T5C5
_T5C4:	mov		ecx,8

		push edx
		call xquad1
		pop edx

_T5C5:	sub		edi,NBUFFW32
		dec		edx
		jnz		_T5Lp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type8:	mov		edx,16
_T8Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T8C1
		add		esi,32
		add		edi,32
		jmp		_T8C2
_T8C1:	mov		ecx,8
_T8Lp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		inc		edi
		ror		eax,16
		stosb
		loop	_T8Lp2
_T8C2:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_T8C3
		add		esi,32
		add		edi,32
		jmp		_T8C4
_T8C3:	mov		ecx,8
_T8Lp3:	lodsd
		stosb
		inc		edi
		ror		eax,16
		stosb
		inc		edi
		loop	_T8Lp3
_T8C4:	sub		edi,NBUFFW32
		dec		edx
		jnz		_T8Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type9:	mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32

_T9Lp1:	push	ecx
		mov		edx,32

_T9Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T9J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_T9C1
		add		esi,eax
		add		edi,eax
		jmp		_T9x
_T9C1:	mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		_T9Od
		shr		ecx,1
		jnc		_T9w
		inc		esi
		inc		edi
		jecxz	_T9x
		jmp		_T9w2
_T9w:	shr		ecx,1
		jnc		_T9Lp3
		inc		esi
		inc		edi
		movsb
		jecxz	_T9x
_T9Lp3:	lodsd
		inc		edi
		ror		eax,8
		stosb
		inc		edi
		ror		eax,16
		stosb
		loop	_T9Lp3
		jmp		_T9x

_T9Od:	shr		ecx,1
		jnc		_T9w2
		movsb
		jecxz	_T9x
		jmp		_T9w
_T9w2:	shr		ecx,1
		jnc		_T9Lp4
		movsb
		inc		esi
		inc		edi
		jecxz	_T9x
_T9Lp4:	lodsd
		stosb
		inc		edi
		ror		eax,16
		stosb
		inc		edi
		loop	_T9Lp4

_T9x:	or		edx,edx
		jz		_T9Nxt
		jmp		_T9Lp2

_T9J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T9Lp2
_T9Nxt:	pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_T9Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeA:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TALp1:	cmp		edi,dword ptr [ClipY]
		jb		_TAC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TAx
_TAC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TAOd
		shr		ecx,2
		jnc		_TALp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TAx
_TALp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TALp2
		jmp		_TAx

_TAOd:	shr		ecx,2
		jnc		_TALp3
		lodsw
		stosb
		inc		edi
		jecxz	_TAx
_TALp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TALp3

_TAx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TAb
		sub		edx,2
		jmp		_TALp1

_TAb:	mov		edx,2
_TALp4:	cmp		edi,dword ptr [ClipY]
		jb		_TAC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TAx2
_TAC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TAOd2
		shr		ecx,2
		jnc		_TALp5
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TAx2
_TALp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TALp5
		jmp		_TAx2

_TAOd2:	shr		ecx,2
		jnc		_TALp6
		lodsw
		stosb
		inc		edi
		jecxz	_TAx2
_TALp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TALp6

_TAx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TALp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeB:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TBLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TBC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TBx
_TBC1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TBOd
		shr		ecx,2
		jnc		_TBLp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TBx
_TBLp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TBLp2
		jmp		_TBx

_TBOd:	shr		ecx,2
		jnc		_TBLp3
		lodsw
		stosb
		inc		edi
		jecxz	_TBx
_TBLp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TBLp3

_TBx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TBb
		add		edi,edx
		sub		edx,2
		jmp		_TBLp1

_TBb:	mov		edx,2
_TBLp4:	cmp		edi,dword ptr [ClipY]
		jb		_TBC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TBx2
_TBC2:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TBOd2
		shr		ecx,2
		jnc		_TBLp5
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TBx2
_TBLp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TBLp5
		jmp		_TBx2

_TBOd2:	shr		ecx,2
		jnc		_TBLp6
		lodsw
		stosb
		inc		edi
		jecxz	_TBx2
_TBLp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TBLp6

_TBx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TBLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeC:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TCLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TCC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TCx
_TCC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TCOd
		shr		ecx,2
		jnc		_TCLp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TCx
_TCLp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TCLp2
		jmp		_TCx

_TCOd:	shr		ecx,2
		jnc		_TCLp3
		lodsw
		stosb
		inc		edi
		jecxz	_TCx
_TCLp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TCLp3

_TCx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TCb
		sub		edx,2
		jmp		_TCLp1

_TCb:	mov		edx,8
_TCLp4:	cmp		edi,dword ptr [ClipY]
		jb		_TCC2
		add		esi,32
		add		edi,32
		jmp		_TCC3
_TCC2:	mov		ecx,8
_TCLp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TCLp5
_TCC3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_TCC4
		add		esi,32
		add		edi,32
		jmp		_TCC5
_TCC4:	mov		ecx,8
_TCLp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TCLp6
_TCC5:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TCLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeD:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TDLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TDC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TDx
_TDC1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TDOd
		shr		ecx,2
		jnc		_TDLp2
		lodsw
		inc		edi
		ror		eax,8
		stosb
		jecxz	_TDx
_TDLp2:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TDLp2
		jmp		_TDx

_TDOd:	shr		ecx,2
		jnc		_TDLp3
		lodsw
		stosb
		inc		edi
		jecxz	_TDx
_TDLp3:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TDLp3

_TDx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TDb
		add		edi,edx
		sub		edx,2
		jmp		_TDLp1

_TDb:	mov		edx,8
_TDLp4:	cmp		edi,dword ptr [ClipY]
		jb		_TDC2
		add		esi,32
		add		edi,32
		jmp		_TDC3
_TDC2:	mov		ecx,8
_TDLp5:	lodsd
		inc		edi
		ror		eax,8
		stosb
		ror		eax,16
		inc		edi
		stosb
		loop	_TDLp5
_TDC3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_TDC4
		add		esi,32
		add		edi,32
		jmp		_TDC5
_TDC4:	mov		ecx,8
_TDLp6:	lodsd
		stosb
		ror		eax,16
		inc		edi
		stosb
		inc		edi
		loop	_TDLp6
_TDC5:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TDLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeG:	mov		edx,16
		xor		eax,eax
_TGLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TGC1
		add		esi,32
		add		edi,32
		jmp		_TGC2
_TGC1:	mov		ecx,8
_TGLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TGLp2
_TGC2:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_TGC3
		add		esi,32
		add		edi,32
		jmp		_TGC4
_TGC3:	mov		ecx,8
_TGLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TGLp3
_TGC4:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TGLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeH:	mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32

_THLp1:	push	ecx
		mov		edx,32

_THLp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_THJ

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_THC1
		add		esi,eax
		add		edi,eax
		jmp		_THx
_THC1:	mov		ecx,eax
		add		esi,ecx
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		_THOd
		xor		eax,eax
		shr		ecx,1
		jnc		_THw
		inc		edi
		jecxz	_THx
		jmp		_THw2
_THw:	shr		ecx,1
		jnc		_THLp3
		inc		edi
		stosb
		jecxz	_THx
_THLp3:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_THLp3
		jmp		_THx

_THOd:	xor		eax,eax
		shr		ecx,1
		jnc		_THw2
		stosb
		jecxz	_THx
		jmp		_THw
_THw2:	shr		ecx,1
		jnc		_THLp4
		stosb
		inc		edi
		jecxz	_THx
_THLp4:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_THLp4

_THx:	or		edx,edx
		jz		_THNxt
		jmp		_THLp2

_THJ:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_THLp2
_THNxt:	pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_THLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeI:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TILp1:	cmp		edi,dword ptr [ClipY]
		jb		_TIC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TIx
_TIC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TIOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TILp2
		inc		edi
		stosb
		jecxz	_TIx
_TILp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TILp2
		jmp		_TIx

_TIOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TILp3
		stosb
		inc		edi
		jecxz	_TIx
_TILp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TILp3

_TIx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TIb
		sub		edx,2
		jmp		_TILp1

_TIb:	mov		edx,2
_TILp4:	cmp		edi,dword ptr [ClipY]
		jb		_TIC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TIx2
_TIC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TIOd2
		xor		eax,eax
		shr		ecx,2
		jnc		_TILp5
		inc		edi
		stosb
		jecxz	_TIx2
_TILp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TILp5
		jmp		_TIx2

_TIOd2:	xor		eax,eax
		shr		ecx,2
		jnc		_TILp6
		stosb
		inc		edi
		jecxz	_TIx2
_TILp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TILp6

_TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TILp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeJ:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TJLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TJC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TJx
_TJC1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TJOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TJLp2
		inc		edi
		stosb
		jecxz	_TJx
_TJLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TJLp2
		jmp		_TJx

_TJOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TJLp3
		stosb
		inc		edi
		jecxz	_TJx
_TJLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TJLp3

_TJx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TJb
		add		edi,edx
		sub		edx,2
		jmp		_TJLp1

_TJb:	mov		edx,2
_TJLp4:	cmp		edi,dword ptr [ClipY]
		jb		_TJC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TJx2
_TJC2:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TJOd2
		xor		eax,eax
		shr		ecx,2
		jnc		_TJLp5
		inc		edi
		stosb
		jecxz	_TJx2
_TJLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TJLp5
		jmp		_TJx2

_TJOd2:	xor		eax,eax
		shr		ecx,2
		jnc		_TJLp6
		stosb
		inc		edi
		jecxz	_TJx2
_TJLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TJLp6

_TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TJLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeK:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TKLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TKC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TKx
_TKC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TKOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TKLp2
		inc		edi
		stosb
		jecxz	_TKx
_TKLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TKLp2
		jmp		_TKx

_TKOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TKLp3
		stosb
		inc		edi
		jecxz	_TKx
_TKLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TKLp3

_TKx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TKb
		sub		edx,2
		jmp		_TKLp1

_TKb:	mov		edx,8
_TKLp4:	cmp		edi,dword ptr [ClipY]
		jb		_TKC2
		add		esi,32
		add		edi,32
		jmp		_TKC3
_TKC2:	mov		ecx,8
		xor		eax,eax
_TKLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TKLp5
_TKC3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_TKC4
		add		esi,32
		add		edi,32
		jmp		_TKC5
_TKC4:	mov		ecx,8
		xor		eax,eax
_TKLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TKLp6
_TKC5:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TKLp4
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeL:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
_TLLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TLC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TLx
_TLC1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		_TLOd
		xor		eax,eax
		shr		ecx,2
		jnc		_TLLp2
		inc		edi
		stosb
		jecxz	_TLx
_TLLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TLLp2
		jmp		_TLx

_TLOd:	xor		eax,eax
		shr		ecx,2
		jnc		_TLLp3
		stosb
		inc		edi
		jecxz	_TLx
_TLLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TLLp3

_TLx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TLb
		add		edi,edx
		sub		edx,2
		jmp		_TLLp1

_TLb:	mov		edx,8
_TLLp4:	cmp		edi,dword ptr [ClipY]
		jb		_TLC2
		add		esi,32
		add		edi,32
		jmp		_TLC3
_TLC2:	mov		ecx,8
		xor		eax,eax
_TLLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		loop	_TLLp5
_TLC3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [ClipY]
		jb		_TLC4
		add		esi,32
		add		edi,32
		jmp		_TLC5
_TLC4:	mov		ecx,8
		xor		eax,eax
_TLLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		loop	_TLLp6
_TLC5:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TLLp4
		jmp		_Done

xbyte1:				// blank first

		shr ecx,1
		jnc xword2
		inc esi
		inc edi

xword1:				// xlat first
		shr ecx,1
		jnc xquadt1
		mov dl,[esi]
		mov dl,[ebx+edx]
		add esi,2
		mov [edi],dl
		add edi,2

xquadt1:
		test cl,cl
		jz xend

xquad1:				// xlat first
/* debug
		shl ecx,2
		add esi,ecx
		add edi,ecx
		ret
*/

		mov eax,[esi]
		add esi,4
		mov dl,al
		shr eax,16
		mov dl,[ebx+edx]
		mov [edi],dl
		mov dl,al
		add edi,4
		mov dl,[ebx+edx]
		dec ecx
		mov [edi-2],dl
		jnz xquad1

		ret


xbyte2:				// xlat first

		shr ecx,1
		jnc xword1
		mov dl,[esi]
		mov dl,[ebx+edx]
		inc esi
		mov	[edi],dl
		inc edi

xword2:				// blank first
		shr ecx,1
		jnc xquadt2
		mov dl,[esi+1]
		mov dl,[ebx+edx]
		add esi,2
		mov [edi+1],dl
		add edi,2

xquadt2:
		test cl,cl
		jz xend

xquad2:				// blank first
/* debug
		shl ecx,2
		add esi,ecx
		add edi,ecx
		ret
*/

		
		mov eax,[esi]
		add esi,4
		mov dl,ah
		shr eax,16
		mov dl,[ebx+edx]
		mov [edi+1],dl
		mov dl,ah
		add edi,4
		mov dl,[ebx+edx]
		dec ecx
		mov [edi-1],dl
		jnz xquad2

xend:
		ret

_Done:	nop

	}		// end of asm block
}

/*-----------------------------------------------------------------------**
** Now with Y clipping and Masking Transparency
**-----------------------------------------------------------------------*/

void TCDecodeM12Tile (BYTE *pDecodeTo, long *mask)
{
	t = &microoffset[0][0];
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,dword ptr [nLVal]
		or		al,al
		jz		_NoLt
		cmp		al,byte ptr [lightmax]
		jz		_Black

		mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jnz		_Speed

		mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		ebx,dword ptr [nLVal];			// Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [pLightTbl];

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		_Type0							// Solid 32x32 block
		cmp		ax,1
		jz		_Type1							// 32x32 block with '0' holes
		cmp		ax,2
		jz		_Type2							// Left Triangle
		cmp		ax,3
		jz		_Type3							// Right Triangle
		cmp		ax,4
		jz		_Type4							// Left Triangle to wall
		jmp		_Type5							// Right Triangle to wall

_Speed:	mov		esi,dword ptr [t]
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [nLVal]
		shl		eax,2
		add		esi,eax							// Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
_SCmp:	cmp		ax,8
		jz		_Type8							// PreTrans Solid 32x32 block
		cmp		ax,9
		jz		_Type9							// PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		_TypeA							// PreTrans Left Triangle
		cmp		ax,11
		jz		_TypeB							// PreTrans Right Triangle
		cmp		ax,12
		jz		_TypeC							// PreTrans Left Triangle to wall
		jmp		_TypeD							// PreTrans Right Triangle to wall

_NoLt:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RNoLt

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RNoLt:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		_SCmp

_Black:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RBlk

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RBlk:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		_TypeG							// Black Trans Solid 32x32 block
		cmp		ax,1
		jz		_TypeH							// Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		_TypeI							// Black Trans Left Triangle
		cmp		ax,3
		jz		_TypeJ							// Black Trans Right Triangle
		cmp		ax,4
		jz		_TypeK							// Black Trans Left Triangle to wall
		jmp		_TypeL							// Black Trans Right Triangle to wall

/*-----------------------------------------------------------------------*/

_Type0:	mov		edx,32
_T0Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T0C1
		add		esi,32
		add		edi,32
		jmp		_T0C2
_T0C1:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T0Lp2:	lodsb
		shl		edx,1
		jnc		_T0S1
		xlatb
		mov		byte ptr [edi],al
_T0S1:	inc		edi
		loop	_T0Lp2
		pop		edx
_T0C2:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T0Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type1:	mov		ecx,32

_T1Lp1:	push	ecx
		mov		eax,dword ptr [mask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [cm],eax

		mov		edx,32

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_T1C1
		add		esi,eax
		add		edi,eax
		jmp		_T1x
_T1C1:	mov		ecx,eax
		push	edx
		mov		edx,dword ptr [cm]
_T1Lp3:	lodsb
		shl		edx,1
		jnc		_T1S1
		xlatb
		mov		byte ptr [edi],al
_T1S1:	inc		edi
		loop	_T1Lp3
		mov		dword ptr [cm],edx
		pop		edx
_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		_T1S2
		push	eax
		mov		eax,dword ptr [cm]
		shl		eax,cl
		mov		dword ptr [cm],eax
		pop		eax
_T1S2:	sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		ecx
		jnz		_T1Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type2:	mov		edx,30
_T2Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T2C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_T2x
_T2C1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T2Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T2x
_T2Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T2Lp2
_T2x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T2b
		sub		edx,2
		jmp		_T2Lp1

_T2b:	mov		edx,2
_T2Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_T2C2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_T2x2
_T2C2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T2Lp4
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T2x2
_T2Lp4:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T2Lp4
_T2x2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_T2Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type3:	mov		edx,30
_T3Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T3C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T3x
_T3C1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T3Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T3x
_T3Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T3Lp2
_T3x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T3b
		add		edi,edx
		sub		edx,2
		jmp		_T3Lp1

_T3b:	mov		edx,2
_T3Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_T3C2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T3x2
_T3C2:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T3Lp4
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T3x2
_T3Lp4:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T3Lp4
_T3x2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_T3Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type4:	mov		edx,30
_T4Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T4C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_T4x
_T4C1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T4Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T4x
_T4Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T4Lp2
_T4x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T4b
		sub		edx,2
		jmp		_T4Lp1

_T4b:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_T4Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_T4C2
		add		esi,32
		add		edi,32
		jmp		_T4C3
_T4C2:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T4Lp4:	lodsb
		shl		edx,1
		jnc		_T4S1
		xlatb
		mov		byte ptr [edi],al
_T4S1:	inc		edi
		loop	_T4Lp4
		pop		edx
_T4C3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T4Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type5:	mov		edx,30
_T5Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T5C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T5x
_T5C1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_T5Lp2
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T5x
_T5Lp2:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T5Lp2
_T5x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T5b
		add		edi,edx
		sub		edx,2
		jmp		_T5Lp1

_T5b:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_T5Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_T5C2
		add		esi,32
		add		edi,32
		jmp		_T5C3
_T5C2:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T5Lp4:	lodsb
		shl		edx,1
		jnc		_T5S1
		xlatb
		mov		byte ptr [edi],al
_T5S1:	inc		edi
		loop	_T5Lp4
		pop		edx
_T5C3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T5Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type8:	mov		edx,32
_T8Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T8C1
		add		esi,32
		add		edi,32
		jmp		_T8C2
_T8C1:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_T8Lp2:	lodsb
		shl		edx,1
		jnc		_T8S1
		mov		byte ptr [edi],al
_T8S1:	inc		edi
		loop	_T8Lp2
		pop		edx
_T8C2:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_T8Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type9:	mov		ecx,32

_T9Lp1:	push	ecx
		mov		eax,dword ptr [mask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [cm],eax

		mov		edx,32

_T9Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T9J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_T9C1
		add		esi,eax
		add		edi,eax
		jmp		_T9x
_T9C1:	mov		ecx,eax
		push	edx
		mov		edx,dword ptr [cm]
_T9Lp3:	lodsb
		shl		edx,1
		jnc		_T9S1
		mov		byte ptr [edi],al
_T9S1:	inc		edi
		loop	_T9Lp3
		mov		dword ptr [cm],edx
		pop		edx
_T9x:	or		edx,edx
		jz		_T9Nxt
		jmp		_T9Lp2

_T9J:	neg		al					// Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		_T9S2
		mov		ebx,dword ptr [cm]
		shl		ebx,cl
		mov		dword ptr [cm],ebx
_T9S2:	sub		edx,eax
		jnz		_T9Lp2
_T9Nxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		ecx
		jnz		_T9Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeA:	mov		edx,30
_TALp1:	cmp		edi,dword ptr [ClipY]
		jb		_TAC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TAx
_TAC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp2
		movsw
		jecxz	_TAx
_TALp2:	rep movsd
_TAx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TAb
		sub		edx,2
		jmp		_TALp1

_TAb:	mov		edx,2
_TALp3:	cmp		edi,dword ptr [ClipY]
		jb		_TAC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TAx2
_TAC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp4
		movsw
		jecxz	_TAx2
_TALp4:	rep movsd
_TAx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TALp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeB:	mov		edx,30
_TBLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TBC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TBx
_TBC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp2
		movsw
		jecxz	_TBx
_TBLp2:	rep movsd
_TBx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TBb
		add		edi,edx
		sub		edx,2
		jmp		_TBLp1

_TBb:	mov		edx,2
_TBLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TBC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TBx2
_TBC2:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp4
		movsw
		jecxz	_TBx2
_TBLp4:	rep movsd
_TBx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TBLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeC:	mov		edx,30
_TCLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TCC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TCx
_TCC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TCLp2
		movsw
		jecxz	_TCx
_TCLp2:	rep movsd
_TCx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TCb
		sub		edx,2
		jmp		_TCLp1

_TCb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TCLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TCC2
		add		esi,32
		add		edi,32
		jmp		_TCC3
_TCC2:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_TCLp4:	lodsb
		shl		edx,1
		jnc		_TCS1
		mov		byte ptr [edi],al
_TCS1:	inc		edi
		loop	_TCLp4
		pop		edx
_TCC3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TCLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeD:	mov		edx,30
_TDLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TDC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TDx
_TDC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TDLp2
		movsw
		jecxz	_TDx
_TDLp2:	rep movsd
_TDx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TDb
		add		edi,edx
		sub		edx,2
		jmp		_TDLp1

_TDb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TDLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TDC2
		add		esi,32
		add		edi,32
		jmp		_TDC3
_TDC2:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
_TDLp4:	lodsb
		shl		edx,1
		jnc		_TDS1
		mov		byte ptr [edi],al
_TDS1:	inc		edi
		loop	_TDLp4
		pop		edx
_TDC3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TDLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeG:	mov		edx,32
_TGLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TGC1
		add		esi,32
		add		edi,32
		jmp		_TGC2
_TGC1:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
_TGLp2:	shl		edx,1
		jnc		_TGS1
		mov		byte ptr [edi],al
_TGS1:	inc		edi
		loop	_TGLp2
		pop		edx
_TGC2:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TGLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeH:	mov		ecx,32

_THLp1:	push	ecx
		mov		eax,dword ptr [mask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [cm],eax

		mov		edx,32

_THLp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_THJ

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_THC1
		add		esi,eax
		add		edi,eax
		jmp		_THx
_THC1:	mov		ecx,eax
		add		esi,ecx
		push	edx
		mov		edx,dword ptr [cm]
		xor		eax,eax
_THLp3:	shl		edx,1
		jnc		_THS1
		mov		byte ptr [edi],al
_THS1:	inc		edi
		loop	_THLp3
		mov		dword ptr [cm],edx
		pop		edx
_THx:	or		edx,edx
		jz		_THNxt
		jmp		_THLp2

_THJ:	neg		al					// Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		_THS2
		mov		ebx,dword ptr [cm]
		shl		ebx,cl
		mov		dword ptr [cm],ebx
_THS2:	sub		edx,eax
		jnz		_THLp2
_THNxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		ecx
		jnz		_THLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeI:	mov		edx,30
		xor		eax,eax
_TILp1:	cmp		edi,dword ptr [ClipY]
		jb		_TIC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TIx
_TIC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TILp2
		stosw
		jecxz	_TIx
_TILp2:	rep stosd
_TIx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TIb
		sub		edx,2
		jmp		_TILp1

_TIb:	mov		edx,2
_TILp3:	cmp		edi,dword ptr [ClipY]
		jb		_TIC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TIx2
_TIC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TILp4
		stosw
		jecxz	_TIx2
_TILp4:	rep stosd
_TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TILp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeJ:	mov		edx,30
		xor		eax,eax
_TJLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TJC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TJx
_TJC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp2
		stosw
		jecxz	_TJx
_TJLp2:	rep stosd
_TJx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TJb
		add		edi,edx
		sub		edx,2
		jmp		_TJLp1

_TJb:	mov		edx,2
_TJLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TJC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TJx2
_TJC2:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp4
		stosw
		jecxz	_TJx2
_TJLp4:	rep stosd
_TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TJLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeK:	mov		edx,30
		xor		eax,eax
_TKLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TKC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TKx
_TKC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TKLp2
		stosw
		jecxz	_TKx
_TKLp2:	rep stosd
_TKx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TKb
		sub		edx,2
		jmp		_TKLp1

_TKb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TKLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TKC2
		add		esi,32
		add		edi,32
		jmp		_TKC3
_TKC2:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
_TKLp4:	shl		edx,1
		jnc		_TKS1
		mov		byte ptr [edi],al
_TKS1:	inc		edi
		loop	_TKLp4
		pop		edx
_TKC3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TKLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeL:	mov		edx,30
		xor		eax,eax
_TLLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TLC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TLx
_TLC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TLLp2
		stosw
		jecxz	_TLx
_TLLp2:	rep stosd
_TLx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TLb
		add		edi,edx
		sub		edx,2
		jmp		_TLLp1

_TLb:	mov		eax,dword ptr [mask]
		sub		eax,64
		mov		dword ptr [mask],eax
		mov		edx,16
_TLLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TLC2
		add		esi,32
		add		edi,32
		jmp		_TLC3
_TLC2:	push	edx
		mov		eax,dword ptr [mask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
_TLLp4:	shl		edx,1
		jnc		_TLS1
		mov		byte ptr [edi],al
_TLS1:	inc		edi
		loop	_TLLp4
		pop		edx
_TLC3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [mask]
		sub		eax,4
		mov		dword ptr [mask],eax
		dec		edx
		jnz		_TLLp3

_Done:	nop

	}		// end of asm block
}


/*-----------------------------------------------------------------------**
** Now with Y clipping
**-----------------------------------------------------------------------*/

void CDecodeMicroTile (BYTE *pDecodeTo)
{
	if (nTrans) {
		switch (gbPartialTrans) {
		case PART_TRANS_NONE:
			TCDecodeMicroTile(pDecodeTo);
			return;
		case PART_TRANS_LEFT:
			wt = nWTypeTable[gnPieceNum];
			if ((wt == WTYPE_LEFT) || (wt == WTYPE_ULC)) {
				TCDecodeM12Tile(pDecodeTo, &sgLeftMask[31]);
				return;
			}
			if (wt == WTYPE_LRC) {
				TCDecodeM12Tile(pDecodeTo, &sgRightMask[31]);
				return;
			}
			break;
		case PART_TRANS_RIGHT:
			wt = nWTypeTable[gnPieceNum];
			if ((wt == WTYPE_RIGHT) || (wt == WTYPE_ULC)) {
				TCDecodeM12Tile(pDecodeTo, &sgRightMask[31]);
				return;
			}
			if (wt == WTYPE_LRC) {
				TCDecodeM12Tile(pDecodeTo, &sgLeftMask[31]);
				return;
			}
		}
	}
	t = &microoffset[0][0];
	__asm {

		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		eax,dword ptr [nLVal]
		or		al,al
		jz		_NoLt
		cmp		al,byte ptr [lightmax]
		jz		_Black

		mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jnz		_Speed

		mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		ebx,dword ptr [nLVal];			// Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [pLightTbl];

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		_Type0							// Solid 32x32 block
		cmp		ax,1
		jz		_Type1							// 32x32 block with '0' holes
		cmp		ax,2
		jz		_Type2							// Left Triangle
		cmp		ax,3
		jz		_Type3							// Right Triangle
		cmp		ax,4
		jz		_Type4							// Left Triangle to wall
		jmp		_Type5							// Right Triangle to wall

_Speed:	mov		esi,dword ptr [t]
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [nLVal]
		shl		eax,2
		add		esi,eax							// Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [gdwPNum]		// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
_SCmp:	cmp		ax,8
		jz		_Type8							// PreTrans Solid 32x32 block
		cmp		ax,9
		jz		_Type9							// PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		_TypeA							// PreTrans Left Triangle
		cmp		ax,11
		jz		_TypeB							// PreTrans Right Triangle
		cmp		ax,12
		jz		_TypeC							// PreTrans Left Triangle to wall
		jmp		_TypeD							// PreTrans Right Triangle to wall

_NoLt:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RNoLt

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RNoLt:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		_SCmp

_Black:	mov		eax,dword ptr [gdwPNum]
		and		eax,08000h
		jz		_RBlk

		mov		esi,dword ptr [t]				// Get old micro tile number
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [gdwPNum],eax
		
_RBlk:	mov		ebx,dword ptr [pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				// Source

		mov		eax,dword ptr [gdwPNum]			// Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		_TypeG							// Black Trans Solid 32x32 block
		cmp		ax,1
		jz		_TypeH							// Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		_TypeI							// Black Trans Left Triangle
		cmp		ax,3
		jz		_TypeJ							// Black Trans Right Triangle
		cmp		ax,4
		jz		_TypeK							// Black Trans Left Triangle to wall
		jmp		_TypeL							// Black Trans Right Triangle to wall

/*-----------------------------------------------------------------------*/

_Type0:	mov		edx,32
		push	ebp
_T0Lp1:	
		push	edx
		cmp		edi,dword ptr [ClipY]
		jb		_T0C1
		add		esi,32
		add		edi,32
		jmp		_T0C2
_T0C1:
		xor		edx,edx		
		mov		ebp,8
_T0Lp2:
		mov eax,[esi]	
		add	esi,4		
		ror eax,16		
		mov dl,al		
		mov cl,[ebx+edx]
		mov dl,ah		
		mov ch,[ebx+edx]
		ror eax,16		
		shl	ecx,16		
		mov dl,al		
		mov cl,[ebx+edx]
		mov dl,ah		
		mov ch,[ebx+edx]
		mov	[edi],ecx	
		add	edi,4

		dec	ebp
		jnz	_T0Lp2
_T0C2:	
		sub		edi,NBUFFW32
		pop		edx
		dec		edx
		jnz		_T0Lp1
		pop ebp
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type1:	push	ebp
		mov		ecx,32
_T1Lp1:	push	ecx
		mov		ebp,32
_T1Lp2:	
		xor		eax,eax		// Load control byte
		mov		al,[esi]
		inc		esi

		or		al,al
		jns		_T1NoSkip
		neg		al
		add		edi,eax
		sub		ebp,eax
		jmp		_T1continue
_T1NoSkip:
		sub		ebp,eax
		cmp		edi,dword ptr [ClipY]
		jb		_T1NotCliped
		add		esi,eax
		add		edi,eax
		jmp		_T1continue
_T1NotCliped:	
		mov		ecx,eax
		call	xbytes
_T1continue:	
		or		ebp,ebp
		jnz		_T1Lp2

		pop		ecx
		sub		edi,NBUFFW32
		dec		ecx
		jnz		_T1Lp1
		pop		ebp
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type2:	
		push	ebp
		mov		ebp,30
_T2Lp1:	
		cmp		edi,dword ptr [ClipY]
		jb		_T2NotClipped
		add		esi,32
		add		edi,32
		sub		esi,ebp
		jmp		_T2continue
_T2NotClipped:	
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		call	xbytes
_T2continue:	
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		_T2Lp1


		mov		ebp,2
_T2Lp2:	
		cmp		edi,dword ptr [ClipY]
		jb		_T2NotClipped2
		add		esi,32
		add		edi,32
		sub		esi,ebp
		jmp		_T2continue2
_T2NotClipped2:	
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		call	xbytes
_T2continue2:	
		add		ebp,2
		sub		edi,NBUFFW32
		cmp		ebp,32
		jnz		_T2Lp2

		pop		ebp
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type3:	mov		edx,30
_T3Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T3C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T3x
_T3C1:	mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

_T3x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T3b
		add		edi,edx
		sub		edx,2
		jmp		_T3Lp1

_T3b:	mov		edx,2
_T3Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_T3C2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T3x2
_T3C2:	mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

_T3x2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_T3Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type4:	mov		edx,30
_T4Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T4C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_T4x
_T4C1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

_T4x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T4b
		sub		edx,2
		jmp		_T4Lp1

_T4b:	mov		edx,16
_T4Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_T4C2
		add		esi,32
		add		edi,32
		jmp		_T4C3
_T4C2:	mov		ecx,8

		push	edx
		call	xquads
		pop		edx

_T4C3:	sub		edi,NBUFFW32
		dec		edx
		jnz		_T4Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type5:	mov		edx,30
_T5Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T5C1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_T5x
_T5C1:	mov		ecx,32
		sub		ecx,edx

		push	edx
		call	xbytes
		pop		edx

_T5x:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_T5b
		add		edi,edx
		sub		edx,2
		jmp		_T5Lp1

_T5b:	mov		edx,16
_T5Lp3:	cmp		edi,dword ptr [ClipY]
		jb		_T5C2
		add		esi,32
		add		edi,32
		jmp		_T5C3
_T5C2:	mov		ecx,8

		push	edx
		call	xquads
		pop		edx

_T5C3:	sub		edi,NBUFFW32
		dec		edx
		jnz		_T5Lp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type8:	mov		edx,32
_T8Lp1:	cmp		edi,dword ptr [ClipY]
		jb		_T8C1
		add		esi,32
		add		edi,32
		jmp		_T8C2
_T8C1:	mov		ecx,8
		rep movsd
_T8C2:	sub		edi,NBUFFW32
		dec		edx
		jnz		_T8Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_Type9:	mov		ecx,32

_T9Lp1:	push	ecx
		mov		edx,32

_T9Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T9J

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_T9C1
		add		esi,eax
		add		edi,eax
		jmp		_T9x
_T9C1:	mov		ecx,eax
		shr		ecx,1
		jnc		_T9w
		movsb
		jecxz	_T9x
_T9w:	shr		ecx,1
		jnc		_T9Lp3
		movsw
		jecxz	_T9x
_T9Lp3:	rep movsd
_T9x:	or		edx,edx
		jz		_T9Nxt
		jmp		_T9Lp2

_T9J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T9Lp2
_T9Nxt:	pop		ecx
		sub		edi,NBUFFW32
		loop	_T9Lp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeA:	mov		edx,30
_TALp1:	cmp		edi,dword ptr [ClipY]
		jb		_TAC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TAx
_TAC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp2
		movsw
		jecxz	_TAx
_TALp2:	rep movsd
_TAx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TAb
		sub		edx,2
		jmp		_TALp1

_TAb:	mov		edx,2
_TALp3:	cmp		edi,dword ptr [ClipY]
		jb		_TAC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TAx2
_TAC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TALp4
		movsw
		jecxz	_TAx2
_TALp4:	rep movsd
_TAx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TALp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeB:	mov		edx,30
_TBLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TBC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TBx
_TBC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp2
		movsw
		jecxz	_TBx
_TBLp2:	rep movsd
_TBx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TBb
		add		edi,edx
		sub		edx,2
		jmp		_TBLp1

_TBb:	mov		edx,2
_TBLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TBC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TBx2
_TBC2:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TBLp4
		movsw
		jecxz	_TBx2
_TBLp4:	rep movsd
_TBx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TBLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeC:	mov		edx,30
_TCLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TCC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TCx
_TCC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TCLp2
		movsw
		jecxz	_TCx
_TCLp2:	rep movsd
_TCx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TCb
		sub		edx,2
		jmp		_TCLp1

_TCb:	mov		edx,16
_TCLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TCC2
		add		esi,32
		add		edi,32
		jmp		_TCC3
_TCC2:	mov		ecx,8
		rep movsd
_TCC3:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TCLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeD:	mov		edx,30
_TDLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TDC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TDx
_TDC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TDLp2
		movsw
		jecxz	_TDx
_TDLp2:	rep movsd
_TDx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TDb
		add		edi,edx
		sub		edx,2
		jmp		_TDLp1

_TDb:	mov		edx,16
_TDLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TDC2
		add		esi,32
		add		edi,32
		jmp		_TDC3
_TDC2:	mov		ecx,8
		rep movsd
_TDC3:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TDLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeG:	mov		edx,32
		xor		eax,eax
_TGLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TGC1
		add		esi,32
		add		edi,32
		jmp		_TGC2
_TGC1:	mov		ecx,8
		rep stosd
_TGC2:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TGLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeH:	mov		ecx,32

_THLp1:	push	ecx
		mov		edx,32

_THLp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_THJ

		sub		edx,eax
		cmp		edi,dword ptr [ClipY]
		jb		_THC1
		add		esi,eax
		add		edi,eax
		jmp		_THx
_THC1:	mov		ecx,eax
		add		esi,ecx
		xor		eax,eax
		shr		ecx,1
		jnc		_THw
		stosb
		jecxz	_THx
_THw:	shr		ecx,1
		jnc		_THLp3
		stosw
		jecxz	_THx
_THLp3:	rep stosd
_THx:	or		edx,edx
		jz		_THNxt
		jmp		_THLp2

_THJ:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_THLp2
_THNxt:	pop		ecx
		sub		edi,NBUFFW32
		loop	_THLp1
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeI:	mov		edx,30
		xor		eax,eax
_TILp1:	cmp		edi,dword ptr [ClipY]
		jb		_TIC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TIx
_TIC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TILp2
		stosw
		jecxz	_TIx
_TILp2:	rep stosd
_TIx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TIb
		sub		edx,2
		jmp		_TILp1

_TIb:	mov		edx,2
_TILp3:	cmp		edi,dword ptr [ClipY]
		jb		_TIC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TIx2
_TIC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TILp4
		stosw
		jecxz	_TIx2
_TILp4:	rep stosd
_TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		_TILp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeJ:	mov		edx,30
		xor		eax,eax
_TJLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TJC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TJx
_TJC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp2
		stosw
		jecxz	_TJx
_TJLp2:	rep stosd
_TJx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TJb
		add		edi,edx
		sub		edx,2
		jmp		_TJLp1

_TJb:	mov		edx,2
_TJLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TJC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TJx2
_TJC2:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TJLp4
		stosw
		jecxz	_TJx2
_TJLp4:	rep stosd
_TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		_TJLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeK:	mov		edx,30
		xor		eax,eax
_TKLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TKC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		_TKx
_TKC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TKLp2
		stosw
		jecxz	_TKx
_TKLp2:	rep stosd
_TKx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TKb
		sub		edx,2
		jmp		_TKLp1

_TKb:	mov		edx,16
_TKLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TKC2
		add		esi,32
		add		edi,32
		jmp		_TKC3
_TKC2:	mov		ecx,8
		rep stosd
_TKC3:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TKLp3
		jmp		_Done

/*-----------------------------------------------------------------------*/

_TypeL:	mov		edx,30
		xor		eax,eax
_TLLp1:	cmp		edi,dword ptr [ClipY]
		jb		_TLC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		_TLx
_TLC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		_TLLp2
		stosw
		jecxz	_TLx
_TLLp2:	rep stosd
_TLx:	sub		edi,NBUFFW32
		or		edx,edx
		jz		_TLb
		add		edi,edx
		sub		edx,2
		jmp		_TLLp1

_TLb:	mov		edx,16
_TLLp3:	cmp		edi,dword ptr [ClipY]
		jb		_TLC2
		add		esi,32
		add		edi,32
		jmp		_TLC3
_TLC2:	mov		ecx,8
		rep stosd
_TLC3:	sub		edi,NBUFFW32
		dec		edx
		jnz		_TLLp3

		jmp		_Done

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

_Done:

	}		// end of asm block
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawBlankMini(BYTE *pDecodeTo)
{
	__asm {
		mov		edi,dword ptr [pDecodeTo]		// Dest

		mov		edx,30
		mov		ebx,1
		xor		eax,eax
		//mov	eax,092929292h	// yellow
_BLp1:	add		edi,edx
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
_BLp2:	add		edi,edx
		mov		ecx,ebx
		rep stosd
		add		edi,edx
		sub		edi,NBUFFW64
		dec		ebx
		add		edx,2
		cmp		edx,32
		jnz		_BLp2
	}
}

