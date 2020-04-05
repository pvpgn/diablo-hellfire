;**************************************************************
;* scroll.asm
;* created 9.27.96
;* written by Collin Murray
;**************************************************************
.386p
.model FLAT
LOCALS
JUMPS

;include	diablo.h
;include	scrlasm.h
;include	gendung.h
;include	lighting.h

WTYPE_NONE		equ 0
WTYPE_LEFT		equ 1
WTYPE_RIGHT		equ 2
WTYPE_ULC		equ 3
WTYPE_LRC		equ 4

PART_TRANS_NONE		equ 0
PART_TRANS_LEFT		equ 1
PART_TRANS_RIGHT	equ 2
NBUFFW32			equ 800
NBUFFW64			equ 832

;***********************************************************
;* segment
;***********************************************************
.DATA

	;***********************************************************
	;* Externs from scrollrt.cpp
	;***********************************************************
	extrn _nLVal			:dword
	extrn _glClipY			:dword
	extrn _gnPieceNum		:dword
	extrn _nTrans			:dword
	extrn _gdwPNum			:dword
	extrn _gbPartialTrans	:byte

	;***********************************************************
	;* Externs from lighting.cpp
	;***********************************************************
	extrn _lightmax			:byte
	extrn _pLightTbl		:dword

	;***********************************************************
	;* Externs from gendung.cpp
	;***********************************************************
	extrn _microoffset		:dword
	extrn _pDungeonCels		:dword
	extrn _pSpeedCels		:dword
	extrn _nWTypeTable		:byte


	;*************************************************************************
	;* Private
	;*************************************************************************
	sgLineVal	dd ?
	sgCM			dd ?
	sgWT			db ?
	sgT			dd ?
	sgMask		dd ?
	sgTimeLow	dd ?
	sgTimeHigh	dd ?
	sgLoopTime	dd ?
	sgUnrollTime	dd ?
	sgRightMask label dword
		dd 0eaaaaaaah	; 1110 1010 1010 1010 1010 1010 1010 1010
		dd 0f5555555h	; 1111 0101 0101 0101 0101 0101 0101 0101
		dd 0feaaaaaah	; 1111 1110 1010 1010 1010 1010 1010 1010
		dd 0ff555555h	; 1111 1111 0101 0101 0101 0101 0101 0101
		dd 0ffeaaaaah	; 1111 1111 1110 1010 1010 1010 1010 1010
		dd 0fff55555h	; 1111 1111 1111 0101 0101 0101 0101 0101
		dd 0fffeaaaah	; 1111 1111 1111 1110 1010 1010 1010 1010
		dd 0ffff5555h	; 1111 1111 1111 1111 0101 0101 0101 0101
		dd 0ffffeaaah	; 1111 1111 1111 1111 1110 1010 1010 1010
		dd 0fffff555h	; 1111 1111 1111 1111 1111 0101 0101 0101
		dd 0fffffeaah	; 1111 1111 1111 1111 1111 1110 1010 1010
		dd 0ffffff55h	; 1111 1111 1111 1111 1111 1111 0101 0101
		dd 0ffffffeah	; 1111 1111 1111 1111 1111 1111 1110 1010
		dd 0fffffff5h	; 1111 1111 1111 1111 1111 1111 1111 0101
		dd 0fffffffeh	; 1111 1111 1111 1111 1111 1111 1111 1110
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111

		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111

	sgLeftMask label dword
		dd 0aaaaaaabh	; 1010 1010 1010 1010 1010 1010 1010 1011
		dd 05555555fh	; 0101 0101 0101 0101 0101 0101 0101 1111
		dd 0aaaaaabfh	; 1010 1010 1010 1010 1010 1010 1011 1111
		dd 0555555ffh	; 0101 0101 0101 0101 0101 0101 1111 1111
		dd 0aaaaabffh	; 1010 1010 1010 1010 1010 1011 1111 1111
		dd 055555fffh	; 0101 0101 0101 0101 0101 1111 1111 1111
		dd 0aaaabfffh	; 1010 1010 1010 1010 1011 1111 1111 1111
		dd 05555ffffh	; 0101 0101 0101 0101 1111 1111 1111 1111
		dd 0aaabffffh	; 1010 1010 1010 1011 1111 1111 1111 1111
		dd 0555fffffh	; 0101 0101 0101 1111 1111 1111 1111 1111
		dd 0aabfffffh	; 1010 1010 1011 1111 1111 1111 1111 1111
		dd 055ffffffh	; 0101 0101 1111 1111 1111 1111 1111 1111
		dd 0abffffffh	; 1010 1011 1111 1111 1111 1111 1111 1111
		dd 05fffffffh	; 0101 1111 1111 1111 1111 1111 1111 1111
		dd 0bfffffffh	; 1011 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111

		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111
		dd 0ffffffffh	; 1111 1111 1111 1111 1111 1111 1111 1111

	sgFullMask label dword
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101

		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101
		dd 0aaaaaaaah	; 1010 1010 1010 1010 1010 1010 1010 1010
		dd 055555555h	; 0101 0101 0101 0101 0101 0101 0101 0101

		sgDivBy3MulBy4 label dword
			dd  0, 0, 0, 4, 4, 4, 8, 8, 8,12,12,12,16,16,16
			dd 20,20,20,24,24,24,28,28,28,32,32,32,36,36,36
			dd 40,40,40,44,44,44,48,48,48,52,52,52,56,56,56
			dd 60,60,60

		TotalDataPerLineBottom label dword
			dd   0,  4,  8, 16, 24, 36, 48, 64, 80
			dd 100,120,144,168,196,224,256,288

		TotalDataPerLineTop label dword
			dd   0, 32, 60, 88,112,136,156,176,192
			dd 208,220,232,240,248,252,256,288


;***********************************************************
;* segment
;***********************************************************
.CODE


;***********************************************************
;* CLIP_BOTTOM(LoopStart, Quit, SkipCountReg, fTopPiece)
;*   Clips top of triangle piece
;* ESI - Source
;* EDI - Dest
;* EDX - # Pixels to skipped past
;* EAX,ECX - trashed
;***********************************************************
CLIP_BOTTOM MACRO LoopStart, Quit, SkipCountReg, fTopPiece, fDither
	LOCAL allClipped

	mov		eax,edi
	sub		eax,dword ptr [_glClipY]
	jb		LoopStart

	; divide eax by 256 to get 3x Lines to clip off
	; add 300h (768) to include the line at ClipY in the clipping
	; add 0ffh (255) so we always round up
	add		eax,3ffh
	shr		eax,8
	if fTopPiece
		cmp		eax,14*3
	else
		cmp		eax,15*3
	endif
	jg		allClipped
	mov		ecx,sgDivBy3MulBy4[eax*4]

	;add	esi,(data for lines skipped)
	mov		eax,ecx
	if fTopPiece
		add		esi,TotalDataPerLineTop[ecx]
	else
		add		esi,TotalDataPerLineBottom[ecx]
	endif

	;sub	edi,768*(lines skipped)
	shl		eax,6
	lea		eax,[eax+eax*2]
	shr		ecx,1
	sub		edi,eax
	if fTopPiece
		add		SkipCountReg,ecx
	else
		sub		SkipCountReg,ecx
	endif
	if fDither 
		mov eax,[sgLineVal]
		shr	ecx,1
		add eax,ecx
		mov	[sgLineVal],eax
	endif
	jmp		LoopStart
allClipped:
	if fTopPiece
		sub		edi,768*15
		add		esi,256		;should match TotalDataPerLineTop[nLines+1]
	else
		sub		edi,768*16
		add		esi,288		;should match TotalDataPerLineBottom[nLines+1]
	endif
	jmp		Quit
ENDM


;***********************************************************
;* X_DWORDS
;*  draw ecx dwords with a given light intesity
;* ECX - number of dwords
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_DWORDS MACRO
	LOCAL @@xnext,@@xend

@@xnext:
		mov eax,[esi]
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
		
		sub cl,4
		cmp	cl,4
		jge @@xnext

@@xend:
ENDM


;***********************************************************
;* X_BYTES_PAD
;*  draw ecx bytes with a given light intesity, accounting
;*  for padding data out to dword boundaries
;* CL - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTES_PAD MACRO
	LOCAL @@xwords,@@xbytes,@@xend

	cmp cl,4
	jl	@@xwords

	X_DWORDS

@@xwords:
	cmp	cl,2
	jl	@@xbytes

	mov dl, [esi]
	mov	ch, [ebx+edx]

	mov [edi],ch
	mov dl, [esi+1]

	mov	ch, [ebx+edx]
	mov [edi+1],ch

	add esi,2
	add edi,2

@@xbytes:
;	and cl,1
;	jz @@xend

;	mov dl, [esi]
;	inc	esi
;	mov dl,	[ebx+edx]
;	mov [edi],dl
;	inc edi

@@xend:
	mov	edx,esi
	and	edx,2
	add	esi,edx
ENDM


;***********************************************************
;* X_BYTES_PREPAD
;*  draw ecx bytes with a given light intesity
;* CL - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTES_PREPAD MACRO
	LOCAL @@xwords,@@xbytes,@@xend

	mov	edx,ecx
	and	edx,2
	add	esi,edx

	cmp cl,4
	jl	@@xwords

	X_DWORDS

@@xwords:
	cmp	cl,2
	jl	@@xbytes

	mov dl, [esi]
	mov	ch, [ebx+edx]

	mov [edi],ch
	mov dl, [esi+1]

	mov	ch, [ebx+edx]
	mov [edi+1],ch

	add esi,2
	add edi,2

@@xbytes:
;	and cl,1
;	jz @@xend

;	mov dl, [esi]
;	inc	esi
;	mov dl,	[ebx+edx]
;	mov [edi],dl
;	inc edi

@@xend:
ENDM


;***********************************************************
;* X_BYTES
;*  draw ecx bytes with a given light intesity
;* CL - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTES MACRO
	LOCAL @@xwords,@@xbytes,@@xend

	cmp cl,4
	jl	@@xwords

	X_DWORDS

@@xwords:
	cmp	cl,2
	jl	@@xbytes

	mov dl, [esi]
	mov	ch, [ebx+edx]

	mov [edi],ch
	mov dl, [esi+1]

	mov	ch, [ebx+edx]
	mov [edi+1],ch

	add esi,2
	add edi,2

@@xbytes:
	and cl,1
	jz @@xend

	mov dl, [esi]
	inc	esi
	mov dl,	[ebx+edx]
	mov [edi],dl
	inc edi

@@xend:
ENDM


;***********************************************************
;* X_DWORD2
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_DWORD2 MACRO
	LOCAL @@xquad2,@@xend
@@xquad2:				; blank first
; debug
;		shl ecx,2
;		add esi,ecx
;		add edi,ecx
;		jmp @@xend
		
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
	jnz @@xquad2
@@xend:
ENDM


;***********************************************************
;* X_WORD2
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_WORD2 MACRO
	LOCAL @@xquadt2,@@xend
	; blank first
	shr ecx,1
	jnc @@xquadt2
	mov dl,[esi+1]
	mov dl,[ebx+edx]
	add esi,2
	mov [edi+1],dl
	add edi,2

@@xquadt2:
	test cl,cl
	jz @@xend

	X_DWORD2

@@xend:
ENDM


;***********************************************************
;* X_DWORD1
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_DWORD1 MACRO
	LOCAL @@xquad1,@@xend
@@xquad1:				; xlat first
; debug
;		shl ecx,2
;		add esi,ecx
;		add edi,ecx
;		jmp @@xend

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
	jnz @@xquad1
@@xend:
ENDM


;***********************************************************
;* X_WORD1
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_WORD1 MACRO
	LOCAL @@xquadt1,@@xend
	; xlat first
	shr ecx,1
	jnc @@xquadt1
	mov dl,[esi]
	mov dl,[ebx+edx]
	add esi,2
	mov [edi],dl
	add edi,2

@@xquadt1:
	test cl,cl
	jz @@xend

	X_DWORD1

@@xend:

ENDM


;***********************************************************
;* X_BYTE1
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTE1 MACRO
	LOCAL @@noxword2,@@xend
	; blank first
	shr ecx,1
	jc @@noxword2
	X_WORD2
	jmp @@xend
@@noxword2:
	inc esi
	inc edi

	X_WORD1
@@xend:
ENDM


;***********************************************************
;* X_BYTE1_PREPAD
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTE1_PREPAD MACRO
	LOCAL noxword2,xend
	mov	edx,ecx
	and	edx,2
	add	esi,edx
	; blank first
	shr ecx,1
	jc noxword2
	X_WORD2
	jmp xend
noxword2:
	inc esi
	inc edi

	X_WORD1
xend:
ENDM


;***********************************************************
;* X_BYTE1_PAD
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTE1_PAD MACRO
	LOCAL noxword2,xend
	; blank first
	shr ecx,1
	jc noxword2
	X_WORD2
	jmp xend
noxword2:
	inc esi
	inc edi

	X_WORD1
xend:
	mov	edx,esi
	and	edx,2
	add	esi,edx
ENDM


;***********************************************************
;* X_BYTE2
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTE2 MACRO
	LOCAL @@no_xword1,@@xend
	; xlat first
	shr ecx,1
	jc @@no_xword1
	X_WORD1
	jmp @@xend
@@no_xword1:
	mov dl,[esi]
	mov dl,[ebx+edx]
	inc esi
	mov	[edi],dl
	inc edi

	X_WORD2
@@xend:
ENDM


;***********************************************************
;* X_BYTE2_PREPAD
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTE2_PREPAD MACRO
	LOCAL @@no_xword1,@@xend
	mov	edx,ecx
	and	edx,2
	add	esi,edx
	; xlat first
	shr ecx,1
	jc @@no_xword1
	X_WORD1
	jmp @@xend
@@no_xword1:
	mov dl,[esi]
	mov dl,[ebx+edx]
	inc esi
	mov	[edi],dl
	inc edi

	X_WORD2
@@xend:
ENDM


;***********************************************************
;* X_BYTE2_PAD
;*  draw ecx bytes with a given light intesity
;* ECX - number of bytes
;* ESI - source
;* EDI - dest
;* EBX - light table pointer
;* EAX - trashed
;* EDX - trashed
;***********************************************************
X_BYTE2_PAD MACRO
	LOCAL @@no_xword1,@@xend
	; xlat first
	shr ecx,1
	jc @@no_xword1
	X_WORD1
	jmp @@xend
@@no_xword1:
	mov dl,[esi]
	mov dl,[ebx+edx]
	inc esi
	mov	[edi],dl
	inc edi

	X_WORD2
@@xend:
	mov	edx,esi
	and	edx,2
	add	esi,edx
ENDM


;***********************************************************
;***********************************************************
RDTSC MACRO
	db 0fh, 31h
ENDM


;***********************************************************
;* static void DrawMTileDitherClipTop (BYTE *pDecodeTo)
;*		ECX = pDecodeTo
;***********************************************************
DrawMTileDitherClipTop proc
	push ebx
	push edx
	push edi
	push esi
	
	
	;sgT = &_microoffset[0][0];
	mov	edi, offset _microoffset
	mov	dword ptr [sgT], edi

	mov		edi, ecx		; Dest

	mov		eax,dword ptr [_nLVal]
	test	al,al
	jz		@@NoLt
	cmp		al,byte ptr [_lightmax]
	jz		@@Black

	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jnz		@@Speed

	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		ebx,dword ptr [_nLVal];			; Light conversion table
	shl		ebx,8
	add		ebx,dword ptr [_pLightTbl];

	mov		eax,dword ptr [_gdwPNum]		; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,0fh
	jz		@@Type0							; Solid 32x32 block
	cmp		ax,1
	jz		@@Type1							; 32x32 block with '0' holes
	cmp		ax,2
	jz		@@Type2							; Left Triangle
	cmp		ax,3
	jz		@@Type3							; Right Triangle
	cmp		ax,4
	jz		@@Type4							; Left Triangle to wall
	jmp		@@Type5							; Right Triangle to wall

@@Speed:	
	mov		esi,dword ptr [sgT]
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,4
	add		eax,dword ptr [_nLVal]
	shl		eax,2
	add		esi,eax							; Source
	mov		eax,dword ptr [esi]
	mov		esi,dword ptr [_pSpeedCels]
	add		esi,eax

	mov		eax,dword ptr [_gdwPNum]		; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,0fh
@@SCmp:
	cmp		ax,8
	jz		@@Type8							; PreTrans Solid 32x32 block
	cmp		ax,9
	jz		@@Type9							; PreTrans 32x32 block with '0' holes
	cmp		ax,10
	jz		@@TypeA							; PreTrans Left Triangle
	cmp		ax,11
	jz		@@TypeB							; PreTrans Right Triangle
	cmp		ax,12
	jz		@@TypeC							; PreTrans Left Triangle to wall
	jmp		@@TypeD							; PreTrans Right Triangle to wall

@@NoLt:	
	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jz		@@RNoLt

	mov		esi,dword ptr [sgT]				; Get old micro tile number
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,6
	add		esi,eax
	mov		eax,dword ptr [_gdwPNum]
	and		eax,0f000h
	add		eax,dword ptr [esi]
	mov		dword ptr [_gdwPNum],eax
		
@@RNoLt:	
	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		eax,dword ptr [_gdwPNum]			; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,07h
	add		eax,8
	jmp		@@SCmp

@@Black:	
	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jz		@@RBlk

	mov		esi,dword ptr [sgT]				; Get old micro tile number
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,6
	add		esi,eax
	mov		eax,dword ptr [_gdwPNum]
	and		eax,0f000h
	add		eax,dword ptr [esi]
	mov		dword ptr [_gdwPNum],eax
		
