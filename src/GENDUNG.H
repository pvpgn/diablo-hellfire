/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/GENDUNG.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define LVLLENGTH	4

#define TLVL_START	0
#define LVL1_START	1
#define LVL2_START	LVL1_START+LVLLENGTH
#define LVL3_START	LVL2_START+LVLLENGTH
#define LVL4_START	LVL3_START+LVLLENGTH
//HellFire JKE 7/29
#define LVL5_START	LVL4_START+LVLLENGTH
#define CRYPTSTART	21
#define CRYPTEND	24
#define HIVESTART	17
#define HIVEEND		20

//#define NUMLEVELS	((4*LVLLENGTH)+1)
#define NUMLEVELS ((6*LVLLENGTH)+1)	//HellFire JKE 7/29
#define NUMSLEVELS	10

#define DIRTEDGE	32
#define DIRTEDGED2	(DIRTEDGE/2)
#define DMAXX		(80+DIRTEDGE)
#define DMAXY		(80+DIRTEDGE)
#define MAXDUNX		DMAXX
#define MAXDUNY		DMAXY

#define MDMAXX		((DMAXX-DIRTEDGE)/2)		// Mega tile dungeon max (max div 2)
#define MDMAXY		((DMAXY-DIRTEDGE)/2)

#define MAXTILES	2048

#define MAXMREND	128

#define NUMSPEEDCELS	64
#define SPEEDSIZE		NUMSPEEDCELS*16384

#define	MAXMICRO	2048

#define MAXDIRT		32

#define NODRAW		0
#define VIEWDRAW	1
#define FULLDRAW	0xff

#define SCRL_NONE	0
#define SCRL_U		1
#define SCRL_UR		2
#define SCRL_R		3
#define SCRL_DR		4
#define SCRL_D		5
#define SCRL_DL		6
#define SCRL_L		7
#define SCRL_UL		8

// bFlags bits
#define BFLAG_AUTOMAP	0x80
#define BFLAG_VISIBLE	0x40
#define BFLAG_PLRLR		0x20
#define BFLAG_MONSTLR	0x10
#define BFLAG_SETPC		0x08
#define BFLAG_DEADPLR	0x04

// Set Piece bit used in all drlg's
#define SETP_BIT	0x80	// Non changeable set piece bit

#define BFLAG_MONSTACTIVE 0x02
#define BFLAG_MISSILE	0x01

#define BFMASK_AUTOMAP	0x7f
#define BFMASK_VISIBLE	0xbf
#define BFMASK_PLRLR	0xdf
#define BFMASK_MONSTLR	0xef
#define BFMASK_SETPC	0xf7
//#define BFMASK_UNUSED	0xfb
//#define BFMASK_UNUSED	0xfd
#define BFMASK_MISSILE	0xfe

#define WTYPE_NONE		0
#define WTYPE_LEFT		1
#define WTYPE_RIGHT		2
#define WTYPE_ULC		3
#define WTYPE_LRC		4

#define D_NORMAL		0
#define D_NIGHTMARE		1
#define D_HELL			2

/*-----------------------------------------------------------------------**
** Macros
**-----------------------------------------------------------------------*/
#define MegaToMini(M) ((M << 1) + DIRTEDGED2)
#define MiniToMega(m) ((m - DIRTEDGED2) >> 1)

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	byte	strig;
	byte	s1;
	byte	s2;
	byte	s3;
	byte	nv1;
	byte	nv2;
	byte	nv3;
} ShadowStruct;

typedef struct {
	int	_sxoff;			// Smooth scroll x,y offsets
	int	_syoff;
	int	_sdx;			// Delta between plr and view x,y
	int	_sdy;
	int _sdir;			// Direction
} ScrollStruct;

