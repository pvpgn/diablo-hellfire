/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  Automap file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/AUTOMAP.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"automap.h"
#include	"engine.h"
#include	"gendung.h"
#include	"scrollrt.h"
#include	"items.h"
#include	"player.h"
#include	"control.h"
#include	"inv.h"
#include	"quests.h"
#include	"multi.h"
#include	"setmaps.h"

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#define AUTOMAPMAX	200
#define AUTOMAPMIN	50
#define AUTOMAPADD	5
#define AUTOMAPST	((AUTOMAPMAX-AUTOMAPMIN)/AUTOMAPADD)+1

#define MAXMEGA		MAXTILES/4

#define AMDC	144
#define AMLC	200
#define AMPC	153		//129

#define AMS_DOORL	0x01
#define AMS_DOORR	0x02
#define AMS_ARCHL	0x04
#define AMS_ARCHR	0x08
#define AMS_GRATEL	0x10
#define AMS_GRATER	0x20
#define AMS_DIRT	0x40
#define AMS_STAIRS	0x80

#define AMS_NONEL	0x15
#define AMS_NONER	0x2a

#define AMS_DIRT8	0x4000		// AMS_DIRT << 8

#define AMS_DIRTLR	0x4007		// Lower right dirt piece

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL automapflag;

int automapscale;
int automaps1, automaps2, automaps3, automaps4, automaps5;
int automapx, automapy;
int amxadd, amyadd;

char automapstbl[AUTOMAPST];

WORD automaptype[MAXMEGA];