@@RBlk:	
	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		eax,dword ptr [_gdwPNum]			; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,07h
	jz		@@TypeG							; Black Trans Solid 32x32 block
	cmp		ax,1
	jz		@@TypeH							; Black Trans 32x32 block with '0' holes
	cmp		ax,2
	jz		@@TypeI							; Black Trans Left Triangle
	cmp		ax,3
	jz		@@TypeJ							; Black Trans Right Triangle
	cmp		ax,4
	jz		@@TypeK							; Black Trans Left Triangle to wall
	jmp		@@TypeL							; Black Trans Right Triangle to wall

;*-----------------------------------------------------------------------*

@@Type0:
		push	ebp	
		mov		ebp,16
@@T0Lp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@T0Done
		mov		ecx,8
		X_DWORD2
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [_glClipY]
		jb		@@T0Done
		mov		ecx,8
		X_DWORD1
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T0Lp1
@@T0Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*

@@Type1:
		push	ebp	
		mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ebp,32
@@T1Lp1:	
		mov		edx,32
@@T1Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T1J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T1Done
		mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		@@T1Od
		push	edx
		X_BYTE1
		pop		edx
		jmp @@T1x
@@T1Od:
		push	edx
		X_BYTE2
		pop		edx
@@T1x:	
		test	edx,edx
		jz		@@T1Nxt
		jmp		@@T1Lp2
@@T1J:	
		neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@T1Lp2
@@T1Nxt:	
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T1Lp1
@@T1Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*

@@Type2:
		push	ebp	
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		ebp,30
@@T2Lp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@T2End
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T2Od
		X_BYTE1_PREPAD
		jmp	@@T2x
@@T2Od:
		X_BYTE2_PREPAD
@@T2x:	
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		@@T2Lp1


@@T2b:	
		mov		ebp,2
@@T2Lp4:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@T2End
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T2Od2
		X_BYTE1_PREPAD
		jmp	@@T2x2
@@T2Od2:
		X_BYTE2_PREPAD
@@T2x2:	
		sub		edi,NBUFFW32
		add		ebp,2
		cmp		ebp,32
		jnz		@@T2Lp4
@@T2End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type3:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		ebp,30
@@T3Lp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@T3End
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T3Od

		X_BYTE1_PAD
		jmp	@@T3x
@@T3Od:
		X_BYTE2_PAD
@@T3x:	
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T3Lp1


@@T3b:	
		mov		ebp,2
@@T3Lp4:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@T3End
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T3Od2

		X_BYTE1_PAD
		jmp	@@T3x2
@@T3Od2:
		X_BYTE2_PAD
@@T3x2:	
		sub		edi,NBUFFW32
		add		edi,ebp
		add		ebp,2
		cmp		ebp,32
		jnz		@@T3Lp4
@@T3End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type4:
	push	ebp	
	xor		eax,eax
	mov		dword ptr [sgLineVal],eax
	mov		ebp,30
@@T4Lp1:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@T4End
	add		edi,ebp
	mov		ecx,32
	sub		ecx,ebp
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	jz		@@T4Od
	X_BYTE1_PREPAD
	jmp	@@T4x
@@T4Od:
	X_BYTE2_PREPAD
@@T4x:	
	sub		edi,NBUFFW32
	sub		ebp,2
	jge		@@T4Lp1

	mov		ebp,8
@@T4Lp4:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@T4End
	mov		ecx,8
	X_DWORD2
	sub		edi,NBUFFW32
	cmp		edi,dword ptr [_glClipY]
	jb		@@T4End
	mov		ecx,8
	X_DWORD1
	sub		edi,NBUFFW32
	dec		ebp
	jnz		@@T4Lp4
@@T4End:
	pop		ebp
	jmp		@@Done

;*-----------------------------------------------------------------------*

@@Type5:
		push	ebp	
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		ebp,30
@@T5Lp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5End
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T5Od
		X_BYTE1_PAD
		jmp	@@T5x
@@T5Od:
		X_BYTE2_PAD
@@T5x:	
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T5Lp1

@@T5b:	
		mov		ebp,8
@@T5Lp4:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5End
		mov		ecx,8
		X_DWORD2
		sub		edi,NBUFFW32
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5End
		mov		ecx,8
		X_DWORD1
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T5Lp4
@@T5End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type8:	
		mov		edx,16
@@T8Lp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@T8Lp2:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@T8Lp2
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@T8Lp3:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@T8Lp3
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@T8Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type9:
		push	ebp	
		mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ebp,32
@@T9Lp1:	
		mov		edx,32
@@T9Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T9J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T9Done
		mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		@@T9Od
		shr		ecx,1
		jnc		@@T9w
		inc		esi
		inc		edi
		test	ecx,ecx
		jz	@@T9x
		jmp		@@T9w2
@@T9w:	
		shr		ecx,1
		jnc		@@T9Lp3
		inc		esi
		inc		edi
		mov		al,[esi]
		inc		esi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@T9x
@@T9Lp3:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@T9Lp3
		jmp		@@T9x
@@T9Od:	
		shr		ecx,1
		jnc		@@T9w2
		mov		al,[esi]
		inc		esi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@T9x
		jmp		@@T9w
@@T9w2:	
		shr		ecx,1
		jnc		@@T9Lp4
		mov		al,[esi]
		add		esi,2
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@T9x
@@T9Lp4:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@T9Lp4
@@T9x:	
		test	edx,edx
		jz		@@T9Nxt
		jmp		@@T9Lp2
@@T9J:	
		neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@T9Lp2
@@T9Nxt:	
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T9Lp1
@@T9Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeA:	
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TALp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TAOd
		shr		ecx,2
		jnc		@@TALp2
		mov		ax,[esi+2]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz		@@TAx
@@TALp2:
		mov		eax,[esi]
		add		esi,4	
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TALp2
		jmp		@@TAx
@@TAOd:	
		shr		ecx,2
		jnc		@@TALp3
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz		@@TAx
@@TALp3:
		mov		eax,[esi]
		add		esi,4	
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TALp3
@@TAx:	
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TALp1


		mov		edx,2
@@TALp4:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TAOd2
		shr		ecx,2
		jnc		@@TALp5
		mov		ax,[esi+2]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz		@@TAx2
@@TALp5:
		mov		eax,[esi]
		add		esi,4	
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TALp5
		jmp		@@TAx2
@@TAOd2:	
		shr		ecx,2
		jnc		@@TALp6
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz		@@TAx2
@@TALp6:
		mov		eax,[esi]
		add		esi,4	
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TALp6
@@TAx2:	
		sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TALp4
		jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeB:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TBLp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TBEnd
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		ebp,ecx
		mov		dword ptr [sgLineVal],eax
		jz		@@TBOd
		shr		ecx,2
		jz		@@TBcontinue
@@TBLp2:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TBLp2
@@TBcontinue:
		and		ebp,2
		jz		@@TBx
		mov		ax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		jmp		@@TBx
@@TBOd:	
		shr		ecx,2
		jz		@@TBcontinue2
@@TBLp3:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TBLp3
@@TBcontinue2:
		and		ebp,2
		jz		@@TBx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
@@TBx:	
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TBLp1


		mov		edx,2
@@TBLp4:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TBEnd
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TBOd2
		shr		ecx,2
		jz		@@TBcontinue3
@@TBLp5:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TBLp5
@@TBcontinue3:
		and		ebp,2
		jz		@@TBx2
		mov		ax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		jmp		@@TBx2
@@TBOd2:	
		shr		ecx,2
		jz		@@TBcontinue4
@@TBLp6:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TBLp6
@@TBcontinue4:
		and		ebp,2
		jz		@@TBx2
		mov		ax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
@@TBx2:	
		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TBLp4
@@TBEnd:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeC:	
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TCLp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TCOd
		shr		ecx,2
		jnc		@@TCLp2
		mov		ax,[esi+2]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz		@@TCx
@@TCLp2:
		mov		eax,[esi]
		add		esi,4	
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TCLp2
		jmp		@@TCx
@@TCOd:	
		shr		ecx,2
		jnc		@@TCLp3
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz		@@TCx
@@TCLp3:
		mov		eax,[esi]
		add		esi,4	
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TCLp3
@@TCx:	
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TCLp1


		mov		edx,8
@@TCLp4:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@TCLp5:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TCLp5
		sub		edi,NBUFFW32
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@TCLp6:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TCLp6
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TCLp4
		jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeD:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TDLp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDEnd
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		ebp,ecx
		mov		dword ptr [sgLineVal],eax
		jz		@@TDOd
		shr		ecx,2
		jz		@@TDcontinue
@@TDLp2:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TDLp2
@@TDcontinue:
		and		ebp,2
		jz		@@TDx
		mov		ax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		jmp		@@TDx

@@TDOd:	
		shr		ecx,2
		jz		@@TDcontinue2
@@TDLp3:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TDLp3
@@TDcontinue2:
		and		ebp,2
		jz		@@TDx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
@@TDx:	
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TDLp1


		mov		edx,8
@@TDLp4:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDEnd
		mov		ecx,8
@@TDLp5:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TDLp5
		sub		edi,NBUFFW32

		cmp		edi,dword ptr [_glClipY]
		jb		@@TDEnd
		mov		ecx,8
@@TDLp6:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TDLp6
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TDLp4
@@TDEnd:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeG:	
	mov		edx,16
	xor		eax,eax
@@TGLp1:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,8
@@TGLp2:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TGLp2
	sub		edi,NBUFFW32

	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,8
@@TGLp3:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TGLp3
	sub		edi,NBUFFW32
	dec		edx
	jnz		@@TGLp1
	jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeH:
	push	ebp
	mov		eax,edi
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	mov		ebp,32

@@THLp1:	
	mov		edx,32

@@THLp2:	
	xor		eax,eax				; Load control byte
	mov		al,[esi]
	inc		esi
	test	al,al
	js		@@THJ

	sub		edx,eax
	cmp		edi,dword ptr [_glClipY]
	jb		@@THDone
	mov		ecx,eax
	add		esi,ecx
	mov		eax,edi
	and		eax,1
	cmp		eax,dword ptr [sgLineVal]
	jnz		@@THOd
	xor		eax,eax
	shr		ecx,1
	jnc		@@THw
	inc		edi
	test	ecx,ecx
	jz		@@THx
	jmp		@@THw2
@@THw:	
	shr		ecx,1
	jnc		@@THLp3
	inc		edi
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz		@@THx
@@THLp3:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@THLp3
	jmp		@@THx

@@THOd:	
	xor		eax,eax
	shr		ecx,1
	jnc		@@THw2
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz		@@THx
	jmp		@@THw
@@THw2:	
	shr		ecx,1
	jnc		@@THLp4
	mov		[edi],al
	add		edi,2
	test	ecx,ecx
	jz		@@THx
@@THLp4:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@THLp4

@@THx:	
	test	edx,edx
	jz		@@THNxt
	jmp		@@THLp2

@@THJ:	
	neg		al					; Do jump
	add		edi,eax
	sub		edx,eax
	jnz		@@THLp2

@@THNxt:	
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	sub		edi,NBUFFW32
	dec		ebp
	jnz		@@THLp1
@@THDone:
	pop		ebp
	jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeI:	
	xor		eax,eax
	mov		dword ptr [sgLineVal],eax
	mov		edx,30
@@TILp1:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	add		edi,edx
	mov		ecx,32
	sub		ecx,edx
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	jz		@@TIOd
	xor		eax,eax
	shr		ecx,2
	jnc		@@TILp2
	inc		edi
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz	@@TIx
@@TILp2:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TILp2
	jmp		@@TIx

@@TIOd:	
	xor		eax,eax
	shr		ecx,2
	jnc		@@TILp3
	mov		[edi],al
	add		edi,2
	test	ecx,ecx
	jz	@@TIx
@@TILp3:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TILp3

@@TIx:	
	sub		edi,NBUFFW32
	test	edx,edx
	jz		@@TIb
	sub		edx,2
	jmp		@@TILp1

@@TIb:	
	mov		edx,2
@@TILp4:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	add		edi,edx
	mov		ecx,32
	sub		ecx,edx
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	jz		@@TIOd2
	xor		eax,eax
	shr		ecx,2
	jnc		@@TILp5
	inc		edi
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz	@@TIx2
@@TILp5:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TILp5
	jmp		@@TIx2

@@TIOd2:	
	xor		eax,eax
	shr		ecx,2
	jnc		@@TILp6
	mov		[edi],al
	add		edi,2
	test	ecx,ecx
	jz	@@TIx2
@@TILp6:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TILp6

@@TIx2:	
	sub		edi,NBUFFW32
	add		edx,2
	cmp		edx,32
	jnz		@@TILp4
	jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeJ:	
	xor		eax,eax
	mov		dword ptr [sgLineVal],eax
	mov		edx,30
@@TJLp1:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,32
	sub		ecx,edx
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	jz		@@TJOd
	xor		eax,eax
	shr		ecx,2
	jnc		@@TJLp2
	inc		edi
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz	@@TJx
@@TJLp2:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TJLp2
	jmp		@@TJx

@@TJOd:	
	xor		eax,eax
	shr		ecx,2
	jnc		@@TJLp3
	mov		[edi],al
	add		edi,2
	test	ecx,ecx
	jz	@@TJx
@@TJLp3:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TJLp3

@@TJx:	
	sub		edi,NBUFFW32
	test	edx,edx
	jz		@@TJb
	add		edi,edx
	sub		edx,2
	jmp		@@TJLp1

@@TJb:	
	mov		edx,2
@@TJLp4:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,32
	sub		ecx,edx
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	jz		@@TJOd2
	xor		eax,eax
	shr		ecx,2
	jnc		@@TJLp5
	inc		edi
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz	@@TJx2
@@TJLp5:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TJLp5
	jmp		@@TJx2

@@TJOd2:	
	xor		eax,eax
	shr		ecx,2
	jnc		@@TJLp6
	mov		[edi],al
	add		edi,2
	test	ecx,ecx
	jz	@@TJx2
@@TJLp6:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TJLp6

@@TJx2:	
	sub		edi,NBUFFW32
	add		edi,edx
	add		edx,2
	cmp		edx,32
	jnz		@@TJLp4
	jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeK:	
	xor		eax,eax
	mov		dword ptr [sgLineVal],eax
	mov		edx,30
@@TKLp1:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	add		edi,edx
	mov		ecx,32
	sub		ecx,edx
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	jz		@@TKOd
	xor		eax,eax
	shr		ecx,2
	jnc		@@TKLp2
	inc		edi
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz	@@TKx
@@TKLp2:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TKLp2
	jmp		@@TKx

@@TKOd:	
	xor		eax,eax
	shr		ecx,2
	jnc		@@TKLp3
	mov		[edi],al
	add		edi,2
	test	ecx,ecx
	jz	@@TKx
@@TKLp3:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TKLp3

@@TKx:	
	sub		edi,NBUFFW32
	test	edx,edx
	jz		@@TKb
	sub		edx,2
	jmp		@@TKLp1

@@TKb:	
	mov		edx,8
	xor		eax,eax
@@TKLp4:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,8
@@TKLp5:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TKLp5
	sub		edi,NBUFFW32

	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,8
@@TKLp6:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TKLp6
	sub		edi,NBUFFW32
	dec		edx
	jnz		@@TKLp4
	jmp		@@Done

;*-----------------------------------------------------------------------*

@@TypeL:	
	xor		eax,eax
	mov		dword ptr [sgLineVal],eax
	mov		edx,30
@@TLLp1:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,32
	sub		ecx,edx
	mov		eax,dword ptr [sgLineVal]
	inc		eax
	and		eax,1
	mov		dword ptr [sgLineVal],eax
	jz		@@TLOd
	xor		eax,eax
	shr		ecx,2
	jnc		@@TLLp2
	inc		edi
	mov		[edi],al
	inc		edi
	test	ecx,ecx
	jz	@@TLx
@@TLLp2:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TLLp2
	jmp		@@TLx

@@TLOd:	
	xor		eax,eax
	shr		ecx,2
	jnc		@@TLLp3
	mov		[edi],al
	add		edi,2
	test	ecx,ecx
	jz	@@TLx
@@TLLp3:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TLLp3

@@TLx:	
	sub		edi,NBUFFW32
	test	edx,edx
	jz		@@TLb
	add		edi,edx
	sub		edx,2
	jmp		@@TLLp1

@@TLb:	
	mov		edx,8
	xor		eax,eax
@@TLLp4:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,8
@@TLLp5:	
	mov		byte ptr [edi+1],al
	mov		byte ptr [edi+3],al
	add		edi,4
	dec		ecx
	jnz		@@TLLp5
	sub		edi,NBUFFW32

	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	mov		ecx,8
@@TLLp6:	
	mov		byte ptr [edi],al
	mov		byte ptr [edi+2],al
	add		edi,4
	dec		ecx
	jnz		@@TLLp6
	sub		edi,NBUFFW32
	dec		edx
	jnz		@@TLLp4
	jmp		@@Done

@@Done:	

	pop esi
	pop edi
	pop edx
	pop ebx
	ret
DrawMTileDitherClipTop endp



;***********************************************************
;* static void DrawMTileHalfDitherClipTop (BYTE *pDecodeTo, long *sgMask)
;*		ECX = pDecodeTo
;*		EDX = sgMask
;***********************************************************
DrawMTileHalfDitherClipTop proc
	push	ebx
	push	edi
	push	esi

	;sgT = &_microoffset[0][0];
	mov	edi, offset _microoffset
	mov	dword ptr [sgT], edi

	mov		edi,ecx		; Dest
	mov		dword ptr [sgMask], edx

	mov		eax,dword ptr [_nLVal]
	test	al,al
	jz		@@NoLt
	cmp		al,byte ptr [_lightmax]
	jz		@@Black

	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jnz		@@Speed

	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		ebx,dword ptr [_nLVal];			; Light conversion table
	shl		ebx,8
	add		ebx,dword ptr [_pLightTbl];

	mov		eax,dword ptr [_gdwPNum]		; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,0fh
	jz		@@Type0							; Solid 32x32 block
	jz		@@TypeG
	cmp		ax,1
	jz		@@Type1							; 32x32 block with '0' holes
	cmp		ax,2
	jz		@@Type2							; Left Triangle
	cmp		ax,3
	jz		@@Type3							; Right Triangle
	cmp		ax,4
	jz		@@Type4							; Left Triangle to wall
	jmp		@@Type5							; Right Triangle to wall

