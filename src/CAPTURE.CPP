//***************************************************************************
// capture.c
// created 4.14.95
// written by Patrick Wyatt
//***************************************************************************


#include "diablo.h"
#pragma hdrstop
#include <io.h>
#include <ctype.h>
#include "engine.h"


//***************************************************************************
// externs
//***************************************************************************
void DrawAndBlit();


//***************************************************************************
// constants
//***************************************************************************
#define PCX_HEADER		10
#define PCX_VERSION		5
#define PCX_ENCODE		1

#define PCX_MAX_REP		63

#define PCX_X_PAL		12
#define PCX_COLORS		16
#define X_PCX_COLORS	256


//***************************************************************************
// types
//***************************************************************************
#pragma pack(push,1)
typedef struct PCXHeader {
	BYTE	nHeader;	// 10 for valid PCX files
	BYTE	nVersion;	// 5 for version 3.0 with palette
	BYTE	nEncode;	// file encoding mode
	BYTE	nBits;		// 8 for 256 color mode
	WORD	x1;
	WORD	y1;
	WORD	x2;
	WORD	y2;
	WORD	nScrWid;
	WORD	nScrHgt;
} PCXHeader;

typedef struct PCXInfo {
	BYTE nMode;			// always 0
	BYTE nPlanes;		// number of bit planes
	WORD nLine;			// bytes per line
	BYTE unused[60];	// fill out to 128 bytes
} PCXInfo;

typedef struct TPCX_RGB {
	BYTE r;
	BYTE g;
	BYTE b;
} TPCX_RGB;

typedef struct TPCX_XPal {
	BYTE nPal;
	TPCX_RGB rgb[X_PCX_COLORS];
} TPCX_XPal;

typedef struct TPCX {
	PCXHeader Header;
	TPCX_RGB Pal16[PCX_COLORS];
	PCXInfo Info;
} TPCX;
#pragma pack(pop)


//***************************************************************************
//***************************************************************************
static BOOL pcx_write_header(HANDLE hFile,WORD wWdt,WORD wHgt) {
	TPCX pcx;

	ZeroMemory(&pcx,sizeof(pcx));

	// initialize header
	pcx.Header.nHeader	= PCX_HEADER;
	pcx.Header.nVersion	= PCX_VERSION;
	pcx.Header.nEncode	= PCX_ENCODE;
	pcx.Header.nBits	= 8;
	//pcx.Header.x1		= 0;
	//pcx.Header.y1		= 0;
	pcx.Header.x2		= wWdt - 1;
	pcx.Header.y2		= wHgt - 1;
	pcx.Header.nScrWid	= wWdt;
	pcx.Header.nScrHgt	= wHgt;

	//pcx.Info.nMode	= 0;
	pcx.Info.nPlanes	= 1;
	pcx.Info.nLine		= wWdt;

	DWORD dwBytes;
	return WriteFile(hFile,&pcx,sizeof(pcx),&dwBytes,NULL)
		&& (dwBytes == sizeof(pcx));
}


//***************************************************************************
//***************************************************************************
static BOOL pcx_write_pal(HANDLE hFile,const PALETTEENTRY pal[256]) {
	// setup extended palette header
	TPCX_XPal xpal;
	xpal.nPal = PCX_X_PAL;

	// copy palette colors
	for (int i = 0; i < X_PCX_COLORS; i++) {
		xpal.rgb[i].r = pal[i].peRed;
		xpal.rgb[i].g = pal[i].peGreen;
		xpal.rgb[i].b = pal[i].peBlue;
	}

	// write it
	DWORD dwBytes;
	return WriteFile(hFile,&xpal,sizeof(xpal),&dwBytes,NULL)
		&& (dwBytes == sizeof(xpal));
}


//***************************************************************************
//***************************************************************************
static BYTE * pcx_compress_line(const BYTE * pSrc,BYTE * pDst,int nWdt) {
	BYTE c;
	int nCount;

	do {
		// get next character
		c = *pSrc++;
		nCount = 1;
		nWdt--;

		// see how long the sequence is
		while ((c == *pSrc) && (nCount < PCX_MAX_REP) && nWdt) {
			nCount++;
			nWdt--;
			pSrc++;
		}

		// write repeat count (if needed)
		if ((nCount > 1) || (c > 0xbf)) {
			nCount |= 0xc0;
			*pDst++ = (BYTE) nCount;
		}

		*pDst++ = c;
	} while (nWdt);

	return pDst;
}


