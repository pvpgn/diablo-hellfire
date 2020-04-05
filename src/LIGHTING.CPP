/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Radial lighting ONLY
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/LIGHTING.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"lighting.h"
#include	"gendung.h"
#include	"engine.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"math.h"
#include	"automap.h"

/*-----------------------------------------------------------------------*/

#define PAUSEBANDS	224					// Pause band starting color
#define PAUSEBANDE	PAUSEBANDS+15		// Pause band ending color

/*-----------------------------------------------------------------------*/

// light test
int lnum = 0;

/*-----------------------------------------------------------------------**
** Global variables
**-----------------------------------------------------------------------*/

extern "C" {
	BYTE *pLightTbl;
	char lightmax;
}

int lightflag = 0;

LightListStruct		LightList[MAXLIGHTS];
BYTE lightactive[MAXLIGHTS];
int numlights;
BOOL dolighting;

LightListStruct		VisionList[MAXVISION];
int numvision;
BOOL dovision;
int visionid;

/*-----------------------------------------------------------------------**
** File variables
**-----------------------------------------------------------------------*/

// table of inc/dec for crawling the lighting radius (clockwise starting with N)
char CrawlTable[2749] = {
  1,  0,  0,
  4,  0,  1,  0, -1, -1,  0,  1,  0,
 16,  0,  2,  0, -2, -1,  2,  1,  2, -1, -2,  1, -2, -1,  1,  1,  1, -1, -1,  1, -1, -2,  1,  2,  1, -2, -1,  2, -1,
     -2,  0,  2,  0,
 24,  0,  3,  0, -3, -1,  3,  1,  3, -1, -3,  1, -3, -2,  3,  2,  3, -2, -3,  2, -3, -2,  2,  2,  2, -2, -2,  2, -2,
     -3,  2,  3,  2, -3, -2,  3, -2, -3,  1,  3,  1, -3, -1,  3, -1, -3,  0,  3,  0,
 32,  0,  4,  0, -4, -1,  4,  1,  4, -1, -4,  1, -4, -2,  4,  2,  4, -2, -4,  2, -4, -3,  4,  3,  4, -3, -4,  3, -4,
     -3,  3,  3,  3, -3, -3,  3, -3, -4,  3,  4,  3, -4, -3,  4, -3, -4,  2,  4,  2, -4, -2,  4, -2,
     -4,  1,  4,  1, -4, -1,  4, -1, -4,  0,  4,  0,
 40,  0,  5,  0, -5, -1,  5,  1,  5, -1, -5,  1, -5, -2,  5,  2,  5, -2, -5,  2, -5, -3,  5,  3,  5, -3, -5,  3, -5,
     -4,  5,  4,  5, -4, -5,  4, -5, -4,  4,  4,  4, -4, -4,  4, -4, -5,  4,  5,  4, -5, -4,  5, -4,
     -5,  3,  5,  3, -5, -3,  5, -3, -5,  2,  5,  2, -5, -2,  5, -2, -5,  1,  5,  1, -5, -1,  5, -1,
     -5,  0,  5,  0,
 48,  0,  6,  0, -6, -1,  6,  1,  6, -1, -6,  1, -6, -2,  6,  2,  6, -2, -6,  2, -6, -3,  6,  3,  6, -3, -6,  3, -6,
     -4,  6,  4,  6, -4, -6,  4, -6, -5,  6,  5,  6, -5, -6,  5, -6, -5,  5,  5,  5, -5, -5,  5, -5,
     -6,  5,  6,  5, -6, -5,  6, -5, -6,  4,  6,  4, -6, -4,  6, -4, -6,  3,  6,  3, -6, -3,  6, -3,
     -6,  2,  6,  2, -6, -2,  6, -2, -6,  1,  6,  1, -6, -1,  6, -1, -6,  0,  6,  0,
 56,  0,  7,  0, -7, -1,  7,  1,  7, -1, -7,  1, -7, -2,  7,  2,  7, -2, -7,  2, -7, -3,  7,  3,  7, -3, -7,  3, -7,
     -4,  7,  4,  7, -4, -7,  4, -7, -5,  7,  5,  7, -5, -7,  5, -7, -6,  7,  6,  7, -6, -7,  6, -7,
     -6,  6,  6,  6, -6, -6,  6, -6, -7,  6,  7,  6, -7, -6,  7, -6, -7,  5,  7,  5, -7, -5,  7, -5,
     -7,  4,  7,  4, -7, -4,  7, -4, -7,  3,  7,  3, -7, -3,  7, -3, -7,  2,  7,  2, -7, -2,  7, -2,
     -7,  1,  7,  1, -7, -1,  7, -1, -7,  0,  7,  0,
 64,  0,  8,  0, -8, -1,  8,  1,  8, -1, -8,  1, -8, -2,  8,  2,  8, -2, -8,  2, -8, -3,  8,  3,  8, -3, -8,  3, -8,
     -4,  8,  4,  8, -4, -8,  4, -8, -5,  8,  5,  8, -5, -8,  5, -8, -6,  8,  6,  8, -6, -8,  6, -8,
     -7,  8,  7,  8, -7, -8,  7, -8, -7,  7,  7,  7, -7, -7,  7, -7, -8,  7,  8,  7, -8, -7,  8, -7,
     -8,  6,  8,  6, -8, -6,  8, -6, -8,  5,  8,  5, -8, -5,  8, -5, -8,  4,  8,  4, -8, -4,  8, -4,
     -8,  3,  8,  3, -8, -3,  8, -3, -8,  2,  8,  2, -8, -2,  8, -2, -8,  1,  8,  1, -8, -1,  8, -1,
     -8,  0,  8,  0,
 72,  0,  9,  0, -9, -1,  9,  1,  9, -1, -9,  1, -9, -2,  9,  2,  9, -2, -9,  2, -9, -3,  9,  3,  9, -3, -9,  3, -9,
     -4,  9,  4,  9, -4, -9,  4, -9, -5,  9,  5,  9, -5, -9,  5, -9, -6,  9,  6,  9, -6, -9,  6, -9,
     -7,  9,  7,  9, -7, -9,  7, -9, -8,  9,  8,  9, -8, -9,  8, -9, -8,  8,  8,  8, -8, -8,  8, -8,
     -9,  8,  9,  8, -9, -8,  9, -8, -9,  7,  9,  7, -9, -7,  9, -7, -9,  6,  9,  6, -9, -6,  9, -6,
     -9,  5,  9,  5, -9, -5,  9, -5, -9,  4,  9,  4, -9, -4,  9, -4, -9,  3,  9,  3, -9, -3,  9, -3,
     -9,  2,  9,  2, -9, -2,  9, -2, -9,  1,  9,  1, -9, -1,  9, -1, -9,  0,  9,  0,
 80,  0, 10,  0,-10, -1, 10,  1, 10, -1,-10,  1,-10, -2, 10,  2, 10, -2,-10,  2,-10, -3, 10,  3, 10, -3,-10,  3,-10,
     -4, 10,  4, 10, -4,-10,  4,-10, -5, 10,  5, 10, -5,-10,  5,-10, -6, 10,  6, 10, -6,-10,  6,-10,
     -7, 10,  7, 10, -7,-10,  7,-10, -8, 10,  8, 10, -8,-10,  8,-10, -9, 10,  9, 10, -9,-10,  9,-10,
     -9,  9,  9,  9, -9, -9,  9, -9,-10,  9, 10,  9,-10, -9, 10, -9,-10,  8, 10,  8,-10, -8, 10, -8,
    -10,  7, 10,  7,-10, -7, 10, -7,-10,  6, 10,  6,-10, -6, 10, -6,-10,  5, 10,  5,-10, -5, 10, -5,
    -10,  4, 10,  4,-10, -4, 10, -4,-10,  3, 10,  3,-10, -3, 10, -3,-10,  2, 10,  2,-10, -2, 10, -2,
    -10,  1, 10,  1,-10, -1, 10, -1,-10,  0, 10,  0,
 88,  0, 11,  0,-11, -1, 11,  1, 11, -1,-11,  1,-11, -2, 11,  2, 11, -2,-11,  2,-11, -3, 11,  3, 11, -3,-11,  3,-11,
     -4, 11,  4, 11, -4,-11,  4,-11, -5, 11,  5, 11, -5,-11,  5,-11, -6, 11,  6, 11, -6,-11,  6,-11,
     -7, 11,  7, 11, -7,-11,  7,-11, -8, 11,  8, 11, -8,-11,  8,-11, -9, 11,  9, 11, -9,-11,  9,-11,
    -10, 11, 10, 11,-10,-11, 10,-11,-10, 10, 10, 10,-10,-10, 10,-10,-11, 10, 11, 10,-11,-10, 11,-10,
    -11,  9, 11,  9,-11, -9, 11, -9,-11,  8, 11,  8,-11, -8, 11, -8,-11,  7, 11,  7,-11, -7, 11, -7,
    -11,  6, 11,  6,-11, -6, 11, -6,-11,  5, 11,  5,-11, -5, 11, -5,-11,  4, 11,  4,-11, -4, 11, -4,
    -11,  3, 11,  3,-11, -3, 11, -3,-11,  2, 11,  2,-11, -2, 11, -2,-11,  1, 11,  1,-11, -1, 11, -1,
    -11,  0, 11,  0,
 96,  0, 12,  0,-12, -1, 12,  1, 12, -1,-12,  1,-12, -2, 12,  2, 12, -2,-12,  2,-12, -3, 12,  3, 12, -3,-12,  3,-12,
     -4, 12,  4, 12, -4,-12,  4,-12, -5, 12,  5, 12, -5,-12,  5,-12, -6, 12,  6, 12, -6,-12,  6,-12,
     -7, 12,  7, 12, -7,-12,  7,-12, -8, 12,  8, 12, -8,-12,  8,-12, -9, 12,  9, 12, -9,-12,  9,-12,
    -10, 12, 10, 12,-10,-12, 10,-12,-11, 12, 11, 12,-11,-12, 11,-12,-11, 11, 11, 11,-11,-11, 11,-11,
    -12, 11, 12, 11,-12,-11, 12,-11,-12, 10, 12, 10,-12,-10, 12,-10,-12,  9, 12,  9,-12, -9, 12, -9,
    -12,  8, 12,  8,-12, -8, 12, -8,-12,  7, 12,  7,-12, -7, 12, -7,-12,  6, 12,  6,-12, -6, 12, -6,
    -12,  5, 12,  5,-12, -5, 12, -5,-12,  4, 12,  4,-12, -4, 12, -4,-12,  3, 12,  3,-12, -3, 12, -3,
    -12,  2, 12,  2,-12, -2, 12, -2,-12,  1, 12,  1,-12, -1, 12, -1,-12,  0, 12,  0,
104,  0, 13,  0,-13, -1, 13,  1, 13, -1,-13,  1,-13, -2, 13,  2, 13, -2,-13,  2,-13, -3, 13,  3, 13, -3,-13,  3,-13,
     -4, 13,  4, 13, -4,-13,  4,-13, -5, 13,  5, 13, -5,-13,  5,-13, -6, 13,  6, 13, -6,-13,  6,-13,
     -7, 13,  7, 13, -7,-13,  7,-13, -8, 13,  8, 13, -8,-13,  8,-13, -9, 13,  9, 13, -9,-13,  9,-13,
    -10, 13, 10, 13,-10,-13, 10,-13,-11, 13, 11, 13,-11,-13, 11,-13,-12, 13, 12, 13,-12,-13, 12,-13,
    -12, 12, 12, 12,-12,-12, 12,-12,-13, 12, 13, 12,-13,-12, 13,-12,-13, 11, 13, 11,-13,-11, 13,-11,
    -13, 10, 13, 10,-13,-10, 13,-10,-13,  9, 13,  9,-13, -9, 13, -9,-13,  8, 13,  8,-13, -8, 13, -8,
    -13,  7, 13,  7,-13, -7, 13, -7,-13,  6, 13,  6,-13, -6, 13, -6,-13,  5, 13,  5,-13, -5, 13, -5,
    -13,  4, 13,  4,-13, -4, 13, -4,-13,  3, 13,  3,-13, -3, 13, -3,-13,  2, 13,  2,-13, -2, 13, -2,
    -13,  1, 13,  1,-13, -1, 13, -1,-13,  0, 13,  0,
112,  0, 14,  0,-14, -1, 14,  1, 14, -1,-14,  1,-14, -2, 14,  2, 14, -2,-14,  2,-14, -3, 14,  3, 14, -3,-14,  3,-14,
     -4, 14,  4, 14, -4,-14,  4,-14, -5, 14,  5, 14, -5,-14,  5,-14, -6, 14,  6, 14, -6,-14,  6,-14,
     -7, 14,  7, 14, -7,-14,  7,-14, -8, 14,  8, 14, -8,-14,  8,-14, -9, 14,  9, 14, -9,-14,  9,-14,
    -10, 14, 10, 14,-10,-14, 10,-14,-11, 14, 11, 14,-11,-14, 11,-14,-12, 14, 12, 14,-12,-14, 12,-14,
    -13, 14, 13, 14,-13,-14, 13,-14,-13, 13, 13, 13,-13,-13, 13,-13,-14, 13, 14, 13,-14,-13, 14,-13,
    -14, 12, 14, 12,-14,-12, 14,-12,-14, 11, 14, 11,-14,-11, 14,-11,-14, 10, 14, 10,-14,-10, 14,-10,
    -14,  9, 14,  9,-14, -9, 14, -9,-14,  8, 14,  8,-14, -8, 14, -8,-14,  7, 14,  7,-14, -7, 14, -7,
    -14,  6, 14,  6,-14, -6, 14, -6,-14,  5, 14,  5,-14, -5, 14, -5,-14,  4, 14,  4,-14, -4, 14, -4,
    -14,  3, 14,  3,-14, -3, 14, -3,-14,  2, 14,  2,-14, -2, 14, -2,-14,  1, 14,  1,-14, -1, 14, -1,
    -14,  0, 14,  0,
120,  0, 15,  0,-15, -1, 15,  1, 15, -1,-15,  1,-15, -2, 15,  2, 15, -2,-15,  2,-15, -3, 15,  3, 15, -3,-15,  3,-15,
     -4, 15,  4, 15, -4,-15,  4,-15, -5, 15,  5, 15, -5,-15,  5,-15, -6, 15,  6, 15, -6,-15,  6,-15,
     -7, 15,  7, 15, -7,-15,  7,-15, -8, 15,  8, 15, -8,-15,  8,-15, -9, 15,  9, 15, -9,-15,  9,-15,
    -10, 15, 10, 15,-10,-15, 10,-15,-11, 15, 11, 15,-11,-15, 11,-15,-12, 15, 12, 15,-12,-15, 12,-15,
    -13, 15, 13, 15,-13,-15, 13,-15,-14, 15, 14, 15,-14,-15, 14,-15,-14, 14, 14, 14,-14,-14, 14,-14,
    -15, 14, 15, 14,-15,-14, 15,-14,-15, 13, 15, 13,-15,-13, 15,-13,-15, 12, 15, 12,-15,-12, 15,-12,
    -15, 11, 15, 11,-15,-11, 15,-11,-15, 10, 15, 10,-15,-10, 15,-10,-15,  9, 15,  9,-15, -9, 15, -9,
    -15,  8, 15,  8,-15, -8, 15, -8,-15,  7, 15,  7,-15, -7, 15, -7,-15,  6, 15,  6,-15, -6, 15, -6,
    -15,  5, 15,  5,-15, -5, 15, -5,-15,  4, 15,  4,-15, -4, 15, -4,-15,  3, 15,  3,-15, -3, 15, -3,
    -15,  2, 15,  2,-15, -2, 15, -2,-15,  1, 15,  1,-15, -1, 15, -1,-15,  0, 15,  0,
(char)128,  0, 16,  0,-16, -1, 16,  1, 16, -1,-16,  1,-16, -2, 16,  2, 16, -2,-16,  2,-16, -3, 16,  3, 16, -3,-16,  3,-16,
     -4, 16,  4, 16, -4,-16,  4,-16, -5, 16,  5, 16, -5,-16,  5,-16, -6, 16,  6, 16, -6,-16,  6,-16,
     -7, 16,  7, 16, -7,-16,  7,-16, -8, 16,  8, 16, -8,-16,  8,-16, -9, 16,  9, 16, -9,-16,  9,-16,
    -10, 16, 10, 16,-10,-16, 10,-16,-11, 16, 11, 16,-11,-16, 11,-16,-12, 16, 12, 16,-12,-16, 12,-16,
    -13, 16, 13, 16,-13,-16, 13,-16,-14, 16, 14, 16,-14,-16, 14,-16,-15, 16, 15, 16,-15,-16, 15,-16,
    -15, 15, 15, 15,-15,-15, 15,-15,-16, 15, 16, 15,-16,-15, 16,-15,-16, 14, 16, 14,-16,-14, 16,-14,
    -16, 13, 16, 13,-16,-13, 16,-13,-16, 12, 16, 12,-16,-12, 16,-12,-16, 11, 16, 11,-16,-11, 16,-11,
    -16, 10, 16, 10,-16,-10, 16,-10,-16,  9, 16,  9,-16, -9, 16, -9,-16,  8, 16,  8,-16, -8, 16, -8,
    -16,  7, 16,  7,-16, -7, 16, -7,-16,  6, 16,  6,-16, -6, 16, -6,-16,  5, 16,  5,-16, -5, 16, -5,
    -16,  4, 16,  4,-16, -4, 16, -4,-16,  3, 16,  3,-16, -3, 16, -3,-16,  2, 16,  2,-16, -2, 16, -2,
    -16,  1, 16,  1,-16, -1, 16, -1,-16,  0, 16,  0,
(char)136,  0, 17,  0,-17, -1, 17,  1, 17, -1,-17,  1,-17, -2, 17,  2, 17, -2,-17,  2,-17, -3, 17,  3, 17, -3,-17,  3,-17,
     -4, 17,  4, 17, -4,-17,  4,-17, -5, 17,  5, 17, -5,-17,  5,-17, -6, 17,  6, 17, -6,-17,  6,-17,
     -7, 17,  7, 17, -7,-17,  7,-17, -8, 17,  8, 17, -8,-17,  8,-17, -9, 17,  9, 17, -9,-17,  9,-17,
    -10, 17, 10, 17,-10,-17, 10,-17,-11, 17, 11, 17,-11,-17, 11,-17,-12, 17, 12, 17,-12,-17, 12,-17,
    -13, 17, 13, 17,-13,-17, 13,-17,-14, 17, 14, 17,-14,-17, 14,-17,-15, 17, 15, 17,-15,-17, 15,-17,
    -16, 17, 16, 17,-16,-17, 16,-17,-16, 16, 16, 16,-16,-16, 16,-16,-17, 16, 17, 16,-17,-16, 17,-16,
    -17, 15, 17, 15,-17,-15, 17,-15,-17, 14, 17, 14,-17,-14, 17,-14,-17, 13, 17, 13,-17,-13, 17,-13,
    -17, 12, 17, 12,-17,-12, 17,-12,-17, 11, 17, 11,-17,-11, 17,-11,-17, 10, 17, 10,-17,-10, 17,-10,
    -17,  9, 17,  9,-17, -9, 17, -9,-17,  8, 17,  8,-17, -8, 17, -8,-17,  7, 17,  7,-17, -7, 17, -7,
    -17,  6, 17,  6,-17, -6, 17, -6,-17,  5, 17,  5,-17, -5, 17, -5,-17,  4, 17,  4,-17, -4, 17, -4,
    -17,  3, 17,  3,-17, -3, 17, -3,-17,  2, 17,  2,-17, -2, 17, -2,-17,  1, 17,  1,-17, -1, 17, -1,
    -17,  0, 17,  0,
(char)144,  0, 18,  0,-18, -1, 18,  1, 18, -1,-18,  1,-18, -2, 18,  2, 18, -2,-18,  2,-18, -3, 18,  3, 18, -3,-18,  3,-18,
     -4, 18,  4, 18, -4,-18,  4,-18, -5, 18,  5, 18, -5,-18,  5,-18, -6, 18,  6, 18, -6,-18,  6,-18,
     -7, 18,  7, 18, -7,-18,  7,-18, -8, 18,  8, 18, -8,-18,  8,-18, -9, 18,  9, 18, -9,-18,  9,-18,
    -10, 18, 10, 18,-10,-18, 10,-18,-11, 18, 11, 18,-11,-18, 11,-18,-12, 18, 12, 18,-12,-18, 12,-18,
    -13, 18, 13, 18,-13,-18, 13,-18,-14, 18, 14, 18,-14,-18, 14,-18,-15, 18, 15, 18,-15,-18, 15,-18,
    -16, 18, 16, 18,-16,-18, 16,-18,-17, 18, 17, 18,-17,-18, 17,-18,-17, 17, 17, 17,-17,-17, 17,-17,
    -18, 17, 18, 17,-18,-17, 18,-17,-18, 16, 18, 16,-18,-16, 18,-16,-18, 15, 18, 15,-18,-15, 18,-15,
    -18, 14, 18, 14,-18,-14, 18,-14,-18, 13, 18, 13,-18,-13, 18,-13,-18, 12, 18, 12,-18,-12, 18,-12,
    -18, 11, 18, 11,-18,-11, 18,-11,-18, 10, 18, 10,-18,-10, 18,-10,-18,  9, 18,  9,-18, -9, 18, -9,
    -18,  8, 18,  8,-18, -8, 18, -8,-18,  7, 18,  7,-18, -7, 18, -7,-18,  6, 18,  6,-18, -6, 18, -6,
    -18,  5, 18,  5,-18, -5, 18, -5,-18,  4, 18,  4,-18, -4, 18, -4,-18,  3, 18,  3,-18, -3, 18, -3,
    -18,  2, 18,  2,-18, -2, 18, -2,-18,  1, 18,  1,-18, -1, 18, -1,-18,  0, 18,  0
};