@@Speed:
	mov		esi,dword ptr [sgT]
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,4
	add		eax,dword ptr [_nLVal]
	shl		eax,2
	add		esi,eax							; Source
	mov		eax,dword ptr [esi]
	mov		esi,dword ptr [_pSpeedCels]
	add		esi,eax

	mov		eax,dword ptr [_gdwPNum]		; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,0fh
@@SCmp:
	cmp		ax,8
	jz		@@Type8							; PreTrans Solid 32x32 block
	cmp		ax,9
	jz		@@Type9							; PreTrans 32x32 block with '0' holes
	cmp		ax,10
	jz		@@TypeA							; PreTrans Left Triangle
	cmp		ax,11
	jz		@@TypeB							; PreTrans Right Triangle
	cmp		ax,12
	jz		@@TypeC							; PreTrans Left Triangle to wall
	jmp		@@TypeD							; PreTrans Right Triangle to wall

@@NoLt:
	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jz		@@RNoLt

	mov		esi,dword ptr [sgT]				; Get old micro tile number
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,6
	add		esi,eax
	mov		eax,dword ptr [_gdwPNum]
	and		eax,0f000h
	add		eax,dword ptr [esi]
	mov		dword ptr [_gdwPNum],eax
		
@@RNoLt:
	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		eax,dword ptr [_gdwPNum]			; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,07h
	add		eax,8
	jmp		@@SCmp

@@Black:
	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jz		@@RBlk

	mov		esi,dword ptr [sgT]				; Get old micro tile number
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,6
	add		esi,eax
	mov		eax,dword ptr [_gdwPNum]
	and		eax,0f000h
	add		eax,dword ptr [esi]
	mov		dword ptr [_gdwPNum],eax
		
@@RBlk:
	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		eax,dword ptr [_gdwPNum]			; Determine draw type
	mov		al,ah
	shr		eax,4
	and		eax,07h
	jz		@@TypeG							; Black Trans Solid 32x32 block
	cmp		ax,1
	jz		@@TypeH							; Black Trans 32x32 block with '0' holes
	cmp		ax,2
	jz		@@TypeI							; Black Trans Left Triangle
	cmp		ax,3
	jz		@@TypeJ							; Black Trans Right Triangle
	cmp		ax,4
	jz		@@TypeK							; Black Trans Left Triangle to wall
	jmp		@@TypeL							; Black Trans Right Triangle to wall

;*-----------------------------------------------------------------------*/

@@Type0:	
	mov		edx,32
@@T0Lp1:	
	cmp		edi,dword ptr [_glClipY]
	jb		@@Done
	push	edx
	mov		eax,dword ptr [sgMask]
	mov		edx,dword ptr [eax]
	mov		ecx,32
@@T0Lp2:	
	mov		al,[esi]
	inc		esi
	shl		edx,1
	jnc		@@T0S1
	xlatb
	mov		byte ptr [edi],al
@@T0S1:	
	inc		edi
	dec		ecx
	jnz		@@T0Lp2
	pop		edx
	sub		edi,NBUFFW32
	mov		eax,dword ptr [sgMask]
	sub		eax,4
	mov		dword ptr [sgMask],eax
	dec		edx
	jnz		@@T0Lp1
	jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type1:	
		mov		ecx,32