//***************************************************************************
//***************************************************************************
static BOOL pcx_write_image(HANDLE hFile,WORD wDstWdt,WORD wDstHgt,WORD wSrcWdt,const BYTE * pSrc) {
	BYTE * pDstEnd;
	BYTE * pDstBase;
	DWORD dwWrite;
	DWORD dwBytes;

	// allocate line buffer -- line cannot be more than 2x larger
	pDstBase = (BYTE *) DiabloAllocPtrSig(2 * wDstWdt,'CAPt');

	while (wDstHgt--) {
		pDstEnd = pcx_compress_line(pSrc,pDstBase,wDstWdt);
		pSrc += wSrcWdt;
		dwWrite = pDstEnd - pDstBase;
		if (! WriteFile(hFile,pDstBase,dwWrite,&dwBytes,NULL)) return FALSE;
		if (dwBytes != dwWrite) return FALSE;
	}

	DiabloFreePtr(pDstBase);
	return TRUE;
}


//***************************************************************************
//***************************************************************************
#define MAX_CAPTURE		100
#define DIG_OFF			6
static const char szCAPTURE[] = "screen??.PCX";
static const char szCAPTUREfmt[] = "screen%02d.PCX";
static HANDLE open_capture_file(char szFileName[MAX_PATH]) {
	int nValue;
	BYTE bUsedTbl[MAX_CAPTURE];

	ZeroMemory(bUsedTbl,sizeof(bUsedTbl));
	struct _finddata_t ffblk;
	long lHandle = _findfirst(szCAPTURE,&ffblk);
	if (lHandle != -1) {
		do {
			if (! isdigit(ffblk.name[DIG_OFF+0]))
				continue;
			if (! isdigit(ffblk.name[DIG_OFF+1]))
				continue;

			nValue = (ffblk.name[DIG_OFF + 0] - '0') * 10;
			nValue += ffblk.name[DIG_OFF + 1] - '0';
			bUsedTbl[nValue] = 1;
		} while (! _findnext(lHandle,&ffblk));
	}

	for (nValue = 0; nValue < MAX_CAPTURE; nValue++) {
		if (bUsedTbl[nValue]) continue;
		sprintf(szFileName,szCAPTUREfmt,nValue);
		return CreateFile(szFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	}

	return INVALID_HANDLE_VALUE;
}


//***************************************************************************
//***************************************************************************
static void red_palette(const PALETTEENTRY src[256]) {
	PALETTEENTRY dst[256];
	for (int i = 0; i < 256; i++) {
		dst[i].peRed = src[i].peRed;
		dst[i].peGreen = 0;
		dst[i].peBlue = 0;
		dst[i].peFlags = 0;
	}
	lpDDPal->SetEntries(0,0,256,dst);
}


//***************************************************************************
//***************************************************************************
void screen_capture(void) {
	HANDLE hFile;
	char szFileName[MAX_PATH];
	PALETTEENTRY pal[256];

	if (INVALID_HANDLE_VALUE == (hFile = open_capture_file(szFileName)))
		return;

	// get the current palette, then flash the screen red
	DrawAndBlit();
	lpDDPal->GetEntries(0,0,256,pal);
	red_palette(pal);

	lock_buf(2);
	app_assert(gpBuffer);
	BOOL bOK = pcx_write_header(hFile,TOTALX,TOTALY);
	if (bOK) bOK = pcx_write_image(hFile,TOTALX,TOTALY,BUFFERX,gpBuffer + 122944);
	if (bOK) bOK = pcx_write_pal(hFile,pal);
	unlock_buf(2);

	CloseHandle(hFile);
	if (! bOK)
		DeleteFile(szFileName);

	// restore palette
	Sleep(300);
	lpDDPal->SetEntries(0,0,256,pal);
}