typedef struct THEME_LOC {
	int	x;				//Upper left coord	
	int y;				//Upper left coord
	int ttval;			//Transparency value
	int width;			//Room width
	int height;			//Room height
} THEME_LOC;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern byte	dungeon[MDMAXX][MDMAXY];
extern byte	pdungeon[MDMAXX][MDMAXY];
extern byte dflags[MDMAXX][MDMAXY];

extern int setpc_x, setpc_y, setpc_w, setpc_h;
extern byte	*pSetPiece;
extern BOOL setloadflag;

extern "C" {
	extern BYTE *pDungeonCels;
	extern BYTE *pSpeedCels;
	extern long microoffset[MAXMREND][16];
	extern byte nWTypeTable[MAXTILES+1];
}

extern BYTE *pSpecialCels;
extern BYTE *pMegaTiles;
extern BYTE *pMiniTiles;

extern BYTE nBlockTable[MAXTILES+1];
extern BYTE nSolidTable[MAXTILES+1];
extern BYTE nTransTable[MAXTILES+1];
extern BYTE nMissileTable[MAXTILES+1];
extern BYTE nTrapTable[MAXTILES+1];

extern int	dminx, dminy, dmaxx, dmaxy;

extern int gnDifficulty;

extern BYTE currlevel;
extern BYTE leveltype;
extern BYTE setlevel;
extern BYTE setlvlnum;
extern BYTE setlvltype;

extern int ViewX, ViewY;
extern int ViewDX, ViewDY;
extern int ViewBX, ViewBY;
extern ScrollStruct	ScrollInfo;

extern int LvlViewX, LvlViewY;

extern int btmbx, btmby;
extern int btmdx, btmdy;

extern int MicroTileLen;

extern char TransVal;
extern BYTE TransList[256];

extern int dPiece[MAXDUNX][MAXDUNY];		// Tile #

typedef struct { WORD mt[16]; } MICROS;
extern MICROS dMT[MAXDUNX][MAXDUNY];		// Micro Tiles
extern MICROS dMT2[MAXDUNX*MAXDUNY];

extern char dTransVal[MAXDUNX][MAXDUNY];	// Transparent active value
extern char dLight[MAXDUNX][MAXDUNY];		// Current light value
extern char dSaveLight[MAXDUNX][MAXDUNY];		// Static light value
extern char dFlags[MAXDUNX][MAXDUNY];		// Flags for Solid collision, etc.
extern char dPlayer[MAXDUNX][MAXDUNY];		// Player
extern int dMonster[MAXDUNX][MAXDUNY];		// Monster
extern char dDead[MAXDUNX][MAXDUNY];		// Dead plr/monster
extern char dObject[MAXDUNX][MAXDUNY];		// Objects
extern char dItem[MAXDUNX][MAXDUNY];		// Items
extern char dMissile[MAXDUNX][MAXDUNY];		// Missiles (0 = none, # = missile, -1 = two or more)
extern char dSpecial[MAXDUNX][MAXDUNY];		// Second layer of tiling
extern int themeCount;
extern THEME_LOC themeLoc[50];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

int CalcRot(int x, int y);
void SetDungeonMicros();
void FillSolidBlockTbls ();

void DRLG_InitTrans();
void DRLG_MRectTrans(int, int, int, int);
void DRLG_MCopyTrans(int, int, int, int, BOOL, BOOL, BOOL, BOOL);
void DRLG_RectTrans(int, int, int, int);
void DRLG_CopyTrans(int, int, int, int);
void DRLG_ListTrans(int, byte *);
void DRLG_AreaTrans(int, byte *);
void DRLG_InitSetPC();
void DRLG_SetPC();
void Make_SetPC(int x, int y, int w, int h);
void DrawDungMiniMap(unsigned char floor);
BOOL DRLG_WillThemeRoomFit(int, int, int, int, int, int *, int *);
void DRLG_CreateThemeRoom(int);
void DRLG_PlaceThemeRooms(int, int, int, int, BOOL);
void DRLG_HoldThemeRooms();
BOOL SkipThemeRoom(int, int);