@@T1Lp1:	
		push	ecx
		mov		eax,dword ptr [sgMask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [sgCM],eax

		mov		edx,32

@@T1Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T1J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@PDone
		mov		ecx,eax
		push	edx
		mov		edx,dword ptr [sgCM]
@@T1Lp3:	
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T1S1
		xlatb
		mov		byte ptr [edi],al
@@T1S1:	inc		edi
		dec		ecx
		jnz		@@T1Lp3
		mov		dword ptr [sgCM],edx
		pop		edx
		test	edx,edx
		jz		@@T1Nxt
		jmp		@@T1Lp2

@@T1J:	neg		al					; Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		@@T1S2
		push	eax
		mov		eax,dword ptr [sgCM]
		shl		eax,cl
		mov		dword ptr [sgCM],eax
		pop		eax
@@T1S2:	sub		edx,eax
		jnz		@@T1Lp2
@@T1Nxt:
		pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		ecx
		jnz		@@T1Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type2:
		mov		edx,30
@@T2Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@T2Lp2
		mov		ax,[esi+2]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@T2x
@@T2Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T2Lp2
@@T2x:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@T2Lp1


		mov		edx,2
@@T2Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@T2Lp4
		mov		ax,[esi+2]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@T2x2
@@T2Lp4:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T2Lp4
@@T2x2:
		sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@T2Lp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type3:
		push	ebp
		mov		edx,30
@@T3Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T3End
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@T3continue
@@T3Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T3Lp2
@@T3continue:
		and		ebp,2
		jz		@@T3x
		mov		ax,[esi]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
@@T3x:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@T3Lp1

		mov		edx,2
@@T3Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T3End
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@T3continue2
@@T3Lp4:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T3Lp4
@@T3continue2:
		and		ebp,2
		jz		@@T3x2
		mov		ax,[esi]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
@@T3x2:
		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@T3Lp3
@@T3End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type4:
		mov		edx,30
@@T4Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@T4Lp2
		mov		ax,[esi+2]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@T4x
@@T4Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T4Lp2
@@T4x:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@T4Lp1

		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@T4Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
		mov		eax,esi
		and		eax,2
		add		esi,eax
@@T4Lp4:	
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T4S1
		xlatb
		mov		[edi],al
@@T4S1:
		inc		edi
		dec		ecx
		jnz		@@T4Lp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@T4Lp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type5:
		push	ebp
		mov		edx,30
@@T5Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5End
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@T5continue
@@T5Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T5Lp2
@@T5continue:
		and		ebp,2
		jz		@@T5x
		mov		ax,[esi]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
@@T5x:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@T5Lp1

		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@T5Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5End
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@T5Lp4:	
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T5S1
		xlatb
		mov		byte ptr [edi],al
@@T5S1:
		inc		edi
		dec		ecx
		jnz		@@T5Lp4
		mov		ebp,esi
		and		ebp,2
		add		esi,ebp
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@T5Lp3
@@T5End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type8:	mov		edx,32
@@T8Lp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@T8Lp2:
		mov		al,[esi]
		inc		esi	
		shl		edx,1
		jnc		@@T8S1
		mov		byte ptr [edi],al
@@T8S1:	inc		edi
		dec		ecx
		jnz		@@T8Lp2
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@T8Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type9:	mov		ecx,32

@@T9Lp1:	push	ecx
		mov		eax,dword ptr [sgMask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [sgCM],eax

		mov		edx,32

@@T9Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T9J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@PDone
		push	edx
		mov		edx,dword ptr [sgCM]
		mov		ecx,eax
@@T9Lp3:	
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T9S1
		mov		byte ptr [edi],al
@@T9S1:	inc		edi
		dec		ecx
		jnz		@@T9Lp3
		mov		dword ptr [sgCM],edx
		pop		edx
		test	edx,edx
		jz		@@T9Nxt
		jmp		@@T9Lp2

@@T9J:	neg		al					; Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		@@T9S2
		mov		ebx,dword ptr [sgCM]
		shl		ebx,cl
		mov		dword ptr [sgCM],ebx
@@T9S2:	sub		edx,eax
		jnz		@@T9Lp2
@@T9Nxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		ecx
		jnz		@@T9Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeA:	
		mov		edx,30
@@TALp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TALp2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@TAx
@@TALp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		dec		ecx
		lea		edi,[edi+4]
		jnz		@@TALp2
@@TAx:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TALp1

		mov		edx,2
@@TALp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TALp4
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		test	ecx,ecx
		lea		edi,[edi+2]
		jz		@@TAx2
@@TALp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TALp4
@@TAx2:
		sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TALp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeB:
		push	ebp	
		mov		edx,30
@@TBLp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TBEnd
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TBcontinue
@@TBLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TBLp2
@@TBcontinue:
		and		ebp,2
		jz		@@TBx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TBx:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TBLp1

@@TBb:	
		mov		edx,2
@@TBLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TBEnd
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TBcontinue2
@@TBLp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TBLp4
@@TBcontinue2:
		and		ebp,2
		jz		@@TBx2
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TBx2:
		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TBLp3
@@TBEnd:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeC:
		mov		edx,30
@@TCLp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TCLp2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TCx
@@TCLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TCLp2
@@TCx:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TCLp1

@@TCb:
		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TCLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@TCLp4:	
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@TCS1
		mov		byte ptr [edi],al
@@TCS1:
		inc		edi
		dec		ecx
		jnz		@@TCLp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TCLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeD:
		push	ebp
		mov		edx,30
@@TDLp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDEnd
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TDcontinue
@@TDLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TDLp2
@@TDcontinue:
		and		ebp,2
		jz		@@TDx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TDx:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TDLp1

		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TDLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDEnd
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@TDLp4:	
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@TDS1
		mov		byte ptr [edi],al
@@TDS1:
		inc		edi
		dec		ecx
		jnz		@@TDLp4
		mov		ebp,esi
		and		ebp,2
		add		esi,ebp
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TDLp3
@@TDEnd:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeG:	mov		edx,32
@@TGLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
@@TGLp2:	shl		edx,1
		jnc		@@TGS1
		mov		byte ptr [edi],al
@@TGS1:	inc		edi
		dec		ecx
		jnz		@@TGLp2
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TGLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeH:	mov		ecx,32

@@THLp1:	push	ecx
		mov		eax,dword ptr [sgMask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [sgCM],eax

		mov		edx,32

@@THLp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@THJ

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@PDone
		push	edx
		mov		edx,dword ptr [sgCM]
		mov		ecx,eax
		add		esi,ecx
		xor		eax,eax
@@THLp3:	shl		edx,1
		jnc		@@THS1
		mov		byte ptr [edi],al
@@THS1:	inc		edi
		dec		ecx
		jnz		@@THLp3
		mov		dword ptr [sgCM],edx
		pop		edx
		test	edx,edx
		jz		@@THNxt
		jmp		@@THLp2

@@THJ:	neg		al					; Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		@@THS2
		mov		ebx,dword ptr [sgCM]
		shl		ebx,cl
		mov		dword ptr [sgCM],ebx
@@THS2:	sub		edx,eax
		jnz		@@THLp2
@@THNxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		ecx
		jnz		@@THLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeI:	mov		edx,30
		xor		eax,eax
@@TILp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TILp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TIx
@@TILp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp2
@@TIx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TIb
		sub		edx,2
		jmp		@@TILp1

@@TIb:	mov		edx,2
@@TILp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx

		shr		ecx,2
		jnc		@@TILp4
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TIx2
@@TILp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp4
@@TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TILp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeJ:	mov		edx,30
		xor		eax,eax
@@TJLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TJx
@@TJLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp2
@@TJx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TJb
		add		edi,edx
		sub		edx,2
		jmp		@@TJLp1

@@TJb:	mov		edx,2
@@TJLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp4
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TJx2
@@TJLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp4
@@TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TJLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeK:	mov		edx,30
		xor		eax,eax
@@TKLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TKLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TKx
@@TKLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TKLp2
@@TKx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TKb
		sub		edx,2
		jmp		@@TKLp1

@@TKb:	mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TKLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
@@TKLp4:	shl		edx,1
		jnc		@@TKS1
		mov		byte ptr [edi],al
@@TKS1:	inc		edi
		dec		ecx
		jnz		@@TKLp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TKLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeL:	mov		edx,30
		xor		eax,eax
@@TLLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TLLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TLx
@@TLLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TLLp2
@@TLx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TLb
		add		edi,edx
		sub		edx,2
		jmp		@@TLLp1

@@TLb:	mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TLLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
@@TLLp4:	shl		edx,1
		jnc		@@TLS1
		mov		byte ptr [edi],al
@@TLS1:	inc		edi
		dec		ecx
		jnz		@@TLLp4
		pop		edx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TLLp3
		jmp		@@Done

@@PDone:	
	pop		eax
@@Done:	
	
	pop	esi
	pop	edi
	pop	ebx
	ret
DrawMTileHalfDitherClipTop endp



;***********************************************************
;* void DrawMTileClipTop (BYTE *pDecodeTo)
;*		ECX = pDecodeTo
;***********************************************************
public @DrawMTileClipTop@4
@DrawMTileClipTop@4 proc
	push	ebx
	push	edx
	push	edi
	push	esi

	mov		edx, dword ptr [_nTrans]
	test	edx,edx
	jz		@@NoTrans

	; if (gbPartialTrans == PART_TRANS_NONE) {
	mov		dl, byte ptr [_gbPartialTrans]
	cmp		dl, PART_TRANS_NONE
	jne		@@TransLeft
	call	DrawMTileDitherClipTop
	jmp		@@Done

@@TransLeft:
	; if (gbPartialTrans == PART_TRANS_LEFT) {
	cmp		dl, PART_TRANS_LEFT
	jne		@@TransRight
	
	;al = nWTypeTable[_gnPieceNum];
	mov		ebx, dword ptr [_gnPieceNum]
	mov		al, _nWTypeTable[ebx]
	
	;	if ((al == WTYPE_LEFT) || (al == WTYPE_ULC)) {
	cmp		al, WTYPE_LEFT
	je		@@TLLeftMask
	cmp		al, WTYPE_ULC
	je		@@TLLeftMask
	jmp		@@TransRight

@@TLLeftMask:
	mov		edx, offset sgLeftMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipTop
	jmp		@@Done

	;	if (al == WTYPE_LRC) {
	cmp		al, WTYPE_LRC
	jne		@@TransRight
	mov		edx, offset sgRightMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipTop
	jmp		@@Done

@@TransRight:
	; if (gbPartialTrans == PART_TRANS_RIGHT) {
	cmp		dl, PART_TRANS_RIGHT
	jne		@@NoTrans

	;al = nWTypeTable[_gnPieceNum];
	mov		ebx, dword ptr [_gnPieceNum]
	mov		al, _nWTypeTable[ebx]
	
	;	if ((al == WTYPE_RIGHT) || (al == WTYPE_ULC)) {
	cmp		al, WTYPE_RIGHT
	je		@@TRRightMask
	cmp		al, WTYPE_ULC
	je		@@TRRightMask
	jmp		@@NoTrans
@@TRRightMask:
	mov		edx, offset sgRightMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipTop
	jmp		@@Done

	;	if (al == WTYPE_LRC) {
	cmp		al, WTYPE_LRC
	jne		@@NoTrans
	mov		edx, offset sgLeftMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipTop
	jmp		@@Done

@@NoTrans:
	;sgT = &_microoffset[0][0];
	mov	edi, offset _microoffset
	mov	dword ptr [sgT], edi

		mov		edi,ecx		; Dest

		mov		eax,dword ptr [_nLVal]
		test	al,al
		jz		@@NoLt
		cmp		al,byte ptr [_lightmax]
		jz		@@Black

		mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jnz		@@Speed

		mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		ebx,dword ptr [_nLVal]			; Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [_pLightTbl]

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		shr		eax,12
		and		eax,0fh
		jz		@@Type0							; Solid 32x32 block
		cmp		ax,1
		jz		@@Type1							; 32x32 block with '0' holes
		cmp		ax,2
		jz		@@Type2							; Left Triangle
		cmp		ax,3
		jz		@@Type3							; Right Triangle
		cmp		ax,4
		jz		@@Type4							; Left Triangle to wall
		jmp		@@Type5							; Right Triangle to wall

@@Speed:
		mov		esi,dword ptr [sgT]
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [_nLVal]
		shl		eax,2
		add		esi,eax							; Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [_pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		shr		eax,12
		and		eax,0fh
@@SCmp:	cmp		ax,8
		jz		@@Type8							; PreTrans Solid 32x32 block
		cmp		ax,9
		jz		@@Type9							; PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		@@TypeA							; PreTrans Left Triangle
		cmp		ax,11
		jz		@@TypeB							; PreTrans Right Triangle
		cmp		ax,12
		jz		@@TypeC							; PreTrans Left Triangle to wall
		jmp		@@TypeD							; PreTrans Right Triangle to wall

@@NoLt:	mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jz		@@RNoLt

		mov		esi,dword ptr [sgT]				; Get old micro tile number
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [_gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [_gdwPNum],eax
		
@@RNoLt:	mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		@@SCmp

@@Black:	mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jz		@@RBlk

		mov		esi,dword ptr [sgT]				; Get old micro tile number
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [_gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [_gdwPNum],eax
		
@@RBlk:	mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		shr		eax,12
		and		eax,07h
		jz		@@TypeG							; Black Trans Solid 32x32 block
		cmp		ax,1
		jz		@@TypeH							; Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		@@TypeI							; Black Trans Left Triangle
		cmp		ax,3
		jz		@@TypeJ							; Black Trans Right Triangle
		cmp		ax,4
		jz		@@TypeK							; Black Trans Left Triangle to wall
		jmp		@@TypeL							; Black Trans Right Triangle to wall

;*-----------------------------------------------------------------------*/

@@Type0:
		mov		edx,32
@@T0Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8*4

		push	edx
		X_DWORDS
		pop		edx

		sub		edi,NBUFFW32
		dec		edx
		jnz		@@T0Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type1:	
		push	ebp
		mov		ebp,32
@@T1Lp1:	
		mov		edx,32
@@T1Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T1J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T1Done
		mov		ecx,eax

		push	edx
		X_BYTES
		pop		edx

		test	edx,edx
		jz		@@T1Nxt
		jmp		@@T1Lp2

@@T1J:	neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@T1Lp2
@@T1Nxt:	
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T1Lp1
@@T1Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type2:
		push	ebp
		mov		ebp,30
@@T2Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T2End
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PREPAD
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		@@T2Lp1

		mov		ebp,2
@@T2Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T2End
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PREPAD
		sub		edi,NBUFFW32
		add		ebp,2
		cmp		ebp,32
		jnz		@@T2Lp3
@@T2End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type3:
		push	ebp
		mov		ebp,30
@@T3Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T3End
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PAD
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T3Lp1

@@T3b:
		mov		ebp,2
@@T3Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T3End
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PAD
		sub		edi,NBUFFW32
		add		edi,ebp
		add		ebp,2
		cmp		ebp,32
		jnz		@@T3Lp3
@@T3End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type4:
		push	ebp
		mov		ebp,30
@@T4Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T4End
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PREPAD
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		@@T4Lp1

@@T4b:
		mov		ebp,16
@@T4Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T4End
		mov		ecx,8*4
		X_DWORDS
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T4Lp3
@@T4End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type5:
		push	ebp
		mov		ebp,30
@@T5Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5End
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PAD
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T5Lp1

@@T5b:
		mov		ebp,16
@@T5Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5End
		mov		ecx,8*4
		X_DWORDS
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T5Lp3
@@T5End:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type8:
		mov		edx,32
@@T8Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@T8Lp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T8Lp2
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@T8Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type9:
		push	ebp
		mov		ebp,32
@@T9Lp1:
		mov		edx,32
@@T9Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T9J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T9Done
		mov		ecx,eax
		shr		ecx,1
		jnc		@@T9w
		mov		al,[esi]
		inc		esi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@T9x
@@T9w:	shr		ecx,1
		jnc		@@T9Lp3
		mov		ax,[esi]
		add		esi,2
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@T9x
@@T9Lp3:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T9Lp3
@@T9x:	test	edx,edx
		jz		@@T9Nxt
		jmp		@@T9Lp2

@@T9J:	neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@T9Lp2
@@T9Nxt:
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T9Lp1
@@T9Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeA:
		mov		edx,30
@@TALp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TALp2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TAx
@@TALp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TALp2
@@TAx:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TALp1

		mov		edx,2
@@TALp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TALp4
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@TAx2
@@TALp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TALp4
@@TAx2:
		sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TALp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeB:
		push	ebp
		mov		edx,30
@@TBLp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TBEnd
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TBcontinue
@@TBLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TBLp2
@@TBcontinue:
		and		ebp,2
		jz		@@TBx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TBx:	
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TBLp1

		mov		edx,2
@@TBLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TBEnd
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TBcontinue2
@@TBLp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TBLp4
@@TBcontinue2:
		and		ebp,2
		jz		@@TBx2
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TBx2:	
		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TBLp3
@@TBEnd:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeC:
		mov		edx,30
@@TCLp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TCLp2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@TCx
@@TCLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TCLp2
@@TCx:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TCLp1

@@TCb:
		mov		edx,16
@@TCLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@TCLp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TCLp4
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TCLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeD:
		push	ebp
		mov		edx,30
@@TDLp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDEnd
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TDcontinue
@@TDLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TDLp2
@@TDcontinue:
		and		ebp,2
		jz		@@TDx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TDx:	
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TDLp1

		mov		edx,16
@@TDLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDEnd
		mov		ecx,8
@@TDLp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TDLp4
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TDLp3
@@TDEnd:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeG:
		mov		edx,32
		xor		eax,eax
@@TGLp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@TGLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TGLp2
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TGLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeH:
		push	ebp
		mov		ebp,32
@@THLp1:
		mov		edx,32
@@THLp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@THJ

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@THDone
		mov		ecx,eax
		add		esi,ecx
		xor		eax,eax
		shr		ecx,1
		jnc		@@THw
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@THx
@@THw:	shr		ecx,1
		jnc		@@THLp3
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@THx
@@THLp3:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@THLp3
@@THx:
		test	edx,edx
		jz		@@THNxt
		jmp		@@THLp2
@@THJ:
		neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@THLp2
@@THNxt:
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@THLp1
@@THDone:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeI:	mov		edx,30
		xor		eax,eax
@@TILp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TILp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TIx
@@TILp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp2
@@TIx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TIb
		sub		edx,2
		jmp		@@TILp1

@@TIb:	mov		edx,2
@@TILp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TILp4
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TIx2
@@TILp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp4
@@TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TILp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeJ:	mov		edx,30
		xor		eax,eax
@@TJLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TJx
@@TJLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp2
@@TJx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TJb
		add		edi,edx
		sub		edx,2
		jmp		@@TJLp1

@@TJb:	mov		edx,2
@@TJLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp4
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TJx2
@@TJLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp4
@@TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TJLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeK:	mov		edx,30
		xor		eax,eax
@@TKLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TKLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TKx
@@TKLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TKLp2
@@TKx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TKb
		sub		edx,2
		jmp		@@TKLp1

@@TKb:	mov		edx,16
@@TKLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@TKLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TKLp4
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TKLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeL:	mov		edx,30
		xor		eax,eax
@@TLLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TLLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TLx
@@TLLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TLLp2
@@TLx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TLb
		add		edi,edx
		sub		edx,2
		jmp		@@TLLp1

@@TLb:	mov		edx,16
@@TLLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@Done
		mov		ecx,8
@@TLLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TLLp4
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TLLp3
		jmp		@@Done

@@PDone:	
	pop		eax

@@Done:	
	pop		esi
	pop		edi
	pop		edx
	pop		ebx
	ret
@DrawMTileClipTop@4 endp



;***********************************************************
;* static void DrawMTileDitherClipBottom (BYTE *pDecodeTo)
;*		ECX = pDecodeTo
;***********************************************************
DrawMTileDitherClipBottom proc
		push	ebx
		push	edx
		push	edi
		push	esi

		;sgT = &_microoffset[0][0];
		mov	edi, offset _microoffset
		mov	dword ptr [sgT], edi

		mov		edi,ecx		; Dest

		mov		eax,dword ptr [_nLVal]
		test	al,al
		jz		@@NoLt
		cmp		al,byte ptr [_lightmax]
		jz		@@Black

		mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jnz		@@Speed

		mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		ebx,dword ptr [_nLVal];			; Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [_pLightTbl];

		mov		eax,dword ptr [_gdwPNum]		; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		@@Type0							; Solid 32x32 block
		cmp		ax,1
		jz		@@Type1							; 32x32 block with '0' holes
		cmp		ax,2
		jz		@@Type2							; Left Triangle
		cmp		ax,3
		jz		@@Type3							; Right Triangle
		cmp		ax,4
		jz		@@Type4							; Left Triangle to wall
		jmp		@@Type5							; Right Triangle to wall

@@Speed:
		mov		esi,dword ptr [sgT]
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [_nLVal]
		shl		eax,2
		add		esi,eax							; Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [_pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [_gdwPNum]		; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
@@SCmp:	cmp		ax,8
		jz		@@Type8							; PreTrans Solid 32x32 block
		cmp		ax,9
		jz		@@Type9							; PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		@@TypeA							; PreTrans Left Triangle
		cmp		ax,11
		jz		@@TypeB							; PreTrans Right Triangle
		cmp		ax,12
		jz		@@TypeC							; PreTrans Left Triangle to wall
		jmp		@@TypeD							; PreTrans Right Triangle to wall

@@NoLt:	mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jz		@@RNoLt

		mov		esi,dword ptr [sgT]				; Get old micro tile number
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [_gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [_gdwPNum],eax
		
@@RNoLt:
		mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		@@SCmp

@@Black:
		mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jz		@@RBlk

		mov		esi,dword ptr [sgT]				; Get old micro tile number
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [_gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [_gdwPNum],eax
		
@@RBlk:
		mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		@@TypeG							; Black Trans Solid 32x32 block
		cmp		ax,1
		jz		@@TypeH							; Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		@@TypeI							; Black Trans Left Triangle
		cmp		ax,3
		jz		@@TypeJ							; Black Trans Right Triangle
		cmp		ax,4
		jz		@@TypeK							; Black Trans Left Triangle to wall
		jmp		@@TypeL							; Black Trans Right Triangle to wall

;*-----------------------------------------------------------------------*/

@@Type0:
		push	ebp
		mov		ebp,16
@@T0Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T0C1
		add		esi,32
		add		edi,32
		jmp		@@T0C2
@@T0C1:
		mov		ecx,8
		X_DWORD2
@@T0C2:
		sub		edi,NBUFFW32
		cmp		edi,dword ptr [_glClipY]
		jb		@@T0C3
		add		esi,32
		add		edi,32
		jmp		@@T0C4
@@T0C3:
		mov		ecx,8
		X_DWORD1
@@T0C4:
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T0Lp1
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type1:
		push	ebp
		mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32
@@T1Lp1:
		push	ecx
		mov		ebp,32
@@T1Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T1J

		sub		ebp,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T1C1
		add		esi,eax
		add		edi,eax
		jmp		@@T1x
@@T1C1:
		mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		@@T1Od
		X_BYTE1
		jmp @@T1x
@@T1Od:
		X_BYTE2
@@T1x:
		test	ebp,ebp
		jz		@@T1Nxt
		jmp		@@T1Lp2
@@T1J:
		neg		al					; Do jump
		add		edi,eax
		sub		ebp,eax
		jnz		@@T1Lp2
@@T1Nxt:
		pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		@@T1Lp1
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type2:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		ebp,30
		CLIP_BOTTOM @@T2Lp1 @@T2b ebp 0 1
@@T2Lp1:
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T2Od
		X_BYTE1_PREPAD
		jmp		@@T2x
@@T2Od:
		X_BYTE2_PREPAD
@@T2x:
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		@@T2Lp1

@@T2b:
		mov		ebp,2
		CLIP_BOTTOM @@T2Lp4 @@T2Done ebp 1 1
@@T2Lp4:
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T2Od2
		X_BYTE1_PREPAD
		jmp		@@T2x2
@@T2Od2:
		X_BYTE2_PREPAD
@@T2x2:
		sub		edi,NBUFFW32
		add		ebp,2
		cmp		ebp,32
		jnz		@@T2Lp4
@@T2Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type3:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		ebp,30
		CLIP_BOTTOM @@T3Lp1 @@T3b ebp 0 1
@@T3Lp1:
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T3Od
		X_BYTE1_PAD
		jmp @@T3x
@@T3Od:
		X_BYTE2_PAD
@@T3x:
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T3Lp1

@@T3b:
		mov		ebp,2
		CLIP_BOTTOM @@T3Lp4 @@T3Done ebp 1 1
@@T3Lp4:
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T3Od2
		X_BYTE1_PAD
		jmp @@T3x2
@@T3Od2:
		X_BYTE2_PAD
@@T3x2:
		sub		edi,NBUFFW32
		add		edi,ebp
		add		ebp,2
		cmp		ebp,32
		jnz		@@T3Lp4
@@T3Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type4:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		ebp,30
		CLIP_BOTTOM @@T4Lp1 @@T4b ebp 0 1
@@T4Lp1:
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T4Od
		X_BYTE1_PREPAD
		jmp @@T4x
@@T4Od:
		X_BYTE2_PREPAD
@@T4x:
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		@@T4Lp1

@@T4b:
		mov		ebp,8
@@T4Lp4:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T4C2
		add		esi,32
		add		edi,32
		jmp		@@T4C3
@@T4C2:
		mov		ecx,8
		X_DWORD2
@@T4C3:
		sub		edi,NBUFFW32
		cmp		edi,dword ptr [_glClipY]
		jb		@@T4C4
		add		esi,32
		add		edi,32
		jmp		@@T4C5
@@T4C4:
		mov		ecx,8
		X_DWORD1
@@T4C5:
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T4Lp4
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type5:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		ebp,30
		CLIP_BOTTOM @@T5Lp1 @@T5b ebp 0 1
@@T5Lp1:
		mov		ecx,32
		sub		ecx,ebp
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@T5Od
		X_BYTE1_PAD
		jmp @@T5x
@@T5Od:
		X_BYTE2_PAD
@@T5x:
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T5Lp1

@@T5b:
		mov		ebp,8
@@T5Lp4:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5C2
		add		esi,32
		add		edi,32
		jmp		@@T5C3
@@T5C2:
		mov		ecx,8
		X_DWORD2
@@T5C3:
		sub		edi,NBUFFW32
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5C4
		add		esi,32
		add		edi,32
		jmp		@@T5C5
@@T5C4:
		mov		ecx,8
		X_DWORD1
@@T5C5:
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T5Lp4
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type8:	mov		edx,16
@@T8Lp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@T8C1
		add		esi,32
		add		edi,32
		jmp		@@T8C2
@@T8C1:	mov		ecx,8
@@T8Lp2:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@T8Lp2
@@T8C2:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [_glClipY]
		jb		@@T8C3
		add		esi,32
		add		edi,32
		jmp		@@T8C4
@@T8C3:	mov		ecx,8
@@T8Lp3:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@T8Lp3
@@T8C4:	sub		edi,NBUFFW32
		dec		edx
		jnz		@@T8Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type9:
		mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32
@@T9Lp1:
		push	ecx
		mov		edx,32
@@T9Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T9J
		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T9C1
		add		esi,eax
		add		edi,eax
		jmp		@@T9x
@@T9C1:
		mov		ecx,eax
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		@@T9Od
		shr		ecx,1
		jnc		@@T9w
		inc		esi
		inc		edi
		test	ecx,ecx
		jz	@@T9x
		jmp		@@T9w2
@@T9w:	shr		ecx,1
		jnc		@@T9Lp3
		inc		esi
		inc		edi
		mov		al,[esi]
		inc		esi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@T9x
@@T9Lp3:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@T9Lp3
		jmp		@@T9x

@@T9Od:	shr		ecx,1
		jnc		@@T9w2
		mov		al,[esi]
		inc		esi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@T9x
		jmp		@@T9w
@@T9w2:	shr		ecx,1
		jnc		@@T9Lp4
		mov		al,[esi]
		add		esi,2
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@T9x
@@T9Lp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@T9Lp4

@@T9x:	test	edx,edx
		jz		@@T9Nxt
		jmp		@@T9Lp2

@@T9J:	neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@T9Lp2
@@T9Nxt:
		pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		@@T9Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeA:	
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
		CLIP_BOTTOM @@TALp1 @@TAb edx 0 1
@@TALp1:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TAOd
		shr		ecx,2
		jnc		@@TALp2
		mov		ax,[esi+2]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz		@@TAx
@@TALp2:	
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TALp2
		jmp		@@TAx
@@TAOd:	
		shr		ecx,2
		jnc		@@TALp3
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz		@@TAx
@@TALp3:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TALp3
@@TAx:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TALp1

@@TAb:
		mov		edx,2
		CLIP_BOTTOM @@TALp4 @@Done edx 1 1
@@TALp4:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TAOd2
		shr		ecx,2
		jnc		@@TALp5
		mov		ax,[esi+2]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz		@@TAx2
@@TALp5:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TALp5
		jmp		@@TAx2
@@TAOd2:
		shr		ecx,2
		jnc		@@TALp6
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],al
		add		edi,2
		dec		ecx
		jz		@@TAx2
@@TALp6:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TALp6
@@TAx2:
		sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TALp4
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeB:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
		CLIP_BOTTOM @@TBLp1 @@TBb edx 0 1
@@TBLp1:
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		ebp,ecx
		mov		dword ptr [sgLineVal],eax
		jz		@@TBOd
		shr		ecx,2
		jz		@@TBcontinue
@@TBLp2:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TBLp2
@@TBcontinue:
		and		ebp,2
		jz		@@TBx
		mov		ax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		jmp		@@TBx
@@TBOd:	
		shr		ecx,2
		jz		@@TBcontinue2
@@TBLp3:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TBLp3
@@TBcontinue2:
		and		ebp,2
		jz		@@TBx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
@@TBx:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TBLp1


@@TBb:
		mov		edx,2
		CLIP_BOTTOM @@TBLp4 @@TBDone edx 1 1
@@TBLp4:
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		ebp,ecx
		mov		dword ptr [sgLineVal],eax
		jz		@@TBOd2
		shr		ecx,2
		jz		@@TBcontinue3
@@TBLp5:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TBLp5
@@TBcontinue3:
		and		ebp,2
		jz		@@TBx2
		mov		ax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		jmp		@@TBx2
@@TBOd2:
		shr		ecx,2
		jz		@@TBcontinue4
@@TBLp6:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TBLp6
@@TBcontinue4:
		and		ebp,2
		jz		@@TBx2
		mov		ax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
@@TBx2:
		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TBLp4
@@TBDone:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeC:	
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
		CLIP_BOTTOM @@TCLp1 @@TCb edx 0 1
@@TCLp1:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TCOd
		shr		ecx,2
		jnc		@@TCLp2
		mov		ax,[esi+2]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz		@@TCx
@@TCLp2:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TCLp2
		jmp		@@TCx

@@TCOd:	shr		ecx,2
		jnc		@@TCLp3
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],al
		test	ecx,ecx
		lea		edi,[edi+2]
		jz		@@TCx
@@TCLp3:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		dec		ecx
		lea		edi,[edi+2]
		jnz		@@TCLp3

@@TCx:	sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TCLp1

@@TCb:	
		mov		edx,8
@@TCLp4:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TCC2
		add		esi,32
		add		edi,32
		jmp		@@TCC3
@@TCC2:
		mov		ecx,8
@@TCLp5:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TCLp5
@@TCC3:
		sub		edi,NBUFFW32
		cmp		edi,dword ptr [_glClipY]
		jb		@@TCC4
		add		esi,32
		add		edi,32
		jmp		@@TCC5
@@TCC4:
		mov		ecx,8
@@TCLp6:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TCLp6
@@TCC5:
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TCLp4
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeD:
		push	ebp
		xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
		CLIP_BOTTOM @@TDLp1 @@TDb edx 0 1
@@TDLp1:	
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		ebp,ecx
		mov		dword ptr [sgLineVal],eax
		jz		@@TDOd
		shr		ecx,2
		jz		@@TDcontinue
@@TDLp2:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TDLp2
@@TDcontinue:
		and		ebp,2
		jz		@@TDx
		mov		ax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		inc		edi
		jmp		@@TDx
@@TDOd:	
		shr		ecx,2
		jz		@@TDcontinue2
@@TDLp3:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TDLp3
@@TDcontinue2:
		and		ebp,2
		jz		@@TDx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
@@TDx:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TDLp1


@@TDb:
		mov		edx,8
@@TDLp4:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDC2
		add		esi,32
		add		edi,32
		jmp		@@TDC3
@@TDC2:
		mov		ecx,8
@@TDLp5:
		mov		eax,[esi]
		add		esi,4
		inc		edi
		ror		eax,8
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		inc		edi
		dec		ecx
		jnz		@@TDLp5
@@TDC3:
		sub		edi,NBUFFW32
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDC4
		add		esi,32
		add		edi,32
		jmp		@@TDC5
@@TDC4:
		mov		ecx,8
@@TDLp6:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],al
		add		edi,2
		ror		eax,16
		mov		[edi],al
		add		edi,2
		dec		ecx
		jnz		@@TDLp6
@@TDC5:
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TDLp4
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeG:	mov		edx,16
		xor		eax,eax
@@TGLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TGC1
		add		esi,32
		add		edi,32
		jmp		@@TGC2
@@TGC1:	mov		ecx,8
@@TGLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TGLp2
@@TGC2:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [_glClipY]
		jb		@@TGC3
		add		esi,32
		add		edi,32
		jmp		@@TGC4
@@TGC3:	mov		ecx,8
@@TGLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TGLp3
@@TGC4:	sub		edi,NBUFFW32
		dec		edx
		jnz		@@TGLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeH:	mov		eax,edi
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		mov		ecx,32

@@THLp1:	push	ecx
		mov		edx,32

@@THLp2:	xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@THJ

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@THC1
		add		esi,eax
		add		edi,eax
		jmp		@@THx
@@THC1:	mov		ecx,eax
		add		esi,ecx
		mov		eax,edi
		and		eax,1
		cmp		eax,dword ptr [sgLineVal]
		jnz		@@THOd
		xor		eax,eax
		shr		ecx,1
		jnc		@@THw
		inc		edi
		test	ecx,ecx
		jz	@@THx
		jmp		@@THw2
@@THw:	shr		ecx,1
		jnc		@@THLp3
		inc		edi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@THx
@@THLp3:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@THLp3
		jmp		@@THx

@@THOd:	xor		eax,eax
		shr		ecx,1
		jnc		@@THw2
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@THx
		jmp		@@THw
@@THw2:	shr		ecx,1
		jnc		@@THLp4
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@THx
@@THLp4:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@THLp4

@@THx:	test	edx,edx
		jz		@@THNxt
		jmp		@@THLp2

@@THJ:	neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@THLp2
@@THNxt:	pop		ecx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		sub		edi,NBUFFW32
		dec		ecx
		jnz		@@THLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeI:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TILp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TIC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TIx
@@TIC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TIOd
		xor		eax,eax
		shr		ecx,2
		jnc		@@TILp2
		inc		edi
		mov		[edi],al
		inc		edi
				test	ecx,ecx
		jz	@@TIx
@@TILp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TILp2
		jmp		@@TIx

@@TIOd:	xor		eax,eax
		shr		ecx,2
		jnc		@@TILp3
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@TIx
@@TILp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TILp3

@@TIx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TIb
		sub		edx,2
		jmp		@@TILp1

@@TIb:	mov		edx,2
@@TILp4:	cmp		edi,dword ptr [_glClipY]
		jb		@@TIC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TIx2
@@TIC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TIOd2
		xor		eax,eax
		shr		ecx,2
		jnc		@@TILp5
		inc		edi
		mov		[edi],al
		inc		edi
				test	ecx,ecx
		jz	@@TIx2
@@TILp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TILp5
		jmp		@@TIx2

@@TIOd2:	xor		eax,eax
		shr		ecx,2
		jnc		@@TILp6
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@TIx2
@@TILp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TILp6

@@TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TILp4
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeJ:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TJLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TJC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TJx
@@TJC1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TJOd
		xor		eax,eax
		shr		ecx,2
		jnc		@@TJLp2
		inc		edi
		mov		[edi],al
		inc		edi
				test	ecx,ecx
		jz	@@TJx
@@TJLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TJLp2
		jmp		@@TJx

@@TJOd:	xor		eax,eax
		shr		ecx,2
		jnc		@@TJLp3
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@TJx
@@TJLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TJLp3

@@TJx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TJb
		add		edi,edx
		sub		edx,2
		jmp		@@TJLp1

@@TJb:	mov		edx,2
@@TJLp4:	cmp		edi,dword ptr [_glClipY]
		jb		@@TJC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TJx2
@@TJC2:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TJOd2
		xor		eax,eax
		shr		ecx,2
		jnc		@@TJLp5
		inc		edi
		mov		[edi],al
		inc		edi
				test	ecx,ecx
		jz	@@TJx2
@@TJLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TJLp5
		jmp		@@TJx2

@@TJOd2:	xor		eax,eax
		shr		ecx,2
		jnc		@@TJLp6
		mov		[edi],al
		add		edi,2
				test	ecx,ecx
		jz	@@TJx2
@@TJLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TJLp6

@@TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TJLp4
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeK:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TKLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TKC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TKx
@@TKC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TKOd
		xor		eax,eax
		shr		ecx,2
		jnc		@@TKLp2
		inc		edi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@TKx
@@TKLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TKLp2
		jmp		@@TKx

@@TKOd:	xor		eax,eax
		shr		ecx,2
		jnc		@@TKLp3
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@TKx
@@TKLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TKLp3

@@TKx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TKb
		sub		edx,2
		jmp		@@TKLp1

@@TKb:	mov		edx,8
@@TKLp4:	cmp		edi,dword ptr [_glClipY]
		jb		@@TKC2
		add		esi,32
		add		edi,32
		jmp		@@TKC3
@@TKC2:	mov		ecx,8
		xor		eax,eax
@@TKLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TKLp5
@@TKC3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [_glClipY]
		jb		@@TKC4
		add		esi,32
		add		edi,32
		jmp		@@TKC5
@@TKC4:	mov		ecx,8
		xor		eax,eax
@@TKLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TKLp6
@@TKC5:	sub		edi,NBUFFW32
		dec		edx
		jnz		@@TKLp4
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeL:	xor		eax,eax
		mov		dword ptr [sgLineVal],eax
		mov		edx,30
@@TLLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TLC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TLx
@@TLC1:	mov		ecx,32
		sub		ecx,edx
		mov		eax,dword ptr [sgLineVal]
		inc		eax
		and		eax,1
		mov		dword ptr [sgLineVal],eax
		jz		@@TLOd
		xor		eax,eax
		shr		ecx,2
		jnc		@@TLLp2
		inc		edi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@TLx
@@TLLp2:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TLLp2
		jmp		@@TLx

@@TLOd:	xor		eax,eax
		shr		ecx,2
		jnc		@@TLLp3
		mov		[edi],al
		add		edi,2
		test	ecx,ecx
		jz	@@TLx
@@TLLp3:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TLLp3

@@TLx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TLb
		add		edi,edx
		sub		edx,2
		jmp		@@TLLp1

@@TLb:	mov		edx,8
@@TLLp4:	cmp		edi,dword ptr [_glClipY]
		jb		@@TLC2
		add		esi,32
		add		edi,32
		jmp		@@TLC3
@@TLC2:	mov		ecx,8
		xor		eax,eax
@@TLLp5:	mov		byte ptr [edi+1],al
		mov		byte ptr [edi+3],al
		add		edi,4
		dec		ecx
		jnz		@@TLLp5
@@TLC3:	sub		edi,NBUFFW32

		cmp		edi,dword ptr [_glClipY]
		jb		@@TLC4
		add		esi,32
		add		edi,32
		jmp		@@TLC5
@@TLC4:	mov		ecx,8
		xor		eax,eax
@@TLLp6:	mov		byte ptr [edi],al
		mov		byte ptr [edi+2],al
		add		edi,4
		dec		ecx
		jnz		@@TLLp6
@@TLC5:	sub		edi,NBUFFW32
		dec		edx
		jnz		@@TLLp4

@@Done:
	pop		esi
	pop		edi
	pop		edx
	pop		ebx
	ret
DrawMTileDitherClipBottom endp



;***********************************************************
;* static void DrawMTileHalfDitherClipBottom (BYTE *pDecodeTo, long *mask)
;*		ECX = pDecodeTo
;***********************************************************
DrawMTileHalfDitherClipBottom proc
	push	ebx
	push	edi
	push	esi

	;sgT = &_microoffset[0][0];
	mov	edi, offset _microoffset
	mov	dword ptr [sgT], edi

	mov		edi,ecx			; Dest
	mov		dword ptr [sgMask], edx

		mov		eax,dword ptr [_nLVal]
		test	al,al
		jz		@@NoLt
		cmp		al,byte ptr [_lightmax]
		jz		@@Black

		mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jnz		@@Speed

		mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		ebx,dword ptr [_nLVal];			; Light conversion table
		shl		ebx,8
		add		ebx,dword ptr [_pLightTbl];

		mov		eax,dword ptr [_gdwPNum]		; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
		jz		@@Type0							; Solid 32x32 block
		cmp		ax,1
		jz		@@Type1							; 32x32 block with '0' holes
		cmp		ax,2
		jz		@@Type2							; Left Triangle
		cmp		ax,3
		jz		@@Type3							; Right Triangle
		cmp		ax,4
		jz		@@Type4							; Left Triangle to wall
		jmp		@@Type5							; Right Triangle to wall
@@Speed:
		mov		esi,dword ptr [sgT]
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,4
		add		eax,dword ptr [_nLVal]
		shl		eax,2
		add		esi,eax							; Source
		mov		eax,dword ptr [esi]
		mov		esi,dword ptr [_pSpeedCels]
		add		esi,eax

		mov		eax,dword ptr [_gdwPNum]		; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,0fh
@@SCmp:	cmp		ax,8
		jz		@@Type8							; PreTrans Solid 32x32 block
		cmp		ax,9
		jz		@@Type9							; PreTrans 32x32 block with '0' holes
		cmp		ax,10
		jz		@@TypeA							; PreTrans Left Triangle
		cmp		ax,11
		jz		@@TypeB							; PreTrans Right Triangle
		cmp		ax,12
		jz		@@TypeC							; PreTrans Left Triangle to wall
		jmp		@@TypeD							; PreTrans Right Triangle to wall

@@NoLt:	mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jz		@@RNoLt

		mov		esi,dword ptr [sgT]				; Get old micro tile number
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [_gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [_gdwPNum],eax
		
@@RNoLt:	mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		add		eax,8
		jmp		@@SCmp

@@Black:	mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jz		@@RBlk

		mov		esi,dword ptr [sgT]				; Get old micro tile number
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [_gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [_gdwPNum],eax
		
@@RBlk:	mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		@@TypeG							; Black Trans Solid 32x32 block
		cmp		ax,1
		jz		@@TypeH							; Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		@@TypeI							; Black Trans Left Triangle
		cmp		ax,3
		jz		@@TypeJ							; Black Trans Right Triangle
		cmp		ax,4
		jz		@@TypeK							; Black Trans Left Triangle to wall
		jmp		@@TypeL							; Black Trans Right Triangle to wall

;*-----------------------------------------------------------------------*/

@@Type0:
		push	ebp
		mov		ebp,32
@@T0Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T0C1
		add		esi,32
		add		edi,32
		jmp		@@T0C2
@@T0C1:
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@T0Lp2:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T0S1
		xlatb
		mov		byte ptr [edi],al
@@T0S1:
		inc		edi
		dec		ecx
		jnz		@@T0Lp2
@@T0C2:
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		ebp
		jnz		@@T0Lp1
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type1:
		mov		ecx,32
@@T1Lp1:
		push	ecx
		mov		eax,dword ptr [sgMask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [sgCM],eax
		mov		edx,32
@@T1Lp2:
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T1J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T1C1
		add		esi,eax
		add		edi,eax
		jmp		@@T1x
@@T1C1:	mov		ecx,eax
		push	edx
		mov		edx,dword ptr [sgCM]
@@T1Lp3:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T1S1
		xlatb
		mov		byte ptr [edi],al
@@T1S1:	inc		edi
		dec		ecx
		jnz		@@T1Lp3
		mov		dword ptr [sgCM],edx
		pop		edx
@@T1x:	test	edx,edx
		jz		@@T1Nxt
		jmp		@@T1Lp2

@@T1J:	neg		al					; Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		@@T1S2
		push	eax
		mov		eax,dword ptr [sgCM]
		shl		eax,cl
		mov		dword ptr [sgCM],eax
		pop		eax
@@T1S2:	sub		edx,eax
		jnz		@@T1Lp2
@@T1Nxt:
		pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		ecx
		jnz		@@T1Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type2:
		mov		edx,30
		CLIP_BOTTOM @@T2Lp1 @@T2b edx 0 0
@@T2Lp1:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@T2Lp2
		mov		ax,[esi+2]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@T2x
@@T2Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T2Lp2
@@T2x:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@T2Lp1

@@T2b:
		mov		edx,2
		CLIP_BOTTOM @@T2Lp3 @@Done edx 1 0
@@T2Lp3:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@T2Lp4
		mov		ax,[esi+2]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@T2x2
@@T2Lp4:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T2Lp4
@@T2x2:	
		sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@T2Lp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type3:
		push	ebp
		mov		edx,30
		CLIP_BOTTOM @@T3Lp1 @@T3b edx 0 0
@@T3Lp1:
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@T3continue
@@T3Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T3Lp2
@@T3continue:
		and		ebp,2
		jz		@@T3x
		mov		ax,[esi]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
@@T3x:	
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@T3Lp1

@@T3b:
		mov		edx,2
		CLIP_BOTTOM @@T3Lp3 @@T3Done edx 1 0
@@T3Lp3:
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@T3continue2
@@T3Lp4:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T3Lp4
@@T3continue2:
		and		ebp,2
		jz		@@T3x2
		mov		ax,[esi]
		add		esi,2
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
@@T3x2:
		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@T3Lp3
@@T3Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type4:
		mov		edx,30
		CLIP_BOTTOM @@T4Lp1 @@T4b edx 0 0
@@T4Lp1:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@T4Lp2
		mov		ax,[esi+2]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@T4x
@@T4Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T4Lp2
@@T4x:
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@T4Lp1


@@T4b:
		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@T4Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T4C2
		add		esi,32
		add		edi,32
		jmp		@@T4C3
@@T4C2:
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@T4Lp4:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T4S1
		xlatb
		mov		[edi],al
@@T4S1:
		inc		edi
		dec		ecx
		jnz		@@T4Lp4
		pop		edx
@@T4C3:
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@T4Lp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type5:
		push	ebp
		mov		edx,30
		CLIP_BOTTOM @@T5Lp1 @@T5b edx 0 0
@@T5Lp1:
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@T5continue
@@T5Lp2:
		mov		eax,[esi]
		add		esi,4
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T5Lp2
@@T5continue:
		and		ebp,2
		jz		@@T5x
		mov		ax,[esi]
		add		esi,4
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		mov		[edi],ax
		add		edi,2
@@T5x:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@T5Lp1


@@T5b:
		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@T5Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5C2
		add		esi,32
		add		edi,32
		jmp		@@T5C3
@@T5C2:
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@T5Lp4:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T5S1
		xlatb
		mov		byte ptr [edi],al
@@T5S1:
		inc		edi
		dec		ecx
		jnz		@@T5Lp4
		mov		ebp,esi
		and		ebp,2
		add		esi,ebp
		pop		edx
@@T5C3:
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@T5Lp3
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type8:	mov		edx,32
@@T8Lp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@T8C1
		add		esi,32
		add		edi,32
		jmp		@@T8C2
@@T8C1:	push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@T8Lp2:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T8S1
		mov		byte ptr [edi],al
@@T8S1:	inc		edi
		dec		ecx
		jnz		@@T8Lp2
		pop		edx
@@T8C2:	sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@T8Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type9:	mov		ecx,32

@@T9Lp1:	push	ecx
		mov		eax,dword ptr [sgMask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [sgCM],eax

		mov		edx,32

@@T9Lp2:	xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T9J

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T9C1
		add		esi,eax
		add		edi,eax
		jmp		@@T9x
@@T9C1:	mov		ecx,eax
		push	edx
		mov		edx,dword ptr [sgCM]
@@T9Lp3:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@T9S1
		mov		byte ptr [edi],al
@@T9S1:	inc		edi
		dec		ecx
		jnz		@@T9Lp3
		mov		dword ptr [sgCM],edx
		pop		edx
@@T9x:	test	edx,edx
		jz		@@T9Nxt
		jmp		@@T9Lp2

@@T9J:	neg		al					; Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		@@T9S2
		mov		ebx,dword ptr [sgCM]
		shl		ebx,cl
		mov		dword ptr [sgCM],ebx
@@T9S2:	sub		edx,eax
		jnz		@@T9Lp2
@@T9Nxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		ecx
		jnz		@@T9Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeA:
		mov		edx,30
		CLIP_BOTTOM @@TALp1 @@TAb edx 0 0
@@TALp1:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TALp2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@TAx
@@TALp2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TALp2
@@TAx:	
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TALp1

@@TAb:
		mov		edx,2
		CLIP_BOTTOM @@TALp3 @@Done edx 1 0
@@TALp3:	
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TALp4
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TAx2
@@TALp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TALp4
@@TAx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TALp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeB:
		push	ebp
		mov		edx,30
 		CLIP_BOTTOM @@TBLp1 @@TBb edx 0 0
@@TBLp1:
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TBcontinue
@@TBLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TBLp2
@@TBcontinue:
		and		ebp,2
		jz		@@TBx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TBx:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TBLp1

@@TBb:
		mov		edx,2
 		CLIP_BOTTOM @@TBLp3 @@TBDone edx 1 0
@@TBLp3:
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TBcontinue2
@@TBLp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TBLp4
@@TBcontinue2:
		and		ebp,2
		jz		@@TBx2
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TBx2:
		sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TBLp3
@@TBDone:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeC:	
		mov		edx,30
 		CLIP_BOTTOM @@TCLp1 @@TCb edx 0 0
@@TCLp1:	
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TCLp2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@TCx
@@TCLp2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TCLp2
@@TCx:	
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TCLp1


@@TCb:	
		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TCLp3:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TCC2
		add		esi,32
		add		edi,32
		jmp		@@TCC3
@@TCC2:	push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@TCLp4:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@TCS1
		mov		[edi],al
@@TCS1:
		inc		edi
		dec		ecx
		jnz		@@TCLp4
		pop		edx
@@TCC3:
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TCLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeD:
		push	ebp
		mov		edx,30
 		CLIP_BOTTOM @@TDLp1 @@TDb edx 0 0
@@TDLp1:
		mov		ecx,32
		sub		ecx,edx
		mov		ebp,ecx
		shr		ecx,2
		jz		@@TDcontinue
@@TDLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TDLp2
@@TDcontinue:
		and		ebp,2
		jz		@@TDx
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TDx:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TDLp1


@@TDb:
		mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TDLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDC2
		add		esi,32
		add		edi,32
		jmp		@@TDC3
@@TDC2:
		push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		mov		ecx,32
@@TDLp4:
		mov		al,[esi]
		inc		esi
		shl		edx,1
		jnc		@@TDS1
		mov		byte ptr [edi],al
@@TDS1:
		inc		edi
		dec		ecx
		jnz		@@TDLp4
		mov		ebp,esi
		and		ebp,2
		add		esi,ebp
		pop		edx
@@TDC3:
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TDLp3
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeG:	mov		edx,32
@@TGLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TGC1
		add		esi,32
		add		edi,32
		jmp		@@TGC2
@@TGC1:	push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
@@TGLp2:	shl		edx,1
		jnc		@@TGS1
		mov		byte ptr [edi],al
@@TGS1:	inc		edi
		dec		ecx
		jnz		@@TGLp2
		pop		edx
@@TGC2:	sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TGLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeH:	mov		ecx,32

@@THLp1:	push	ecx
		mov		eax,dword ptr [sgMask]
		mov		eax,dword ptr [eax]
		mov		dword ptr [sgCM],eax

		mov		edx,32

@@THLp2:	xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@THJ

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@THC1
		add		esi,eax
		add		edi,eax
		jmp		@@THx
@@THC1:	mov		ecx,eax
		add		esi,ecx
		push	edx
		mov		edx,dword ptr [sgCM]
		xor		eax,eax
@@THLp3:	shl		edx,1
		jnc		@@THS1
		mov		byte ptr [edi],al
@@THS1:	inc		edi
		dec		ecx
		jnz		@@THLp3
		mov		dword ptr [sgCM],edx
		pop		edx
@@THx:	test	edx,edx
		jz		@@THNxt
		jmp		@@THLp2

@@THJ:	neg		al					; Do jump
		add		edi,eax
		mov		ecx,eax
		and		ecx,01fh
		jz		@@THS2
		mov		ebx,dword ptr [sgCM]
		shl		ebx,cl
		mov		dword ptr [sgCM],ebx
@@THS2:	sub		edx,eax
		jnz		@@THLp2
@@THNxt:	pop		ecx
		sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		ecx
		jnz		@@THLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeI:	mov		edx,30
		xor		eax,eax
@@TILp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TIC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TIx
@@TIC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TILp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TIx
@@TILp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp2
@@TIx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TIb
		sub		edx,2
		jmp		@@TILp1

@@TIb:	mov		edx,2
@@TILp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TIC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TIx2
@@TIC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TILp4
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TIx2
@@TILp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp4
@@TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TILp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeJ:	mov		edx,30
		xor		eax,eax
@@TJLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TJC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TJx
@@TJC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TJx
@@TJLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp2
@@TJx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TJb
		add		edi,edx
		sub		edx,2
		jmp		@@TJLp1

@@TJb:	mov		edx,2
@@TJLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TJC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TJx2
@@TJC2:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp4
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TJx2
@@TJLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp4
@@TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TJLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeK:	mov		edx,30
		xor		eax,eax
@@TKLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TKC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TKx
@@TKC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TKLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TKx
@@TKLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TKLp2
@@TKx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TKb
		sub		edx,2
		jmp		@@TKLp1

@@TKb:	mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TKLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TKC2
		add		esi,32
		add		edi,32
		jmp		@@TKC3
@@TKC2:	push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
@@TKLp4:	shl		edx,1
		jnc		@@TKS1
		mov		byte ptr [edi],al
@@TKS1:	inc		edi
		dec		ecx
		jnz		@@TKLp4
		pop		edx
@@TKC3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TKLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeL:	mov		edx,30
		xor		eax,eax
@@TLLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TLC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TLx
@@TLC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TLLp2
		mov		[edi],ax
		add		edi,2
				test	ecx,ecx
		jz	@@TLx
@@TLLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TLLp2
@@TLx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TLb
		add		edi,edx
		sub		edx,2
		jmp		@@TLLp1

@@TLb:	mov		eax,dword ptr [sgMask]
		sub		eax,64
		mov		dword ptr [sgMask],eax
		mov		edx,16
@@TLLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TLC2
		add		esi,32
		add		edi,32
		jmp		@@TLC3
@@TLC2:	push	edx
		mov		eax,dword ptr [sgMask]
		mov		edx,dword ptr [eax]
		xor		eax,eax
		mov		ecx,32
@@TLLp4:	shl		edx,1
		jnc		@@TLS1
		mov		byte ptr [edi],al
@@TLS1:	inc		edi
		dec		ecx
		jnz		@@TLLp4
		pop		edx
@@TLC3:	sub		edi,NBUFFW32
		mov		eax,dword ptr [sgMask]
		sub		eax,4
		mov		dword ptr [sgMask],eax
		dec		edx
		jnz		@@TLLp3

@@Done:
	pop		esi
	pop		edi
	pop		ebx
	ret
DrawMTileHalfDitherClipBottom endp


;***********************************************************
;* CLIP_BOTTOM_UNROLL(Quit, fTopPiece)
;*   Clips top of triangle piece
;* ESI - Source
;* EDI - Dest
;* EBX - set to 4*(lines to skip)
;* EAX - trashed
;* if fLeftPiece
;*		ECX - trashed
;***********************************************************
CLIP_BOTTOM_UNROLL MACRO Start, Quit, fTopPiece
	mov		eax,edi
	sub		eax,dword ptr [_glClipY]
	jle		Start
	shr		eax,8
	if fTopPiece
		cmp		eax,14*3
	else
		cmp		eax,15*3
	endif
	jg		Quit
	mov		ebx,sgDivBy3MulBy4[eax*4]

	;add	esi,(data for lines skipped)
	mov		eax,ebx
	if fTopPiece
		add		esi,TotalDataPerLineTop[ebx]
	else
		add		esi,TotalDataPerLineBottom[ebx]
	endif

	;sub	edi,768*(lines skipped)
	shl		eax,6
	lea		eax,[eax+eax*2]
	sub		edi,eax
ENDM


;***********************************************************
;* UNROLLED_COPY_BOTTOM_RIGHT_TRIANGLE_CLIP
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_BOTTOM_RIGHT_TRIANGLE_CLIP MACRO
	LOCAL l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15,allClipped,Done,CTRTclipping

	CLIP_BOTTOM_UNROLL l0 allClipped 0
	jmp		CTRTclipping[ebx]
	
	CTRTclipping label dword
		dd offset l0,offset l1
		dd offset l2,offset l3
		dd offset l4,offset l5
		dd offset l6,offset l7
		dd offset l8,offset l9
		dd offset l10,offset l11
		dd offset l12,offset l13
		dd offset l14,offset l15

l0:	;2 bytes
	mov		ax,[esi]
	add		esi,4
	mov		[edi],ax
	sub		edi,NBUFFW32-32
l1:	;4 bytes
	mov		eax,[esi]
	add		esi, 4
	mov		[edi],eax
	sub		edi,NBUFFW32-32
l2:	;6 bytes
	mov		bx,[esi+4]
	mov		eax,[esi]
	mov		[edi+4],bx
	mov		[edi],eax
	add		esi, 8
	sub		edi,NBUFFW32-32
l3:	;8 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
	sub		edi,NBUFFW32-32
l4:	;10 bytes	
	mov		cx,[esi+8]
	mov		eax,[esi]
	mov		[edi+8],cx
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 12
	sub		edi,NBUFFW32-32
l5:	;12 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
	sub		edi,NBUFFW32-32
l6:	;14 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		cx,[esi+12]
	mov		eax,[esi+8]
	mov		[edi+12],cx
	mov		[edi+8],eax
	add		esi, 16
	sub		edi,NBUFFW32-32
l7:	;16 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
	sub		edi,NBUFFW32-32
l8:	;18 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+16]
	add		esi, 20
	mov		[edi+16],cx
	sub		edi,NBUFFW32-32
l9:	;20 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	add		esi, 20
	mov		[edi+16],eax
	sub		edi,NBUFFW32-32
l10:	;22 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+20]
	mov		eax,[esi+16]
	mov		[edi+20],cx
	mov		[edi+16],eax
	add		esi, 24
	sub		edi,NBUFFW32-32
l11:	;24 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
	sub		edi,NBUFFW32-32
l12:	;26 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+24]
	add		esi, 28
	mov		[edi+24],cx
	sub		edi,NBUFFW32-32
l13:	;28 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
	sub		edi,NBUFFW32-32
l14:	;30 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+28]
	mov		eax,[esi+24]
	mov		[edi+28],cx
	mov		[edi+24],eax
	add		esi, 32
	sub		edi,NBUFFW32-32
l15:	;32 bytes
	IRP bOffset,0,8,16,24
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 32
	sub		edi,NBUFFW32-32
	jmp		Done
allClipped:
	sub		edi,768*16
	add		esi,288		;TotalDataPerLine[15]
Done:
ENDM


;***********************************************************
;* UNROLLED_COPY_BOTTOM_RIGHT_TRIANGLE
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_BOTTOM_RIGHT_TRIANGLE MACRO
;2 bytes
	mov		ax,[esi]
	add		esi,4
	mov		[edi],ax
	sub		edi,NBUFFW32-32
;4 bytes
	mov		eax,[esi]
	add		esi, 4
	mov		[edi],eax
	sub		edi,NBUFFW32-32
;6 bytes
	mov		bx,[esi+4]
	mov		eax,[esi]
	mov		[edi+4],bx
	mov		[edi],eax
	add		esi, 8
	sub		edi,NBUFFW32-32
;8 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
	sub		edi,NBUFFW32-32
;10 bytes	
	mov		cx,[esi+8]
	mov		eax,[esi]
	mov		[edi+8],cx
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 12
	sub		edi,NBUFFW32-32
;12 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
	sub		edi,NBUFFW32-32
;14 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		cx,[esi+12]
	mov		eax,[esi+8]
	mov		[edi+12],cx
	mov		[edi+8],eax
	add		esi, 16
	sub		edi,NBUFFW32-32
;16 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
	sub		edi,NBUFFW32-32
;18 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+16]
	add		esi, 20
	mov		[edi+16],cx
	sub		edi,NBUFFW32-32
;20 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	add		esi, 20
	mov		[edi+16],eax
	sub		edi,NBUFFW32-32
;22 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+20]
	mov		eax,[esi+16]
	mov		[edi+20],cx
	mov		[edi+16],eax
	add		esi, 24
	sub		edi,NBUFFW32-32
;24 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
	sub		edi,NBUFFW32-32
;26 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+24]
	add		esi, 28
	mov		[edi+24],cx
	sub		edi,NBUFFW32-32
;28 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
	sub		edi,NBUFFW32-32
;30 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+28]
	mov		eax,[esi+24]
	mov		[edi+28],cx
	mov		[edi+24],eax
	add		esi, 32
	sub		edi,NBUFFW32-32
;32 bytes
	IRP bOffset,0,8,16,24
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 32
	sub		edi,NBUFFW32-32
ENDM


;***********************************************************
;* COPY_BOTTOM_RIGHT_TRIANGLE
;*	copies the bottom half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,EBX,ECX,EDX - trashed
;***********************************************************
COPY_BOTTOM_RIGHT_TRIANGLE_CLIP MACRO
		LOCAL Loop1,Loop2,Continue,Skip,Done

		mov		edx,30
		CLIP_BOTTOM Loop1 Done edx 0 0
Loop1:	
		mov		ecx,32
		sub		ecx,edx
		mov		ebx,ecx
		shr		ecx,2
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:
		and		ebx,2
		jz		Skip
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
Skip:
		add		edi,edx
		sub		edi,NBUFFW32
		sub		edx,2
		jge		Loop1
Done:
ENDM


;***********************************************************
;* COPY_BOTTOM_RIGHT_TRIANGLE
;*	copies the bottom half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,EBX,ECX,EDX - trashed
;***********************************************************
COPY_BOTTOM_RIGHT_TRIANGLE MACRO
		LOCAL Loop1,Loop2,Continue,Skip,Done

		mov		edx,30
Loop1:	
		mov		ecx,32
		sub		ecx,edx
		mov		ebx,ecx
		shr		ecx,2
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:
		and		ebx,2
		jz		Skip
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
Skip:
		add		edi,edx
		sub		edi,NBUFFW32
		sub		edx,2
		jge		Loop1
Done:
ENDM


;***********************************************************
;* UNROLLED_COPY_TOP_RIGHT_TRIANGLE
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_TOP_RIGHT_TRIANGLE MACRO
	LOCAL l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,allClipped,Done,CTRTclipping

	CLIP_BOTTOM_UNROLL l0 allClipped 1
	jmp		CTRTclipping[ebx]
	
	CTRTclipping label dword
		dd offset l0,offset l1
		dd offset l2,offset l3
		dd offset l4,offset l5
		dd offset l6,offset l7
		dd offset l8,offset l9
		dd offset l10,offset l11
		dd offset l12,offset l13
		dd offset l14

l0:	;30 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+28]
	mov		eax,[esi+24]
	mov		[edi+28],cx
	mov		[edi+24],eax
	add		esi, 32
	sub		edi,NBUFFW32-32
l1:	;28 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
	sub		edi,NBUFFW32-32
l2:	;26 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+24]
	add		esi, 28
	mov		[edi+24],cx
	sub		edi,NBUFFW32-32
l3:	;24 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
	sub		edi,NBUFFW32-32
l4:	;22 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+20]
	mov		eax,[esi+16]
	mov		[edi+20],cx
	mov		[edi+16],eax
	add		esi, 24
	sub		edi,NBUFFW32-32
l5:	;20 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	add		esi, 20
	mov		[edi+16],eax
	sub		edi,NBUFFW32-32
l6:	;18 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+16]
	add		esi, 20
	mov		[edi+16],cx
	sub		edi,NBUFFW32-32
l7:	;16 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
	sub		edi,NBUFFW32-32
l8:	;14 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		cx,[esi+12]
	mov		eax,[esi+8]
	mov		[edi+12],cx
	mov		[edi+8],eax
	add		esi, 16
	sub		edi,NBUFFW32-32
l9:	;12 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
	sub		edi,NBUFFW32-32
l10:	;10 bytes	
	mov		cx,[esi+8]
	mov		eax,[esi]
	mov		[edi+8],cx
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 12
	sub		edi,NBUFFW32-32
l11:	;8 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
	sub		edi,NBUFFW32-32
l12:	;6 bytes
	mov		bx,[esi+4]
	mov		eax,[esi]
	mov		[edi+4],bx
	mov		[edi],eax
	add		esi, 8
	sub		edi,NBUFFW32-32
l13:	;4 bytes
	mov		eax,[esi]
	add		esi, 4
	mov		[edi],eax
	sub		edi,NBUFFW32-32
l14:	;2 bytes
	mov		ax,[esi]
	add		esi,4
	mov		[edi],ax
	sub		edi,NBUFFW32-32
	jmp		Done
allClipped:
	sub		edi,768*15
	add		esi,256		;TotalDataPerLine[14]
Done:
ENDM


;***********************************************************
;* UNROLLED_COPY_TOP_RIGHT_TRIANGLE_CLIP
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_TOP_RIGHT_TRIANGLE_CLIP MACRO
;30 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+28]
	mov		eax,[esi+24]
	mov		[edi+28],cx
	mov		[edi+24],eax
	add		esi, 32
	sub		edi,NBUFFW32-32
;28 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
	sub		edi,NBUFFW32-32
;26 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+24]
	add		esi, 28
	mov		[edi+24],cx
	sub		edi,NBUFFW32-32
;24 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
	sub		edi,NBUFFW32-32
;22 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+20]
	mov		eax,[esi+16]
	mov		[edi+20],cx
	mov		[edi+16],eax
	add		esi, 24
	sub		edi,NBUFFW32-32
;20 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	add		esi, 20
	mov		[edi+16],eax
	sub		edi,NBUFFW32-32
;18 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+16]
	add		esi, 20
	mov		[edi+16],cx
	sub		edi,NBUFFW32-32
;16 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
	sub		edi,NBUFFW32-32
;14 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		cx,[esi+12]
	mov		eax,[esi+8]
	mov		[edi+12],cx
	mov		[edi+8],eax
	add		esi, 16
	sub		edi,NBUFFW32-32
;12 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
	sub		edi,NBUFFW32-32
;10 bytes	
	mov		cx,[esi+8]
	mov		eax,[esi]
	mov		[edi+8],cx
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 12
	sub		edi,NBUFFW32-32
;8 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
	sub		edi,NBUFFW32-32
;6 bytes
	mov		bx,[esi+4]
	mov		eax,[esi]
	mov		[edi+4],bx
	mov		[edi],eax
	add		esi, 8
	sub		edi,NBUFFW32-32
;4 bytes
	mov		eax,[esi]
	add		esi, 4
	mov		[edi],eax
	sub		edi,NBUFFW32-32
;2 bytes
	mov		ax,[esi]
	add		esi,4
	mov		[edi],ax
	sub		edi,NBUFFW32-32
ENDM


;***********************************************************
;* COPY_TOP_RIGHT_TRIANGLE_CLIP
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,EBX,ECX,EDX - trashed
;***********************************************************
COPY_TOP_RIGHT_TRIANGLE_CLIP MACRO
		LOCAL Loop1,Loop2,Continue,Skip,Done

		mov		edx,2
		CLIP_BOTTOM Loop1 Done edx 1 0
Loop1:	
		mov		ecx,32
		sub		ecx,edx
		mov		ebx,ecx
		shr		ecx,2
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:	
		and		ebx,2
		jz		Skip
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
Skip:
		add		edi,edx
		add		edx,2
		sub		edi,NBUFFW32
		cmp		edx,32
		jl		Loop1
Done:
ENDM


;***********************************************************
;* COPY_TOP_RIGHT_TRIANGLE
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,EBX,ECX,EDX - trashed
;***********************************************************
COPY_TOP_RIGHT_TRIANGLE MACRO
		LOCAL Loop1,Loop2,Continue,Skip,Done

		mov		edx,2
Loop1:	
		mov		ecx,32
		sub		ecx,edx
		mov		ebx,ecx
		shr		ecx,2
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:	
		and		ebx,2
		jz		Skip
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
Skip:
		add		edi,edx
		add		edx,2
		sub		edi,NBUFFW32
		cmp		edx,32
		jl		Loop1
Done:
ENDM


;***********************************************************
;* UNROLLED_COPY_BOTTOM_LEFT_TRIANGLE_CLIP
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_BOTTOM_LEFT_TRIANGLE_CLIP MACRO
	LOCAL l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15,allClipped,Done,CTRTclipping

	CLIP_BOTTOM_UNROLL l0 allClipped 0
	jmp		CTRTclipping[ebx]
	
	CTRTclipping label dword
		dd offset l0,offset l1
		dd offset l2,offset l3
		dd offset l4,offset l5
		dd offset l6,offset l7
		dd offset l8,offset l9
		dd offset l10,offset l11
		dd offset l12,offset l13
		dd offset l14,offset l15

l0:	;2 bytes
	add		edi,30
	mov		ax,[esi+2]
	add		esi,4
	mov		[edi],ax
	sub		edi,NBUFFW32-2
l1:	;4 bytes
	add		edi,28
	mov		eax,[esi]
	add		esi, 4
	mov		[edi],eax
	sub		edi,NBUFFW32-4
l2:	;6 bytes
	add		edi,26
	mov		bx,[esi+2]
	mov		eax,[esi+4]
	mov		[edi],bx
	mov		[edi+2],eax
	add		esi, 8
	sub		edi,NBUFFW32-6
l3:	;8 bytes
	add		edi,24
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
	sub		edi,NBUFFW32-8
l4:	;10 bytes	
	add		edi,22
	mov		cx,[esi+2]
	mov		eax,[esi+4]
	mov		[edi],cx
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	add		esi, 12
	sub		edi,NBUFFW32-10
l5:	;12 bytes
	add		edi,20
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
	sub		edi,NBUFFW32-12
l6:	;14 bytes
	add		edi,18
	mov		eax,[esi+4]
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	mov		cx,[esi+2]
	mov		eax,[esi+12]
	mov		[edi],cx
	mov		[edi+10],eax
	add		esi, 16
	sub		edi,NBUFFW32-14
l7:	;16 bytes
	add		edi,16
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
	sub		edi,NBUFFW32-16
l8:	;18 bytes
	add		edi,14
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	add		esi, 20
	mov		[edi],cx
	sub		edi,NBUFFW32-18
l9:	;20 bytes
	add		edi,12
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	add		esi, 20
	mov		[edi+16],eax
	sub		edi,NBUFFW32-20
l10:	;22 bytes
	add		edi,10
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+20]
	mov		[edi],cx
	mov		[edi+18],eax
	add		esi, 24
	sub		edi,NBUFFW32-22
