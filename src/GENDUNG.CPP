/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 General Dungeon file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/GENDUNG.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
** FillDungeonInfo
** DRLG
** MakeDungeonLayout
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"gendung.h"
#include	"engine.h"
#include	"lighting.h"
#include	"palette.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"objects.h"
#include	"missiles.h"
#include	"spells.h"

/*-----------------------------------------------------------------------**
** Global variables
**-----------------------------------------------------------------------*/

byte	dungeon[MDMAXX][MDMAXY];
byte	pdungeon[MDMAXX][MDMAXY];
byte	dflags[MDMAXX][MDMAXY];
int setpc_x, setpc_y, setpc_w, setpc_h;		// Where the important set piece of a level is
byte	*pSetPiece;
BOOL setloadflag;

BYTE *pSpecialCels;
BYTE *pMegaTiles;
BYTE *pMiniTiles;
extern "C" {
	BYTE *pDungeonCels;
	BYTE *pSpeedCels;
	long microoffset[MAXMREND][16];
	byte nWTypeTable[MAXTILES+1];
}

int mcount[MAXMICRO];
int mlist[MAXMICRO];
//WORD mtype[MAXMICRO];
static WORD mtype[MAXMICRO];	// resolve ambiguity with monsters JKE 7/30
int msize[MAXMICRO];
int nummicros;

BYTE nBlockTable[MAXTILES+1];
BYTE nSolidTable[MAXTILES+1];
BYTE nTransTable[MAXTILES+1];
BYTE nMissileTable[MAXTILES+1];
BYTE nTrapTable[MAXTILES+1];

int	dminx, dminy, dmaxx, dmaxy;

int gnDifficulty;

BYTE leveltype;
BYTE currlevel;
BYTE setlevel;
BYTE setlvlnum;
BYTE setlvltype;

int ViewX, ViewY;
int ViewDX, ViewDY;
int ViewBX, ViewBY;
ScrollStruct	ScrollInfo;

int LvlViewX, LvlViewY;

int btmbx, btmby;
int btmdx, btmdy;

int MicroTileLen;

char TransVal;
BYTE TransList[256];

int dPiece[MAXDUNX][MAXDUNY];		// Tile #

MICROS dMT[MAXDUNX][MAXDUNY];		// Micro Tiles
MICROS dMT2[(MAXDUNX)*(MAXDUNY)];

