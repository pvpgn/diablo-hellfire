//****************************************************************************
//	artfont.cpp
//	created 10.15.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"


//****************************************************************************
//****************************************************************************

	static TARTFONT sgHugeFont;
	static TARTFONT sgHugeFontGray;
	static TARTFONT sgBigFont;
	static TARTFONT sgBigFontGray;
	static TARTFONT sgMedFont;
	static TARTFONT sgMedFontGray;
	static TARTFONT sgSmallFont;
	static TARTFONT sgSmallFontGray;
	static TPARTFONT sgpCurrFont;


//****************************************************************************
//****************************************************************************
void ArtFontSetHuge(void) {
	sgpCurrFont = &sgHugeFont;
}


//****************************************************************************
//****************************************************************************
void ArtFontSetHugeGray(void) {
	sgpCurrFont = &sgHugeFontGray;
}


//****************************************************************************
//****************************************************************************
void ArtFontSetBig(void) {
	sgpCurrFont = &sgBigFont;
}


//****************************************************************************
//****************************************************************************
void ArtFontSetBigGray(void) {
	sgpCurrFont = &sgBigFontGray;
}


//****************************************************************************
//****************************************************************************
void ArtFontSetMed(void) {
	sgpCurrFont = &sgMedFont;
}


//****************************************************************************
//****************************************************************************
void ArtFontSetMedGray(void) {
	sgpCurrFont = &sgMedFontGray;
}


//****************************************************************************
//****************************************************************************
void ArtFontSetSmall(void) {
	sgpCurrFont = &sgSmallFont;
}


//****************************************************************************
//****************************************************************************
void ArtFontSetSmallGray(void) {
	sgpCurrFont = &sgSmallFontGray;
}


//****************************************************************************
//****************************************************************************
void ArtFontSet(int fontnum) {
	switch (fontnum) {
		case AF_HUGE:
			ArtFontSetHuge();
		break;
		case AF_HUGEGRAY:
			ArtFontSetHugeGray();
		break;
		case AF_BIG:
			ArtFontSetBig();
		break;
		case AF_BIGGRAY:
			ArtFontSetBigGray();
		break;
		case AF_MED:
			ArtFontSetMed();
		break;
		case AF_MEDGRAY:
			ArtFontSetMedGray();
		break;
		case AF_SMALL:
			ArtFontSetSmall();
		break;
		case AF_SMALLGRAY:
		default:
			ArtFontSetSmallGray();
		break;
	}
}


//****************************************************************************
//****************************************************************************
void ArtFontInit(void) {
	sgHugeFont.bValid = FALSE;
	sgHugeFontGray.bValid = FALSE;
	sgBigFont.bValid = FALSE;
	sgBigFontGray.bValid = FALSE;
	sgMedFont.bValid = FALSE;
	sgMedFontGray.bValid = FALSE;
	sgSmallFont.bValid = FALSE;
	sgSmallFontGray.bValid = FALSE;
	sgpCurrFont = NULL;
}


//****************************************************************************
//****************************************************************************
static void ArtFontDestroyFont(TPARTFONT pFont) {
	if (! pFont->bValid) return;

 	for (int index = 0; index < MAX_CHARS; index++) {
		if (pFont->pCharData[index]) {
			STransDelete(pFont->pCharData[index]);
			pFont->pCharData[index] = NULL;
		}
	}

	pFont->bValid = FALSE;
}


//****************************************************************************
//****************************************************************************
void ArtFontDestroy(void) {
	ArtFontDestroyFont(&sgHugeFont);
	ArtFontDestroyFont(&sgHugeFontGray);
	ArtFontDestroyFont(&sgBigFont);
	ArtFontDestroyFont(&sgBigFontGray);
	ArtFontDestroyFont(&sgMedFont);
	ArtFontDestroyFont(&sgMedFontGray);
	ArtFontDestroyFont(&sgSmallFont);
	ArtFontDestroyFont(&sgSmallFontGray);

	sgpCurrFont = NULL;
}


//****************************************************************************
//****************************************************************************
static void ArtFontLoadFont(TPARTFONT pFont, LPCSTR datafile, LPCSTR artfile) {
	HSFILE file;
	LPBYTE pFontBmp;
	SIZE BmpSize;

	if (pFont->bValid) return;

	// load the font specs for the font
	if (! SFileOpenFile(datafile, &file))
		return;
	if (! SFileReadFile(file, &pFont->tFontSpecs, SFileGetFileSize(file))) {
		SFileCloseFile(file);
		return;
	}
	SFileCloseFile(file);

	// load the artwork for the font
	UiLoadBmpFile(artfile, &pFontBmp, &BmpSize);

	memset(pFont->pCharData, 0, sizeof(sgBigFont.pCharData));
	if (pFontBmp) {
		RECT rect;
		// create an HTRANS for each character in the font
		for (int index = 0; index <= MAX_CHARS; index++) {
			if (! pFont->tFontSpecs.bCharWidths[index]) continue;

			rect.left = 0;
			rect.right = pFont->tFontSpecs.bCharWidths[index];
			rect.top = index * pFont->tFontSpecs.bCharHeight;
			rect.bottom = rect.top + pFont->tFontSpecs.bCharHeight - 1;

			STransCreate(
				pFontBmp,
				BmpSize.cx,
				BmpSize.cy,
				8,
				&rect,
				PALETTEINDEX(32),
				&pFont->pCharData[index]
			);
		}

		pFont->bValid = TRUE;
		FREE(pFontBmp);
	}
}