l11:	;24 bytes
	add		edi,8
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
	sub		edi,NBUFFW32-24
l12:	;26 bytes
	add		edi,6
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	add		esi, 28
	mov		[edi],cx
	sub		edi,NBUFFW32-26
l13:	;28 bytes
	add		edi,4
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
	sub		edi,NBUFFW32-28
l14:	;30 bytes
	add		edi,2
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+28]
	mov		[edi],cx
	mov		[edi+26],eax
	add		esi, 32
	sub		edi,NBUFFW32-30
l15:	;32 bytes
	IRP bOffset,0,8,16,24
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 32
	sub		edi,NBUFFW32-32
	jmp		Done
allClipped:
	sub		edi,768*16
	add		esi,288		;TotalDataPerLine[15]
Done:
ENDM


;***********************************************************
;* UNROLLED_COPY_BOTTOM_LEFT_TRIANGLE
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_BOTTOM_LEFT_TRIANGLE MACRO
;2 bytes
	add		edi,30
	mov		ax,[esi+2]
	add		esi,4
	mov		[edi],ax
;4 bytes
	mov		eax,[esi]
	sub		edi,NBUFFW32-30
	add		esi, 4
	mov		[edi],eax
;6 bytes
	mov		bx,[esi+2]
	sub		edi,NBUFFW32-30
	mov		eax,[esi+4]
	mov		[edi],bx
	mov		[edi+2],eax
	add		esi, 8