// points to corresponding radius in above table
char * pCrawlEntry[19] = {
	&(CrawlTable[0]),				// 0
	&(CrawlTable[3]),				// 1
	&(CrawlTable[12]),				// 2
	&(CrawlTable[45]),				// 3
	&(CrawlTable[94]),				// 4
	&(CrawlTable[159]),				// 5
	&(CrawlTable[240]),				// 6
	&(CrawlTable[337]),				// 7
	&(CrawlTable[450]),				// 8
	&(CrawlTable[579]),				// 9
	&(CrawlTable[724]),				// 10
	&(CrawlTable[885]),				// 11
	&(CrawlTable[1062]),			// 12
	&(CrawlTable[1255]),			// 13
	&(CrawlTable[1464]),			// 14
	&(CrawlTable[1689]),			// 15
	&(CrawlTable[1930]),			// 16
	&(CrawlTable[2187]),			// 17
	&(CrawlTable[2460])				// 18
};

BYTE vCrawlTable[23][30] = {
	{1,0, 2,0, 3,0, 4,0, 5,0, 6,0, 7,0, 8,0, 9,0, 10,0,  11,0,  12,0,  13,0, 14,0, 15,0},    	 	// 0
	{1,0, 2,0, 3,0, 4,0, 5,0, 6,0, 7,0, 8,1, 9,1, 10,1,  11,1,  12,1,  13,1, 14,1, 15,1},	 		// 1
	{1,0, 2,0, 3,0, 4,1, 5,1, 6,1, 7,1, 8,1, 9,1, 10,1,  11,1,  12,2,  13,2, 14,2, 15,2},	 		// 2
	{1,0, 2,0, 3,1, 4,1, 5,1, 6,1, 7,1, 8,2, 9,2, 10,2,  11,2,  12,2,  13,3, 14,3, 15,3},	 		// 3
	{1,0, 2,1, 3,1, 4,1, 5,1, 6,2, 7,2, 8,2, 9,3, 10,3,  11,3,  12,3,  13,4, 14,4, 0,0},	     	// 4
	{1,0, 2,1, 3,1, 4,1, 5,2, 6,2, 7,3, 8,3, 9,3, 10,4,  11,4,  12,4,  13,5, 14,5, 0,0},	     	// 5
	{1,0, 2,1, 3,1, 4,2, 5,2, 6,3, 7,3, 8,3, 9,4, 10,4,  11,5,  12,5,  13,6, 14,6, 0,0},	     	// 6
	{1,1, 2,1, 3,2, 4,2, 5,3, 6,3, 7,4, 8,4, 9,5, 10,5,  11,6,  12,6,  13,7, 0,0,  0,0},	     	// 7
	{1,1, 2,1, 3,2, 4,2, 5,3, 6,4, 7,4, 8,5, 9,6, 10,6,  11,7,  12,7,  12,8, 13,8, 0,0},	     	// 8
	{1,1, 2,2, 3,2, 4,3, 5,4, 6,5, 7,5, 8,6, 9,7, 10,7,  10,8,  11,8,  12,9, 0,0,  0,0},	     	// 9
	{1,1, 2,2, 3,3, 4,4, 5,5, 6,5, 7,6, 8,7, 9,8, 10,9,  11,9,  11,10, 0,0,  0,0,  0,0},	     	// 10
	{1,1, 2,2, 3,3, 4,4, 5,5, 6,6, 7,7, 8,8, 9,9, 10,10, 11,11, 0,0,   0,0,  0,0,  0,0},	     	// 11
	{1,1, 2,2, 3,3, 4,4, 5,5, 5,6, 6,7, 7,8, 8,9, 9,10,  9,11,  10,11, 0,0,  0,0,  0,0},	     	// 12
	{1,1, 2,2, 2,3, 3,4, 4,5, 5,6, 5,7, 6,8, 7,9, 7,10,  8,10,  8,11,  9,12, 0,0,  0,0},	     	// 13
	{1,1, 1,2, 2,3, 2,4, 3,5, 4,6, 4,7, 5,8, 6,9, 6,10,  7,11,  7,12,  8,12, 8,13, 0,0},	     	// 14
	{1,1, 1,2, 2,3, 2,4, 3,5, 3,6, 4,7, 4,8, 5,9, 5,10,  6,11,  6,12,  7,13, 0,0,  0,0},	     	// 15
	{0,1, 1,2, 1,3, 2,4, 2,5, 3,6, 3,7, 3,8, 4,9, 4,10,  5,11,  5,12,  6,13, 6,14, 0,0},	     	// 16
	{0,1, 1,2, 1,3, 1,4, 2,5, 2,6, 3,7, 3,8, 3,9, 4,10,  4,11,  4,12,  5,13, 5,14, 0,0},	     	// 17
	{0,1, 1,2, 1,3, 1,4, 1,5, 2,6, 2,7, 2,8, 3,9, 3,10,  3,11,  3,12,  4,13, 4,14, 0,0},	     	// 18
	{0,1, 0,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,8, 2,9, 2,10,  2,11,  2,12,  3,13, 3,14, 3,15},	 		// 19
	{0,1, 0,2, 0,3, 1,4, 1,5, 1,6, 1,7, 1,8, 1,9, 1,10,  1,11,  2,12,  2,13, 2,14, 2,15},	 		// 20
	{0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,8, 1,9, 1,10,  1,11,  1,12,  1,13, 1,14, 1,15},	 		// 21
	{0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 0,8, 0,9, 0,10,  0,11,  0,12,  0,13, 0,14, 0,15}	     	// 22	
};																										   