//****************************************************************************
//****************************************************************************
BOOL ArtFontLoad(void) {
   	ArtFontLoadFont(&sgBigFont, "ui_art\\font30.bin", "ui_art\\font30g.pcx");
   	ArtFontLoadFont(&sgBigFontGray, "ui_art\\font30.bin", "ui_art\\font30s.pcx");
   	ArtFontLoadFont(&sgMedFont, "ui_art\\font24.bin", "ui_art\\font24g.pcx");
   	ArtFontLoadFont(&sgMedFontGray, "ui_art\\font24.bin", "ui_art\\font24s.pcx");
   	ArtFontLoadFont(&sgSmallFont, "ui_art\\font16.bin", "ui_art\\font16g.pcx");
   	ArtFontLoadFont(&sgSmallFontGray, "ui_art\\font16.bin", "ui_art\\font16s.pcx");
   	ArtFontLoadFont(&sgHugeFont, "ui_art\\font42.bin", "ui_art\\font42g.pcx");
   	ArtFontLoadFont(&sgHugeFontGray, "ui_art\\font42.bin", "ui_art\\font42y.pcx");

	return 1;
}


//****************************************************************************
// return the height of the current font
//****************************************************************************
int ArtFontHeight(void) {
	if (! sgpCurrFont) return 0;
	if (! sgpCurrFont->bValid) return 0;
	return sgpCurrFont->tFontSpecs.bCharHeight;
}


//****************************************************************************
//****************************************************************************
int ArtFontGridWidth(void) {
	if (! sgpCurrFont) return 0;
	if (! sgpCurrFont->bValid) return 0;

	return sgpCurrFont->tFontSpecs.bCharWidth;
}


//****************************************************************************
// calculate the witdth of the given string
//****************************************************************************
int ArtFontPixelWidth(LPCTSTR text) {
	unsigned char c;
	int len = 0;

	if (! sgpCurrFont) return 0;
	if (! sgpCurrFont->bValid) return 0;

	while (0 != (c = *text)) {
		if (! sgpCurrFont->tFontSpecs.bCharWidths[c])
			len += sgpCurrFont->tFontSpecs.bCharWidth;
		else
			len += sgpCurrFont->tFontSpecs.bCharWidths[c];
		text++;
	}

	return len;
}


//****************************************************************************
//****************************************************************************
int ArtFontWordLen(LPCTSTR text) {
	unsigned char c;
	int len = 0;

	if (! sgpCurrFont) return 0;
	if (! sgpCurrFont->bValid) return 0;

	while (0 != (c = *text)) {
		switch (c) {
			case ' ':
			case '\n':
			return len;

			default:
				if (! sgpCurrFont->tFontSpecs.bCharWidths[c])
					return len;
				else
					len += sgpCurrFont->tFontSpecs.bCharWidths[c];
			break;
		}
		text++;
	}
	return len;
}


//****************************************************************************
//****************************************************************************
void ArtFontDraw(LPCTSTR text, TPBMP dest, int x, int y) {
	unsigned char c;
	if (! dest) return;
	if (! dest->data) return;
	if (! sgpCurrFont) return;
	if (! sgpCurrFont->bValid) return;

	if (x < 0) x = 0;
	if (y < 0) y = 0;

	while (0 != (c = *text)) {

		// check for vertical overflow
		if ((y + sgpCurrFont->tFontSpecs.bCharHeight) > dest->datasize.cy)
			break;

		if (c == '\n') {
			x = 0;
			y += sgpCurrFont->tFontSpecs.bCharHeight;
			text++;
		}
		else if (! sgpCurrFont->tFontSpecs.bCharWidths[c]) {		// word break
			if ((x + sgpCurrFont->tFontSpecs.bCharWidth + ArtFontWordLen(text+1)) >= dest->datasize.cx) {	// line overrun
				x = 0;
				y += sgpCurrFont->tFontSpecs.bCharHeight;
			}
			else {
				x += sgpCurrFont->tFontSpecs.bCharWidth;
			}
			text++;
		}
		else if (sgpCurrFont->pCharData[c]) {
			if ((x + sgpCurrFont->tFontSpecs.bCharWidths[c]) > dest->datasize.cx) {
				x = 0;
				y += sgpCurrFont->tFontSpecs.bCharHeight;
				continue;
			}

			STransBlt(dest->data, x, y, dest->datasize.cx, sgpCurrFont->pCharData[c]);
			x += sgpCurrFont->tFontSpecs.bCharWidths[c];
			// advance char
			text++;
		}
	}
}