;8 bytes
	sub		edi,NBUFFW32-30
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
;10 bytes	
	sub		edi,NBUFFW32-30
	mov		cx,[esi+2]
	mov		eax,[esi+4]
	mov		[edi],cx
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	add		esi, 12
;12 bytes
	sub		edi,NBUFFW32-30
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
;14 bytes
	mov		eax,[esi+4]
	sub		edi,NBUFFW32-30
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	mov		cx,[esi+2]
	mov		eax,[esi+12]
	mov		[edi],cx
	mov		[edi+10],eax
	add		esi, 16
;16 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
;18 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		[edi],cx
	add		esi, 20
;20 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	mov		[edi+16],eax
	add		esi, 20
;22 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+20]
	mov		[edi],cx
	mov		[edi+18],eax
	add		esi, 24
;24 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
;26 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	add		esi, 28
	mov		[edi],cx
;28 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
;30 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+28]
	mov		[edi],cx
	mov		[edi+26],eax
	add		esi, 32
;32 bytes
	sub		edi,NBUFFW32-30
	IRP bOffset,0,8,16,24
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 32
	sub		edi,NBUFFW32-32
ENDM


;***********************************************************
;* COPY_BOTTOM_LEFT_TRIANGLE_CLIP
;*	copies the bottom half of a 32x32 left-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,ECX,EDX - trashed
;***********************************************************
COPY_BOTTOM_LEFT_TRIANGLE_CLIP MACRO
	LOCAL Loop1,Loop2,Continue,Done

		mov		edx,30
		CLIP_BOTTOM Loop1 Done edx 0 0
