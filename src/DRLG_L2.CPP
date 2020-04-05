/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Dungeon file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L2.CPP 2     1/30/97 2:43p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
** CreateL2Dungeon
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"drlg_l2.h"
#include	"gendung.h"
#include	"engine.h"
#include	"trigs.h"
#include	"lighting.h"
#include	"monster.h"
#include	"objects.h"
#include	"quests.h"
#include	"themes.h"

/*-----------------------------------------------------------------------**
** Registration info
**-----------------------------------------------------------------------*/
#include "regconst.h"
char sgszRegSig3[REG_LEN] = "REGISTRATION_BLOCK";

/*-----------------------------------------------------------------------**
** File Variables
**-----------------------------------------------------------------------*/

int Area_Min = AREA_MIN;
int Room_Max = ROOM_MAX;
//int Room_Min = ROOM_MIN;
int Room_Min = 4;

HALLNODE * pHallList;
ROOMNODE	RoomList [MAX_ROOMS + 1];
BYTE predungeon[MDMAXX][MDMAXY];
int Dir_Xadd[5] =  {0,  0, 1, 0, -1};
int Dir_Yadd[5] =  {0, -1, 0, 1,  0};
int nRoomCnt;
int nSx1, nSy1, nSx2, nSy2;

ShadowStruct	SPATSL2[L2_NUMSPATS] = {
				{ 6, 3, 0, 3, 48, 0, 50 },
				{ 9, 3, 0, 3, 48, 0, 50 }
};

// Types for tile substitution
byte BTYPESL2[L2_NUMBLOCKS] = { 0,
		1, 2, 3, 4, 5, 6, 7, 8,	9,											// L2Base
		0, 0, 0, 0, 0, 0, 0,												// L2Dirt
		17, 18,																// L2Tops
		1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 8,			// L2Misc
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,								// L2Archs
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,						// L2Skel
		1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1,									// L2Blood
		0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3,									// L2Ruins
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 3, 0, 3,			// L2Flats
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// L2Big
		0, 0, 0, 0, 0, 0,
		0, 0, 0,															// L2New
		0, 0, 0, 0, 0, 0, 0,												// L2Dirt2
		0, 0, 0, 0, 0, 0, 0, 0,												// L2NewDrs
		0, 0, 0 };															// L2Stair2

byte BSTYPESL2[L2_NUMBLOCKS] = { 0,
		1, 2, 3, 0, 0, 6, 0, 0,	9,											// L2Base
		0, 0, 0, 0, 0, 0, 0,												// L2Dirt
		0, 0,																// L2Tops
		1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,			// L2Misc
		6, 6, 6, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0,								// L2Archs
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,						// L2Skel
		1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1,									// L2Blood
		1, 1, 1, 6, 2, 2, 2, 0, 3, 3, 3, 3,									// L2Ruins
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,			// L2Flats
		3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1,	// L2Big
		3, 3, 2, 2, 3, 3,
		0, 0, 0,															// L2New
		0, 0, 0, 0, 0, 0, 0,												// L2Dirt2
		0, 0, 0, 0, 0, 0, 0, 0,												// L2NewDrs
		0, 0, 0 };															// L2Stair2

byte VARCH1[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, URWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,
					47, 44,
					0, 0};
byte VARCH2[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, ULWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,	
					47, 44,
					0, 0};
byte VARCH3[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, LRWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,	
					47, 44,
					0, 0};
byte VARCH4[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, LLWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,	
					47, 44,
					0, 0};
byte VARCH5[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, DURWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,	
					47, 44,
					0, 0};
byte VARCH6[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, DULWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,	
					47, 44,
					0, 0};
byte VARCH7[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLRWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,	
					47, 44,
					0, 0};
byte VARCH8[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VWALL_PIECE,
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLLWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,	
					47, 44,
					0, 0};