char dTransVal[MAXDUNX][MAXDUNY];	// Transparent active value
char dLight[MAXDUNX][MAXDUNY];		// Current light value
char dSaveLight[MAXDUNX][MAXDUNY];	// Static light value
char dFlags[MAXDUNX][MAXDUNY];		// Flags for Solid collision, etc.
char dPlayer[MAXDUNX][MAXDUNY];		// Player
int dMonster[MAXDUNX][MAXDUNY];		// Monster
char dDead[MAXDUNX][MAXDUNY];		// Dead plr/monster
char dObject[MAXDUNX][MAXDUNY];		// Objects
char dItem[MAXDUNX][MAXDUNY];		// Items
char dMissile[MAXDUNX][MAXDUNY];	// Missiles
char dSpecial[MAXDUNX][MAXDUNY];	// Second layer of tiling
int themeCount;						// Theme room count
THEME_LOC themeLoc[50];				// Theme room array

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void FillSolidBlockTbls() {
	DWORD dwTiles;
	BYTE *pSBFile;

	ZeroMemory(nBlockTable,sizeof(nBlockTable));
	ZeroMemory(nSolidTable,sizeof(nSolidTable));
	ZeroMemory(nTransTable,sizeof(nTransTable));
	ZeroMemory(nMissileTable,sizeof(nMissileTable));
	ZeroMemory(nTrapTable,sizeof(nTrapTable));

	switch (leveltype) {
		case 0:
			pSBFile = LoadFileInMemSig("NLevels\\TownData\\Town.SOL",&dwTiles,'SOL ');
		break;
		case 1:
			if (currlevel < 17)
				pSBFile = LoadFileInMemSig("Levels\\L1Data\\L1.SOL",&dwTiles,'SOL ');
			else
				pSBFile = LoadFileInMemSig("NLevels\\L5Data\\L5.SOL",&dwTiles,'SOL ');	//JKE Attributes loaded

		break;
		case 2:
			pSBFile = LoadFileInMemSig("Levels\\L2Data\\L2.SOL",&dwTiles,'SOL ');
		break;
		case 3:
			if (currlevel < 17)
				pSBFile = LoadFileInMemSig("Levels\\L3Data\\L3.SOL",&dwTiles,'SOL ');
			else
				pSBFile = LoadFileInMemSig("NLevels\\L6Data\\L6.SOL",&dwTiles,'SOL ');	//JKE Attributes loaded
		break;
		case 4:
			pSBFile = LoadFileInMemSig("Levels\\L4Data\\L4.SOL",&dwTiles,'SOL ');
		break;
		default:
			app_fatal("FillSolidBlockTbls");
		break;
	}

	BYTE * pTmp = pSBFile;
	for (DWORD d = 1; d <= dwTiles; d++) {
		BYTE bv = *pTmp++;
		if ((bv & 0x01) != 0) nSolidTable[d] = TRUE;
		if ((bv & 0x02) != 0) nBlockTable[d] = TRUE;
		if ((bv & 0x04) != 0) nMissileTable[d] = TRUE;
		if ((bv & 0x08) != 0) nTransTable[d] = TRUE;
		if ((bv & 0x80) != 0) nTrapTable[d] = TRUE;
		nWTypeTable[d] = (bv & 0x70) >> 4;
	}

	DiabloFreePtr(pSBFile);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void bs_swap(int a, int b) {
	int		v;

	v = mcount[a];
	mcount[a] = mcount[b];
	mcount[b] = v;
	v = mlist[a];
	mlist[a] = mlist[b];
	mlist[b] = v;
	v = mtype[a];
	mtype[a] = mtype[b];
	mtype[b] = v;
	v = msize[a];
	msize[a] = msize[b];
	msize[b] = v;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void bubblesort(int n) {
	int j,k,sorted;

	k = n;
	sorted = 0;

	while ((k > 0) && (sorted == 0)) {
		sorted = 1;
		for (j = 0; j < k; j++) {
			if (mcount[j] < mcount[j+1]) {
				bs_swap(j, j+1);
				sorted = 0;
			}
		}
		k--;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void PreRendMicros()
{
	long i,j,llvls;
	WORD k;
	long nPNum;
	long tl, tms, total;
	int numrend;
	WORD *mt;
	int t, tlen;
	int ok2rend;

	for (i = 0; i < MAXMICRO; i++) {
		mlist[i] = i;
		mcount[i] = 0;
		mtype[i] = 0;
	}

	if (leveltype != 4) tlen = 10;
	else tlen = 12;
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			mt = &(dMT[i][j].mt[0]);
			for(t = 0; t < tlen; t++)
			{
				nPNum = mt[t];
				if (nPNum) {
					mcount[nPNum & 0xfff]++;
					mtype[nPNum & 0xfff] = nPNum & 0x7000;
				}
			}
		}
	}

	__asm {
		mov		ebx,dword ptr [pDungeonCels]
		mov		eax,dword ptr [ebx]
		mov		dword ptr [tms],eax
	}
	nummicros = tms & 0xffff;

	for (i = 0; i < nummicros; i++) {
		nPNum = i;
		__asm {
			mov		ebx,dword ptr [pDungeonCels]
			mov		eax,dword ptr [nPNum]
			shl		eax,2
			add		ebx,eax
			mov		eax,dword ptr [ebx+4]				// Size
			sub		eax,dword ptr [ebx]
			mov		dword ptr [tms],eax
		}
		msize[i] = tms & 0xffff;
	}
	msize[0] = 0;		// Zero doesn't exist


	// On level type 4 we cycle the lava using the translation tables so it can't
	// be prerendered
	if (leveltype == 4) {
		for (i = 0; i < nummicros; i++) {
			nPNum = i;
			ok2rend = 1;
			if (mcount[i] == 0) continue;
			if (mtype[i] != 0x1000) {
				tl = msize[i];
				__asm {
					mov		ebx,dword ptr [pDungeonCels]
					mov		eax,dword ptr [nPNum]
					shl		eax,2
					add		ebx,eax
					mov		esi,dword ptr [pDungeonCels]
					add		esi,dword ptr [ebx]				// Source

					xor		ebx,ebx

					mov		ecx,dword ptr [tl]				// Number of bytes to translate
					jecxz	_XSkip
_XLp1:				lodsb
					cmp		al,0
					je		_XOk1
					cmp		al,32
					jae		_XOk1
					mov		dword ptr [ok2rend],ebx
_XOk1:				loop	_XLp1
_XSkip:				nop
				}
			} else {
				__asm {
					mov		ebx,dword ptr [pDungeonCels]
					mov		eax,dword ptr [nPNum]
					shl		eax,2
					add		ebx,eax
					mov		esi,dword ptr [pDungeonCels]
					add		esi,dword ptr [ebx]				// Source

					xor		ebx,ebx

					mov		ecx,32
_X1Lp1:				push	ecx
					mov		edx,32
_X1Lp2:				xor		eax,eax				// Load control byte
					lodsb
					or		al,al
					js		_X1J

					sub		edx,eax

					mov		ecx,eax
_X1Lp3:				lodsb
					cmp		al,0
					je		_XOk2
					cmp		al,32
					jae		_XOk2
					mov		dword ptr [ok2rend],ebx
_XOk2:				loop	_X1Lp3
					or		edx,edx
					jz		_X1Nxt
					jmp		_X1Lp2

_X1J:				neg		al					// Do jump
					sub		edx,eax
					jnz		_X1Lp2
_X1Nxt:				pop		ecx
					loop	_X1Lp1
				}
			}
			if (ok2rend == 0) mcount[i] = 0;
		}
	}

	bubblesort(MAXMICRO-1);

	total = 0;
	numrend = 0;
	if (light4flag) {
		while (total < 1048576) {
			total = total + ((long)msize[numrend] << 1);
			numrend++;
		}
	} else {
		while (total < 1048576) {
			total = total + ((long)msize[numrend] << 4) - ((long)msize[numrend] << 1);
			numrend++;
		}
	}
	numrend--;

	if (numrend > MAXMREND) numrend = MAXMREND;

	// Pre render the most used
	total = 0;
	if (light4flag) llvls = 3;
	else llvls = 15;
	for (j = 0; j < numrend; j++) {
		nPNum = mlist[j];
		microoffset[j][0] = nPNum;
		if (mtype[j] != 0x1000) {
			tl = msize[j];
			// Pre render for each light level
			for (i = 1; i < llvls; i++) {
				microoffset[j][i] = total;
				__asm {
					mov		ebx,dword ptr [pDungeonCels]
					mov		eax,dword ptr [nPNum]
					shl		eax,2
					add		ebx,eax
					mov		esi,dword ptr [pDungeonCels]
					add		esi,dword ptr [ebx]				// Source

					mov		edi,dword ptr [pSpeedCels]
					add		edi,dword ptr [total]

					mov		ebx,dword ptr [i]				// Light conversion table
					shl		ebx,8
					add		ebx,dword ptr [pLightTbl]

					mov		ecx,dword ptr [tl]				// Number of bytes to translate
					jecxz	_Skip

_Lp1:				lodsb
					xlatb
					stosb
					loop	_Lp1
_Skip:				nop
				}
				total += tl;
			}
		} else {
			// Pre render for each light level
			for (i = 1; i < llvls; i++) {
				microoffset[j][i] = total;
				__asm {
					mov		ebx,dword ptr [pDungeonCels]
					mov		eax,dword ptr [nPNum]
					shl		eax,2
					add		ebx,eax
					mov		esi,dword ptr [pDungeonCels]
					add		esi,dword ptr [ebx]				// Source

					mov		edi,dword ptr [pSpeedCels]
					add		edi,dword ptr [total]

					mov		ebx,dword ptr [i]				// Light conversion table
					shl		ebx,8
					add		ebx,dword ptr [pLightTbl]

					mov		ecx,32
_T1Lp1:				push	ecx
					mov		edx,32
_T1Lp2:				xor		eax,eax				// Load control byte
					lodsb
					stosb
					or		al,al
					js		_T1J

					sub		edx,eax

					mov		ecx,eax
_T1Lp3:				lodsb
					xlatb
					stosb
					loop	_T1Lp3
					or		edx,edx
					jz		_T1Nxt
					jmp		_T1Lp2

_T1J:				neg		al					// Do jump
					sub		edx,eax
					jnz		_T1Lp2
_T1Nxt:				pop		ecx
					loop	_T1Lp1
				}
				total += msize[j];
			}
		}
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dPiece[i][j] != 0) {
				mt = &(dMT[i][j].mt[0]);
				for(t = 0; t < tlen; t++)
				{
					if (mt[t]) {
						for (k = 0; k < numrend; k++) {
							if ((mt[t] & 0xfff) == mlist[k]) {
								mt[t] = mtype[k] + 0x8000 + k;
								k = numrend;
							}
						}
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

int CalcRot(int x, int y)
{
	int rot;

	if(x < DMAXX - y)
		rot = ((y*y+y) + ((2*y + x + 3)*x))/2;
	else
	{
		x = DMAXX - x - 1;
		y = DMAXY - y - 1;
		rot = (DMAXX)*(DMAXY)-(1 + ((y*y+y) + ((2*y + x + 3)*x))/2);
	}
	return rot;
}

/*-----------------------------------------------------------------------**
 * RotateMicros
 *
 * Rotates the dMT array by 45 degrees, so that when drawn to the screen,
 * the elements are read sequentially.  Previously, DrawHTileLine had to
 * scan through dMT diagonally, which causes a lot of caching.
**-----------------------------------------------------------------------*/

void RotateMicros()
{
	int x,y;

	for(x = 0; x < DMAXX; x++)
	{
		for(y = 0; y < DMAXX; y++)
		{
				dMT2[CalcRot(x,y)] = dMT[x][y];
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetDungeonMicros()
{
	int	wv;
	int i,j;
	WORD *mtsource;
	int t;
	WORD *mt;
	int tlen;

	if (leveltype != 4) {
		MicroTileLen = 10;
		tlen = 10;
	} else {
		MicroTileLen = 12;
		tlen = 16;
	}
	// Init the light values for each piece
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			wv = dPiece[i][j];
			mt = &(dMT[i][j].mt[0]);
			if (wv != 0) {
				wv--;
				if (leveltype != 4) mtsource = (WORD *)(pMiniTiles + 20*wv);
				else mtsource = (WORD *)(pMiniTiles + 32*wv);
				for(t = 0; t < tlen; t++)
					// MiniTiles array uses opposite y direction
					// hence wierd index on next line
					mt[t] = mtsource[(tlen-2)-(t&0xe)+(t&1)];
			}
			else
			{
				for(t = 0; t < tlen; t++)
					mt[t] = 0;
			}
		}
	}

	PreRendMicros();

	RotateMicros();

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

void DRLG_InitTrans()
{
	// Init the transparent values for each piece
	ZeroMemory(dTransVal,sizeof(dTransVal));
	ZeroMemory(TransList,sizeof(TransList));
	// JKE TransVal = 1;
	TransVal = 1;
}

/*-----------------------------------------------------------------------**
** Do Transparent values on megatiles
**-----------------------------------------------------------------------*/

void DRLG_MRectTrans(int x1, int y1, int x2, int y2)
{
	int	i,j;

	x1 = (x1 << 1) + DIRTEDGED2 + 1;
	y1 = (y1 << 1) + DIRTEDGED2 + 1;
	x2 = (x2 << 1) + DIRTEDGED2;
	y2 = (y2 << 1) + DIRTEDGED2;
	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) dTransVal[i][j] = TransVal;
	}
	TransVal++;
}

/*-----------------------------------------------------------------------**
** Copy Transparent values on megatiles level with 4 flags for mini
**-----------------------------------------------------------------------*/

void DRLG_MCopyTrans(int sx, int sy, int dx, int dy, BOOL ul, BOOL ur, BOOL ll, BOOL lr)
{
	char v;

	sx = (sx << 1) + DIRTEDGED2;
	sy = (sy << 1) + DIRTEDGED2;
	v = dTransVal[sx][sy];
	dx = (dx << 1) + DIRTEDGED2;
	dy = (dy << 1) + DIRTEDGED2;
	if (ul) dTransVal[dx][dy] = v;
	if (ur) dTransVal[dx+1][dy] = v;
	if (ll) dTransVal[dx][dy+1] = v;
	if (lr) dTransVal[dx+1][dy+1] = v;
}

/*-----------------------------------------------------------------------**
** Do Transparent values on minitiles
**-----------------------------------------------------------------------*/

void DRLG_RectTrans(int x1, int y1, int x2, int y2)
{
	int	i,j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) dTransVal[i][j] = TransVal;
	}
	TransVal++;
}

/*-----------------------------------------------------------------------**
** Copy Transparent values on minitile level
**-----------------------------------------------------------------------*/

void DRLG_CopyTrans(int sx, int sy, int dx, int dy)
{
	dTransVal[dx][dy] = dTransVal[sx][sy];
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DRLG_ListTrans(int num, byte *List)
{
	int i;
	byte x1,y1,x2,y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DRLG_AreaTrans(int num, byte *List)
{
	int i;
	byte x1,y1,x2,y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
		TransVal--;
	}
	TransVal++;
}

/*-----------------------------------------------------------------------**
** Fuckin all problems solved with this routine
**-----------------------------------------------------------------------*/

void DRLG_InitSetPC()
{
	setpc_x = 0;
	setpc_y = 0;
	setpc_w = 0;
	setpc_h = 0;
}

/*-----------------------------------------------------------------------**
** Don't put rnd monsters, objects in set piece areas
**-----------------------------------------------------------------------*/

void DRLG_SetPC()
{
	int i,j;
	int x,y,w,h;

	w = setpc_w << 1;
	h = setpc_h << 1;
	x = (setpc_x << 1) + DIRTEDGED2;
	y = (setpc_y << 1) + DIRTEDGED2;
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) dFlags[x+i][y+j] |= BFLAG_SETPC;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Make_SetPC(int x, int y, int w, int h)
{
	int i,j;
	int dx, dy, dh, dw;

	dw = w << 1;
	dh = h << 1;
	dx = (x << 1) + DIRTEDGED2;
	dy = (y << 1) + DIRTEDGED2;
	for (j = 0; j < dh; j++) {
		for (i = 0; i < dw; i++) dFlags[dx+i][dy+j] |= BFLAG_SETPC;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawDungMiniMap(unsigned char floor)
/*-----------------------------------------------------------------------**
** DESCRIPTION: Draws a mini map of the dungeon and player locations.
** INPUT: floor = The floor type to look for
** RETURN: None
/*-----------------------------------------------------------------------*/
{
	int				*p;
	byte			*d;
	unsigned int	pSize;
	int				pIndex,	pXMega, pYMega, pX, pY;

	p = &plr[0]._px;
	d = &dungeon[0][0];
	pSize = sizeof(PlayerStruct);
	
	//*NOTE* The x and y coords had to be swapped in the compares to ecx and edx
	//       to get the proper output coordinates for the players.

	app_assert(gpBuffer);
	__asm {
		mov		esi,dword ptr [d]
		mov		edi,dword ptr [gpBuffer]
		add		edi,135248
		mov		edx,MDMAXY
_YLp:	
		push	edi
		mov		ecx,MDMAXX
_XLp:	
		lodsb								//Read byte from dungeon array

//Start Search for Player coords
		push	eax
		push	esi
		push	edi
		mov		esi,dword ptr [p]			//PlayerStruct._px
		mov		edi,0						//Initialize plr array index
LoopThruPlayers:
		lodsd								//Get _px
		mov		pX,eax						//Fill in pX
		lodsd								//Get _py
		mov		pY,eax 						//Fill in pY
		add		esi,pSize					//Add sizeof(PlayerStruct)
		sub		esi,8						//Subtract 2 dwords read
		cmp		pX,0						//Ignore 0,0 coords
		jnz		PNotOrigin
		cmp		pY,0						//Ignore 0,0 coords
		jz		PNoX
PNotOrigin:
		mov		pIndex,edi					//Save plr index
		jmp		PMiniToMega					//Convert mini tiles to mega tiles
PMegaFound:
		cmp		edx,pXMega					//Did we match a players x coord?
		jnz		PNoX
		cmp		ecx,pYMega					//Did we match a players y coord?
		jz		Player
PNoX:
		inc		edi							//Increment index
		cmp		edi,MAX_PLRS				//Are we done with our for loop?
		jl		LoopThruPlayers				
		pop		edi
		pop		esi
		pop		eax
//End Search for Player coords

		cmp		al,floor					//Did we find a floor piece?
		jz		NoSave						//Let background show thru
		jmp		Other						//We found a wall or something
Player:	
		pop		edi
		pop		esi
		pop     eax
		cmp		pIndex,0					//Are we dealing with player 0?
		jnz		Player1
		mov		al,240						//Set color to white
		jmp		Save
Player1:
		cmp		pIndex,1					//Are we dealing with player 1?
		jnz		Player2
		mov		al,139						//Set color to red
		jmp		Save
Player2:
		cmp		pIndex,2					//Are we dealing with player 2?
		jnz		Player3
		mov		al,154						//Set color to orange
		jmp		Save
Player3:									//We are dealing with player 3
		mov		al,147						//Set color to yellow
		jmp		Save
Other:	
		mov		al,131						//Set color to blue
Save:	
		mov		byte ptr [edi],al			
		mov		byte ptr [edi+1],al
		mov		byte ptr [edi+768],al
		mov		byte ptr [edi+769],al
NoSave:
		add		edi,1536
		dec		ecx
		jnz		_XLp
		pop		edi
		add		edi,2
		dec		edx
		jnz		_YLp
		jmp		EndL2DrawDung

//Start Convert player mini tiles to mega tiles
PMiniToMega:
		push	eax
		push	edx
		mov		eax,pX													
		sub		eax,DIRTEDGED2				
		shr		eax,1						
		mov		edx,MDMAXX					
		sub		edx,eax						
		mov		pXMega,edx
		mov		eax,pY
		sub		eax,DIRTEDGED2
		shr		eax,1
		mov		edx,MDMAXY
		sub		edx,eax
		mov		pYMega,edx
		pop		edx
		pop		eax
		jmp		PMegaFound
//End Convert player mini tiles to mega tiles

EndL2DrawDung:
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL DRLG_WillThemeRoomFit(int floor,
		 				   int x,
		                   int y,
			  	           int minSize,
			  		       int maxSize,
			  	           int *width,
			               int *height)
/*-----------------------------------------------------------------------**
** DESCRIPTION: Checks if a theme room  between minSize and maxSize will fit at
**              (x, y).
** INPUT: floor   = Floor value to look for
**        x, y    = Top left corner to start search from
**        minSize = Min sized room to create. This will actually create a room of 
**                  size (minSize - 2) to account for a hall, 1 megatile wide, 
**                  around the room.
**        maxSize = Max sized room to create. This will actually create a room of 
**                  size (maxSize - 2) to account for a hall, 1 megatile wide,
**                  around the room.
**        *width  = Actual room width return value
**        *height = Actual room height return value
**
** RETURN:  FALSE = Could not fit a room with specified parameters
**          TRUE  = Room dimensions were found
/*-----------------------------------------------------------------------*/
{
	int		ii, xx, yy;
	int		xSmallest, ySmallest;
	int		xArray[20];
	int		yArray[20];
	int		xCount = 0;
	int		yCount = 0;
	BOOL	yFlag = TRUE;
	BOOL	xFlag = TRUE;

	//Check to see if we are within the bounds of the dungeon[40][40] array
	if ((x > (MDMAXX - maxSize)) && (y > (MDMAXY - maxSize)))
		return FALSE;

	//Skip any existing theme rooms
	if (!SkipThemeRoom(x, y))
		return FALSE;

	//Intialize arrays
	memset(xArray, 0x00, sizeof(xArray));
	memset(yArray, 0x00, sizeof(yArray));

	//Find the lengths of each row and column
	for (ii = 0; ii < maxSize; ii++) {
		//Find the row lengths
		if (xFlag) { 
			for (xx = x; xx < (x + maxSize); xx++) {
				//We ran into a wall or something
				if (dungeon[xx][y+ii] != floor) {
					if (xx < minSize) xFlag = FALSE;
					else break;
				} else xCount++;
			} 
			if (xFlag) {
				xArray[ii] = xCount;
				xCount = 0;
			} 
		} 
		//Find the column lengths
		if (yFlag) {
			for (yy = y; yy < (y + maxSize); yy++) {
				//We ran into a wall or something
				if (dungeon[x+ii][yy] != floor) {
					if (yy < minSize) yFlag = FALSE;
					else break;
				} else yCount++;
			}
			if (yFlag) {
				yArray[ii] = yCount;
				yCount = 0;
			} 
		} 
	} 
	//Make sure we meet the minimum requirements
	for (ii = 0; ii < minSize; ii++) {
		if (xArray[ii] < minSize || yArray[ii] < minSize) return FALSE;
	} 
	//Initialize
	xSmallest = xArray[0];
	ySmallest = yArray[0];
	//Find the best x and y values
	for (ii = 0; ii < maxSize; ii++) {	
		if (xArray[ii] >= minSize && yArray[ii] >= minSize) {
			if (xArray[ii] < xSmallest) xSmallest = xArray[ii];
			if (yArray[ii] < ySmallest)	ySmallest = yArray[ii];
		} else break;
	} 
	//Set the room dimenions
	*width = xSmallest - 2; //Subtract 2 to acount for hall around room
	*height = ySmallest - 2; //Subtract 2 to acount for hall around room

	return TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DRLG_CreateThemeRoom(int themeIndex)
/*-----------------------------------------------------------------------**
** DESCRIPTION: Creates themeIndex theme room in the themeLoc array.
** INPUT: themeIndex = Index into themeLoc array
** RETURN: None
/*-----------------------------------------------------------------------*/
{
	int xx;
	int yy;

	//Add the walls
	for (yy = themeLoc[themeIndex].y; yy < (themeLoc[themeIndex].y + themeLoc[themeIndex].height); yy++) {
		for (xx = themeLoc[themeIndex].x; xx < (themeLoc[themeIndex].x + themeLoc[themeIndex].width); xx++) {
			if (leveltype == 2) {
				//TOP/BOTTOM WALLS
				if ((yy == themeLoc[themeIndex].y && (xx >= themeLoc[themeIndex].x && (xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width))) ||
					((yy == themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)) && (xx >= themeLoc[themeIndex].x && (xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width))))
					dungeon[xx][yy] = 2; //HWALL_PIECE
				//RIGHT/LEFT WALLS
				else if ((xx == themeLoc[themeIndex].x && (yy >= themeLoc[themeIndex].y && (yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height))) ||
						 ((xx == themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)) && (yy >= themeLoc[themeIndex].y && (yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height))))
						 dungeon[xx][yy] = 1; //VWALL_PIECE
				//FLOOR PIECE
				else dungeon[xx][yy] = 3; //FLOOR_PIECE
			} 
			if (leveltype == 3) {
				//UPPER/LOWER WALL
				if ((yy == themeLoc[themeIndex].y && (xx >= themeLoc[themeIndex].x && (xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width))) ||
					((yy == themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)) && (xx >= themeLoc[themeIndex].x && (xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width))))
					dungeon[xx][yy] = 134; //WOOD_HORIZWALL
				//RIGHT/LEFT WALL
				else if ((xx == themeLoc[themeIndex].x && (yy >= themeLoc[themeIndex].y && (yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height))) ||
						 ((xx == themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)) && (yy >= themeLoc[themeIndex].y && (yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height))))
					dungeon[xx][yy] = 137; //WOOD_VERTWALL
				//FLOOR PIECE
				else  dungeon[xx][yy] = 7; //FLOOR
			} 
			if (leveltype == 4) {
				//UPPER/LOWER WALL
				if ((yy == themeLoc[themeIndex].y && (xx >= themeLoc[themeIndex].x && (xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width))) ||
					((yy == themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)) && (xx >= themeLoc[themeIndex].x && (xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width))))
					dungeon[xx][yy] = 2; //HWALL_PIC
				//RIGHT/LEFT WALL
				else if ((xx == themeLoc[themeIndex].x && (yy >= themeLoc[themeIndex].y && (yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height))) ||
						 ((xx == themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)) && (yy >= themeLoc[themeIndex].y && (yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height))))
					dungeon[xx][yy] = 1; //VWALL_PIC
				//FLOOR PIECE
				else  dungeon[xx][yy] = 6; //FLOOR_PIC
			} 
		} 
	} 

	//Add the corners
	if (leveltype == 2) {
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y] = 8; //ULWALL_PIECE
		dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y] = 7; //URWALL_PIECE
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 9; //LLWALL_PIECE
		dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 6; //LRWALL_PIECE
	} 
	if (leveltype == 3) {
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y] = 150; //WOOD_ULCORNER
		dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y] = 151; //WOOD_URCORNER
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 152; //WOOD_LLCORNER
		dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 138; //WOOD_LRCORNER
	} 
	if (leveltype == 4) {
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y] = 9; //ULWALL_PIC
		dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y] = 16; //URWALL_PIC
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 15; //LLWALL_PIC
		dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 12; //LRWALL_PIC
	} 

	//Add the door to the east or south wall
	if (leveltype == 2) {
		switch(random(0, 2)) {
			//East wall
			case 0 : dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height / 2)] = 4; //VDOOR_PIECE
					 break;
			//South wall
			case 1 : dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width / 2)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 5; //HDOOR_PIECE
					 break;
		} 
	} 
	if (leveltype == 3) {
		switch(random(0, 2)) {
			//East wall
			case 0 : dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height / 2)] = 147; //WOOD_VERTGATE
					 break;
			//South wall
			case 1 : dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width / 2)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 146; //WOOD_HORIZGATE
					 break;
		} 
	} 
	if (leveltype == 4) {
		switch(random(0, 2)) {
			//East wall
			case 0 : dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][(themeLoc[themeIndex].y + (themeLoc[themeIndex].height / 2))-1] = 53; //VERT RIGHT
					 dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height / 2)] = 6; //FLOOR_PIC
					 dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-1)][(themeLoc[themeIndex].y + (themeLoc[themeIndex].height / 2))+1] = 52; //VERT LEFT
					 //Add Shadows
					 dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width-2)][(themeLoc[themeIndex].y + (themeLoc[themeIndex].height / 2))-1] = 54; 
					 break;
			//South wall
			case 1 : dungeon[(themeLoc[themeIndex].x + (themeLoc[themeIndex].width / 2))-1][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 57; //HORIZ LEFT
					 dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width / 2)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 6; //FLOOR_PIC
					 dungeon[(themeLoc[themeIndex].x + (themeLoc[themeIndex].width / 2))+1][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-1)] = 56; //HORIZ RIGHT
					 //Add Shadows
					 dungeon[themeLoc[themeIndex].x + (themeLoc[themeIndex].width / 2)][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-2)] = 59; 
					 dungeon[(themeLoc[themeIndex].x + (themeLoc[themeIndex].width / 2))-1][themeLoc[themeIndex].y + (themeLoc[themeIndex].height-2)] = 58; 
					 break;
		} 
	} 
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DRLG_PlaceThemeRooms(int minSize,
						  int maxSize,
						  int floor,
						  int freq,
						  BOOL rndSize)