Loop1:	
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		Loop2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:	
		sub		edi,NBUFFW32
		sub		edx,2
		jge		Loop1
Done:
ENDM


;***********************************************************
;* COPY_BOTTOM_LEFT_TRIANGLE
;*	copies the bottom half of a 32x32 left-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,ECX,EDX - trashed
;***********************************************************
COPY_BOTTOM_LEFT_TRIANGLE MACRO
	LOCAL Loop1,Loop2,Continue,Done

		mov		edx,30
Loop1:	
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		Loop2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:	
		sub		edi,NBUFFW32
		sub		edx,2
		jge		Loop1
Done:
ENDM


;***********************************************************
;* UNROLLED_COPY_TOP_LEFT_TRIANGLE_CLIP
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_TOP_LEFT_TRIANGLE_CLIP MACRO
	LOCAL l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,allClipped,Done,CTRTclipping

	CLIP_BOTTOM_UNROLL l0 allClipped 1
	jmp		CTRTclipping[ebx]
	
	CTRTclipping label dword
		dd offset l0,offset l1
		dd offset l2,offset l3
		dd offset l4,offset l5
		dd offset l6,offset l7
		dd offset l8,offset l9
		dd offset l10,offset l11
		dd offset l12,offset l13
		dd offset l14

l0:	;30 bytes
	add		edi,2
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+28]
	mov		[edi],cx
	mov		[edi+26],eax
	add		esi, 32
	sub		edi,NBUFFW32-30
l1:	;28 bytes
	add		edi,4
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
	sub		edi,NBUFFW32-28
l2:	;26 bytes
	add		edi,6
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	add		esi, 28
	mov		[edi],cx
	sub		edi,NBUFFW32-26
l3:	;24 bytes
	add		edi,8
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
	sub		edi,NBUFFW32-24
l4:	;22 bytes
	add		edi,10
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+20]
	mov		[edi],cx
	mov		[edi+18],eax
	add		esi, 24
	sub		edi,NBUFFW32-22
l5:	;20 bytes
	add		edi,12
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	add		esi, 20
	mov		[edi+16],eax
	sub		edi,NBUFFW32-20
l6:	;18 bytes
	add		edi,14
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	add		esi, 20
	mov		[edi],cx
	sub		edi,NBUFFW32-18
l7:	;16 bytes
	add		edi,16
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
	sub		edi,NBUFFW32-16
l8:	;14 bytes
	add		edi,18
	mov		eax,[esi+4]
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	mov		cx,[esi+2]
	mov		eax,[esi+12]
	mov		[edi],cx
	mov		[edi+10],eax
	add		esi, 16
	sub		edi,NBUFFW32-14
l9:	;12 bytes
	add		edi,20
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
	sub		edi,NBUFFW32-12
l10:	;10 bytes	
	add		edi,22
	mov		cx,[esi+2]
	mov		eax,[esi+4]
	mov		[edi],cx
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	add		esi, 12
	sub		edi,NBUFFW32-10
l11:	;8 bytes
	add		edi,24
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
	sub		edi,NBUFFW32-8
l12:	;6 bytes
	add		edi,26
	mov		bx,[esi+2]
	mov		eax,[esi+4]
	mov		[edi],bx
	mov		[edi+2],eax
	add		esi, 8
	sub		edi,NBUFFW32-6
l13:	;4 bytes
	add		edi,28
	mov		eax,[esi]
	add		esi, 4
	mov		[edi],eax
	sub		edi,NBUFFW32-4
l14:	;2 bytes
	add		edi,30
	mov		ax,[esi+2]
	add		esi,4
	mov		[edi],ax
	sub		edi,NBUFFW32-2
	jmp		Done
allClipped:
	sub		edi,768*15
	add		esi,256		;TotalDataPerLine[14]
Done:
ENDM


;***********************************************************
;* UNROLLED_COPY_TOP_LEFT_TRIANGLE
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;*  assumes segment is cs
;* ESI - Source
;* EDI - Dest
;* EAX - trashed
;* EBX - trashed
;***********************************************************
UNROLLED_COPY_TOP_LEFT_TRIANGLE MACRO
;30 bytes
	add		edi,2
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+28]
	mov		[edi],cx
	mov		[edi+26],eax
	add		esi, 32
	sub		edi,NBUFFW32-34
;28 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+24]
	add		esi, 28
	mov		[edi+24],eax
	sub		edi,NBUFFW32-34
;26 bytes
	IRP bOffset,2,10,18
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	add		esi, 28
	mov		[edi],cx
	sub		edi,NBUFFW32-34
;24 bytes
	IRP bOffset,0,8,16
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 24
	sub		edi,NBUFFW32-34
;22 bytes
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	mov		eax,[esi+20]
	mov		[edi],cx
	mov		[edi+18],eax
	add		esi, 24
	sub		edi,NBUFFW32-34
;20 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		eax,[esi+16]
	add		esi, 20
	mov		[edi+16],eax
	sub		edi,NBUFFW32-34
;18 bytes
	IRP bOffset,2,10
		mov		eax,[esi+bOffset+2]
		mov		ebx,[esi+bOffset+6]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	mov		cx,[esi+2]
	add		esi, 20
	mov		[edi],cx
	sub		edi,NBUFFW32-34
;16 bytes
	IRP bOffset,0,8
		mov		eax,[esi+bOffset]
		mov		ebx,[esi+bOffset+4]
		mov		[edi+bOffset],eax
		mov		[edi+bOffset+4],ebx
	ENDM
	add		esi, 16
	sub		edi,NBUFFW32-34
;14 bytes
	mov		eax,[esi+4]
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	mov		cx,[esi+2]
	mov		eax,[esi+12]
	mov		[edi],cx
	mov		[edi+10],eax
	add		esi, 16
	sub		edi,NBUFFW32-34
;12 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	mov		eax,[esi+8]
	add		esi, 12
	mov		[edi+8],eax
	sub		edi,NBUFFW32-34
;10 bytes	
	mov		cx,[esi+2]
	mov		eax,[esi+4]
	mov		[edi],cx
	mov		ebx,[esi+8]
	mov		[edi+2],eax
	mov		[edi+6],ebx
	add		esi, 12
	sub		edi,NBUFFW32-34
;8 bytes
	mov		eax,[esi]
	mov		ebx,[esi+4]
	mov		[edi],eax
	mov		[edi+4],ebx
	add		esi, 8
	sub		edi,NBUFFW32-34
;6 bytes
	mov		bx,[esi+2]
	mov		eax,[esi+4]
	mov		[edi],bx
	mov		[edi+2],eax
	add		esi, 8
	sub		edi,NBUFFW32-34
;4 bytes
	mov		eax,[esi]
	add		esi, 4
	mov		[edi],eax
	sub		edi,NBUFFW32-34
;2 bytes
	mov		ax,[esi+2]
	add		esi,4
	mov		[edi],ax
	sub		edi,NBUFFW32-2
ENDM


;***********************************************************
;* COPY_TOP_LEFT_TRIANGLE_CLIP
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,ECX,EDX - trashed
;***********************************************************
COPY_TOP_LEFT_TRIANGLE_CLIP MACRO
		LOCAL Loop1,Loop2,Continue,Done

		mov		edx,2
		CLIP_BOTTOM Loop1 Done edx 1 0
Loop1:
		mov		ecx,32
		add		edi,edx
		sub		ecx,edx
		shr		ecx,2
		jnc		Loop2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:
		add		edx,2
		sub		edi,NBUFFW32
		cmp		edx,32
		jl		Loop1
Done:
ENDM


;***********************************************************
;* COPY_TOP_LEFT_TRIANGLE
;*	copies the top half of a 32x32 right-side triangle,
;*	clips to y bounds using ClipY
;* ESI - Source
;* EDI - Dest
;* EAX,ECX,EDX - trashed
;***********************************************************
COPY_TOP_LEFT_TRIANGLE MACRO
		LOCAL Loop1,Loop2,Continue,Done

		mov		edx,2
Loop1:
		mov		ecx,32
		add		edi,edx
		sub		ecx,edx
		shr		ecx,2
		jnc		Loop2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		Continue
Loop2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		Loop2
Continue:
		add		edx,2
		sub		edi,NBUFFW32
		cmp		edx,32
		jl		Loop1
Done:
ENDM