byte VARCH9[] = { 2, 4,														// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, URWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};
byte VARCH10[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, ULWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};
byte VARCH11[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, LRWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};
byte VARCH12[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, LLWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};
byte VARCH13[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DURWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};
byte VARCH14[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DULWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};
byte VARCH15[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLRWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};
byte VARCH16[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLLWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 42,														
					47, 44,
					0, 0};

byte VARCH17[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, URWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};
byte VARCH18[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, ULWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};
byte VARCH19[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, LRWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};
byte VARCH20[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, LLWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};

byte VARCH21[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, DURWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};
byte VARCH22[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, DULWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};
byte VARCH23[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLRWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};
byte VARCH24[] = { 2, 3,													// X size, Y size

					HWALL_PIECE, URWALL_PIECE,								// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLLWALL_PIECE,

					141, 39,														// Pattern to sub
					47, 44,
					0, 0};

byte VARCH25[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, URWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,
					47, 44,
					0, 0};
byte VARCH26[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, ULWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,														
					47, 44,
					0, 0};
byte VARCH27[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, LRWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,														
					47, 44,
					0, 0};
byte VARCH28[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, LLWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,														
					47, 44,
					0, 0};

byte VARCH29[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, DURWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,														
					47, 44,
					0, 0};
byte VARCH30[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, DULWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,														
					47, 44,
					0, 0};
byte VARCH31[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, DLRWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,														
					47, 44,
					0, 0};
byte VARCH32[] = { 2, 4,													// X size, Y size

					FLOOR_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, VDOOR_PIECE,
					FLOOR_PIECE, VWALL_PIECE,
					0, DLLWALL_PIECE,

					48, 0,														// Pattern to sub
					51, 39,														
					47, 44,
					0, 0};

byte VARCH33[] = { 2, 4,														// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, URWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};
byte VARCH34[] = { 2, 4,													// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, ULWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};
byte VARCH35[] = { 2, 4,													// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, LRWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};
byte VARCH36[] = { 2, 4,													// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, LLWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};
byte VARCH37[] = { 2, 4,													// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DURWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};
byte VARCH38[] = { 2, 4,													// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DULWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};
byte VARCH39[] = { 2, 4,													// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLRWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};
byte VARCH40[] = { 2, 4,													// X size, Y size

					HWALL_PIECE, 0,											// Pattern to look for
					FLOOR_PIECE, ULWALL_PIECE,								
					FLOOR_PIECE, VDOOR_PIECE,
					0, DLLWALL_PIECE,

					142, 0,														// Pattern to sub
					51, 42,
					47, 44,
					0, 0};


byte HARCH1[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, LLWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH2[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, LRWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH3[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, ULWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH4[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, URWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH5[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, DLLWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH6[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, DLRWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH7[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, DULWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH8[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HWALL_PIECE, HDOOR_PIECE, DURWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};

byte HARCH9[] = { 3, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, LLWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH10[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, LRWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH11[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, ULWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH12[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, URWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH13[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DLLWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH14[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DLRWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH15[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DULWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH16[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DURWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					43, 45, 0};

byte HARCH17[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, LLWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH18[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, LRWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH19[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, ULWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH20[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, URWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH21[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DLLWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH22[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DLRWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH23[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DULWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};
byte HARCH24[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					ULWALL_PIECE, HDOOR_PIECE, DURWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					43, 45, 0};

byte HARCH25[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, LLWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH26[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, LRWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH27[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, ULWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH28[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, URWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH29[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, DLLWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH30[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, DLRWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH31[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, DULWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH32[] = { 3, 2,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					HDOOR_PIECE, HWALL_PIECE, DURWALL_PIECE,

					49, 46, 0,													// Pattern to sub
					40, 45, 0};

byte HARCH33[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, LLWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH34[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, LRWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH35[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, ULWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH36[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, URWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH37[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, DLLWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH38[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, DLRWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH39[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, DULWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};
byte HARCH40[] = { 3, 2,													// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 0,							// Pattern to look for
					LLWALL_PIECE, HDOOR_PIECE, DURWALL_PIECE,

					140, 46, 0,													// Pattern to sub
					40, 45, 0};

/*byte USTAIRS[] = { 6, 6,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, // Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,

					0, 0, 0,  0,  0, 0,												// Pattern to sub
					0, 0, 0,  0,  0, 0,
					0, 0, 72, 77, 0, 0,
					0, 0, 76, 0,  0, 0,
					0, 0, 0,  0,  0, 0,
					0, 0, 0,  0,  0, 0};

byte DSTAIRS[] = { 6, 6,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, // Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,

					0, 0, 0,  0,  0, 0,												// Pattern to sub
					0, 0, 0,  0,  0, 0,
					0, 0, 48, 71, 0, 0,
					0, 0, 50, 78, 0, 0,
					0, 0, 0,  0,  0, 0,
					0, 0,  0, 0,  0, 0}; */

byte USTAIRS[] = { 4, 4,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,

					0, 0,  0,  0,
					0, 72, 77, 0,
					0, 76, 0,  0,
					0, 0,  0,  0};

byte DSTAIRS[] = { 4, 4,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,

					0, 0,  0,  0,
					0, 48, 71, 0,
					0, 50, 78, 0,
					0,  0, 0,  0};

// add to prevent confusion with lvl1 JKE
static byte WARPSTAIRS[] = { 4, 4,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,

					0, 0,  0,  0,
					0, 158, 160, 0,
					0, 159, 0,  0,
					0,  0, 0,  0};

byte CRUSHCOL[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					HWALL_PIECE, LRWALL_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,

					0, 0,  0,													// Pattern to sub
					0, 83, 0, 
					0, 0,  0};

byte BIG1[] = { 2, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 								// Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE,

					113, 0, 														// Pattern to sub
					112, 0};

byte BIG2[] = { 2, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE,								// Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE,

					114, 115,													// Pattern to sub
					0,   0};													

byte BIG3[] = { 1, 2,														// X size, Y size

					VWALL_PIECE,				 								// Pattern to look for
					VWALL_PIECE,

					117,	 														// Pattern to sub
					116};

byte BIG4[] = { 2, 1,														// X size, Y size

					HWALL_PIECE, HWALL_PIECE, 								// Pattern to look for

					118, 119};													// Pattern to sub

byte BIG5[] = { 2, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 								// Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE,

					120, 122,													// Pattern to sub
					121, 123};

byte BIG6[] = { 1, 2,														// X size, Y size

					VWALL_PIECE,				 								// Pattern to look for
					VWALL_PIECE,

					125,															// Pattern to sub
					124};

byte BIG7[] = { 2, 1,														// X size, Y size

					HWALL_PIECE, HWALL_PIECE, 								// Pattern to look for

					126, 127};													// Pattern to sub

byte BIG8[] = { 2, 2,														// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, 								// Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE,

					128, 130,													// Pattern to sub
					129, 131};

byte BIG9[] = { 2, 2,														// X size, Y size

					VWALL_PIECE, FLOOR_PIECE, 								// Pattern to look for
					VWALL_PIECE, FLOOR_PIECE,

					133, 135,													// Pattern to sub
					132, 134};

byte BIG10[] = { 2, 2,														// X size, Y size

					HWALL_PIECE, HWALL_PIECE, 								// Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE,

					136, 137,													// Pattern to sub
					3, 3};
//			138, 139};

byte RUINS1[] = { 1, 1,														// X size, Y size

					VWALL_PIECE,				 								// Pattern to look for

					80};															// Pattern to sub
byte RUINS2[] = { 1, 1,														// X size, Y size

					VWALL_PIECE,				 								// Pattern to look for

					81};															// Pattern to sub
byte RUINS3[] = { 1, 1,														// X size, Y size

					VWALL_PIECE,				 								// Pattern to look for

					82};															// Pattern to sub

byte RUINS4[] = { 1, 1,														// X size, Y size

					HWALL_PIECE,				 								// Pattern to look for

					84};															// Pattern to sub
byte RUINS5[] = { 1, 1,														// X size, Y size

					HWALL_PIECE,				 								// Pattern to look for

					85};															// Pattern to sub
byte RUINS6[] = { 1, 1,														// X size, Y size

					HWALL_PIECE,				 								// Pattern to look for

					86};															// Pattern to sub
byte RUINS7[] = { 1, 1,														// X size, Y size

					ULWALL_PIECE,				 								// Pattern to look for

					87};															// Pattern to sub

byte PANCREAS1[] = { 5, 3,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,	// Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					
					0, 0, 0,   0, 0,											// Pattern to sub
					0, 0, 108, 0, 0,
					0, 0, 0,   0, 0};

byte PANCREAS2[] = { 5, 3,													// X size, Y size

					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,	// Pattern to look for
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,
					FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE, FLOOR_PIECE,

					0, 0, 0,   0, 0, 											// Pattern to sub
					0, 0, 110, 0, 0,
					0, 0, 0,   0, 0};

byte CTRDOOR1[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, LLWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};
byte CTRDOOR2[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, ULWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};
byte CTRDOOR3[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, LRWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};
byte CTRDOOR4[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, URWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};
byte CTRDOOR5[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, DLLWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};
byte CTRDOOR6[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, DULWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};
byte CTRDOOR7[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, DLRWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};
byte CTRDOOR8[] = { 3, 3,													// X size, Y size

					FLOOR_PIECE, VWALL_PIECE, FLOOR_PIECE,				// Pattern to look for
					0, VDOOR_PIECE, 0,
					0, DURWALL_PIECE, 0, 

					0, 4, 0,			 											// Pattern to sub
					0, 1, 0,
					0, 0, 0};

int Patterns[100][10] = {
	{0,0,0,0,0,0,0,0,0,FLOOR_PIECE},										// catch all
	
	{0,0,0,0,CFLOOR,0,0,0,0,FLOOR_PIECE},								// fl
	{0,CEoF,0,0,CWALL,0,0,CDoF,0,HWALL_PIECE},						// thw
	{0,CDoF,0,0,CWALL,0,0,CEoF,0,HWALL_PIECE},						// bhw
	{0,0,0,CEoF,CWALL,CDoF,0,0,0,VWALL_PIECE},						// lvw
	{0,0,0,CDoF,CWALL,CEoF,0,0,0,VWALL_PIECE},						// rvw
	{0,CWALL,0,0,CDOOR,0,0,CWALL,0,VDOOR_PIECE},						// vd
	{0,0,0,CWALL,CDOOR,CWALL,0,0,0,HDOOR_PIECE},						// hd

	{0,CDoW,0,CDoW,CWALL,0,0,0,0,LRWALL_PIECE},						// lr
	{0,CDoW,0,0,CWALL,CDoW,0,0,0,LLWALL_PIECE},						// ll
	{0,0,0,CDoW,CWALL,0,0,CDoW,0,URWALL_PIECE},						// ur
	{0,0,0,0,CWALL,CDoW,0,CDoW,0,ULWALL_PIECE},						// ul
	
	{0,CDoW,0,CDoW,CDoW,0,CDoWoF,CDoW,0,URWALL_PIECE},				// tint points l
	{0,CDoW,CDoWoF,CDoW,CDoW,CDoW,0,0,0,LLWALL_PIECE},				// tint points u
	{0,CDoW,0,0,CDoW,CDoW,0,CDoW,CDoWoF,ULWALL_PIECE},				// tint points r

	{CDoW,CDoW,CDoW,CDoW,CDoW,CDoW,0,CDoW,0,ULWALL_PIECE},							// tint points d
	{CFLOOR,CDoW,CDoW,CDoW,CDoW,CDoW,0,CDoW,0,ULWALL_PIECE},							// tint points d
	{CEoF,CEoF,CEoF,CDoW,CDoW,CDoW,0,CDoW,0,ULWALL_PIECE},							// tint points d
	{CDoW,CDoW,CFLOOR,CDoW,CDoW,CDoW,0,CDoW,0,ULWALL_PIECE},							// tint points d
	{CDoW,CFLOOR,CDoW,CDoW,CDoW,CDoW,0,CDoW,0,ULWALL_PIECE},							// tint points d
	{CFLOOR,CDoW,CDoW,CDoW,CDoW,CDoW,0,CDoW,0,ULWALL_PIECE},							// tint points d
	{CDoW,CEoF,CEoF,CDoW,CDoW,CDoW,0,CDoW,0,ULWALL_PIECE},							// tint points d
	{CEMPTY,CEMPTY,CDoW,CDoW,CDoW,CDoW,CFLOOR,CDoW,CFLOOR,ULWALL_PIECE},			// tint points d
	
	{CFLOOR,CFLOOR,CFLOOR,CFLOOR,CDoW,CFLOOR,CFLOOR,CDoW,CFLOOR,URWALL_PIECE},	// line of walls topend
	{CFLOOR,CFLOOR,CFLOOR,CFLOOR,CDoW,CFLOOR,CDoW,CDoW,CDoW,URWALL_PIECE},		// line of walls topend
	{CFLOOR,CFLOOR,CDoW,CFLOOR,CDoW,CDoW,CFLOOR,CFLOOR,CDoW,LLWALL_PIECE},		// line of walls topend
	{CFLOOR,CDoW,CFLOOR,CFLOOR,CDoW,CFLOOR,CFLOOR,CFLOOR,CFLOOR,LRWALL_PIECE},	// line of walls bottomend
	{CFLOOR,CFLOOR,CFLOOR,CFLOOR,CDoW,CDoW,CFLOOR,CFLOOR,CFLOOR,LLWALL_PIECE},	// line of walls bottomend
	{CFLOOR,CFLOOR,CFLOOR,CDoW,CDoW,CFLOOR,CFLOOR,CFLOOR,CFLOOR,LRWALL_PIECE},	// line of walls bottomend
	{CFLOOR,CFLOOR,0,CFLOOR,CDoW,CDoW,CFLOOR,CFLOOR,0,LLWALL_PIECE},				// line of walls bottomend
	
	{0,0,0,0,CEMPTY,0,0,0,0,DFLOOR_PIECE},												// mt
	
	{0,CWALL,0,0,CWALL,CEMPTY,0,CWALL,0,DVWALL_PIECE},								// dvw
	{0,0,0,CWALL,CWALL,CWALL,0,CEMPTY,0,DHWALL_PIECE},								// dhw
	{0,0,0,CDoW,CWALL,CEMPTY,0,CWALL,0,DURWALL_PIECE},								// dur
	{0,CDoW,0,CWALL,CWALL,0,0,CEMPTY,0,DLRWALL_PIECE},								// dlr
	{0,CDoW,0,0,CWALL,CWALL,0,CEMPTY,0,DLLWALL_PIECE},								// dll
	{0,0,0,0,CWALL,CWALL,0,CWALL,CEMPTY,DULWALL_PIECE},							// dul
	
	{CDoWoF,CDoWoF,CDoWoF,CDoWoF,CWALL,CWALL,0,CWALL,CWALL,DULWALL_PIECE},		// double hwall start
	{CDoWoF,CDoWoF,CEMPTY,CDoWoF,CWALL,CWALL,0,CWALL,CWALL,DVWALL_PIECE},		// double hwall start
	{0,0,0,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,DHWALL_PIECE},						// double hwall cont1
	{CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CFLOOR,CFLOOR,CDoWoF,HWALL_PIECE},		// double hwall cont2
	{0,CWALL,0,CWALL,CWALL,CEMPTY,CWALL,CWALL,0,DLRWALL_PIECE},						// double hwall end1
	{0,0,0,CWALL,CWALL,CWALL,CWALL,CWALL,CEMPTY,DHWALL_PIECE},						// double hwall end2
	{CWALL,CWALL,CEMPTY,CWALL,CWALL,CWALL,0,CFLOOR,CFLOOR,HWALL_PIECE},			// double hwall end3
	{CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CDoW,CFLOOR,CDoW,HWALL_PIECE},			// double hwall end4
	{CEMPTY,CWALL,CWALL,CWALL,CWALL,CWALL,CDoW,CFLOOR,CDoW,HWALL_PIECE},			// double hwall end5
	{CFLOOR,CFLOOR,CFLOOR,CWALL,CWALL,CWALL,CEMPTY,CWALL,CWALL,DHWALL_PIECE},	// double hwall end6
	{CEMPTY,CWALL,CWALL,CWALL,CWALL,CWALL,CFLOOR,CFLOOR,CFLOOR,HWALL_PIECE},	// double hwall end7
	{CWALL,CWALL,CEMPTY,CWALL,CWALL,CWALL,CFLOOR,CFLOOR,CWALL,HWALL_PIECE},		// double hwall end8
	{CEMPTY,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CFLOOR,CFLOOR,HWALL_PIECE}, 	// double hwall end9
	{CFLOOR,CFLOOR,CDoW,CWALL,CWALL,CWALL,CEMPTY,CWALL,CWALL,DHWALL_PIECE}, 	// double hwall end10
	{CEMPTY,CWALL,CWALL,CWALL,CWALL,CWALL,CFLOOR,CFLOOR,CDoW,HWALL_PIECE}, 		// double hwall end11
	{CWALL,CFLOOR,CFLOOR,CWALL,CWALL,CWALL,CEMPTY,CWALL,CWALL,DHWALL_PIECE}, 	// double hwall end12
	
	{0,CWALL,CWALL,0,CWALL,CWALL,0,CWALL,CWALL,DVWALL_PIECE},						// double vwall cont1
	{CFLOOR,CWALL,CWALL,CDOOR,CWALL,CWALL,CFLOOR,CWALL,CWALL,DURWALL_PIECE},	// double vwall cont 1a
	{CWALL,CWALL,0,CWALL,CWALL,CFLOOR,CWALL,CWALL,0,VWALL_PIECE},					// double vwall cont2
	{0,CEMPTY,0,CWALL,CWALL,CWALL,0,CWALL,CWALL,DURWALL_PIECE},						// double vwall end1
	{CEMPTY,CWALL,0,CWALL,CWALL,0,CWALL,CWALL,0,VWALL_PIECE},						// double vwall end2
	{0,CWALL,0,CEMPTY,CWALL,CWALL,0,CWALL,CWALL,DLLWALL_PIECE},						// double vwall end3
	{CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,0,CFLOOR,CFLOOR,HWALL_PIECE},			// double vwall end4
	{0,CWALL,CWALL,CFLOOR,CWALL,CWALL,CFLOOR,CWALL,CEMPTY,DVWALL_PIECE}, 		// double vwall end5
	{CFLOOR,CWALL,CWALL,CWALL,CWALL,CWALL,0,CEMPTY,0,DLRWALL_PIECE},				// double vwall end6
	{CWALL,CWALL,CEMPTY,CWALL,CWALL,CFLOOR,0,CWALL,CFLOOR,VWALL_PIECE},	 		// double vwall end7
	{CFLOOR,CWALL,CWALL,CFLOOR,CWALL,CWALL,CWALL,CWALL,CEMPTY,DVWALL_PIECE},	// double vwall end8
	{CWALL,CWALL,CFLOOR,CWALL,CWALL,CFLOOR,CEMPTY,CWALL,CDoWoF,VWALL_PIECE},	// double vwall end9
	{CFLOOR,CWALL,CEMPTY,CWALL,CWALL,CWALL,CEMPTY,CEMPTY,CWALL,DLRWALL_PIECE},	// double vwall end10
	
	{CFLOOR,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,DLRWALL_PIECE}, 	// 2doubles LR connect
	{CWALL,CWALL,CFLOOR,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,DLLWALL_PIECE}, 	// 2doubles LL connect
	{CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CFLOOR,CWALL,CWALL,DURWALL_PIECE},  	// 2doubles UR connect
	{CEMPTY,CWALL,CWALL,CWALL,CWALL,CWALL,CFLOOR,CWALL,CWALL,DURWALL_PIECE},  	// 2doubles UR connect
	{CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CWALL,CFLOOR,ULWALL_PIECE},  	// 2doubles UL connect
	
	{0,0,0,0,255,0,0,0,0,0}													// end
};

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static BOOL DRLG_L2PlaceMiniSet(byte miniset[], int tmin, int tmax, int cx, int cy, int setview, int ldir)
{
	int sx, sy;
	int sw, sh;
	int xx, yy;
	int i, ii, numt;
	int found, bailcnt;

	// Width and height of the mini set piece
	sw = miniset[0];
	sh = miniset[1];

	// Number of pieces to place
	if ((tmax - tmin) == 0) numt = 1;
	else numt = random(0, tmax - tmin) + tmin;

	for (i = 0; i < numt; i++) {
		// Random starting pos
		sx = random(0, MDMAXX-sw);
		sy = random(0, MDMAXY-sh);

		// Find a location for the mini set piece
		found = 0;
		bailcnt = 0;
		while ((found == 0) && (bailcnt < 200)) {
			found = 1;
			
			if (((sx >= nSx1) && (sx <= nSx2)) && ((sy >= nSy1) && (sy <= nSy2))) found = 0;

			if ((cx != -1) && (sx >= (cx - sw)) && (sx <= (cx + 12))) {
				sx = random(0, MDMAXX-sw);
				sy = random(0, MDMAXY-sh);
				found = 0;
			}
			if ((cy != -1) && (sy >= (cy - sh)) && (sy <= (cy + 12))) {
				sx = random(0, MDMAXX-sw);
				sy = random(0, MDMAXY-sh);
				found = 0;
			}
			ii = 2;
			for (yy = 0; ((yy < sh) && (found == 1)); yy++) {
				for (xx = 0; ((xx < sw) && (found == 1)); xx++) {
					if ((miniset[ii] != 0) && (dungeon[sx+xx][sy+yy] != miniset[ii])) found = 0;
					if (dflags[sx+xx][sy+yy] != 0) found = 0;
					ii++;
				}
			}
			if (found == 0) {
				sx++;
				if (sx == (MDMAXX - sw)) {
					sx = 0;
					sy++;
					if (sy == (MDMAXY - sh)) sy = 0;
				}
			}
			bailcnt++;
		}

		if (bailcnt >= 200) return(FALSE);

		// Place mini set piece
		ii = (sh * sw) + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) dungeon[sx+xx][sy+yy] = miniset[ii];
				ii++;
			}
		}
	}

	if (setview == 1) {
		ViewX = (sx << 1) + 3 + (DIRTEDGED2) + 2;
		ViewY = (sy << 1) + 4 + (DIRTEDGED2) + 2;
	}

	if (ldir == LVL_DOWN) {
		LvlViewX = (sx << 1) + 3 + (DIRTEDGED2) + 2;
		LvlViewY = (sy << 1) + 4 + (DIRTEDGED2) + 2;
	}

	if (ldir == LVL_TWARPDN) {
		LvlViewX = (sx << 1) + 3 + (DIRTEDGED2) + 2;
		LvlViewY = (sy << 1) + 4 + (DIRTEDGED2) + 2;
	}

	return(TRUE);
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2PlaceRndSet(byte miniset[], int rndper)
{
	int sx, sy;
	int sw, sh;
	int xx, yy;
	int ii, jj, kk;
	int found;

	// Width and height of the mini set piece
	sw = miniset[0];
	sh = miniset[1];

	// Find a location for the mini set piece
	for (sy = 0; sy < (MDMAXY - sh); sy++) {
		for (sx = 0; sx < (MDMAXX - sw); sx++) {
			found = 1;
			ii = 2;
			
			if (((sx >= nSx1) && (sx <= nSx2)) && ((sy >= nSy1) && (sy <= nSy2))) found = 0;

			for (yy = 0; ((yy < sh) && (found == 1)); yy++) {
				for (xx = 0; ((xx < sw) && (found == 1)); xx++) {
					if ((miniset[ii] != 0) && (dungeon[sx+xx][sy+yy] != miniset[ii])) found = 0;
					if (dflags[sx+xx][sy+yy] != 0) found = 0;
					ii++;
				}
			}
			
			kk = (sh * sw) + 2;
			
			if (found == 1) {
				for (ii = sy - sh; ((ii < (sy + (sh << 1))) && (found == 1)); ii ++) {
					for (jj = sx - sw; jj < sx + (sw << 1); jj ++)
						if (dungeon[jj][ii] == miniset[kk]) found = 0;
				}
			}

			if ((found == 1) && (random(0, 100) < rndper)) {
				// Place mini set piece
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
						if (miniset[kk] != 0) dungeon[sx+xx][sy+yy] = miniset[kk];
						kk++;
					}
				}
			}
		}
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2Subs()
{
	int	x, y, i, j, k, rv;
	byte	c;

	for (y = 0; y < MDMAXY; y++) {
		for (x = 0; x < MDMAXX; x++) {
			if (((x < nSx1) || (x > nSx2)) && ((y < nSy1) || (y > nSy2))) {
				rv = random(0, 4);
				if (rv == 0) {
					c = dungeon[x][y];
					c = BTYPESL2[c];
					if (c != 0) {
						rv = random(0, 16);
						i = -1;
						while (rv >= 0) {
							i++;
							if (i == L2_NUMBLOCKS) i = 0;
							if (c == BTYPESL2[i]) rv--;
						}

						for (j = y - 2; j < y + 2; j++) {
							for (k = x - 2; k < x + 2; k++) {
								if (dungeon[k][j] == i) {
									j = y + 3; // indicate a hit
									k = x + 2;
								}
							}
						}
						if (j < y + 3) dungeon[x][y] = i;
					}
				}
			}
		}
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2Shadows()
{
	int	x, y, i, patflag;
	byte	sd[2][2];

	for (y = 1; y < MDMAXY; y++) {
		for (x = 1; x < MDMAXX; x++) {
			if ((x == 60) && (y == 21)) patflag = 1;
			sd[0][0] = BSTYPESL2[dungeon[x][y]];
			sd[1][0] = BSTYPESL2[dungeon[x-1][y]];
			sd[0][1] = BSTYPESL2[dungeon[x][y-1]];
			sd[1][1] = BSTYPESL2[dungeon[x-1][y-1]];
			for (i = 0; i < L2_NUMSPATS; i++) {
				if (SPATSL2[i].strig == sd[0][0]) {
					patflag = 1;
					if ((SPATSL2[i].s1 != 0) && (SPATSL2[i].s1 != sd[1][1])) patflag = 0;
					if ((SPATSL2[i].s2 != 0) && (SPATSL2[i].s2 != sd[0][1])) patflag = 0;
					if ((SPATSL2[i].s3 != 0) && (SPATSL2[i].s3 != sd[1][0])) patflag = 0;
					if (patflag == 1) {
						if (SPATSL2[i].nv1 != 0) dungeon[x-1][y-1] = SPATSL2[i].nv1;
						if (SPATSL2[i].nv2 != 0) dungeon[x][y-1] = SPATSL2[i].nv2;
						if (SPATSL2[i].nv3 != 0) dungeon[x-1][y] = SPATSL2[i].nv3;
					}
				}
			}
		}
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitDungeon()
{
	int i, j;
   
	for (i = 0; i < MDMAXY; i++) {

		for (j = 0; j < MDMAXX; j++) {
			predungeon[j][i] = NO_CHAR;
			dflags[j][i] = 0;
		}	
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_LoadL2SP()
{
	setloadflag = FALSE;
	if (QuestStatus(Q_BLIND)) {
		pSetPiece = LoadFileInMemSig("Levels\\L2Data\\Blind2.DUN",NULL,'STPC');
		setloadflag = TRUE;
	}
	else if (QuestStatus(Q_BLOOD)) {
		pSetPiece = LoadFileInMemSig("Levels\\L2Data\\Blood1.DUN",NULL,'STPC');
		setloadflag = TRUE;
	}
	else if (QuestStatus(Q_SCHAMB)) {
		pSetPiece = LoadFileInMemSig("Levels\\L2Data\\Bonestr2.DUN",NULL,'STPC');
		setloadflag = TRUE;
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_FreeL2SP() {
	DiabloFreePtr(pSetPiece);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2SetRoom(int rx1, int ry1)
{
	int		rw,rh;
	int		i,j;
	byte	*sp;

	sp = pSetPiece;
	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;

	//DRLG_MRectTrans(rx1, ry1, rx2, ry2);
	//DRLG_MRectTrans(rx1+2, ry1+2, rx2-2, ry2-2);

	sp = pSetPiece+4;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dungeon[rx1+i][ry1+j] = *sp;
				dflags[rx1+i][ry1+j] |= SETP_BIT;
			} else dungeon[rx1+i][ry1+j] = 3;
			sp+=2;
		}
	}
 }
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DefineRoom(int nX1, int nY1, int nX2, int nY2, int ForceHW)
{
	int i, j;
	BYTE ft;

	// Four corners
	predungeon[nX1][nY1] = ULWALL_CHAR;
	predungeon[nX1][nY2] = LLWALL_CHAR;
	predungeon[nX2][nY1] = URWALL_CHAR;
	predungeon[nX2][nY2] = LRWALL_CHAR;

	nRoomCnt++;

	RoomList[nRoomCnt].nRoomx1 = nX1;
	RoomList[nRoomCnt].nRoomx2 = nX2;

	RoomList[nRoomCnt].nRoomy1 = nY1;
	RoomList[nRoomCnt].nRoomy2 = nY2;
	if (ForceHW ==1) {
		for (i = nX1; i < nX2;i++) {
			for (j = nY1; i < nY2; i++) {
				dflags[i][j] |= SETP_BIT;
			}
		}
	}
	// Horizontal top of room
	for (i = nX1 + 1; i <= nX2 - 1; i++) {
		predungeon[i][nY1] = WALL_CHAR;
		predungeon[i][nY2] = WALL_CHAR;
	}
	
	nY1++;
	nY2--;
	ft = FLOOR_CHAR;

	for (i = nY1; i <= nY2; i++) {
		predungeon[nX1][i] = WALL_CHAR;
		predungeon[nX2][i] = WALL_CHAR;
		j = nX1 + 1;
		
		while (j < nX2) {
			predungeon[j][i] = ft;
			j++;
		}
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void CreateDoorType(int nX, int nY)
{
	char dt;
	BOOL fDoneflag;

	fDoneflag = FALSE;
	
	dt = predungeon[nX - 1][nY];
	if (dt == DOOR_CHAR) fDoneflag = TRUE;
	
	dt = predungeon[nX + 1][nY];
	if (dt == DOOR_CHAR) fDoneflag = TRUE;
	
	dt = predungeon[nX][nY - 1];
	if (dt == DOOR_CHAR) fDoneflag = TRUE;
	
	dt = predungeon[nX][nY + 1];
	if (dt == DOOR_CHAR) fDoneflag = TRUE;

	if (predungeon[nX][nY] == URWALL_CHAR  || predungeon[nX][nY] == ULWALL_CHAR ||
		predungeon[nX][nY] == LRWALL_CHAR  || predungeon[nX][nY] == LLWALL_CHAR)
		fDoneflag = TRUE;

	if (!fDoneflag) predungeon[nX][nY] = DOOR_CHAR;
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void PlaceHallExt(int nX, int nY)
{
	if (predungeon[nX][nY] == NO_CHAR)
		predungeon[nX][nY] = HALL_CHAR;
}
#endif


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void AddHall(int nX1, int nY1, int nX2, int nY2, int nHd)
{
	HALLNODE * p1;
	HALLNODE * p2;

	if (pHallList == NULL) {
		pHallList = (HALLNODE *) DiabloAllocPtrSig(sizeof HALLNODE,'HALL');
		pHallList->nHallx1 = nX1;
		pHallList->nHally1 = nY1;
		pHallList->nHallx2 = nX2;
		pHallList->nHally2 = nY2;
		pHallList->nHalldir = nHd;
		pHallList->pNext = NULL;
	}
	else {
		p2 = (HALLNODE *) DiabloAllocPtrSig(sizeof HALLNODE,'HALL');
		p2->nHallx1 = nX1;
		p2->nHally1 = nY1;
		p2->nHallx2 = nX2;
		p2->nHally2 = nY2;
		p2->nHalldir = nHd;
		p2->pNext = NULL;
		p1 = pHallList;
		
		while (p1->pNext != NULL)
			p1 = p1->pNext;
		p1->pNext = p2;
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void CreateRoom(int nX1, int nY1, int nX2, int nY2, int nRDest, int nHDir, int ForceHW, int nH, int nW)
{
	int nAw, nAh;
	int nRw, nRh;
	int nRx1, nRy1, nRx2, nRy2;
	int nHx1, nHy1, nHx2, nHy2;
	int nRid;

	if (nRoomCnt >= MAX_ROOMS) return;

	nAw = nX2 - nX1;
	nAh = nY2 - nY1;

	if ((nAw >= Area_Min) && (nAh >= Area_Min)) {
		if ((nAw > Room_Max))
			nRw = random(0, Room_Max - Room_Min) + Room_Min;
		else {
			if (nAw > Room_Min)
				nRw = random(0, nAw - Room_Min) + Room_Min;
			else
				nRw = nAw;
		}
				
		if (nAh > Room_Max)
			nRh = random(0, Room_Max - Room_Min) + Room_Min;
		else {
			if	(nAh > Room_Min)
				nRh = random(0, nAh - Room_Min) + Room_Min;
			else
				nRh = nAh;
		}

		if (ForceHW == 1) {
			nRw = nW;
			nRh = nH;
		}

		nRx1 = random(0, nX2 - nX1) + nX1;
		nRy1 = random(0, nY2 - nY1) + nY1;
		nRx2 = nRx1 + nRw;
		nRy2 = nRy1 + nRh;

		if (nRx2 > nX2) {
			nRx2 = nX2;
			nRx1 = nRx2 - nRw;
		}

		if (nRy2 > nY2) {
			nRy2 = nY2;
			nRy1 = nRy2 - nRh;
		}

		if (nRx1 >= MDMAXX - 2) nRx1 = MDMAXX - 2;
		if (nRy1 >= MDMAXY - 2) nRy1 = MDMAXY - 2;
		if (nRx1 <=  1) nRx1 = 1;
		if (nRy1 <=  1) nRy1 = 1;
		if (nRx2 >= MDMAXX - 2) nRx2 = MDMAXX - 2;
		if (nRy2 >= MDMAXY - 2) nRy2 = MDMAXY - 2;
		if (nRx2 <=  1) nRx2 = 1;
		if (nRy2 <=  1) nRy2 = 1;

		DefineRoom(nRx1, nRy1, nRx2, nRy2, ForceHW);
		if (ForceHW == 1) {
			nSx1 = nRx1 + 2;
			nSy1 = nRy1 + 2;
			nSx2 = nRx2;
			nSy2 = nRy2;
		}


		nRid = nRoomCnt;
		RoomList[nRid].nRoomDest = nRDest;

		if (nRDest != 0) {
			if (nHDir == DIR_NORTH) {
				nHx1 = random(0, nRx2 - nRx1 - 2) + nRx1 + 1;
				nHy1 = nRy1;
				nHx2 = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
				nHx2 = random(0, nHx2) + RoomList[nRDest].nRoomx1 + 1;
				nHy2 = RoomList[nRDest].nRoomy2;
			}
			if (nHDir == DIR_SOUTH) {
				nHx1 = random(0, nRx2 - nRx1 - 2) + nRx1 + 1;
				nHy1 = nRy2;
				nHx2 = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
				nHx2 = random(0, nHx2) + RoomList[nRDest].nRoomx1 + 1;
				nHy2 = RoomList[nRDest].nRoomy1;
			}
			if (nHDir == DIR_EAST) {
				nHx1 = nRx2;
				nHy1 = random(0, nRy2 - nRy1 - 2) + nRy1 + 1;
				nHx2 = RoomList[nRDest].nRoomx1;
				nHy2 = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
				nHy2 = random(0, nHy2) + RoomList[nRDest].nRoomy1 + 1;
			}
			if (nHDir == DIR_WEST) {
				nHx1 = nRx1;
				nHy1 = random(0, nRy2 - nRy1 - 2) + nRy1 + 1;
				nHx2 = RoomList[nRDest].nRoomx2;
				nHy2 = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
				nHy2 = random(0, nHy2) + RoomList[nRDest].nRoomy1 + 1;
			}
			AddHall(nHx1, nHy1, nHx2, nHy2, nHDir);
		}

		if (nRh > nRw) {
			CreateRoom(nX1  + SNUM2, nY1  + SNUM2, nRx1 - SNUM1, nRy2 - SNUM1, nRid, DIR_EAST, 0, 0, 0);
			CreateRoom(nRx2 + SNUM1, nRy1 + SNUM1, nX2  - SNUM2, nY2  - SNUM2, nRid, DIR_WEST, 0, 0, 0);
			CreateRoom(nX1  + SNUM2, nRy2 + SNUM1, nRx2 - SNUM1, nY2  - SNUM2, nRid, DIR_NORTH, 0, 0, 0);
			CreateRoom(nRx1 + SNUM1, nY1  + SNUM2, nX2  - SNUM2, nRy1 - SNUM1, nRid, DIR_SOUTH, 0, 0, 0);
		}
		else {
			CreateRoom(nX1  + SNUM2, nY1  + SNUM2, nRx2 - SNUM1, nRy1 - SNUM1, nRid, DIR_SOUTH, 0, 0, 0);
			CreateRoom(nRx1 + SNUM1, nRy2 + SNUM1, nX2  - SNUM2, nY2  - SNUM2, nRid, DIR_NORTH, 0, 0, 0);
			CreateRoom(nX1  + SNUM2, nRy1 + SNUM1, nRx1 - SNUM1, nY2  - SNUM2, nRid, DIR_EAST, 0, 0, 0);
			CreateRoom(nRx2 + SNUM1, nY1  + SNUM2, nX2  - SNUM2, nRy2 - SNUM1, nRid, DIR_WEST, 0, 0, 0);
		} 
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void GetHall(int * nX1, int * nY1, int * nX2, int * nY2, int * nHd)
{
	HALLNODE * p1;

	p1  = pHallList->pNext;
	*nX1 = pHallList->nHallx1;
	*nY1 = pHallList->nHally1;
	*nX2 = pHallList->nHallx2;
	*nY2 = pHallList->nHally2;
	*nHd = pHallList->nHalldir;
	DiabloFreePtr (pHallList);
	pHallList = p1;
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void ConnectHall(int nX1, int nY1, int nX2, int nY2, int nHd)
{
	BOOL fDoneflag, fInroom;
	int nCurrd, nDx, nDy, nRp;
	int nOrigX1, nOrigY1;
	int fMinusFlag, fPlusFlag;

 	fDoneflag = FALSE;
	fMinusFlag = random(0, 100);
	fPlusFlag = random(0, 100);
	nOrigX1 = nX1;
	nOrigY1 = nY1;

	CreateDoorType(nX1, nY1);
	CreateDoorType(nX2, nY2);

	nDx = abs(nX2 - nX1);
	nDy = abs(nY2 - nY1);

	nCurrd = nHd;
	nX2 = nX2 - Dir_Xadd[nCurrd];
	nY2 = nY2 - Dir_Yadd[nCurrd];
	predungeon[nX2][nY2] = HALL_CHAR;
	fInroom = FALSE;
	
	while (!fDoneflag) {
		if (nX1 >= MDMAXX - 2 && nCurrd == 2) nCurrd = 4;
		if (nY1 >= MDMAXY - 2 && nCurrd == 3) nCurrd = 1;
		if (nX1 <=  1    && nCurrd == 4) nCurrd = 2;
		if (nY1 <=  1    && nCurrd == 1) nCurrd = 3;

		if (predungeon[nX1][nY1] == ULWALL_CHAR && (nCurrd == 1 || nCurrd == 4))
			nCurrd = 2;
		if (predungeon[nX1][nY1] == URWALL_CHAR && (nCurrd == 1 || nCurrd == 2))
			nCurrd = 3;
		if (predungeon[nX1][nY1] == LLWALL_CHAR && (nCurrd == 4 || nCurrd == 3))
			nCurrd = 1;
		if (predungeon[nX1][nY1] == LRWALL_CHAR && (nCurrd == 2 || nCurrd == 3))
			nCurrd = 4;

		nX1 = nX1 + Dir_Xadd[nCurrd];
		nY1 = nY1 + Dir_Yadd[nCurrd];

		if (predungeon[nX1][nY1] == NO_CHAR) {
			if (fInroom)
				CreateDoorType(nX1 - Dir_Xadd[nCurrd], nY1 - Dir_Yadd[nCurrd]);
			else {
				if (fMinusFlag < 50) {
					if (nCurrd == 1 || nCurrd == 3)
						PlaceHallExt(nX1 - 1, nY1);
					else
						PlaceHallExt(nX1, nY1 - 1);
				}
				if (fPlusFlag < 50) {
					if (nCurrd == 1 || nCurrd == 3)
						PlaceHallExt(nX1 + 1, nY1);
					else
						PlaceHallExt(nX1, nY1 + 1);
				}
			}
			predungeon[nX1][nY1] = HALL_CHAR;
			fInroom = FALSE;
		}
		else {
			if (!fInroom && predungeon[nX1][nY1] == WALL_CHAR) CreateDoorType(nX1,nY1);
			if (predungeon[nX1][nY1] != HALL_CHAR) fInroom = TRUE;
		}

		nDx = abs(nX2 - nX1);
		nDy = abs(nY2 - nY1);

		if (nDx > nDy) {
			nRp = nDx * 2;
			if (nRp > 30) nRp = 30;
			if (random(0, 100) < nRp) {
				if ((nX2 > nX1) && (nX1 < MDMAXX))
					nCurrd = 2;
				else
					nCurrd = 4;
			}
		}
		else {
			nRp = nDy * 5;
			if (nRp > 80) nRp = 80;
			if (random(0, 100) < nRp) {
				if ((nY2 > nY1) && (nY1 < MDMAXY))
					nCurrd = 3;
				else 
					nCurrd = 1;
			}
		}

		if (nDy < 10) {
			if ((nX1 == nX2) && ((nCurrd == DIR_EAST) || (nCurrd == DIR_WEST))) {
				if ((nY2 > nY1) && (nY1 < MDMAXY))
					nCurrd = 3;
				else
					nCurrd = 1;
			}
		}

		if (nDx < 10) {
			if ((nY1 == nY2) && ((nCurrd == DIR_NORTH) || (nCurrd == DIR_SOUTH))) {
				if ((nX2 > nX1) && (nX1 < MDMAXX))
					nCurrd = 2;
				else
					nCurrd = 4;
			}
		}

		if (nDy == 1 && nDx > 1 && (nCurrd == DIR_NORTH || nCurrd == DIR_SOUTH)) {
			if ((nX2 > nX1) && (nX1 < MDMAXX))
				nCurrd = 2;
			else
				nCurrd = 4;
		}

		if (nDx == 1 && nDy > 1 && (nCurrd == DIR_EAST || nCurrd == DIR_WEST)) {
			if ((nY2 > nY1) && (nX1 < MDMAXY))
				nCurrd = 3;
			else
				nCurrd = 1;
		}

		if (nDx == 0 && predungeon[nX1][nY1] != NO_CHAR && (nCurrd == DIR_EAST || nCurrd == DIR_WEST)) {
			if ((nX2 > nOrigX1) && (nX1 < MDMAXX))
				nCurrd = 3;
			else
				nCurrd = 1;
		}

		if (nDy == 0 && predungeon[nX1][nY1] != NO_CHAR && (nCurrd == DIR_NORTH || nCurrd == DIR_SOUTH)) {
			if ((nY2 > nOrigY1) && (nY1 < MDMAXY))
				nCurrd = 2;
			else
				nCurrd = 4;
		}

		if ((nX1 == nX2) && (nY1 == nY2)) fDoneflag = TRUE;
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DoPatternCheck(int i, int j)
{
	int k, l;
	int x, y;
	int nOk;

	for (k = 0; Patterns[k][4] != 255; k++) {
		x = i - 1;
		y = j - 1;
		nOk = OK;

		for (l = 0; l < 9 && nOk == OK; l ++) {
			nOk = NOTOK;
			if (l == 3 || l == 6) {
				y ++;
				x = i - 1;
			}

			if (x < 0 || x >= MDMAXX || y < 0 || y >= MDMAXY)
				nOk = OK;
			else {
				switch (Patterns[k][l]) {
					case CNO:
						nOk = OK;
						break;
					case CWALL:
						if (predungeon[x][y] == WALL_CHAR)
							nOk = OK;
						break;
					case CFLOOR:
						if (predungeon[x][y] == FLOOR_CHAR)
							nOk = OK;
						break;
					case CEMPTY:
						if (predungeon[x][y] == NO_CHAR)
							nOk = OK;
						break;
					case CDOOR:
						if (predungeon[x][y] == DOOR_CHAR)
							nOk = OK;
						break;
					case CDoF:
						if (predungeon[x][y] == DOOR_CHAR || predungeon[x][y] == FLOOR_CHAR)
							nOk = OK;
						break;
					case CDoW:
						if (predungeon[x][y] == DOOR_CHAR || predungeon[x][y] == WALL_CHAR)
							nOk = OK;
						break;
					case CEoF:
						if (predungeon[x][y] == NO_CHAR || predungeon[x][y] == FLOOR_CHAR)
							nOk = OK;
						break;
					case CDoWoF:
						if (predungeon[x][y] == DOOR_CHAR || predungeon[x][y] == WALL_CHAR ||
							predungeon[x][y] == FLOOR_CHAR)
							nOk = OK;
						break;
				}
			}
			x ++;
		}
		if (nOk == OK)
			dungeon[i][j] = (BYTE) Patterns[k][9];
		else
			nOk = NOTOK;
	}
} 
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void L2TileFix()
{
	int i, j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] == 3)) dungeon[i][j+1] = 1;
			if ((dungeon[i][j] ==  3) && (dungeon[i][j+1] == 1)) dungeon[i][j+1] = 3;

			if ((dungeon[i][j] ==  3) && (dungeon[i+1][j] == 7)) dungeon[i+1][j] = 3;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 3)) dungeon[i+1][j] = 2;
			if ((dungeon[i][j] == 11) && (dungeon[i+1][j] == 14)) dungeon[i+1][j] = 16;
		}
	}
}
#endif

#define L2_CONT			0
#define L2_BOUNDS		1
#define L2_WALL			2

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static BOOL DL2_Cont(BOOL x1f, BOOL y1f, BOOL x2f, BOOL y2f)
{
	if (x1f && x2f && y1f && y2f) return(FALSE);
	if (x1f && x2f && (y1f || y2f)) return(TRUE);
	if (y1f && y2f && (x1f || x2f)) return(TRUE);
	return(FALSE);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static int DL2_NumNoChar()
{
	int t = 0;
	int ii, jj;
	for (jj = 0; jj < MDMAXY; jj++)
		for (ii = 0; ii < MDMAXX; ii++)
			if (predungeon[ii][jj] == NO_CHAR) t++;
	return(t);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DL2_DrawRoom(int x1, int y1, int x2, int y2)
{
	int ii, jj;

	for (jj = y1; jj <= y2; jj++) {
		for (ii = x1; ii <= x2; ii++) predungeon[ii][jj] = FLOOR_CHAR;
	}
	for (jj = y1; jj <= y2; jj++) {
		predungeon[x1][jj] = WALL_CHAR;
		predungeon[x2][jj] = WALL_CHAR;
	}
	for (ii = x1; ii <= x2; ii++) {
		predungeon[ii][y1] = WALL_CHAR;
		predungeon[ii][y2] = WALL_CHAR;
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DL2_KnockWalls(int x1, int y1, int x2, int y2)
{
	int ii, jj;

	for (ii = x1+1; ii < x2; ii++) {
		if ((predungeon[ii][y1-1] == FLOOR_CHAR) && (predungeon[ii][y1+1] == FLOOR_CHAR)) predungeon[ii][y1] = FLOOR_CHAR;
		if ((predungeon[ii][y2-1] == FLOOR_CHAR) && (predungeon[ii][y2+1] == FLOOR_CHAR)) predungeon[ii][y2] = FLOOR_CHAR;
		if (predungeon[ii][y1-1] == DOOR_CHAR) predungeon[ii][y1-1] = FLOOR_CHAR;
		if (predungeon[ii][y2+1] == DOOR_CHAR) predungeon[ii][y2+1] = FLOOR_CHAR;
	}
	for (jj = y1+1; jj < y2; jj++) {
		if ((predungeon[x1-1][jj] == FLOOR_CHAR) && (predungeon[x1+1][jj] == FLOOR_CHAR)) predungeon[x1][jj] = FLOOR_CHAR;
		if ((predungeon[x2-1][jj] == FLOOR_CHAR) && (predungeon[x2+1][jj] == FLOOR_CHAR)) predungeon[x2][jj] = FLOOR_CHAR;
		if (predungeon[x1-1][jj] == DOOR_CHAR) predungeon[x1-1][jj] = FLOOR_CHAR;
		if (predungeon[x2+1][jj] == DOOR_CHAR) predungeon[x2+1][jj] = FLOOR_CHAR;
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*
static void DL2_DoorOrEmptyX(int x, int yy, int y1, int y2)
{
	int sy1, sy2, ry;

	sy1 = sy2 = yy;
	while (((predungeon[x-1][sy1-1] == FLOOR_CHAR) && (predungeon[x+1][sy1-1] == FLOOR_CHAR)) && (sy1 > y1)) sy1--;
	while (((predungeon[x-1][sy2+1] == FLOOR_CHAR) && (predungeon[x+1][sy2+1] == FLOOR_CHAR)) && (sy2 < y2)) sy2++;
	if (random(0, 3) && ((sy2-sy1) > 1)) {
		ry = random(0, sy2 - sy1 - 1) + sy1 + 1;
		predungeon[x][ry] = DOOR_CHAR;
	} else {
		for (ry = sy1; ry <= sy2; ry++) predungeon[x][ry] = FLOOR_CHAR;
	}
}
*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*
static void DL2_DoorOrEmptyY(int y, int xx, int x1, int x2)
{
	int sx1, sx2, rx;

	sx1 = sx2 = xx;
	while (((predungeon[sx1-1][y-1] == FLOOR_CHAR) && (predungeon[sx1-1][y+1] == FLOOR_CHAR)) && (sx1 > x1)) sx1--;
	while (((predungeon[sx2+1][y-1] == FLOOR_CHAR) && (predungeon[sx2+1][y+1] == FLOOR_CHAR)) && (sx2 < x2)) sx2++;
	if (random(0, 3) && ((sx2-sx1) > 1)) {
		rx = random(0, sx2 - sx1 - 1) + sx1 + 1;
		predungeon[rx][y] = DOOR_CHAR;
	} else {
		for (rx = sx1; rx <= sx2; rx++) predungeon[rx][y] = FLOOR_CHAR;
	}
}
*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static BOOL DL2_FillVoids()
{
	int ii, jj, xx, yy;
	int x1, x2, y1, y2;
	BOOL xf1, xf2, yf1, yf2;

	int to = 0;
	while ((DL2_NumNoChar() > 700) && (to < 100)) {
		xx = random(0, MDMAXX-2) + 1;
		yy = random(0, MDMAXY-2) + 1;
		if (predungeon[xx][yy] == WALL_CHAR) {
			xf1 = xf2 = yf1 = yf2 = FALSE;
			if ((predungeon[xx-1][yy] == NO_CHAR) && (predungeon[xx+1][yy] == FLOOR_CHAR)) {
				if ((predungeon[xx+1][yy-1] == FLOOR_CHAR) && (predungeon[xx+1][yy+1] == FLOOR_CHAR) &&
					(predungeon[xx-1][yy-1] == NO_CHAR) && (predungeon[xx-1][yy+1] == NO_CHAR))
					xf1 = yf1 = yf2 = TRUE;
			} else
			if ((predungeon[xx+1][yy] == NO_CHAR) && (predungeon[xx-1][yy] == FLOOR_CHAR)) {
				if ((predungeon[xx-1][yy-1] == FLOOR_CHAR) && (predungeon[xx-1][yy+1] == FLOOR_CHAR) &&
					(predungeon[xx+1][yy-1] == NO_CHAR) && (predungeon[xx+1][yy+1] == NO_CHAR))
					xf2 = yf1 = yf2 = TRUE;
			} else
			if ((predungeon[xx][yy-1] == NO_CHAR) && (predungeon[xx][yy+1] == FLOOR_CHAR)) {
				if ((predungeon[xx-1][yy+1] == FLOOR_CHAR) && (predungeon[xx+1][yy+1] == FLOOR_CHAR) &&
					(predungeon[xx-1][yy-1] == NO_CHAR) && (predungeon[xx+1][yy-1] == NO_CHAR))
					yf1 = xf1 = xf2 = TRUE;
			} else
			if ((predungeon[xx][yy+1] == NO_CHAR) && (predungeon[xx][yy-1] == FLOOR_CHAR)) {
				if ((predungeon[xx-1][yy-1] == FLOOR_CHAR) && (predungeon[xx+1][yy-1] == FLOOR_CHAR) &&
					(predungeon[xx-1][yy+1] == NO_CHAR) && (predungeon[xx+1][yy+1] == NO_CHAR))
					yf2 = xf1 = xf2 = TRUE;
			}
			if (DL2_Cont(xf1,yf1,xf2,yf2)) {
				if (xf1) x1 = xx-1;
				else x1 = xx;
				if (xf2) x2 = xx+1;
				else x2 = xx;
				if (yf1) y1 = yy-1;
				else y1 = yy;
				if (yf2) y2 = yy+1;
				else y2 = yy;
				if (!xf1) {
					while (yf1 || yf2) {
						if (y1 == 0) yf1 = FALSE;
						if (y2 == MDMAXY-1) yf2 = FALSE;
						if ((y2 - y1) >= ROOM_MAX+4) {
							yf1 = FALSE;
							yf2 = FALSE;
						}
						if (yf1) y1--;
						if (yf2) y2++;
						if (predungeon[x2][y1] != NO_CHAR) yf1 = FALSE;
						if (predungeon[x2][y2] != NO_CHAR) yf2 = FALSE;
					}
					y1 += 2;
					y2 -= 2;
					if ((y2 - y1) > 5) {
						while (xf2) {
							if (x2 == MDMAXX-1) xf2 = FALSE;
							if ((x2 - x1) >= ROOM_MAX+2) xf2 = FALSE;
							for (jj = y1; jj <= y2; jj++)
								if (predungeon[x2][jj] != NO_CHAR) xf2 = FALSE;
							if (xf2) x2++;
						}
						x2 -= 2;
						if ((x2 - x1) > 5) {
							DL2_DrawRoom(x1, y1, x2, y2);
							DL2_KnockWalls(x1, y1, x2, y2);
							//DL2_DoorOrEmptyX(x1, yy, y1, y2);
						}
					}
				} else
				if (!xf2) {
					while (yf1 || yf2) {
						if (y1 == 0) yf1 = FALSE;
						if (y2 == MDMAXY-1) yf2 = FALSE;
						if ((y2 - y1) >= ROOM_MAX+4) {
							yf1 = FALSE;
							yf2 = FALSE;
						}
						if (yf1) y1--;
						if (yf2) y2++;
						if (predungeon[x1][y1] != NO_CHAR) yf1 = FALSE;
						if (predungeon[x1][y2] != NO_CHAR) yf2 = FALSE;
					}
					y1 += 2;
					y2 -= 2;
					if ((y2 - y1) > 5) {
						while (xf1) {
							if (x1 == 0) xf1 = FALSE;
							if ((x2 - x1) >= ROOM_MAX+2) xf1 = FALSE;
							for (jj = y1; jj <= y2; jj++)
								if (predungeon[x1][jj] != NO_CHAR) xf1 = FALSE;
							if (xf1) x1--;
						}
						x1 += 2;
						if ((x2 - x1) > 5) {
							DL2_DrawRoom(x1, y1, x2, y2);
							DL2_KnockWalls(x1, y1, x2, y2);
							//DL2_DoorOrEmptyX(x2, yy, y1, y2);
						}
					}
				} else
				if (!yf1) {
					while (xf1 || xf2) {
						if (x1 == 0) xf1 = FALSE;
						if (x2 == MDMAXX-1) xf2 = FALSE;
						if ((x2 - x1) >= ROOM_MAX+4) {
							xf1 = FALSE;
							xf2 = FALSE;
						}
						if (xf1) x1--;
						if (xf2) x2++;
						if (predungeon[x1][y2] != NO_CHAR) xf1 = FALSE;
						if (predungeon[x2][y2] != NO_CHAR) xf2 = FALSE;
					}
					x1 += 2;
					x2 -= 2;
					if ((x2 - x1) > 5) {
						while (yf2) {
							if (y2 == MDMAXY-1) yf2 = FALSE;
							if ((y2 - y1) >= ROOM_MAX+2) yf2 = FALSE;
							for (ii = x1; ii <= x2; ii++)
								if (predungeon[ii][y2] != NO_CHAR) yf2 = FALSE;
							if (yf2) y2++;
						}
						y2 -= 2;
						if ((y2 - y1) > 5) {
							DL2_DrawRoom(x1, y1, x2, y2);
							DL2_KnockWalls(x1, y1, x2, y2);
							//DL2_DoorOrEmptyY(y1, xx, x1, x2);
						}
					}
				} else
				if (!yf2) {
					while (xf1 || xf2) {
						if (x1 == 0) xf1 = FALSE;
						if (x2 == MDMAXX-1) xf2 = FALSE;
						if ((x2 - x1) >= ROOM_MAX+4) {
							xf1 = FALSE;
							xf2 = FALSE;
						}
						if (xf1) x1--;
						if (xf2) x2++;
						if (predungeon[x1][y1] != NO_CHAR) xf1 = FALSE;
						if (predungeon[x2][y1] != NO_CHAR) xf2 = FALSE;
					}
					x1 += 2;
					x2 -= 2;
					if ((x2 - x1) > 5) {
						while (yf1) {
							if (y1 == 0) yf1 = FALSE;
							if ((y2 - y1) >= ROOM_MAX+2) yf1 = FALSE;
							for (ii = x1; ii <= x2; ii++)
								if (predungeon[ii][y1] != NO_CHAR) yf1 = FALSE;
							if (yf1) y1--;
						}
						y1 += 2;
						if ((y2 - y1) > 5) {
							DL2_DrawRoom(x1, y1, x2, y2);
							DL2_KnockWalls(x1, y1, x2, y2);
							//DL2_DoorOrEmptyY(y2, xx, x1, x2);
						}
					}
				}
			}
			to++;
		}
	}
	if (DL2_NumNoChar() > 700) return(FALSE);
	return(TRUE);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static BOOL CreateDungeon()
{
	int i, j;
	int nHx1, nHy1, nHx2, nHy2, nHd;
	int ForceH,ForceW,ForceHW;
	
	ForceHW = ForceH = ForceW = 0;

	switch(currlevel) {
		case 5 :	
			if (quests[Q_BLOOD]._qactive != QUEST_NOTAVAIL) {
				ForceHW = 1;
				ForceH = 20;
				ForceW = 14;
			}
			break;
		case 6 :	
			if (quests[Q_SCHAMB]._qactive != QUEST_NOTAVAIL) {	
				ForceHW = 1;
				ForceH = ForceW = 10;
			}
			break;
		case 7 :
			if(quests[Q_BLIND]._qactive != QUEST_NOTAVAIL) {
				ForceHW = 1;
				ForceH = ForceW = 15;
			}
			break;
		case 8 :
			break;
		default:
			break;	// to use level 2 for crypt
	}

	CreateRoom(2, 2, MDMAXX - 1, MDMAXY - 1, 0, DIR_NONE, ForceHW, ForceH, ForceW);

	while (pHallList != NULL) {
		GetHall(&nHx1, &nHy1, &nHx2, &nHy2, &nHd);
		ConnectHall(nHx1, nHy1, nHx2, nHy2, nHd);
	}
	
	for (j = 0; j <= MDMAXY; j++) {
		for (i = 0; i <= MDMAXX; i++) {
			if (predungeon[i][j] == ULWALL_CHAR) predungeon[i][j] = WALL_CHAR;
			if (predungeon[i][j] == URWALL_CHAR) predungeon[i][j] = WALL_CHAR;
			if (predungeon[i][j] == LLWALL_CHAR) predungeon[i][j] = WALL_CHAR;
			if (predungeon[i][j] == LRWALL_CHAR) predungeon[i][j] = WALL_CHAR;
			if (predungeon[i][j] == HALL_CHAR) {
				predungeon[i][j] = FLOOR_CHAR;
				if (predungeon[i - 1][j - 1] == NO_CHAR) predungeon[i - 1][j - 1] = WALL_CHAR;				
				if (predungeon[i - 1][j - 0] == NO_CHAR) predungeon[i - 1][j - 0] = WALL_CHAR;				
				if (predungeon[i - 1][j + 1] == NO_CHAR) predungeon[i - 1][j + 1] = WALL_CHAR;				
				if (predungeon[i + 1][j - 1] == NO_CHAR) predungeon[i + 1][j - 1] = WALL_CHAR;				
				if (predungeon[i + 1][j - 0] == NO_CHAR) predungeon[i + 1][j - 0] = WALL_CHAR;				
				if (predungeon[i + 1][j + 1] == NO_CHAR) predungeon[i + 1][j + 1] = WALL_CHAR;				
				if (predungeon[i - 0][j - 1] == NO_CHAR) predungeon[i - 0][j - 1] = WALL_CHAR;				
				if (predungeon[i - 0][j + 1] == NO_CHAR) predungeon[i - 0][j + 1] = WALL_CHAR;
			}
		}
	}

	// New Dave code 10/11
	if (!DL2_FillVoids()) return(FALSE);
	// End new Dave code

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++)
			DoPatternCheck(i, j);
	}
	return(TRUE);
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2Pass3()
{
	int	i,j,xx,yy;
	long v1,v2,v3,v4,lv;

	// Init dungeon to dirt
	lv = 11;			// 12 - 1
	__asm {
		mov		esi,dword ptr [pMegaTiles]
		mov		eax,dword ptr [lv];
		shl		eax,3
		add		esi,eax
		xor		eax,eax
		lodsw
		inc		eax
		mov		dword ptr [v1],eax
		lodsw
		inc		eax
		mov		dword ptr [v2],eax
		lodsw
		inc		eax
		mov		dword ptr [v3],eax
		lodsw
		inc		eax
		mov		dword ptr [v4],eax
	}
	for (yy = 0; yy < DMAXY; yy+=2) {
		for (xx = 0; xx < DMAXX; xx+=2) {
			dPiece[xx][yy] = (int) v1;
			dPiece[xx+1][yy] = (int) v2;
			dPiece[xx][yy+1] = (int) v3;
			dPiece[xx+1][yy+1] = (int) v4;
		}
	}

	// Convert dungeon mega tiles to mini tiles
	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			lv = ((long)dungeon[i][j]) - 1;
			__asm {
				mov		esi,dword ptr [pMegaTiles]
				mov		eax,dword ptr [lv];
				shl		eax,3
				add		esi,eax
				xor		eax,eax
				lodsw
				inc		eax
				mov		dword ptr [v1],eax
				lodsw
				inc		eax
				mov		dword ptr [v2],eax
				lodsw
				inc		eax
				mov		dword ptr [v3],eax
				lodsw
				inc		eax
				mov		dword ptr [v4],eax
			}
			dPiece[xx][yy] = (int) v1;
			dPiece[xx+1][yy] = (int) v2;
			dPiece[xx][yy+1] = (int) v3;
			dPiece[xx+1][yy+1] = (int) v4;
			xx += 2;
		}
		yy += 2;
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2FTVR(int i, int j, int x, int y, int d)
{
	if ((dTransVal[x][y] == 0) && (dungeon[i][j] == FLOOR_PIECE)) {
		dTransVal[x][y] = TransVal;
		dTransVal[x+1][y] = TransVal;
		dTransVal[x][y+1] = TransVal;
		dTransVal[x+1][y+1] = TransVal;
		DRLG_L2FTVR(i+1,j, x+2,y, 1);
		DRLG_L2FTVR(i-1,j, x-2,y, 2);
		DRLG_L2FTVR(i,j+1, x,y+2, 3);
		DRLG_L2FTVR(i,j-1, x,y-2, 4);

		DRLG_L2FTVR(i-1,j-1, x-2,y-2, 5);
		DRLG_L2FTVR(i+1,j-1, x+2,y-2, 6);
		DRLG_L2FTVR(i-1,j+1, x-2,y+2, 7);
		DRLG_L2FTVR(i+1,j+1, x+2,y+2, 8);
	} else {
		if (d == 1) {
			dTransVal[x][y] = TransVal;
			dTransVal[x][y+1] = TransVal;
		}
		if (d == 2) {
			dTransVal[x+1][y] = TransVal;
			dTransVal[x+1][y+1] = TransVal;
		}
		if (d == 3) {
			dTransVal[x][y] = TransVal;
			dTransVal[x+1][y] = TransVal;
		}
		if (d == 4) {
			dTransVal[x][y+1] = TransVal;
			dTransVal[x+1][y+1] = TransVal;
		}
		if (d == 5) dTransVal[x+1][y+1] = TransVal;
		if (d == 6) dTransVal[x][y+1] = TransVal;
		if (d == 7) dTransVal[x+1][y] = TransVal;
		if (d == 8) dTransVal[x][y] = TransVal;
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2FloodTVal()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == FLOOR_PIECE) && (dTransVal[xx][yy] == 0)) {
				DRLG_L2FTVR(i,j,xx,yy,0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2TransFix()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == DURWALL_PIECE) && (dungeon[i][j-1] == DVWALL_PIECE)) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if ((dungeon[i][j] == DLLWALL_PIECE) && (dungeon[i+1][j] == DHWALL_PIECE)) {
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == DVWALL_PIECE) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == DHWALL_PIECE) {
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == DLRWALL_PIECE) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			xx += 2;
		}
		yy += 2;
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void L2DirtFix()
{
	int i, j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] ==  13) && (dungeon[i+1][j] != 11)) dungeon[i][j] = 146;
			if ((dungeon[i][j] ==  11) && (dungeon[i+1][j] != 11)) dungeon[i][j] = 144;
			if ((dungeon[i][j] ==  15) && (dungeon[i+1][j] != 11)) dungeon[i][j] = 148;

			if ((dungeon[i][j] ==  10) && (dungeon[i][j+1] != 10)) dungeon[i][j] = 143;
			if ((dungeon[i][j] ==  13) && (dungeon[i][j+1] != 10)) dungeon[i][j] = 146;
			if ((dungeon[i][j] ==  14) && (dungeon[i][j+1] != 15)) dungeon[i][j] = 147;
		}
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void L2LockoutFix()
{
	int i, j;
	BOOL doorok;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 4) && (dungeon[i-1][j] != 3)) dungeon[i][j] = 1;
			if ((dungeon[i][j] == 5) && (dungeon[i][j-1] != 3)) dungeon[i][j] = 2;
		}
	}

	for (j = 1; j < MDMAXY-1; j++) {
		for (i = 1; i < MDMAXX-1; i++) {
			if (!(dflags[i][j] & SETP_BIT)) {
				if (((dungeon[i][j] == 2) || (dungeon[i][j] == 5)) && (dungeon[i][j-1] == 3) && (dungeon[i][j+1] == 3)) {
					doorok = FALSE;
					while (((dungeon[i][j] == 2) || (dungeon[i][j] == 5)) && (dungeon[i][j-1] == 3) && (dungeon[i][j+1] == 3)) {
						if (dungeon[i][j] == 5) doorok = TRUE;
						i++;
					}
					if (!doorok && !(dflags[i-1][j] & SETP_BIT)) {
						dungeon[i-1][j] = 5;
					}
				}
			}
		}
	}
	for (i = 1; i < MDMAXX-1; i++) {
		for (j = 1; j < MDMAXY-1; j++) {
			if (!(dflags[i][j] & SETP_BIT)) {
				if (((dungeon[i][j] == 1) || (dungeon[i][j] == 4)) && (dungeon[i-1][j] == 3) && (dungeon[i+1][j] == 3)) {
					doorok = FALSE;
					while (((dungeon[i][j] == 1) || (dungeon[i][j] == 4)) && (dungeon[i-1][j] == 3) && (dungeon[i+1][j] == 3)) {
						if (dungeon[i][j] == 4) doorok = TRUE;
						j++;
					}
					if (!doorok && !(dflags[i][j-1] & SETP_BIT)) {
						dungeon[i][j-1] = 4;
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void L2DoorFix()
{
	int i, j;

	for (j = 1; j < MDMAXY; j++) {
		for (i = 1; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 4) && (dungeon[i][j-1] == 3))
				dungeon[i][j] = 7;
			if ((dungeon[i][j] == 5) && (dungeon[i-1][j] == 3))
				dungeon[i][j] = 9;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_L2(int entry)
{
	int		i,j;
	BOOL	doneflag;

	doneflag = FALSE;
	while (!doneflag) {
		nRoomCnt = 0;
		InitDungeon();
		DRLG_InitTrans();
		
		if (CreateDungeon()) {
			L2TileFix();

	if (setloadflag) {
		DRLG_L2SetRoom(nSx1,nSy1);
	}
			DRLG_L2FloodTVal();
			DRLG_L2TransFix();

			if (entry == LVL_DOWN) {
				doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 1, LVL_DOWN);

				#if IS_VERSION(RETAIL)
				if (doneflag) doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 0, LVL_UP);
				#endif

			if ((doneflag) && (currlevel == 5)) doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 0, LVL_TWARPDN);

				ViewY -= 2;
			} else {
				if (entry == LVL_UP) {
					doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 0, LVL_DOWN);
					#if IS_VERSION(RETAIL)
					if (doneflag) doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 1, LVL_UP);
					#endif
					if ((doneflag) && (currlevel == 5)) doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 0, LVL_TWARPDN);
					ViewX--;
				} else {
					doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 0, LVL_DOWN);
					#if IS_VERSION(RETAIL)
					if (doneflag) doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 0, LVL_UP);
					#endif
					if ((doneflag) && (currlevel == 5)) doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 1, LVL_TWARPDN);
					ViewY -= 2;
				}
			}
		}
	}

	L2LockoutFix();

	L2DoorFix();

	L2DirtFix();

	if (doneflag) DRLG_PlaceThemeRooms(6, 10, FLOOR_PIECE, 0, FALSE);

	DRLG_L2PlaceRndSet(CTRDOOR1, 100);
	DRLG_L2PlaceRndSet(CTRDOOR2, 100);
	DRLG_L2PlaceRndSet(CTRDOOR3, 100);
	DRLG_L2PlaceRndSet(CTRDOOR4, 100);
	DRLG_L2PlaceRndSet(CTRDOOR5, 100);
	DRLG_L2PlaceRndSet(CTRDOOR6, 100);
	DRLG_L2PlaceRndSet(CTRDOOR7, 100);
	DRLG_L2PlaceRndSet(CTRDOOR8, 100);

	DRLG_L2PlaceRndSet(VARCH33, 100);
	DRLG_L2PlaceRndSet(VARCH34, 100);
	DRLG_L2PlaceRndSet(VARCH35, 100);
	DRLG_L2PlaceRndSet(VARCH36, 100);
	DRLG_L2PlaceRndSet(VARCH37, 100);
	DRLG_L2PlaceRndSet(VARCH38, 100);
	DRLG_L2PlaceRndSet(VARCH39, 100);
	DRLG_L2PlaceRndSet(VARCH40, 100);
		  
	DRLG_L2PlaceRndSet(VARCH1, 100);
	DRLG_L2PlaceRndSet(VARCH2, 100);
	DRLG_L2PlaceRndSet(VARCH3, 100);
	DRLG_L2PlaceRndSet(VARCH4, 100);
	DRLG_L2PlaceRndSet(VARCH5, 100);
	DRLG_L2PlaceRndSet(VARCH6, 100);
	DRLG_L2PlaceRndSet(VARCH7, 100);
	DRLG_L2PlaceRndSet(VARCH8, 100);

	DRLG_L2PlaceRndSet(VARCH9, 100);
	DRLG_L2PlaceRndSet(VARCH10, 100);
	DRLG_L2PlaceRndSet(VARCH11, 100);
	DRLG_L2PlaceRndSet(VARCH12, 100);
	DRLG_L2PlaceRndSet(VARCH13, 100);
	DRLG_L2PlaceRndSet(VARCH14, 100);
	DRLG_L2PlaceRndSet(VARCH15, 100);
	DRLG_L2PlaceRndSet(VARCH16, 100);

	DRLG_L2PlaceRndSet(VARCH17, 100);
	DRLG_L2PlaceRndSet(VARCH18, 100);
	DRLG_L2PlaceRndSet(VARCH19, 100);
	DRLG_L2PlaceRndSet(VARCH20, 100);
	DRLG_L2PlaceRndSet(VARCH21, 100);
	DRLG_L2PlaceRndSet(VARCH22, 100);
	DRLG_L2PlaceRndSet(VARCH23, 100);
	DRLG_L2PlaceRndSet(VARCH24, 100);

	DRLG_L2PlaceRndSet(VARCH25, 100);
	DRLG_L2PlaceRndSet(VARCH26, 100);
	DRLG_L2PlaceRndSet(VARCH27, 100);
	DRLG_L2PlaceRndSet(VARCH28, 100);
	DRLG_L2PlaceRndSet(VARCH29, 100);
	DRLG_L2PlaceRndSet(VARCH30, 100);
	DRLG_L2PlaceRndSet(VARCH31, 100);
	DRLG_L2PlaceRndSet(VARCH32, 100);
		 
	DRLG_L2PlaceRndSet(HARCH1, 100);
	DRLG_L2PlaceRndSet(HARCH2, 100);
	DRLG_L2PlaceRndSet(HARCH3, 100);
	DRLG_L2PlaceRndSet(HARCH4, 100);
	DRLG_L2PlaceRndSet(HARCH5, 100);
	DRLG_L2PlaceRndSet(HARCH6, 100);
	DRLG_L2PlaceRndSet(HARCH7, 100);
	DRLG_L2PlaceRndSet(HARCH8, 100);
	DRLG_L2PlaceRndSet(HARCH9, 100);
	DRLG_L2PlaceRndSet(HARCH10, 100);
	DRLG_L2PlaceRndSet(HARCH11, 100);
	DRLG_L2PlaceRndSet(HARCH12, 100);
	DRLG_L2PlaceRndSet(HARCH13, 100);
	DRLG_L2PlaceRndSet(HARCH14, 100);
	DRLG_L2PlaceRndSet(HARCH15, 100);
	DRLG_L2PlaceRndSet(HARCH16, 100);
	DRLG_L2PlaceRndSet(HARCH17, 100);
	DRLG_L2PlaceRndSet(HARCH18, 100);
	DRLG_L2PlaceRndSet(HARCH19, 100);
	DRLG_L2PlaceRndSet(HARCH20, 100);
	DRLG_L2PlaceRndSet(HARCH21, 100);
	DRLG_L2PlaceRndSet(HARCH22, 100);
	DRLG_L2PlaceRndSet(HARCH23, 100);
	DRLG_L2PlaceRndSet(HARCH24, 100);

	DRLG_L2PlaceRndSet(HARCH25, 100);
	DRLG_L2PlaceRndSet(HARCH26, 100);
	DRLG_L2PlaceRndSet(HARCH27, 100);
	DRLG_L2PlaceRndSet(HARCH28, 100);
	DRLG_L2PlaceRndSet(HARCH29, 100);
	DRLG_L2PlaceRndSet(HARCH30, 100);
	DRLG_L2PlaceRndSet(HARCH31, 100);
	DRLG_L2PlaceRndSet(HARCH32, 100);

	DRLG_L2PlaceRndSet(HARCH33, 100);
	DRLG_L2PlaceRndSet(HARCH34, 100);
	DRLG_L2PlaceRndSet(HARCH35, 100);
	DRLG_L2PlaceRndSet(HARCH36, 100);
	DRLG_L2PlaceRndSet(HARCH37, 100);
	DRLG_L2PlaceRndSet(HARCH38, 100);
	DRLG_L2PlaceRndSet(HARCH39, 100);
	DRLG_L2PlaceRndSet(HARCH40, 100);

	DRLG_L2PlaceRndSet(CRUSHCOL, 99);

	DRLG_L2PlaceRndSet(RUINS1, 10);
	DRLG_L2PlaceRndSet(RUINS2, 10);
	DRLG_L2PlaceRndSet(RUINS3, 10);
	DRLG_L2PlaceRndSet(RUINS4, 10);
	DRLG_L2PlaceRndSet(RUINS5, 10);
	DRLG_L2PlaceRndSet(RUINS6, 10);
	DRLG_L2PlaceRndSet(RUINS7, 50);

	DRLG_L2PlaceRndSet(PANCREAS1, 1);
	DRLG_L2PlaceRndSet(PANCREAS2, 1);

	DRLG_L2PlaceRndSet(BIG1, 3);		// 1x2 water
	DRLG_L2PlaceRndSet(BIG2, 3);		// 2x1 water
	DRLG_L2PlaceRndSet(BIG3, 3);		// 1x2 ruined box
	DRLG_L2PlaceRndSet(BIG4, 3);		// 2x1 ruined box
	DRLG_L2PlaceRndSet(BIG5, 3);		// 2x2 water
	DRLG_L2PlaceRndSet(BIG6, 20);		// 1x2 wall water
	DRLG_L2PlaceRndSet(BIG7, 20);		// 2x1 wall water
	DRLG_L2PlaceRndSet(BIG8, 3);		// 2x2 rubble pile
	DRLG_L2PlaceRndSet(BIG9, 20);		// 2x2 vwall ruins
	DRLG_L2PlaceRndSet(BIG10, 20);		// 2x2 hwall ruins

	DRLG_L2Subs();
	DRLG_L2Shadows();

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
	}

	extern void DRLG_Init_Globals();
	DRLG_Init_Globals();
	// Check for any mini quest pieces
	DRLG_CheckQuests(nSx1, nSy1);
	
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
static void DRLG_InitL2Vals()
{
	int i,j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			// place tops of arches
			int nPiece = dPiece[i][j];
				 if (nPiece == 541) nPiece = 5;
			else if (nPiece == 178) nPiece = 5;
			else if (nPiece == 551) nPiece = 5;
			else if (nPiece == 542) nPiece = 6;
			else if (nPiece == 553) nPiece = 6;
			else if (nPiece == 13)  nPiece = 5;
			else if (nPiece == 17)  nPiece = 6;
			else continue;

			dSpecial[i][j] = nPiece;
		}
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dPiece[i][j] == 132) {
				dSpecial[i][j+1] = 2;
				dSpecial[i][j+2] = 1;
			}
			else if ((dPiece[i][j] == 135) || (dPiece[i][j] == 139)) {
				dSpecial[i+1][j] = 3;
				dSpecial[i+2][j] = 4;
			}
		}
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
void LoadL2Dungeon(char sFileName[], int vx, int vy)
{
	int		i, j, rw, rh;
	byte	*pLevelMap, *lm;

	InitDungeon();
	DRLG_InitTrans();

	pLevelMap = LoadFileInMemSig(sFileName,NULL,'LMPt');
	lm = pLevelMap;

	// Fill with dirt
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			dungeon[i][j] = DFLOOR_PIECE;
			dflags[i][j] = 0;
		}
	}

	rw = *lm;
	lm+=2;
	rh = *lm;
	lm+=2;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
				dflags[i][j] |= SETP_BIT;
			} else dungeon[i][j] = FLOOR_PIECE;
			lm+=2;
		}
	}
	
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if (dungeon[i][j] == 0)
				dungeon[i][j] = DFLOOR_PIECE;
		}
	} 

	DRLG_L2Pass3();

	extern void DRLG_Init_Globals();
	DRLG_Init_Globals();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			int nPiece = dPiece[i][j];
			int top = 0;
			if (nPiece == 541) top = 5;
			if (nPiece == 178) top = 5;
			if (nPiece == 551) top = 5;
			if (nPiece == 542) top = 6;
			if (nPiece == 553) top = 6;
			if (nPiece == 13)  top = 5;
			if (nPiece == 17)  top = 6;
			dSpecial[i][j] = top;
		}
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dPiece[i][j] == 132) {
				dSpecial[i][j+1] = 2;
				dSpecial[i][j+2] = 1;
			}
			else if ((dPiece[i][j] == 135) || (dPiece[i][j] == 139)) {
				dSpecial[i+1][j] = 3;
				dSpecial[i+2][j] = 4;
			}
		}
	}

	ViewX = vx;
	ViewY = vy;

	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);
	//SetMapItems(pLevelMap);

	DiabloFreePtr(pLevelMap);
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
void LoadPreL2Dungeon(char sFileName[], int vx, int vy)
{
	int		i, j, rw, rh;
	byte	*pLevelMap, *lm;

	InitDungeon();
	DRLG_InitTrans();

	pLevelMap = LoadFileInMemSig(sFileName,NULL,'LMPt');
	lm = pLevelMap;

	// Fill with dirt
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			dungeon[i][j] = DFLOOR_PIECE;
			dflags[i][j] = 0;
		}
	}

	rw = *lm;
	lm+=2;
	rh = *lm;
	lm+=2;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
				dflags[i][j] |= SETP_BIT;
			} else dungeon[i][j] = FLOOR_PIECE;
			lm+=2;
		}
	}

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++)
			if (dungeon[i][j] == 0) dungeon[i][j] = DFLOOR_PIECE;
	} 

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
	}

	DiabloFreePtr(pLevelMap);
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
void CreateL2Dungeon(unsigned int rseed, int entry)
{
// dig.patch1.start.1/30/97

	// The variables nSx1,nSx2,nSy1,nSy2 weren't getting initialized for level 8,
	// nor for level 7 when the Q_BLIND quest is not chosen.
	// The result of not setting those variables is that those levels
	// differ slightly when they are loaded versus when they are generated.

	// Fix: In the above mentioned cases (level 8, and level 7 without Q_BLIND)
	// we need to generate values for nSx1 etc. that are identical to how they
	// were set when the levels were generated.  The way to do that is to figure
	// out which was the last level which set those variables, and temporarily
	// recreate that level.
	// Level 6 always generates values for the variables.
	// Level 7 generates values only if (quests[Q_BLIND]._qactive != QUEST_NOTAVAIL)

	extern DWORD glSeedTbl[NUMLEVELS];
	extern BYTE gbMaxPlayers;
	if (gbMaxPlayers == 1) {
		if (currlevel == 7 && quests[Q_BLIND]._qactive == QUEST_NOTAVAIL) {
			currlevel = 6;
			CreateL2Dungeon(glSeedTbl[6], LVL_NODIR);
			currlevel = 7;
		}
		if (currlevel == 8) {
			if (quests[Q_BLIND]._qactive == QUEST_NOTAVAIL) {
				currlevel = 6;
				CreateL2Dungeon(glSeedTbl[6], LVL_NODIR);
				currlevel = 8;
			}
			else {
				currlevel = 7;
				CreateL2Dungeon(glSeedTbl[7], LVL_NODIR);
				currlevel = 8;
			}
		}
	}
// dig.patch1.end.1/30/97

	SetRndSeed(rseed);

	dminx = DIRTEDGED2;
	dminy = DIRTEDGED2;
	dmaxx = DMAXX - (DIRTEDGED2);
	dmaxy = DMAXY - (DIRTEDGED2);

	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL2SP();
	DRLG_L2(entry);
	DRLG_L2Pass3();
	DRLG_FreeL2SP();
	DRLG_InitL2Vals();
	DRLG_SetPC();
}
#endif
