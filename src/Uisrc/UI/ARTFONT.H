//****************************************************************************
//	artfont.h
//	created 10.15.96
//****************************************************************************


//****************************************************************************
//****************************************************************************
	#define MAX_CHARS	256

	enum _artfonts {
		AF_SMALL,
		AF_SMALLGRAY,
		AF_MED,
		AF_MEDGRAY,
		AF_BIG,
		AF_BIGGRAY,
		AF_HUGE,
		AF_HUGEGRAY
	};

	typedef struct _fontspecs {
		BYTE bCharWidth;				// max char width
		BYTE bCharHeight;				// max char height
		BYTE bCharWidths[MAX_CHARS];	// actual char widths
	} TFONTSPECS, * TPFONTSPECS;
	
	typedef struct _artfont {
		TFONTSPECS tFontSpecs;
		HTRANS pCharData[MAX_CHARS];
		BOOL bValid;
	} TARTFONT, * TPARTFONT;


//****************************************************************************
//****************************************************************************
	void ArtFontInit(void);
	void ArtFontDestroy(void);
	BOOL ArtFontLoad(void);

	void ArtFontSetBig(void);
	void ArtFontSetBigGray(void);
	void ArtFontSetMed(void);
	void ArtFontSetMedGray(void);
	void ArtFontSetSmall(void);
	void ArtFontSetSmallGray(void);
	void ArtFontSet(int fontnum);

	void ArtFontDraw(LPCSTR text, TPBMP dest, int x, int y);
	int ArtFontGridWidth(void);
	int ArtFontPixelWidth(LPCSTR text);
	int ArtFontHeight(void);