/*-----------------------------------------------------------------------**
** DESCRIPTION: Places theme rooms in dungeon types 2,3 & 4.
** INPUT: minSize  = The minimum sized room to create
**        maxSize  = The maximum sized room to create
**        floor    = The floor value to look for
**        freq     = The frequency to check for theme rooms. Set to 0 if you 
**                   want to check everything. 
**        rndSize  = If TRUE, then create rooms between max and min. If FALSE,
**                   then the largest theme rooms in an area will be created.
** RETURN: None
/*-----------------------------------------------------------------------*/
{
	int	i;
	int	j;
	int	themeW;
	int	themeH;
	
	//Initialize
	themeCount = 0;
	memset(themeLoc, 0x00, sizeof(THEME_LOC));
	
	//Loop thru dungeon array
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			//Did we find a floor piece?
			if ((dungeon[i][j] == floor) && (!random(0, freq))) {
				//Check to see if theme room can fit
				if (DRLG_WillThemeRoomFit(floor, i, j, minSize, maxSize, &themeW, &themeH)) {
					//Do we want a random room size between max and min?
					if (rndSize) {
						int rv1, rv2, min, max;
						min = minSize - 2;
						max = maxSize - 2;
						//Get a random width
						rv1 = random(0, ((themeW-min)+1));
						rv2 = min + random(0, rv1);
						if (rv2 < min || rv2 > max) themeW = min;
						else themeW = rv2;
						//Get a random height
						rv1 = random(0, ((themeH-min)+1));
						rv2 = min + random(0, rv1);
						if (rv2 < min || rv2 > max) themeH = min;
						else themeH = rv2;
					}
					//Update the theme array
					themeLoc[themeCount].x = i+1; 
					themeLoc[themeCount].y = j+1;
					themeLoc[themeCount].width = themeW;
					themeLoc[themeCount].height = themeH;
					//Get transparency value
					//Had to add special case in for L3
					if (leveltype == 3) {
						DRLG_RectTrans(
							((i+2) << 1) + DIRTEDGED2,
							((j+2) << 1) + DIRTEDGED2,
							(((i+themeW)-1) << 1) + DIRTEDGED2+1,
							(((j+themeH)-1) << 1) + DIRTEDGED2+1);
					} else DRLG_MRectTrans(i+1, j+1, (i+themeW), (j+themeH));
					themeLoc[themeCount].ttval = TransVal-1;
					//Add the new theme room to the dungeon
					DRLG_CreateThemeRoom(themeCount);
					//Increment theme count
					themeCount++;
				}
			} 
		} 
	} 
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DRLG_HoldThemeRooms()
/*-----------------------------------------------------------------------**
** DESCRIPTION: Holds theme rooms created in dungeon types 2,3 & 4.
** INPUT: None
** RETURN: None
/*-----------------------------------------------------------------------*/
{
	int i, x, y, xx, yy;

	if (themeCount > 0) {
		for (i = 0; i < themeCount; i++) {
			for (y = themeLoc[i].y; y < (themeLoc[i].y + themeLoc[i].height)-1; y++) {
				for (x = themeLoc[i].x; x < (themeLoc[i].x + themeLoc[i].width)-1; x++) {
					//Mega tiles to mini tiles
					xx = (x << 1) + DIRTEDGED2;
					yy = (y << 1) + DIRTEDGED2;
					dFlags[xx][yy] |= BFLAG_SETPC;
					dFlags[xx+1][yy] |= BFLAG_SETPC;
					dFlags[xx][yy+1] |= BFLAG_SETPC;
					dFlags[xx+1][yy+1] |= BFLAG_SETPC;
				}
			} 
		} 
	} 
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL SkipThemeRoom( int x, int y )
/*-----------------------------------------------------------------------**
** DESCRIPTION: Skips any existing theme rooms
** INPUT: None
** RETURN: TRUE  = If not in a theme room
**         FALSE = If in a theme room
/*-----------------------------------------------------------------------*/
{
	int	i;

	//Skip all L2, L3, & L4 themes rooms 
	for (i = 0; i < themeCount; i++) {
		if ((x >= themeLoc[i].x-2 && x <= (themeLoc[i].x + themeLoc[i].width)+2) &&
			(y >= themeLoc[i].y-2 && y <= (themeLoc[i].y + themeLoc[i].height)+2))
			//We are inside a theme room
			return FALSE;
	} 

	return TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitLevels()
{
	if (! leveldebug) {
		currlevel = 0;
		leveltype = 0;
		setlevel = FALSE;
	}
}