BYTE LightLvls[16][128];
	

BYTE LightLvls4[18][18] = {
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 1
	{ 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2
	{ 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 3
	{ 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 4
	{ 0, 0, 1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 1
	{ 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 6
	{ 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 7
	{ 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 8
	{ 0, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 9
	{ 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3}, //  2
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3}, // 11
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3}, // 12
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3}, // 13
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3}, // 14
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3}, //  3
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3}, // 16
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3}, // 17
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2}  // 18
};

BYTE Dist[64][256];
	
BYTE RadiusAdj[23] = {0,0,0,0,1,1,1,2,2,2,3,4,3,2,2,2,1,1,1,0,0,0,0};


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RotateOffset(int &x, int &y, int &rx, int &ry, int &ox, int &oy, int &ax, int &ay)
{
	int tmp;

	ax = 0;
	ay = 0;

	// rotate rx,ry by 90 deg. clockwise
	tmp = rx;
	rx = 7-ry;
	ry = tmp;

	// rotate origin by 90 deg. clockwise
	tmp = ox;
	ox = 7-oy;
	oy = tmp;

	// set x,y to rx,ry shifted relative to origin
	x = rx - ox;
	y = ry - oy;

	// bring them back into 0..7 range
	if(x < 0)
	{
		x += 8;
		ax = 1;
	}
	if(y < 0)
	{
		y += 8;
		ay = 1;
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoLighting (int nXPos, int nYPos, int nRadius, int Lnum)
{
	int LightVal;
	int nCrawlX, nCrawlY;
	int i, j;
	int dist;
	int xoff=0,yoff=0;					// offset into tile, where tile is broken
									// into 8x8 grid for increased distance resolution.
	int rxoff, ryoff;			// rotated offsets
	int orgx=0, orgy=0;
	int addx=0, addy=0;
	int dtab;
	int maxleft,maxright,maxup,maxdown;		// clipping vars: offsets from nXPos,nYpos; absolute vals

	if(Lnum >= 0)
	{
		xoff = LightList[Lnum]._xoff;
		yoff = LightList[Lnum]._yoff;

		if(xoff < 0)
		{
			xoff += 8;
			nXPos--;
		}
		if(yoff < 0)
		{
			yoff += 8;
			nYPos--;
		}
	}
	rxoff = xoff;
	ryoff = yoff;

	if((nXPos - 15) < 0)
		maxleft = nXPos + 1;
	else
		maxleft = 15;
	if((nXPos + 15) > MAXDUNX)
		maxright = MAXDUNX - nXPos;
	else
		maxright = 15;
	if((nYPos - 15) < 0)
		maxup = nYPos + 1;
	else
		maxup = 15;
	if((nYPos + 15) > MAXDUNY)
		maxdown = MAXDUNY - nYPos;
	else
		maxdown = 15;

	if (currlevel < HIVESTART)
		dLight[nXPos][nYPos] = 0;
	else
		if(dLight[nXPos][nYPos] > LightLvls[nRadius][0])
			dLight[nXPos][nYPos] = LightLvls[nRadius][0];


	// Quadrant 1
	//   |x
	//  ---
	//   |
	dtab = xoff + 8*yoff;

	for (j = 0; j < maxup; j++)
	{
		for (i = 1; i < maxright; i++)
		{
			dist = Dist[dtab][i+j*16];
			if(dist < 128)
			{
				nCrawlX = nXPos + i;
				nCrawlY = nYPos + j;
				LightVal = LightLvls[nRadius][dist];
				if(LightVal < dLight[nCrawlX][nCrawlY])
					dLight[nCrawlX][nCrawlY] = LightVal;
			}
		}
	}

	// Quadrant 2
	//   |
	//  ---
	//   |x

	RotateOffset(xoff,yoff, rxoff,ryoff, orgx,orgy, addx,addy);
	dtab = xoff + 8*yoff;
	for (j = 0; j < maxdown; j++)
	{
		for (i = 1; i < maxright; i++)
		{
			dist = Dist[dtab][(i+addx)+(j+addy)*16];
			if(dist < 128)
			{
				nCrawlX = nXPos + j;
				nCrawlY = nYPos - i;
				LightVal = LightLvls[nRadius][dist];
				if(LightVal < dLight[nCrawlX][nCrawlY])
					dLight[nCrawlX][nCrawlY] = LightVal;
			}
		}
	}

	// Quadrant 3
	//   |
	//  ---
	//  x|
	RotateOffset(xoff,yoff, rxoff,ryoff, orgx,orgy, addx,addy);
	dtab = xoff + 8*yoff;
	for (j = 0; j < maxdown; j++)
	{
		for (i = 1; i < maxleft; i++)
		{
			dist = Dist[dtab][(i+addx)+(j+addy)*16];
			if(dist < 128)
			{
				nCrawlX = nXPos - i;
				nCrawlY = nYPos - j;
				LightVal = LightLvls[nRadius][dist];
				if(LightVal < dLight[nCrawlX][nCrawlY])
					dLight[nCrawlX][nCrawlY] = LightVal;
			}
		}
	}

	// Quadrant 4
	//  x|
	//  ---
	//   |
	RotateOffset(xoff,yoff, rxoff,ryoff, orgx,orgy, addx,addy);
	dtab = xoff + 8*yoff;
	for (j = 0; j < maxup; j++)
	{
		for (i = 1; i < maxleft; i++)
		{
			dist = Dist[dtab][(i+addx)+(j+addy)*16];
			if(dist < 128)
			{
				nCrawlX = nXPos - j;
				nCrawlY = nYPos + i;
				LightVal = LightLvls[nRadius][dist];
				if(LightVal < dLight[nCrawlX][nCrawlY])
					dLight[nCrawlX][nCrawlY] = LightVal;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoUnLight (int nXPos, int nYPos, int nRadius)
{
	int i, j;
	int	x1,y1,x2,y2;

	nRadius++;
	y1 = nYPos - nRadius;
	y2 = nYPos + nRadius;
	x1 = nXPos - nRadius;
	x2 = nXPos + nRadius;
	if (y1 < 0) y1 = 0;
	if (y2 > DMAXY) y2 = DMAXY;
	if (x1 < 0) x1 = 0;
	if (x2 > DMAXX) x2 = DMAXX;
	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) dLight[i][j] = dSaveLight[i][j];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoUnVision (int nXPos, int nYPos, int nRadius)
{
	int i, j;
	int x1,y1,x2,y2;

	nRadius++;
	y1 = nYPos - nRadius;
	y2 = nYPos + nRadius;
	x1 = nXPos - nRadius;
	x2 = nXPos + nRadius;
	if (y1 < 0) y1 = 0;
	if (y2 > DMAXY) y2 = DMAXY;
	if (x1 < 0) x1 = 0;
	if (x2 > DMAXX) x2 = DMAXX;
	for (i = x1; i < x2; i++) {
		for (j = y1; j < y2; j++) dFlags[i][j] &= ~(BFLAG_VISIBLE | BFLAG_MONSTACTIVE);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoVision (int nXPos, int nYPos, int nRadius, BOOL doautomap, BOOL visible)
{
	int nCrawlX, nCrawlY, nLineLen;
	int nBlockerFlag;
	int i, j, k, v;
	int x1adj, x2adj, y1adj, y2adj;

	// handle origin separately.  Why?  I don't know.
	if (nXPos >= 0 && nXPos <= DMAXX && nYPos >= 0 && nYPos <= DMAXY) {
		if (doautomap) {
			if (dFlags[nXPos][nYPos] >= 0) SetAutomapView(nXPos, nXPos);
			dFlags[nXPos][nYPos] |= BFLAG_AUTOMAP;
		}
		if (visible)
			dFlags[nXPos][nYPos] |= BFLAG_VISIBLE;
		dFlags[nXPos][nYPos] |= BFLAG_MONSTACTIVE;
	}
	
	for (k = 0; k < 4; k ++) {
		for (j = 0; j < 23; j ++) {

			nBlockerFlag = 0;
			nLineLen = (nRadius - RadiusAdj[j]) << 1;

			for (i = 0; i < nLineLen && !nBlockerFlag; i += 2) {
				x1adj = 0;
				x2adj = 0;
				y1adj = 0;
				y2adj = 0;

				switch (k) {
					case 0:
						nCrawlX  = nXPos + vCrawlTable[j][i];
						nCrawlY  = nYPos + vCrawlTable[j][i + 1];
						if (vCrawlTable[j][i] > 0 && vCrawlTable[j][i+1] > 0) {
							x1adj = -1;
							y2adj = -1;
						}
						break;
					case 1:
						nCrawlX = nXPos - vCrawlTable[j][i];
						nCrawlY = nYPos - vCrawlTable[j][i + 1];
						if ((vCrawlTable[j][i] > 0) && (vCrawlTable[j][i+1] > 0)) {
							y1adj = 1;
							x2adj = 1;
						}
						break;
					case 2:
						nCrawlX = nXPos + vCrawlTable[j][i];
						nCrawlY = nYPos - vCrawlTable[j][i + 1];
						if (vCrawlTable[j][i] > 0 && vCrawlTable[j][i+1] > 0) {
							x1adj = -1;
							y2adj =  1;
						}
						break;
					case 3:
						nCrawlX = nXPos - vCrawlTable[j][i];
						nCrawlY = nYPos + vCrawlTable[j][i + 1];
						if (vCrawlTable[j][i] > 0 && vCrawlTable[j][i+1] > 0) {
							y1adj = -1;
							x2adj =  1;
						}
						break;
				}

				if (nCrawlX >= 0 && nCrawlX <= DMAXX && nCrawlY >= 0 && nCrawlY <= DMAXY) {

					nBlockerFlag = nBlockTable[dPiece[nCrawlX][nCrawlY]];

					if (!nBlockTable[dPiece[nCrawlX + x1adj][nCrawlY + y1adj]]
						|| !nBlockTable[dPiece[nCrawlX + x2adj][nCrawlY + y2adj]]) {
						if (doautomap) {
							if (dFlags[nCrawlX][nCrawlY] >= 0) SetAutomapView(nCrawlX, nCrawlY);
							dFlags[nCrawlX][nCrawlY] |= BFLAG_AUTOMAP;
						}
						if (visible)
							dFlags[nCrawlX][nCrawlY] |= BFLAG_VISIBLE;

						dFlags[nCrawlX][nCrawlY] |= BFLAG_MONSTACTIVE;
						if (!nBlockerFlag) {
							v = dTransVal[nCrawlX][nCrawlY];
							if (v != 0) TransList[v] = TRUE;
						}
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void FreeLightTable() {
	DiabloFreePtr(pLightTbl);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitLightTable()
{
	// Palettes for 16 light values
	app_assert(! pLightTbl);
	pLightTbl = DiabloAllocPtrSig(LIGHTSIZE,'LGTt');
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void MakeLightTable()
{
	int i,j,k,idx,tlen;
	BYTE ev,sv;
	BYTE *InfraTemp;
	BYTE *itp, *pLT;
	int x,y;
	int rad;
	int d;
	int a,b;
	double ftmp, radiusmod;

	// Palettes for 16 light values
	pLT = pLightTbl;

	idx = 0;
	if (light4flag) tlen = 3;
	else tlen = 15;
	for (i = 0; i < tlen; i++) {
		*pLT++ = 0;
		for (j = 0; j < 8; j++) {
			sv = idx + (j * 16);
			ev = 15 + (j * 16);
			for (k = 0; k < 16; k++) {
				if ((k != 0) || (j != 0)) *pLT++ = sv;
				if (sv < ev) sv++;
				else sv = ev = 0;
			}
		}
		for (j = 16; j < 20; j++) {
			sv = (idx >> 1) + (j * 8);
			ev = 7 + (j * 8);
			for (k = 0; k < 8; k++) {
				*pLT++ = sv;
				if (sv < ev) sv++;
				else sv = ev = 0;
			}
		}
		for (j = 10; j < 16; j++) {
			sv = idx + (j * 16);
			ev = 15 + (j * 16);
			for (k = 0; k < 16; k++) {
				*pLT++ = sv;
				if (sv < ev) sv++;
				else sv = ev = 0;
				if (sv == 255) sv = ev = 0;
			}
		}
		if (light4flag) idx += 5;
		else idx++;
	}
	for (j = 0; j < 256; j++)
		*pLT++ = 0;


	// Init fading for blood
	if (leveltype == 4) {
		byte bloodfadehold[16];
		pLT = pLightTbl;
		for (i = 0; i < tlen; i++) {
			int d = tlen - i;
			int a = tlen / d;
			int r = tlen % d;
			int tr = d;
			int ta = 0;
			sv = 1;
			bloodfadehold[0] = 0;
			for (k = 1; k < 16; k++) {
				bloodfadehold[k] = sv;
				tr += r;
				if ((tr > d) && (k < 15)) {
					k++;
					tr -= d;
					bloodfadehold[k] = sv;
				}
				ta++;
				if (ta == a) {
					sv++;
					ta = 0;
				}
			}
			*pLT++ = 0;
			for (k = 1; k < 16; k++) *pLT++ = bloodfadehold[k];
			for (k = 15; k > 0; k--) *pLT++ = bloodfadehold[k];
			*pLT = 1;
			pLT += 225;
		}
		*pLT++ = 0;
		for (j = 1; j < 32; j++) *pLT++ = 1;
		pLT += 224;
	}
	if (currlevel >= HIVESTART) {
		pLT = pLightTbl;
		for (i = 0; i < tlen; i++)
		{
			*pLT++ = 0;
//			for (j = 1; j < 9; j++)		// 16 is the number of rotated colors. JKE
			for (j = 1; j < 16; j++)
				*pLT++ = j;
//			pLT += 247;
			pLT += 240;
		}
		*pLT++ = 0;
		for (j = 1; j < 16; j++) *pLT++ = 1;
		pLT += 240;

	}

	// Load infra vision table
	InfraTemp = LoadFileInMemSig("PlrGFX\\Infra.TRN",NULL,'LGTt');
	itp = InfraTemp;
	for (j = 0; j < 256; j++) *pLT++ = *itp++;
	DiabloFreePtr(InfraTemp);

	// Load stone curse table
	InfraTemp = LoadFileInMemSig("PlrGFX\\Stone.TRN",NULL,'LGTt');
	itp = InfraTemp;
	for (j = 0; j < 256; j++) *pLT++ = *itp++;
	DiabloFreePtr(InfraTemp);

	// Create pause table
	for (j = 0; j < 8; j++) {
		for (sv = PAUSEBANDS+2; sv < PAUSEBANDE; sv++) {
			if ((j == 0) && (sv == PAUSEBANDS+2)) *pLT++ = 0;
			else *pLT++ = sv;
		}
		*pLT++ = 0;
		*pLT++ = 0;
		*pLT++ = 0;
	}
	for (j = 0; j < 4; j++) {
		sv = PAUSEBANDS;
		for (i = PAUSEBANDS; i < PAUSEBANDE; i+=2) {
			*pLT++ = sv;
			sv += 2;
		}
	}
	for (j = 0; j < 6; j++) {
		for (sv = PAUSEBANDS; sv < PAUSEBANDE; sv++) *pLT++ = sv;
		*pLT++ = 0;
	}

	// Doron's new smooth light tables
	for(rad = 0; rad < 16; rad++)
	{
		for(i = 0; i < 128; i++)
		{
			if(i > 8*(rad+1))
				LightLvls[rad][i] = 15;
			else
			{
				ftmp = (double)15*i/((double)8*(rad+1));
				// round
				LightLvls[rad][i] = (BYTE)(ftmp + 0.5);
			}
		}
	}
//	if (currlevel >= HIVESTART && currlevel < CRYPTSTART)
	if (currlevel >= HIVESTART)
	{
		for(rad = 0; rad < 16; rad++)		// avoid divide by zero
		{
			radiusmod = (sqrt((double)(16 - rad)))/128.0;
			radiusmod *= radiusmod;
			for(i = 0; i < 128; i++)
			{
				LightLvls[15-rad][i] = 15 - (BYTE)(radiusmod * (double)((128 - i) * (128 - i)));
				if (LightLvls[15-rad][i] > 15)
					LightLvls[15-rad][i] = 0;
				LightLvls[15-rad][i] = LightLvls[15-rad][i] - (BYTE)((15-rad)/2);
				if (LightLvls[15-rad][i] > 15)
					LightLvls[15-rad][i] = 0;
			}
		}
	}
	// Distance Table
	for(j = 0; j < 8; j++)
	{
		for(i = 0; i < 8; i++)
		{
			d = i + j*8;
			for(y = 0; y < 16; y++)
			{
				for(x = 0; x < 16; x++)
				{
					a = (x<<3)-i;
					b = (y<<3)-j;
					ftmp = (BYTE)sqrt((double)(a*a + b*b));
					// round
					ftmp += (ftmp < 0) ? -0.5:0.5;

					Dist[d][x+16*y] = (BYTE)ftmp;
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ResetLight ()
{
	if (lightflag == 0) {
		FillMemory(dLight,sizeof(dLight),lightmax);
		for (int i = 0; i < MAX_PLRS; i++) {
			if (! plr[i].plractive) continue;
			if (currlevel != plr[i].plrlevel) continue;
			DoLighting(plr[i]._px, plr[i]._py, plr[i]._pLightRad, -1);
		}
	}
	else {
		ZeroMemory(dLight,sizeof(dLight));
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ToggleLight ()
{
	lightflag ^= 1;
	if (lightflag == 0) {
		CopyMemory(dLight,dSaveLight,sizeof(dLight));
		for (int i = 0; i < MAX_PLRS; i++) {
			if (! plr[i].plractive) continue;
			if (currlevel != plr[i].plrlevel) continue;
			DoLighting(plr[i]._px, plr[i]._py, plr[i]._pLightRad, -1);
		}
	}
	else {
		ZeroMemory(dLight,sizeof(dLight));
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitLightMax()
{
	if (light4flag) lightmax = 3;
	else lightmax = 15;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitLighting()
{
	int i;

	numlights = 0;
	dolighting = FALSE;
	lightflag = 0;
	for(i = 0; i < MAXLIGHTS; i++)
		lightactive[i] = i;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int AddLight(int x, int y, int r)
{
	int lid;

	if (lightflag != 0) return(-1);
	lid = -1;
	if (numlights < MAXLIGHTS) {
		lid = lightactive[numlights++];
		LightList[lid]._lx = x;
		LightList[lid]._ly = y;
		LightList[lid]._lradius = r;
		LightList[lid]._xoff = 0;
		LightList[lid]._yoff = 0;
		LightList[lid]._ldel = FALSE;
		LightList[lid]._lunflag = FALSE;
		dolighting = TRUE;
	}
	return(lid);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddUnLight(int i)
{
	if (lightflag != 0) return;
	if (i == -1) return;

	LightList[i]._ldel = TRUE;
	dolighting = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ChangeLightRadius(int i, int r)
{
	if (lightflag != 0) return;
	if (i == -1) return;

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._lradius = r;
	dolighting = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ChangeLightXY(int i, int x, int y)
{
	if (lightflag != 0) return;
	if (i == -1) return;

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._lx = x;
	LightList[i]._ly = y;
	dolighting = TRUE;
}

/*-----------------------------------------------------------------------*
 *
 * ChangeLightOff
 *
 *	For increased lighting effect, we specify a position (-8..8,-8..8)
 *	within each dungeon grid coordinate.  This is _xoff and _yoff.
**-----------------------------------------------------------------------*/
void ChangeLightOff(int i, int x, int y)
{
	if (lightflag != 0) return;
	if (i == -1) return;

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._xoff = x;
	LightList[i]._yoff = y;
	dolighting = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ChangeLight(int i, int x, int y, int r)
{
	if (lightflag != 0) return;
	if (i == -1) return;

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._lx = x;
	LightList[i]._ly = y;
	LightList[i]._lradius = r;
	dolighting = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ProcessLightList()
{
	int	i,j;
	BYTE temp;

	if (lightflag != 0) return;
	if (dolighting) {
		for (j = 0; j < numlights; j++) {
			i = lightactive[j];
			if (LightList[i]._ldel) DoUnLight(LightList[i]._lx, LightList[i]._ly, LightList[i]._lradius);
			if (LightList[i]._lunflag) {
				DoUnLight(LightList[i]._lunx, LightList[i]._luny, LightList[i]._lunr);
				LightList[i]._lunflag = FALSE;
			}
		}
		for (j = 0; j < numlights; j++) {
			i = lightactive[j];
			if (!LightList[i]._ldel)
			{
				DoLighting(LightList[i]._lx, LightList[i]._ly, LightList[i]._lradius, i);
			}
		}
		for (j = 0; j < numlights; ) {
			i = lightactive[j];
			if (LightList[i]._ldel) {
				temp = lightactive[--numlights];
				lightactive[numlights] = lightactive[j];
				lightactive[j] = temp;
			}
			else
				j++;
		}
	}
	dolighting = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SavePreLighting()
{
	CopyMemory(dSaveLight,dLight,sizeof(dSaveLight));
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitVision()
{
	int i;

	numvision = 0;
	dovision = FALSE;
	visionid = 1;

	for (i = 0; i < TransVal; i++) TransList[i] = FALSE;

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int AddVision(int x, int y, int r, BOOL mine)
{
	int vid;

	//if (lightflag != 0) return(-1);
	if (numvision < MAXVISION) {
		VisionList[numvision]._lx = x;
		VisionList[numvision]._ly = y;
		VisionList[numvision]._lradius = r;
		vid = visionid++;
		VisionList[numvision]._lid = vid;
		VisionList[numvision]._ldel = FALSE;
		VisionList[numvision]._lunflag = FALSE;
		VisionList[numvision]._lflags = mine ? LFLAG_MINE : 0;
		numvision++;
		dovision = TRUE;
	}
	return(vid);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddUnVision(int id)
{
	int	i;

	//if (lightflag != 0) return;
	for (i = 0; i < numvision; i++) {
		if (VisionList[i]._lid == id) {
			VisionList[i]._ldel = TRUE;
			dovision = TRUE;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ChangeVisionRadius(int id, int r)
{
	int	i;

	//if (lightflag != 0) return;
	for (i = 0; i < numvision; i++) {
		if (VisionList[i]._lid == id) {
			VisionList[i]._lunflag = TRUE;
			VisionList[i]._lunx = VisionList[i]._lx;
			VisionList[i]._luny = VisionList[i]._ly;
			VisionList[i]._lunr = VisionList[i]._lradius;
			VisionList[i]._lradius = r;
			dovision = TRUE;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ChangeVisionXY(int id, int x, int y)
{
	int	i;

	//if (lightflag != 0) return;
	for (i = 0; i < numvision; i++) {
		if (VisionList[i]._lid == id) {
			VisionList[i]._lunflag = TRUE;
			VisionList[i]._lunx = VisionList[i]._lx;
			VisionList[i]._luny = VisionList[i]._ly;
			VisionList[i]._lunr = VisionList[i]._lradius;
			VisionList[i]._lx = x;
			VisionList[i]._ly = y;
			dovision = TRUE;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ChangeVision(int id, int x, int y, int r)
{
	int	i;

	//if (lightflag != 0) return;
	for (i = 0; i < numvision; i++) {
		if (VisionList[i]._lid == id) {
			VisionList[i]._lunflag = TRUE;
			VisionList[i]._lunx = VisionList[i]._lx;
			VisionList[i]._luny = VisionList[i]._ly;
			VisionList[i]._lunr = VisionList[i]._lradius;
			VisionList[i]._lx = x;
			VisionList[i]._ly = y;
			VisionList[i]._lradius = r;
			dovision = TRUE;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ProcessVisionList()
{
	int	i;
	BOOL delflag;

	//if (lightflag != 0) return;
	if (dovision) {
		for (i = 0; i < numvision; i++) {
			if (VisionList[i]._ldel) DoUnVision(VisionList[i]._lx, VisionList[i]._ly, VisionList[i]._lradius);
			if (VisionList[i]._lunflag) {
				DoUnVision(VisionList[i]._lunx, VisionList[i]._luny, VisionList[i]._lunr);
				VisionList[i]._lunflag = FALSE;
			}
		}
		for (i = 0; i < TransVal; i++) TransList[i] = FALSE;
		for (i = 0; i < numvision; i++)
			if (!VisionList[i]._ldel)
				DoVision(VisionList[i]._lx, VisionList[i]._ly, VisionList[i]._lradius,
						 VisionList[i]._lflags & LFLAG_MINE, VisionList[i]._lflags & LFLAG_MINE);
		do {
			delflag = FALSE;
			for (i = 0; i < numvision; i++) {
				if (VisionList[i]._ldel) {
					numvision--;
					if ((numvision > 0) && (i != numvision)) VisionList[i] = VisionList[numvision];
					delflag = TRUE;
				}
			}
		} while (delflag);
	}
	dovision = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void BloodCycle()
{
	int i,k,tlen;
	BYTE h;
	BYTE *pLT;

	// Cycle the blood
	pLT = pLightTbl;
	if (light4flag) tlen = 4;
	else tlen = 16;
	if (leveltype == 4) {
		pLT = pLightTbl;
		for (i = 0; i < tlen; i++) {
			pLT++;
			h = *pLT;
			for (k = 0; k < 30; k++) {
				*pLT = *(pLT+1);
				*pLT++;
			}
			*pLT = h;
			pLT += 225;
		}
	}
}