BYTE automapview[AUTOMAPX][AUTOMAPY];

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitAutomapOnce()
{
	automapflag = FALSE;
	automapscale = AUTOMAPMIN;
	automaps1 = (automapscale << 6) / 100;
	automaps2 = automaps1 >> 1;
	automaps3 = automaps2 >> 1;
	automaps4 = automaps3 >> 1;
	automaps5 = automaps4 >> 1;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitAutomap()
{
	int i, j, a, v;
	DWORD dwTiles;
	byte *pAFile, *pTmp;

	a = AUTOMAPMIN;
	for (i = 0; i < AUTOMAPST; i++) {
		v = (a << 6) / 100;
		automapstbl[i] = ((320 / v) << 1) + 1;
		if ((320 % v) != 0) automapstbl[i]++;
		if ((320 % v) >= ((a << 5) / 100)) automapstbl[i]++;
		a += AUTOMAPADD;
	}

	ZeroMemory(automaptype,sizeof(automaptype));

	switch (leveltype) {
		case 1:
			if (currlevel < CRYPTSTART)
			{
				pAFile = LoadFileInMemSig("Levels\\L1Data\\L1.AMP",&dwTiles,'AMAP');
				dwTiles /= 2;
			}
			else
			{
				pAFile = LoadFileInMemSig("NLevels\\L5Data\\L5.AMP",&dwTiles,'AMAP');
				dwTiles /= 2;
			}

		break;

		case 2:
			pAFile = LoadFileInMemSig("Levels\\L2Data\\L2.AMP",&dwTiles,'AMAP');
			dwTiles /= 2;
		break;

		case 3:
			if (currlevel < HIVESTART)
			{
				pAFile = LoadFileInMemSig("Levels\\L3Data\\L3.AMP",&dwTiles,'AMAP');
				dwTiles /= 2;
			}
			else
			{
				pAFile = LoadFileInMemSig("NLevels\\L6Data\\L6.AMP",&dwTiles,'AMAP');
				dwTiles /= 2;
			}

		break;

		case 4:
			pAFile = LoadFileInMemSig("Levels\\L4Data\\L4.AMP",&dwTiles,'AMAP');
			dwTiles /= 2;
		break;
		default:
			// get out!
		return;
	}

	pTmp = pAFile;
	for (DWORD d = 1; d <= dwTiles; d++) {
		byte b1 = *pTmp++;
		byte b2 = *pTmp++;
		automaptype[d] = b1 + (b2 << 8);
	}

	DiabloFreePtr(pAFile);

	// Clear automap vision
	ZeroMemory(automapview,sizeof(automapview));

	// Get rid of any residue prevision calls
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++)
			dFlags[i][j] &= BFMASK_AUTOMAP;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void StartAutomap()
{
	amxadd = 0;
	amyadd = 0;
	automapflag = TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AutomapUp()
{
	amxadd--;
	amyadd--;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AutomapDown()
{
	amxadd++;
	amyadd++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AutomapLeft()
{
	amxadd--;
	amyadd++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AutomapRight()
{
	amxadd++;
	amyadd--;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AutomapZoomIn()
{
	if (automapscale < AUTOMAPMAX) {
		automapscale += AUTOMAPADD;
		automaps1 = (automapscale << 6) / 100;
		automaps2 = automaps1 >> 1;
		automaps3 = automaps2 >> 1;
		automaps4 = automaps3 >> 1;
		automaps5 = automaps4 >> 1;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AutomapZoomOut()
{
	if (automapscale > AUTOMAPMIN) {
		automapscale -= AUTOMAPADD;
		automaps1 = (automapscale << 6) / 100;
		automaps2 = automaps1 >> 1;
		automaps3 = automaps2 >> 1;
		automaps4 = automaps3 >> 1;
		automaps5 = automaps4 >> 1;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawAMSquare(int x, int y)
{
	int sx1, sy1, sx2, sy2;

	sx1 = x - automaps2;
	sy1 = y - automaps3;
	sx2 = sx1 + automaps1;
	sy2 = sy1 + automaps2;

	DrawLine(x, sy1, sx1, y, 131);
	DrawLine(x, sy1, sx2, y, 131);
	DrawLine(x, sy2, sx1, y, 131);
	DrawLine(x, sy2, sx2, y, 131);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DrawAMQuarterSquare(int x, int y, int color)
{
	int sx1, sy1, sx2, sy2;

	sx1 = x - (automaps2 / 2);
	sy1 = y - (automaps3 / 2);
	sx2 = sx1 + (automaps1 / 2);
	sy2 = sy1 + (automaps2 / 2);

	DrawLine(x, sy1, sx1, y, color);
	DrawLine(x, sy1, sx2, y, color);
	DrawLine(x, sy2, sx1, y, color);
	DrawLine(x, sy2, sx2, y, color);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawAMShape(int x, int y, WORD shape)
{
	int x1, y1, x2, y2;
	BYTE f;
	BOOL lwf, rwf, llwf, lrwf;

	//DrawAMSquare(x, y);

	f = (shape >> 8) & 0xff;

	if (f & AMS_DIRT) {
		DrawPoint(x, y, AMLC);
		DrawPoint(x - automaps4, y - automaps5, AMLC);
		DrawPoint(x - automaps4, y + automaps5, AMLC);
		DrawPoint(x + automaps4, y - automaps5, AMLC);
		DrawPoint(x + automaps4, y + automaps5, AMLC);
		DrawPoint(x - automaps3, y, AMLC);
		DrawPoint(x + automaps3, y, AMLC);
		DrawPoint(x, y - automaps4, AMLC);
		DrawPoint(x, y + automaps4, AMLC);

		DrawPoint(x - automaps2 + automaps4, y + automaps5, AMLC);
		DrawPoint(x + automaps2 - automaps4, y + automaps5, AMLC);
		DrawPoint(x - automaps3, y + automaps4, AMLC);
		DrawPoint(x + automaps3, y + automaps4, AMLC);
		DrawPoint(x - automaps4, y + automaps3 - automaps5, AMLC);
		DrawPoint(x + automaps4, y + automaps3 - automaps5, AMLC);
		DrawPoint(x, y + automaps3, AMLC);
	}
	if (f & AMS_STAIRS) {
		DrawLine(x-automaps4, y-automaps4-automaps5, x+automaps3+automaps4, y+automaps5, AMDC);
		DrawLine(x-automaps3, y-automaps4, x+automaps3, y+automaps4, AMDC);
		DrawLine(x-automaps3-automaps4, y-automaps5, x+automaps4, y+automaps4+automaps5, AMDC);
		DrawLine(x-automaps2, y, x, y+automaps3, AMDC);
	}

	lwf = FALSE;
	rwf = FALSE;
	llwf = FALSE;
	lrwf = FALSE;
	switch(shape & 0xf) {
		case 1:
			x1 = x - automaps3;					// Column
			y1 = y - automaps3;
			x2 = x1 + automaps2;
			y2 = y - automaps4;
			DrawLine(x, y1, x1, y2, AMLC);
			DrawLine(x, y1, x2, y2, AMLC);
			DrawLine(x, y, x1, y2, AMLC);
			DrawLine(x, y, x2, y2, AMLC);
			break;
		case 2:
		case 5:
			lwf = TRUE;
			break;
		case 3:
		case 6:
			rwf = TRUE;
			break;
		case 4:
			lwf = TRUE;
			rwf = TRUE;
			break;
		case 8:
			lwf = TRUE;
			llwf = TRUE;
			break;
		case 9:
			rwf = TRUE;
			lrwf = TRUE;
			break;
		case 10:
			llwf = TRUE;
			break;
		case 11:
			lrwf = TRUE;
			break;
		case 12:
			llwf = TRUE;
			lrwf = TRUE;
			break;
	}
	if (lwf) {
		if (f & AMS_DOORL) {
			x1 = x - automaps2;
			x2 = x - automaps3;
			y1 = y - automaps3;
			y2 = y - automaps4;
			DrawLine(x, y1, x-automaps4, y1+automaps5, AMLC);
			DrawLine(x1, y, x1+automaps4, y-automaps5, AMLC);
			DrawLine(x2, y1, x1, y2, AMDC);
			DrawLine(x2, y1, x, y2, AMDC);
			DrawLine(x2, y, x1, y2, AMDC);
			DrawLine(x2, y, x, y2, AMDC);
		}
		if (f & AMS_GRATEL) {
			DrawLine(x-automaps3, y-automaps4, x-automaps2, y, AMLC);
			f |= AMS_ARCHL;	// Force arch square
		}
		if (f & AMS_ARCHL) {
			x1 = x - automaps3;
			y1 = y - automaps3;
			x2 = x1 + automaps2;
			y2 = y - automaps4;
			DrawLine(x, y1, x1, y2, AMLC);
			DrawLine(x, y1, x2, y2, AMLC);
			DrawLine(x, y, x1, y2, AMLC);
			DrawLine(x, y, x2, y2, AMLC);
		}
		if ((f & AMS_NONEL) == 0) DrawLine(x, y-automaps3, x-automaps2, y, AMLC);		// Left wall
	}
	if (rwf) {
		if (f & AMS_DOORR) {
			x1 = x + automaps3;
			x2 = x + automaps2;
			y1 = y - automaps3;
			y2 = y - automaps4;
			DrawLine(x, y1, x+automaps4, y1+automaps5, AMLC);
			DrawLine(x2, y, x2-automaps4, y-automaps5, AMLC);
			DrawLine(x1, y1, x, y2, AMDC);
			DrawLine(x1, y1, x2, y2, AMDC);
			DrawLine(x1, y, x, y2, AMDC);
			DrawLine(x1, y, x2, y2, AMDC);
		}
		if (f & AMS_GRATER) {
			DrawLine(x+automaps3, y-automaps4, x+automaps2, y, AMLC);
			f |= AMS_ARCHR;	// Force arch square
		}
		if (f & AMS_ARCHR) {
			x1 = x - automaps3;
			y1 = y - automaps3;
			x2 = x1 + automaps2;
			y2 = y - automaps4;
			DrawLine(x, y1, x1, y2, AMLC);
			DrawLine(x, y1, x2, y2, AMLC);
			DrawLine(x, y, x1, y2, AMLC);
			DrawLine(x, y, x2, y2, AMLC);
		}
		if ((f & AMS_NONER) == 0) DrawLine(x, y-automaps3, x+automaps2, y, AMLC);		// Right wall
	}
	if (llwf) {
		if (f & AMS_DOORL) {
			x1 = x - automaps2;
			x2 = x - automaps3;
			y1 = y + automaps3;
			y2 = y + automaps4;
			DrawLine(x, y1, x-automaps4, y1-automaps5, AMLC);
			DrawLine(x1, y, x1+automaps4, y+automaps5, AMLC);
			DrawLine(x2, y1, x1, y2, AMDC);
			DrawLine(x2, y1, x, y2, AMDC);
			DrawLine(x2, y, x1, y2, AMDC);
			DrawLine(x2, y, x, y2, AMDC);
		} else
			DrawLine(x, y+automaps3, x-automaps2, y, AMLC);		// Lower Left wall
	}
	if (lrwf) {
		if (f & AMS_DOORR) {
			x1 = x + automaps3;
			x2 = x + automaps2;
			y1 = y + automaps3;
			y2 = y + automaps4;
			DrawLine(x, y1, x+automaps4, y1-automaps5, AMLC);
			DrawLine(x2, y, x2-automaps4, y+automaps5, AMLC);
			DrawLine(x1, y1, x, y2, AMDC);
			DrawLine(x1, y1, x2, y2, AMDC);
			DrawLine(x1, y, x, y2, AMDC);
			DrawLine(x1, y, x2, y2, AMDC);
		} else
			DrawLine(x, y+automaps3, x+automaps2, y, AMLC);		// Lower Right wall
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawAllObjects()
{
	int px, py;
	if (plr[myplr]._pmode == PM_WALK3) {
		px = plr[myplr]._pfutx;
		py = plr[myplr]._pfuty;
		if (plr[myplr]._pdir == DIR_L) px++;
		else py++;
	} else {
		px = plr[myplr]._px;
		py = plr[myplr]._py;
	}
	
	int beginx = px - 8;
	if (beginx < 0)
		beginx = 0;
	else if (beginx > MAXDUNX)
		beginx = MAXDUNX;
		
	int beginy = py - 8;
	if (beginy < 0)
		beginy = 0;
	else if (beginy > MAXDUNY)
		beginy = MAXDUNY;
		
	int endx = px + 8;
	if (endx < 0)
		endx = 0;
	else if (endx > MAXDUNX)
		endx = MAXDUNX;
		
	int endy = py + 8;
	if (endy < 0)
		endy = 0;
	else if (endy > MAXDUNY)
		endy = MAXDUNY;
	
	
	for (int ix=beginx; ix < endx; ++ix)
	{
		for (int iy = beginy; iy < endy; ++iy)
		{
			if (dItem[ix][iy] != 0)
			{
				int dx = ix - ViewX - (amxadd << 1);
				int dy = iy - ViewY - (amyadd << 1);
			
				int x = 384 + (dx * automaps3) - (dy * automaps3);
				int y = 336 + (dx * automaps4) + (dy * automaps4);
		
				x += ((ScrollInfo._sxoff * automapscale) / 100) >> 1;
				y += ((ScrollInfo._syoff * automapscale) / 100) >> 1;
	
				if (invflag || sbookflag) x -= 160;
				if (chrflag || questlog) x += 160;
			
				y -= automaps4;
				
				DrawAMQuarterSquare(x,y, 129);
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DrawAutomapPlr()
{
	int px, py;
	int dx, dy;
	int x, y;

	if (plr[myplr]._pmode == PM_WALK3) {
		px = plr[myplr]._pfutx;
		py = plr[myplr]._pfuty;
		if (plr[myplr]._pdir == DIR_L) px++;
		else py++;
	} else {
		px = plr[myplr]._px;
		py = plr[myplr]._py;
	}

	dx = px - ViewX - (amxadd << 1);
	dy = py - ViewY - (amyadd << 1);

	x = 384 + (dx * automaps3) - (dy * automaps3);
	y = 336 + (dx * automaps4) + (dy * automaps4);

	x += ((plr[myplr]._pxoff * automapscale) / 100) >> 1;
	y += ((plr[myplr]._pyoff * automapscale) / 100) >> 1;

	x += ((ScrollInfo._sxoff * automapscale) / 100) >> 1;
	y += ((ScrollInfo._syoff * automapscale) / 100) >> 1;

	if (invflag || sbookflag) x -= 160;
	if (chrflag || questlog) x += 160;

	y -= automaps4;

	switch (plr[myplr]._pdir) {
		case DIR_U:
			DrawLine(x, y, x, y-automaps3, AMPC);
			DrawLine(x, y-automaps3, x-automaps5, y-automaps4, AMPC);
			DrawLine(x, y-automaps3, x+automaps5, y-automaps4, AMPC);
			break;
		case DIR_UR:
			DrawLine(x, y, x+automaps3, y-automaps4, AMPC);
			DrawLine(x+automaps3, y-automaps4, x+automaps4, y-automaps4, AMPC);
			DrawLine(x+automaps3, y-automaps4, x+automaps4+automaps5, y, AMPC);
			break;
		case DIR_R:
			DrawLine(x, y, x+automaps3, y, AMPC);
			DrawLine(x+automaps3, y, x+automaps4, y-automaps5, AMPC);
			DrawLine(x+automaps3, y, x+automaps4, y+automaps5, AMPC);
			break;
		case DIR_DR:
			DrawLine(x, y, x+automaps3, y+automaps4, AMPC);
			DrawLine(x+automaps3, y+automaps4, x+automaps4+automaps5, y, AMPC);
			DrawLine(x+automaps3, y+automaps4, x+automaps4, y+automaps4, AMPC);
			break;
		case DIR_D:
			DrawLine(x, y, x, y+automaps3, AMPC);
			DrawLine(x, y+automaps3, x+automaps5, y+automaps4, AMPC);
			DrawLine(x, y+automaps3, x-automaps5, y+automaps4, AMPC);
			break;
		case DIR_DL:
			DrawLine(x, y, x-automaps3, y+automaps4, AMPC);
			DrawLine(x-automaps3, y+automaps4, x-automaps4-automaps5, y, AMPC);
			DrawLine(x-automaps3, y+automaps4, x-automaps4, y+automaps4, AMPC);
			break;
		case DIR_L:
			DrawLine(x, y, x-automaps3, y, AMPC);
			DrawLine(x-automaps3, y, x-automaps4, y-automaps5, AMPC);
			DrawLine(x-automaps3, y, x-automaps4, y+automaps5, AMPC);
			break;
		case DIR_UL:
			DrawLine(x, y, x-automaps3, y-automaps4, AMPC);
			DrawLine(x-automaps3, y-automaps4, x-automaps4, y-automaps4, AMPC);
			DrawLine(x-automaps3, y-automaps4, x-automaps4-automaps5, y, AMPC);
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static WORD GetAutomapType(int x, int y, BOOL view)
{
	WORD rv, t;

	if ((view) && (x == -1) && (y >= 0) && (y < AUTOMAPY) && (automapview[0][y])) {
		if (GetAutomapType(0, y, FALSE) & AMS_DIRT8) return(0);
		else return(0x4000);
	}
	if ((view) && (y == -1) && (x >= 0) && (x < AUTOMAPX) && (automapview[x][0])) {
		if (GetAutomapType(x, 0, FALSE) & AMS_DIRT8) return(0);
		else return(0x4000);
	}
	if ((x < 0) || (x >= AUTOMAPX)) return(0);
	if ((y < 0) || (y >= AUTOMAPY)) return(0);
	if ((!automapview[x][y]) && (view)) return(0);
	rv = automaptype[dungeon[x][y]];
	if (rv == 7) {
		t = GetAutomapType(x-1,y,FALSE) >> 8;
		if (t & AMS_ARCHR) {
			t = GetAutomapType(x,y-1,FALSE) >> 8;
			if (t & AMS_ARCHL) rv = 1;
		}
	}
	return(rv);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
/*
void DrawAutomapTest()
{
	int i, j, x, y;

	y = 336;
	x = 384 - (automaps1 * 2);
	for (i = 1; i < 5; i++) {
		DrawAMShape(x, y, 0x0300 + i);
		x += automaps1;
	}
}
*/


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void draw_game_info() {
	#define LINE_HGT	15
	char szBuf[256];
	int y = 20;
	if (gbMaxPlayers > 1) {
		strcat(strcpy(szBuf,"game: "),gszGameName);
		PrintStringXY(8,y,szBuf,ICOLOR_GOLD);
		y += LINE_HGT;
		if (gszGamePass[0]) {
			strcat(strcpy(szBuf,"password: "),gszGamePass);
			PrintStringXY(8,y,szBuf,ICOLOR_GOLD);
			y += LINE_HGT;
		}
	}

	if (setlevel) {
		PrintStringXY(8,y,SetLevelName[setlvlnum],ICOLOR_GOLD);
	}
	else if (currlevel) {
		if (currlevel >= HIVESTART && currlevel <= HIVEEND)
			sprintf(szBuf, "Level: Nest %i", currlevel - HIVESTART + 1);
		else if (currlevel >= CRYPTSTART && currlevel <= CRYPTEND)
			sprintf(szBuf, "Level: Crypt %i", currlevel - CRYPTSTART + 1);
		else
			sprintf(szBuf,"Level: %i", currlevel);
		PrintStringXY(8,y,szBuf,ICOLOR_GOLD);
	}
	#undef LINE_HGT
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawAutomap()
{
	int i, j;
	int	x, y;
	int xs, ys;
	int mx, my;
	int ams;
	WORD s;

	if (leveltype == 0) {
		draw_game_info();
		return;
	}

	app_assert(gpBuffer);
	glClipY = (long) gpBuffer + 393216;		// (352 + 160) * 768

	automapx = (ViewX - DIRTEDGED2) >> 1;
	while ((automapx + amxadd) < 0) amxadd++;
	while ((automapx + amxadd) >= AUTOMAPX) amxadd--;
	automapx += amxadd;
	automapy = (ViewY - DIRTEDGED2) >> 1;
	while ((automapy + amyadd) < 0) amyadd++;
	while ((automapy + amyadd) >= AUTOMAPY) amyadd--;
	automapy += amyadd;

	ams = automapstbl[(automapscale - AUTOMAPMIN) / AUTOMAPADD];
	if ((ScrollInfo._sxoff + ScrollInfo._syoff) != 0) ams++;

	mx = automapx - ams;
	my = automapy - 1;
	if (ams & 1) {
		xs = 384 - (automaps1 * ((ams-1) >> 1));
		ys = 336 - (automaps2 * ((ams+1) >> 1));
	} else {
		xs = 384 - (automaps1 * (ams >> 1)) + automaps2;
		ys = 336 - (automaps2 * (ams >> 1)) - automaps3;
	}
	if (ViewX & 1) {
		xs -= automaps3;
		ys -= automaps4;
	}
	if (ViewY & 1) {
		xs += automaps3;
		ys -= automaps4;
	}
	xs += ((ScrollInfo._sxoff * automapscale) / 100) >> 1;
	ys += ((ScrollInfo._syoff * automapscale) / 100) >> 1;

	if (invflag || sbookflag) xs -= 160;
	if (chrflag || questlog) xs += 160;

	for (j = 0; j <= (ams+1); j++) {
		x = xs;
		y = ys;
		for (i = 0; i < ams; i++) {
			s = GetAutomapType(mx+i,my-i,TRUE);
			if (s != 0) DrawAMShape(x, y, s);
			//else DrawAMSquare(x, y);
			x += automaps1;
		}
		my++;
		x = xs - automaps2;
		y = ys + automaps3;
		for (i = 0; i <= ams; i++) {
			s = GetAutomapType(mx+i,my-i,TRUE);
			if (s != 0) DrawAMShape(x, y, s);
			//else DrawAMSquare(x, y);
			x += automaps1;
		}
		mx++;
		ys = ys + automaps2;
	}

	DrawAutomapPlr();
	if (HighLightAllItems)
	{
		DrawAllObjects();
	}
	draw_game_info();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetAutomapView(int x, int y)
{
	int xx, yy;
	WORD s, d;

	xx = (x - DIRTEDGED2) >> 1;
	yy = (y - DIRTEDGED2) >> 1;
	if ((xx < 0) || (xx >= AUTOMAPX)) return;
	if ((yy < 0) || (yy >= AUTOMAPY)) return;
	automapview[xx][yy] = TRUE;
	s = GetAutomapType(xx, yy, FALSE);
	d = s & AMS_DIRT8;
	s = s & 0xf;
	switch (s) {
		case 2:
			if (d) {
				if (GetAutomapType(xx, yy+1, FALSE) == AMS_DIRTLR) automapview[xx][yy+1] = TRUE;
			} else {
				if (GetAutomapType(xx-1, yy, FALSE) & AMS_DIRT8) automapview[xx-1][yy] = TRUE;
			}
			break;
		case 3:
			if (d) {
				if (GetAutomapType(xx+1, yy, FALSE) == AMS_DIRTLR) automapview[xx+1][yy] = TRUE;
			} else {
				if (GetAutomapType(xx, yy-1, FALSE) & AMS_DIRT8) automapview[xx][yy-1] = TRUE;
			}
			break;
		case 4:
			if (d) {
				if (GetAutomapType(xx, yy+1, FALSE) == AMS_DIRTLR) automapview[xx][yy+1] = TRUE;
				if (GetAutomapType(xx+1, yy, FALSE) == AMS_DIRTLR) automapview[xx+1][yy] = TRUE;
			} else {
				if (GetAutomapType(xx-1, yy, FALSE) & AMS_DIRT8) automapview[xx-1][yy] = TRUE;
				if (GetAutomapType(xx, yy-1, FALSE) & AMS_DIRT8) automapview[xx][yy-1] = TRUE;
				if (GetAutomapType(xx-1, yy-1, FALSE) & AMS_DIRT8) automapview[xx-1][yy-1] = TRUE;
			}
			break;
		case 5:
			if (d) {
				if (GetAutomapType(xx, yy-1, FALSE) & AMS_DIRT8) automapview[xx][yy-1] = TRUE;
				if (GetAutomapType(xx, yy+1, FALSE) == AMS_DIRTLR) automapview[xx][yy+1] = TRUE;
			} else {
				if (GetAutomapType(xx-1, yy, FALSE) & AMS_DIRT8) automapview[xx-1][yy] = TRUE;
			}
			break;
		case 6:
			if (d) {
				if (GetAutomapType(xx-1, yy, FALSE) & AMS_DIRT8) automapview[xx-1][yy] = TRUE;
				if (GetAutomapType(xx+1, yy, FALSE) == AMS_DIRTLR) automapview[xx+1][yy] = TRUE;
			} else {
				if (GetAutomapType(xx, yy-1, FALSE) & AMS_DIRT8) automapview[xx][yy-1] = TRUE;
			}
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SyncAutomap()
{
	automaps1 = (automapscale << 6) / 100;
	automaps2 = automaps1 >> 1;
	automaps3 = automaps2 >> 1;
	automaps4 = automaps3 >> 1;
	automaps5 = automaps4 >> 1;
	amxadd = 0;
	amyadd = 0;
}