;***********************************************************
;* void DrawMTileClipBottom (BYTE *pDecodeTo)
;*		ECX = pDecodeTo
;***********************************************************
public @DrawMTileClipBottom@4
@DrawMTileClipBottom@4 proc
	push	ebx
	push	edx
	push	edi
	push	esi


	mov		edx, dword ptr [_nTrans]
	test	edx,edx
	jz		@@NoTrans

	; if (gbPartialTrans == PART_TRANS_NONE) {
	mov		dl, byte ptr [_gbPartialTrans]
	cmp		dl, PART_TRANS_NONE
	jne		@@TransLeft
	call	DrawMTileDitherClipBottom
	jmp		@@Done

@@TransLeft:
	; if (gbPartialTrans == PART_TRANS_LEFT) {
	cmp		dl, PART_TRANS_LEFT
	jne		@@TransRight
	
	;al = nWTypeTable[_gnPieceNum];
	mov		ebx, dword ptr [_gnPieceNum]
	mov		al, _nWTypeTable[ebx]
	
	;	if ((al == WTYPE_LEFT) || (al == WTYPE_ULC)) {
	cmp		al, WTYPE_LEFT
	je		@@TLLeftMask
	cmp		al, WTYPE_ULC
	je		@@TLLeftMask
	jmp		@@TransRight

@@TLLeftMask:
	mov		edx, offset sgLeftMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipBottom
	jmp		@@Done

	;	if (al == WTYPE_LRC) {
	cmp		al, WTYPE_LRC
	jne		@@TransRight
	mov		edx, offset sgRightMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipBottom
	jmp		@@Done

@@TransRight:
	; if (gbPartialTrans == PART_TRANS_RIGHT) {
	cmp		dl, PART_TRANS_RIGHT
	jne		@@NoTrans

	;al = nWTypeTable[_gnPieceNum];
	mov		ebx, dword ptr [_gnPieceNum]
	mov		al, _nWTypeTable[ebx]
	
	;	if ((al == WTYPE_RIGHT) || (al == WTYPE_ULC)) {
	cmp		al, WTYPE_RIGHT
	je		@@TRRightMask
	cmp		al, WTYPE_ULC
	je		@@TRRightMask
	jmp		@@NoTrans
@@TRRightMask:
	mov		edx, offset sgRightMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipBottom
	jmp		@@Done

	;	if (al == WTYPE_LRC) {
	cmp		al, WTYPE_LRC
	jne		@@NoTrans
	mov		edx, offset sgLeftMask
	add		edx, 31*4
	call	DrawMTileHalfDitherClipBottom
	jmp		@@Done

@@NoTrans:
	;sgT = &_microoffset[0][0];
	mov	edi, offset _microoffset
	mov	dword ptr [sgT], edi

	mov		edi,ecx		; Dest

	mov		eax,dword ptr [_nLVal]
	test	al,al
	jz		@@NoLt
	cmp		al,byte ptr [_lightmax]
	jz		@@Black

	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jnz		@@Speed

	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		ebx,dword ptr [_nLVal];			; Light conversion table
	shl		ebx,8
	add		ebx,dword ptr [_pLightTbl];

	mov		eax,dword ptr [_gdwPNum]		; Determine draw type
	shr		eax,12
	and		eax,0fh
	jz		@@Type0							; Solid 32x32 block
	cmp		ax,1
	jz		@@Type1							; 32x32 block with '0' holes
	cmp		ax,2
	jz		@@Type2							; Left Triangle
	cmp		ax,3
	jz		@@Type3							; Right Triangle
	cmp		ax,4
	jz		@@Type4							; Left Triangle to wall
	jmp		@@Type5							; Right Triangle to wall

@@Speed:
	mov		esi,dword ptr [sgT]
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,4
	add		eax,dword ptr [_nLVal]
	shl		eax,2
	add		esi,eax							; Source
	mov		eax,dword ptr [esi]
	mov		esi,dword ptr [_pSpeedCels]
	add		esi,eax

	mov		eax,dword ptr [_gdwPNum]		; Determine draw type
	shr		eax,12
	and		eax,0fh
@@SCmp:	
	cmp		ax,8
	jz		@@Type8							; PreTrans Solid 32x32 block
	cmp		ax,9
	jz		@@Type9							; PreTrans 32x32 block with '0' holes
	cmp		ax,10
	jz		@@TypeA							; PreTrans Left Triangle
	cmp		ax,11
	jz		@@TypeB							; PreTrans Right Triangle
	cmp		ax,12
	jz		@@TypeC							; PreTrans Left Triangle to wall
	jmp		@@TypeD							; PreTrans Right Triangle to wall

@@NoLt:	
	mov		eax,dword ptr [_gdwPNum]
	and		eax,08000h
	jz		@@RNoLt

	mov		esi,dword ptr [sgT]				; Get old micro tile number
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,6
	add		esi,eax
	mov		eax,dword ptr [_gdwPNum]
	and		eax,0f000h
	add		eax,dword ptr [esi]
	mov		dword ptr [_gdwPNum],eax
		
@@RNoLt:	
	mov		ebx,dword ptr [_pDungeonCels]
	mov		esi,ebx
	mov		eax,dword ptr [_gdwPNum]
	and		eax,00fffh
	shl		eax,2
	add		ebx,eax
	add		esi,dword ptr [ebx]				; Source

	mov		eax,dword ptr [_gdwPNum]			; Determine draw type
	shr		eax,12
	and		eax,07h
	add		eax,8
	jmp		@@SCmp

@@Black:	mov		eax,dword ptr [_gdwPNum]
		and		eax,08000h
		jz		@@RBlk

		mov		esi,dword ptr [sgT]				; Get old micro tile number
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,6
		add		esi,eax
		mov		eax,dword ptr [_gdwPNum]
		and		eax,0f000h
		add		eax,dword ptr [esi]
		mov		dword ptr [_gdwPNum],eax
		
@@RBlk:	mov		ebx,dword ptr [_pDungeonCels]
		mov		esi,ebx
		mov		eax,dword ptr [_gdwPNum]
		and		eax,00fffh
		shl		eax,2
		add		ebx,eax
		add		esi,dword ptr [ebx]				; Source

		mov		eax,dword ptr [_gdwPNum]			; Determine draw type
		mov		al,ah
		shr		eax,4
		and		eax,07h
		jz		@@TypeG							; Black Trans Solid 32x32 block
		cmp		ax,1
		jz		@@TypeH							; Black Trans 32x32 block with '0' holes
		cmp		ax,2
		jz		@@TypeI							; Black Trans Left Triangle
		cmp		ax,3
		jz		@@TypeJ							; Black Trans Right Triangle
		cmp		ax,4
		jz		@@TypeK							; Black Trans Left Triangle to wall
		jmp		@@TypeL							; Black Trans Right Triangle to wall

;*-----------------------------------------------------------------------*/

@@Type0:
		mov		edx,32
		push	ebp
@@T0Lp1:	
		push	edx
		cmp		edi,dword ptr [_glClipY]
		jb		@@T0C1
		add		esi,32
		add		edi,32
		jmp		@@T0C2
@@T0C1:
		xor		edx,edx		
		mov		ebp,8
@@T0Lp2:
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
		jnz	@@T0Lp2
@@T0C2:	
		sub		edi,NBUFFW32
		pop		edx
		dec		edx
		jnz		@@T0Lp1
		pop ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type1:
		push	ebp
		mov		ecx,32
@@T1Lp1:
		push	ecx
		mov		ebp,32
@@T1Lp2:	
		xor		eax,eax		; Load control byte
		mov		al,[esi]
		inc		esi

		test	al,al
		jns		@@T1NoSkip
		neg		al
		add		edi,eax
		sub		ebp,eax
		jmp		@@T1continue
@@T1NoSkip:
		sub		ebp,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T1NotCliped
		add		esi,eax
		add		edi,eax
		jmp		@@T1continue
@@T1NotCliped:	
		mov		ecx,eax
		X_BYTES
@@T1continue:	
		test	ebp,ebp
		jnz		@@T1Lp2

		pop		ecx
		sub		edi,NBUFFW32
		dec		ecx
		jnz		@@T1Lp1
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type2:
		push	ebp
		mov		ebp,30
		CLIP_BOTTOM @@T2Lp1 @@T2b ebp 0 0
@@T2Lp1:	
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
;		X_BYTES_PREPAD
		mov	edx,ecx
		and	edx,2
		add	esi,edx
@@T2PixLine:
		mov dl,[esi]
		inc	esi
		mov dl,[ebx+edx]
		mov [edi],dl
		inc edi
		dec ecx
		jnz @@T2PixLine
@@T2continue:	
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		@@T2Lp1


@@T2b:
		mov		ebp,2
		CLIP_BOTTOM @@T2Lp2 @@T2Done ebp 1 0
@@T2Lp2:	
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
;		X_BYTES_PREPAD
		mov	edx,ecx
		and	edx,2
		add	esi,edx
@@T2PixLine2:
		mov dl,[esi]
		inc	esi
		mov dl,[ebx+edx]
		mov [edi],dl
		inc edi
		dec ecx
		jnz @@T2PixLine2
@@T2continue2:	
		add		ebp,2
		sub		edi,NBUFFW32
		cmp		ebp,32
		jnz		@@T2Lp2
@@T2Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type3:
		push	ebp
		mov		ebp,30
		CLIP_BOTTOM @@T3Lp1 @@T3b ebp 0 0
@@T3Lp1:
		mov		ecx,32
		sub		ecx,ebp
;		X_BYTES_PAD
@@T3PixLine:
		mov dl,[esi]
		inc	esi
		mov dl,[ebx+edx]
		mov [edi],dl
		inc edi
		dec ecx
		jnz @@T3PixLine

		mov	edx,esi
		and	edx,2
		add	esi,edx
@@T3x:
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T3Lp1


@@T3b:
		mov		ebp,2
		CLIP_BOTTOM @@T3Lp3 @@T3Done ebp 1 0
@@T3Lp3:
		mov		ecx,32
		sub		ecx,ebp
;		X_BYTES_PAD
@@T3PixLine2:
		mov dl,[esi]
		inc	esi
		mov dl,[ebx+edx]
		mov [edi],dl
		inc edi
		dec ecx
		jnz @@T3PixLine2

		mov	edx,esi
		and	edx,2
		add	esi,edx
@@T3x2:
		sub		edi,NBUFFW32
		add		edi,ebp
		add		ebp,2
		cmp		ebp,32
		jnz		@@T3Lp3
@@T3Done:
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type4:
		push	ebp
		mov		ebp,30
		CLIP_BOTTOM @@T4Lp1 @@T4b ebp 0 0
@@T4Lp1:
		add		edi,ebp
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PREPAD
@@T4x:	
		sub		edi,NBUFFW32
		sub		ebp,2
		jge		@@T4Lp1


@@T4b:
		mov		ebp,16
@@T4Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T4C2
		add		esi,32
		add		edi,32
		jmp		@@T4C3
@@T4C2:
		mov		ecx,8*4
		X_DWORDS
@@T4C3:
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T4Lp3
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type5:
		push	ebp
		mov		ebp,30
		CLIP_BOTTOM @@T5Lp1 @@T5b ebp 0 0
@@T5Lp1:
		mov		ecx,32
		sub		ecx,ebp
		X_BYTES_PAD
@@T5x:
		sub		edi,NBUFFW32
		add		edi,ebp
		sub		ebp,2
		jge		@@T5Lp1


@@T5b:
		mov		ebp,16
@@T5Lp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T5C2
		add		esi,32
		add		edi,32
		jmp		@@T5C3
@@T5C2:
		mov		ecx,8*4
		X_DWORDS
@@T5C3:
		sub		edi,NBUFFW32
		dec		ebp
		jnz		@@T5Lp3
		pop		ebp
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type8:
		mov		edx,32
@@T8Lp1:
		cmp		edi,dword ptr [_glClipY]
		jb		@@T8C1
		add		esi,32
		add		edi,32
		jmp		@@T8C2
@@T8C1:
		mov		ecx,8
@@T8Lp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T8Lp2
@@T8C2:
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@T8Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@Type9:
		mov		ecx,32
@@T9Lp1:
		push	ecx
		mov		edx,32
@@T9Lp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@T9J
		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@T9C1
		add		esi,eax
		add		edi,eax
		jmp		@@T9x
@@T9C1:
		mov		ecx,eax
		shr		ecx,1
		jnc		@@T9w
		mov		al,[esi]
		inc		esi
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz	@@T9x
@@T9w:
		shr		ecx,1
		jnc		@@T9Lp3
		mov		ax,[esi]
		add		esi,2
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@T9x
@@T9Lp3:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@T9Lp3
@@T9x:
		test	edx,edx
		jz		@@T9Nxt
		jmp		@@T9Lp2

@@T9J:
		neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@T9Lp2
@@T9Nxt:
		pop		ecx
		sub		edi,NBUFFW32
		dec		ecx
		jnz		@@T9Lp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeA:
		COPY_BOTTOM_LEFT_TRIANGLE_CLIP
		COPY_TOP_LEFT_TRIANGLE_CLIP
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeB:
		COPY_BOTTOM_RIGHT_TRIANGLE_CLIP
		COPY_TOP_RIGHT_TRIANGLE_CLIP
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeC:	
		mov		edx,30
		CLIP_BOTTOM @@TCLp1 @@TCb edx 0 0
@@TCLp1:
		add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TCLp2
		mov		ax,[esi+2]
		add		esi,4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz		@@TCx
@@TCLp2:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TCLp2
@@TCx:	
		sub		edi,NBUFFW32
		sub		edx,2
		jge		@@TCLp1


@@TCb:	
		mov		edx,16
@@TCLp3:
		cmp		edi,dword ptr [_glClipY]
		jb		@@TCC2
		add		esi,32
		add		edi,32
		jmp		@@TCC3
@@TCC2:	
		mov		ecx,8
@@TCLp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TCLp4
@@TCC3:	
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TCLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeD:	
		mov		edx,30
		CLIP_BOTTOM @@TDLp1 @@TDb edx 0 0
@@TDLp1:	
		mov		ecx,32
		sub		ecx,edx
		mov		ebx,ecx
		shr		ecx,2
		jz		@@TDx
@@TDLp2:	
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TDLp2
@@TDx:
		and		ebx,2
		jz		@@TDSkip
		mov		ax,[esi]
		add		esi,4
		mov		[edi],ax
		add		edi,2
@@TDSkip:
		sub		edi,NBUFFW32
		add		edi,edx
		sub		edx,2
		jge		@@TDLp1


@@TDb:	
		mov		edx,16
@@TDLp3:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TDC2
		add		esi,32
		add		edi,32
		jmp		@@TDC3
@@TDC2:	
		mov		ecx,8
@@TDLp4:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TDLp4
@@TDC3:	
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TDLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeG:	
		mov		edx,32
		xor		eax,eax
@@TGLp1:	
		cmp		edi,dword ptr [_glClipY]
		jb		@@TGC1
		add		esi,32
		add		edi,32
		jmp		@@TGC2
@@TGC1:	
		mov		ecx,8
@@TGLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TGLp2
@@TGC2:	
		sub		edi,NBUFFW32
		dec		edx
		jnz		@@TGLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeH:	mov		ecx,32

@@THLp1:	push	ecx
		mov		edx,32

@@THLp2:	
		xor		eax,eax				; Load control byte
		mov		al,[esi]
		inc		esi
		test	al,al
		js		@@THJ

		sub		edx,eax
		cmp		edi,dword ptr [_glClipY]
		jb		@@THC1
		add		esi,eax
		add		edi,eax
		jmp		@@THx
@@THC1:	mov		ecx,eax
		add		esi,ecx
		xor		eax,eax
		shr		ecx,1
		jnc		@@THw
		mov		[edi],al
		inc		edi
		test	ecx,ecx
		jz		@@THx
@@THw:	shr		ecx,1
		jnc		@@THLp3
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@THx
@@THLp3:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@THLp3
@@THx:	test	edx,edx
		jz		@@THNxt
		jmp		@@THLp2

@@THJ:	neg		al					; Do jump
		add		edi,eax
		sub		edx,eax
		jnz		@@THLp2
@@THNxt:	pop		ecx
		sub		edi,NBUFFW32
		dec		ecx
		jnz		@@THLp1
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeI:	mov		edx,30
		xor		eax,eax
@@TILp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TIC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TIx
@@TIC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TILp2
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TIx
@@TILp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp2
@@TIx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TIb
		sub		edx,2
		jmp		@@TILp1

@@TIb:	mov		edx,2
@@TILp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TIC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TIx2
@@TIC2:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TILp4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TIx2
@@TILp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TILp4
@@TIx2:	sub		edi,NBUFFW32
		add		edx,2
		cmp		edx,32
		jnz		@@TILp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeJ:	mov		edx,30
		xor		eax,eax
@@TJLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TJC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TJx
@@TJC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp2
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TJx
@@TJLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp2
@@TJx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TJb
		add		edi,edx
		sub		edx,2
		jmp		@@TJLp1

@@TJb:	mov		edx,2
@@TJLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TJC2
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TJx2
@@TJC2:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TJLp4
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TJx2
@@TJLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TJLp4
@@TJx2:	sub		edi,NBUFFW32
		add		edi,edx
		add		edx,2
		cmp		edx,32
		jnz		@@TJLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeK:	mov		edx,30
		xor		eax,eax
@@TKLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TKC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		jmp		@@TKx
@@TKC1:	add		edi,edx
		mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TKLp2
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TKx
@@TKLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TKLp2
@@TKx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TKb
		sub		edx,2
		jmp		@@TKLp1

@@TKb:	mov		edx,16
@@TKLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TKC2
		add		esi,32
		add		edi,32
		jmp		@@TKC3
@@TKC2:
		mov		ecx,8
@@TKLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TKLp4
@@TKC3:	sub		edi,NBUFFW32
		dec		edx
		jnz		@@TKLp3
		jmp		@@Done

;*-----------------------------------------------------------------------*/

@@TypeL:	mov		edx,30
		xor		eax,eax
@@TLLp1:	cmp		edi,dword ptr [_glClipY]
		jb		@@TLC1
		add		esi,32
		sub		esi,edx
		add		edi,32
		sub		edi,edx
		jmp		@@TLx
@@TLC1:	mov		ecx,32
		sub		ecx,edx
		shr		ecx,2
		jnc		@@TLLp2
		mov		[edi],ax
		add		edi,2
		test	ecx,ecx
		jz	@@TLx
@@TLLp2:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TLLp2
@@TLx:	sub		edi,NBUFFW32
		test	edx,edx
		jz		@@TLb
		add		edi,edx
		sub		edx,2
		jmp		@@TLLp1

@@TLb:	mov		edx,16
@@TLLp3:	cmp		edi,dword ptr [_glClipY]
		jb		@@TLC2
		add		esi,32
		add		edi,32
		jmp		@@TLC3
@@TLC2:
		mov		ecx,8
@@TLLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@TLLp4
@@TLC3:	sub		edi,NBUFFW32
		dec		edx
		jnz		@@TLLp3

@@Done:
	pop		esi
	pop		edi
	pop		edx
	pop		ebx
	ret
@DrawMTileClipBottom@4 endp


;***********************************************************
;* void DrawBlankMTile (BYTE *pDecodeTo)
;*		ECX = pDecodeTo
;***********************************************************
public @DrawBlankMTile@4
@DrawBlankMTile@4 proc
	push	ebx
	push	edx
	push	edi
	push	esi

	mov		edi,ecx		; Dest

	mov		edx,30
	mov		ebx,1
	xor		eax,eax
	;mov	eax,092929292h	; yellow
@@BLp1:
	add		edi,edx
	mov		ecx,ebx
@@BLp3:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@BLp3
	add		edi,edx
	sub		edi,NBUFFW64
	test	edx,edx
	jz		@@Bb
	sub		edx,2
	inc		ebx
	jmp		@@BLp1

@@Bb:	
	mov		edx,2
	mov		ebx,15
@@BLp2:	
	add		edi,edx
	mov		ecx,ebx
@@BLp4:
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		@@BLp4
	add		edi,edx
	sub		edi,NBUFFW64
	dec		ebx
	add		edx,2
	cmp		edx,32
	jnz		@@BLp2

	pop		esi
	pop		edi
	pop		edx
	pop		ebx
	ret
@DrawBlankMTile@4 endp


;****************************************************************************************
;****************************************************************************************
;****************************************************************************************
	END