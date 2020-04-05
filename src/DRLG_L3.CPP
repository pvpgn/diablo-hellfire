/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Dungeon file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L3.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
** CreateL1Dungeon
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"drlg_l3.h"
#include	"gendung.h"
#include	"scrollrt.h"
#include	"engine.h"
#include	"trigs.h"
#include	"lighting.h"
#include	"monster.h"
#include	"objects.h"
#include	"quests.h"

/*-----------------------------------------------------------------------**
** Registration info
**-----------------------------------------------------------------------*/
#include "regconst.h"
char sgszRegSig4[REG_LEN] = "REGISTRATION_BLOCK";
//int SP3x1, SP3y1, SP3x2, SP3y2;

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static const byte L3ConvTbl[16] = { 8, 11, 3, 10, 1, 9, 12, 12, 6, 13, 4, 13, 2, 14, 5, 7 };

static const byte L3UP[] = { 3, 3,														// X size, Y size

					FILL, FILL, 0,												// Pattern to look for
					TOP_WALL, TOP_WALL, 0,
					FLOOR, FLOOR, 0,

					51, 50, 0,													// Pattern to sub
					48, 49, 0,
					0, 0, 0};

//JKE level 6 stuff
static const byte L6UP[] = { 3, 3,														// X size, Y size

					FILL, FILL, 0,												// Pattern to look for
					TOP_WALL, TOP_WALL, 0,
					FLOOR, FLOOR, 0,

					20, 19, 0,													// Pattern to sub
					17, 18, 0,
					0, 0, 0};

static const byte L3DOWN[] = { 3, 3,														// X size, Y size

					FILL, LEFT_WALL, FLOOR,									// Pattern to look for
					FILL, LEFT_WALL, FLOOR,
					0, 0, 0,

					0, 47, 0,													// Pattern to sub
					0, 46, 0,
					0, 0, 0};

// JKE level 6 stuff
static const byte L6DOWN[] = { 3, 3,														// X size, Y size

					FILL, LEFT_WALL, FLOOR,									// Pattern to look for
					FILL, LEFT_WALL, FLOOR,
					0, 0, 0,

					0, 16, 0,													// Pattern to sub
					0, 15, 0,
					0, 0, 0};

static const byte L3HOLDWARP[] = { 3, 3,														// X size, Y size

					FILL, FILL, 0,												// Pattern to look for
					TOP_WALL, TOP_WALL, 0,
					FLOOR, FLOOR, 0,

					125, 125, 0,													// Pattern to sub
					125, 125, 0,
					0, 0, 0};

// JKE level 6 stuff
static const byte L6HOLDWARP[] = { 3, 3,														// X size, Y size

					FILL, FILL, 0,												// Pattern to look for
					TOP_WALL, TOP_WALL, 0,
					FLOOR, FLOOR, 0,

					24, 23, 0,													// Pattern to sub
					21, 22, 0,
					0, 0, 0};

static const byte L3TITE1[] = { 4, 4,													// X size, Y size

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					0, 0, 0, 0,													// Pattern to sub
					0, 57, 58, 0,
					0, 56, 55, 0,
					0, 0, 0, 0};

static const byte L3TITE2[] = { 4, 4,													// X size, Y size

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					0, 0, 0, 0,													// Pattern to sub
					0, 61, 62, 0,
					0, 60, 59, 0,
					0, 0, 0, 0};

static const byte L3TITE3[] = { 4, 4,													// X size, Y size

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					0, 0, 0, 0,													// Pattern to sub
					0, 65, 66, 0,
					0, 64, 63, 0,
					0, 0, 0, 0};

static const byte L3TITE4[] = { 4, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					0, 0, 0, 0,													// Pattern to sub
					0, 70, 71, 0,
					0, 0, 0, 0};

static const byte L3TITE5[] = { 3, 4,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					0, 0, 0,														// Pattern to sub
					0, 73, 0,
					0, 72, 0,
					0, 0, 0};

static const byte L3TITE6[] = { 5, 4,													// X size, Y size

					FLOOR, FLOOR, FLOOR, FLOOR, FLOOR,					// Pattern to look for
					FLOOR, FLOOR, FLOOR, 0, FLOOR,
					FLOOR, FLOOR, FLOOR, 0, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR, FLOOR,

					0, 0, 0, 0, 0,												// Pattern to sub
					0, 77, 78, 0, 0,
					0, 76, 74, 75, 0,
					0, 0, 0, 0, 0};

static const byte L3TITE7[] = { 4, 5,													// X size, Y size

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, 0, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					0, 0, 0, 0,													// Pattern to sub
					0, 83, 0, 0,
					0, 82, 80, 0,
					0, 81, 79, 0,
					0, 0, 0, 0};

static const byte L3TITE8[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					0, 0, 0,														// Pattern to sub
					0, 52, 0,
					0, 0, 0};

static const byte L3TITE9[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					0, 0, 0,														// Pattern to sub
					0, 53, 0,
					0, 0, 0};

static const byte L3TITE10[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					0, 0, 0,														// Pattern to sub
					0, 54, 0,
					0, 0, 0};

static const byte L3TITE11[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					0, 0, 0,														// Pattern to sub
					0, 67, 0,
					0, 0, 0};

static const byte L3TITE12[] = { 2, 1,													// X size, Y size

					LEFT_WALL, FLOOR,											// Pattern to look for

					68, 0};														// Pattern to sub

static const byte L3TITE13[] = { 1, 2,													// X size, Y size

					TOP_WALL,													// Pattern to look for
					FLOOR,

					69,															// Pattern to sub
					0};

static const byte L3CREV1[] = { 2, 1,													// X size, Y size

					FILL, FLOOR,												// Pattern to look for

					84, 85};														// Pattern to sub

static const byte L3CREV2[] = { 2, 1,													// X size, Y size

					FILL, UL_WALL,												// Pattern to look for

					86, 87};														// Pattern to sub

static const byte L3CREV3[] = { 1, 2,													// X size, Y size

					FILL,															// Pattern to look for
					TOP_WALL,

					89,															// Pattern to sub
					88};

static const byte L3CREV4[] = { 2, 1,													// X size, Y size

					FILL, FLOOR,												// Pattern to look for

					90, 91};														// Pattern to sub

static const byte L3CREV5[] = { 1, 2,													// X size, Y size

					FILL,															// Pattern to look for
					UL_WALL,

					92,															// Pattern to sub
					93};

static const byte L3CREV6[] = { 1, 2,													// X size, Y size

					FILL,															// Pattern to look for
					TOP_WALL,

					95,															// Pattern to sub
					94};

static const byte L3CREV7[] = { 2, 1,													// X size, Y size

					FILL, FLOOR,												// Pattern to look for

					96, 101};													// Pattern to sub

static const byte L3CREV8[] = { 1, 2,													// X size, Y size

					BOTTOM_WALL,												// Pattern to look for
					FILL,

					102,															// Pattern to sub
					97};

static const byte L3CREV9[] = { 2, 1,													// X size, Y size

					UR_WALL, FILL,												// Pattern to look for

					103, 98};													// Pattern to sub

static const byte L3CREV10[] = { 2, 1,													// X size, Y size

					RIGHT_WALL, FILL,											// Pattern to look for

					104, 99};													// Pattern to sub

static const byte L3CREV11[] = { 1, 2,													// X size, Y size

					LR_WALL,														// Pattern to look for
					FILL,

					105,															// Pattern to sub
					100};

static const byte L3ISLE1[] = { 2, 3,													// X size, Y size

					UL_ISLE, UR_ISLE,											// Pattern to look for
					RIGHT_WALL, LEFT_WALL,
					LL_ISLE, LR_ISLE,

					7, 7,															// Pattern to sub
					7, 7,
					7, 7};

static const byte L3ISLE2[] = { 3, 2,													// X size, Y size

					UL_ISLE, BOTTOM_WALL, UR_ISLE,						// Pattern to look for
					LL_ISLE, TOP_WALL, LR_ISLE, 

					7, 7, 7,														// Pattern to sub
					7, 7, 7};

static const byte L3ISLE3[] = { 2, 3,													// X size, Y size

					UL_ISLE, UR_ISLE,											// Pattern to look for
					RIGHT_WALL, LEFT_WALL,
					LL_ISLE, LR_ISLE,

					29, 30,														// Pattern to sub
					25, 28,
					31, 32};

static const byte L3ISLE4[] = { 3, 2,													// X size, Y size

					UL_ISLE, BOTTOM_WALL, UR_ISLE,						// Pattern to look for
					LL_ISLE, TOP_WALL, LR_ISLE, 

					29, 26, 30,													// Pattern to sub
					31, 27, 32};

static const byte L3ISLE5[] = { 2, 2,													// X size, Y size

					UL_ISLE, UR_ISLE,											// Pattern to look for
					LL_ISLE, LR_ISLE, 

					7, 7,															// Pattern to sub
					7, 7};

static const byte L3XTRA1[] = { 1, 1,													// X size, Y size

					FLOOR,														// Pattern to look for

					106};															// Pattern to sub

static const byte L3XTRA2[] = { 1, 1,													// X size, Y size

					FLOOR,														// Pattern to look for

					107};															// Pattern to sub

static const byte L3XTRA3[] = { 1, 1,													// X size, Y size

					FLOOR,														// Pattern to look for

					108};															// Pattern to sub

static const byte L3XTRA4[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					109};															// Pattern to sub

static const byte L3XTRA5[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					110};															// Pattern to sub

static const byte L3ANVIL[] = { 11, 11,

					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,

					 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
					 0,  0, 29, 26, 26, 26, 26, 26, 30,  0,  0,
					 0, 29, 34, 33, 33, 37, 36, 33, 35, 30,  0,
					 0, 25, 33, 37, 27, 32, 31, 36, 33, 28,  0,
					 0, 25, 37, 32,  7,  7,  7, 31, 27, 32,  0,
					 0, 25, 28,  7,  7,  7,  7, BOTTOM_WALL, BOTTOM_WALL, BOTTOM_WALL,  0,
					 0, 25, 35, 30,  7,  7,  7, 29, 26, 30,  0,
					 0, 25, 33, 35, 26, 30, 29, 34, 33, 28,  0,
					 0, 31, 36, 33, 33, 35, 34, 33, 37, 32,  0,
					 0,  0, 31, 27, 27, 27, 27, 27, 32,  0,  0,
					 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };

// JKE begin L6 extras
static const byte L6FILL1[] = { 1, 1,													// X size, Y size

					FILL,													// Pattern to look for

					25};															// Pattern to sub
static const byte L6FILL2[] = { 1, 1,													// X size, Y size

					FILL,													// Pattern to look for

					26};															// Pattern to sub
static const byte L6FILL3[] = { 1, 1,													// X size, Y size

					FILL,													// Pattern to look for

					27};															// Pattern to sub
static const byte L6FILL4[] = { 1, 1,													// X size, Y size

					FILL,													// Pattern to look for

					28};															// Pattern to sub
static const byte L6FLOOR1[] = { 1, 1,													// X size, Y size

					FLOOR,													// Pattern to look for

					29};															// Pattern to sub
static const byte L6FLOOR2[] = { 1, 1,													// X size, Y size

					FLOOR,													// Pattern to look for

					30};															// Pattern to sub
static const byte L6FLOOR3[] = { 1, 1,													// X size, Y size

					FLOOR,													// Pattern to look for

					31};															// Pattern to sub
static const byte L6FLOOR4[] = { 1, 1,													// X size, Y size

					FLOOR,													// Pattern to look for

					32};															// Pattern to sub
static const byte L6FLOOR5[] = { 3, 3,													// X size, Y size

					FLOOR,FLOOR,FLOOR,										// Pattern to look for
					FLOOR,FLOOR,FLOOR,
					FLOOR,FLOOR,FLOOR,

					0, 0 ,0,
					0,126,0,
					0, 0 ,0};															// Pattern to sub
static const byte L6FLOOR6[] = { 3, 3,													// X size, Y size

					FLOOR,FLOOR,FLOOR,										// Pattern to look for
					FLOOR,FLOOR,FLOOR,
					FLOOR,FLOOR,FLOOR,

					0, 0 ,0,
					0,124,0,
					0, 0 ,0};															// Pattern to sub
static const byte L6WALL1[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					33};															// Pattern to sub
static const byte L6WALL2[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					34};															// Pattern to sub
static const byte L6WALL3[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					35};															// Pattern to sub
static const byte L6WALL4[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					36};															// Pattern to sub
static const byte L6WALL5[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					37};															// Pattern to sub
static const byte L6CORNER1[] = { 1, 1,													// X size, Y size

					UL_WALL,													// Pattern to look for

					38};															// Pattern to sub
static const byte L6WALL6[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					39};															// Pattern to sub
static const byte L6WALL7[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					40};															// Pattern to sub
static const byte L6WALL8[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					41};															// Pattern to sub
static const byte L6WALL9[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					42};															// Pattern to sub
static const byte L6WALL10[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					43};															// Pattern to sub
static const byte L6CORNER2[] = { 1, 1,													// X size, Y size

					UL_WALL,													// Pattern to look for

					44};															// Pattern to sub
static const byte L6WALL11[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					45};															// Pattern to sub
static const byte L6WALL12[] = { 1, 1,													// X size, Y size

					LEFT_WALL,													// Pattern to look for

					46};															// Pattern to sub
static const byte L6WALL13[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					47};															// Pattern to sub
static const byte L6WALL14[] = { 1, 1,													// X size, Y size

					TOP_WALL,													// Pattern to look for

					48};															// Pattern to sub
static const byte L6CORNER3[] = { 1, 1,													// X size, Y size

					UL_WALL,													// Pattern to look for

					49};															// Pattern to sub
static const byte L6CORNER4[] = { 1, 1,													// X size, Y size

					UL_WALL,													// Pattern to look for

					50};															// Pattern to sub
//=======================================================================================
//			Stalagmites
//=======================================================================================

static const byte L6TITE1[] = { 3, 3,													// X size, Y size

					FLOOR,FLOOR, FLOOR,										// Pattern to look for
					FLOOR,FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					67, 0, 0,													// Pattern to sub
					66,51, 0,
					 0, 0, 0};

static const byte L6TITE2[] = { 3,3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					69, 0, 0,														// Pattern to sub
					68, 52,0,
					 0, 0, 0};

static const byte L6TITE3[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					70, 0, 0,														// Pattern to sub
					71, 53, 0,
					0, 0, 0};
static const byte L6TITE4[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					73, 0, 0,														// Pattern to sub
					72, 54, 0,
					0, 0, 0};
static const byte L6TITE5[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					75, 0, 0,														// Pattern to sub
					74, 55, 0,
					0, 0, 0};
static const byte L6TITE6[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					77, 0, 0,														// Pattern to sub
					76, 56, 0,
					0, 0, 0};
static const byte L6TITE7[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					79, 0, 0,														// Pattern to sub
					78, 57, 0,
					0, 0, 0};
static const byte L6TITE8[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					81, 0, 0,														// Pattern to sub
					80, 58, 0,
					0, 0, 0};
static const byte L6TITE9[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					83, 0, 0,														// Pattern to sub
					82, 59, 0,
					0, 0, 0};
static const byte L6TITE10[] = { 3, 3,													// X size, Y size

					FLOOR, FLOOR, FLOOR,										// Pattern to look for
					FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR,

					84, 0, 0,														// Pattern to sub
					85, 60, 0,
					0, 0, 0};

static const byte ACIDISLE1[] = { 2, 3,													// X size, Y size

					UL_ISLE, UR_ISLE,											// Pattern to look for
					RIGHT_WALL, LEFT_WALL,
					LL_ISLE, LR_ISLE,

					7, 7,															// Pattern to sub
					7, 7,
					7, 7};

static const byte ACIDISLE2[] = { 3, 2,													// X size, Y size

					UL_ISLE, BOTTOM_WALL, UR_ISLE,						// Pattern to look for
					LL_ISLE, TOP_WALL, LR_ISLE, 

					7, 7, 7,														// Pattern to sub
					7, 7, 7};

static const byte ACIDISLE3[] = { 2, 3,													// X size, Y size

					UL_ISLE, UR_ISLE,											// Pattern to look for
					RIGHT_WALL, LEFT_WALL,
					LL_ISLE, LR_ISLE,

					107, 115,														// Pattern to sub
					119, 122,
					131, 123};

static const byte ACIDISLE4[] = { 3, 2,													// X size, Y size

					UL_ISLE, BOTTOM_WALL, UR_ISLE,						// Pattern to look for
					LL_ISLE, TOP_WALL, LR_ISLE, 

					107, 120, 115,													// Pattern to sub
					131, 121, 123};

static const byte ACIDISLE5[] = { 2, 2,													// X size, Y size

					UL_ISLE, UR_ISLE,											// Pattern to look for
					LL_ISLE, LR_ISLE, 

					7, 7,															// Pattern to sub
					7, 7};

static const byte L6ACIDTEST[] = { 1, 1,													// X size, Y size

					FLOOR,													// Pattern to look for

					131};															// Pattern to sub

static const byte L6ACID1[] = { 4, 4,

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					7, 7 , 7 ,7,
					7,107,115,7,
					7,131,123,7,
					7, 7 , 7 ,7};

static const byte L6ACID2[] = { 4, 4,

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					7, 7 , 7 ,7,
					7, 7 ,108,7,
					7,109,112,7,
					7, 7 , 7 ,7};

static const byte L6ACID3[] = { 4, 5,

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					7, 7 , 7 ,7,
					7,107,115,7,
					7,119,122,7,
					7,131,123,7,
					7, 7 , 7 ,7};

static const byte L6ACID4[] = { 4, 5,

					FLOOR, FLOOR, FLOOR, FLOOR,							// Pattern to look for
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,
					FLOOR, FLOOR, FLOOR, FLOOR,

					7, 7 , 7 ,7,
					7,126,108,7,
					7, 7 ,117,7,
					7,109,112,7,
					7, 7 , 7 ,7};

static int abyssx;
static BYTE lavapool;

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void InitL3Dungeon() {
	ZeroMemory(dungeon,sizeof(dungeon));
	int i, j;
   
	for (i = 0; i < MDMAXY; i++) {

		for (j = 0; j < MDMAXX; j++) {
			dungeon[j][i] = D3_NULL;
			dflags[j][i] = 0;
		}	
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*

void DRLG_LoadL3SP()
{
	setloadflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*
void DRLG_FreeL3SP() {
	DiabloFreePtr(pSetPiece);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*

void DRLG_L3SetRoom(int rx1, int ry1)
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

	sp = pSetPiece+4;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dungeon[rx1+i][ry1+j] = *sp;
				dflags[rx1+i][ry1+j] |= SETP_BIT;
			} else dungeon[rx1+i][ry1+j] = FLOOR;
			sp+=2;
		}
	}

 }

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static int DRLG_L3FillRoom(int x1, int y1, int x2, int y2)
{
	int	i,j,v,rv,rf;

	if ((x1 > 1) && (x2 < (MDMAXX-6)) && (y1 > 1) && (y2 < (MDMAXY-2))) {
		v = 0;
		for (j = y1; j <= y2; j++) {
			for (i = x1; i <= x2; i++) v += dungeon[i][j];
		}
		if (v == 0) {
			for (j = (y1+1); j < y2; j++) {
				for (i = (x1+1); i < x2; i++) dungeon[i][j] = 1;
			}
			for (j = y1; j <= y2; j++) {
				rf = random(0, 2);
				if (rf != 0) dungeon[x1][j] = 1;
				rf = random(0, 2);
				if (rf != 0) dungeon[x2][j] = 1;
			}
			for (i = x1; i <= x2; i++) {
				rf = random(0, 2);
				if (rf != 0) dungeon[i][y1] = 1;
				rf = random(0, 2);
				if (rf != 0) dungeon[i][y2] = 1;
			}
			rv = FR_TRUE;
		} else rv = FR_FALSE;
	} else rv = FR_FALSE;
	return (rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void DRLG_L3CreateBlock(int x, int y, int obs, int dir)
{
	int blksizex, blksizey, cbd;
	int x1,y1,x2,y2;
	int contflag;

	blksizex = random(0, 2) + 3;		// 3-5 width and height
	blksizey = random(0, 2) + 3;

	if (dir == BLK_U) {
		y2 = y - 1;
		y1 = y2 - blksizey;
		if (blksizex < obs) x1 = random(0, blksizex) + x;	
		if (blksizex == obs) x1 = x;
		if (blksizex > obs) x1 = x - random(0, blksizex);
		x2 = x1 + blksizex;
	}
	if (dir == BLK_L) {
		x2 = x - 1;
		x1 = x2 - blksizex;
		if (blksizey < obs) y1 = random(0, blksizey) + y;
		if (blksizey == obs) y1 = y;
		if (blksizey > obs) y1 = y - random(0, blksizey);
		y2 = y1 + blksizey;
	}
	if (dir == BLK_D) {
		y1 = y + 1;
		y2 = y1 + blksizey;
		if (blksizex < obs) x1 = random(0, blksizex) + x;
		if (blksizex == obs) x1 = x;
		if (blksizex > obs) x1 = x - random(0, blksizex);
		x2 = x1 + blksizex;
	}
	if (dir == BLK_R) {
		x1 = x + 1;
		x2 = x1 + blksizex;
		if (blksizey < obs) y1 = random(0, blksizey) + y;
		if (blksizey == obs) y1 = y;
		if (blksizey > obs) y1 = y - random(0, blksizey);
		y2 = y1 + blksizey;
	}
	contflag = DRLG_L3FillRoom(x1,y1,x2,y2);
	if (contflag == FR_TRUE) {
		cbd = random(0, 4);
		if ((cbd != 0) && (dir != BLK_D)) DRLG_L3CreateBlock(x1,y1,blksizey,BLK_U);
		if ((cbd != 0) && (dir != BLK_L)) DRLG_L3CreateBlock(x2,y1,blksizex,BLK_R);
		if ((cbd != 0) && (dir != BLK_U)) DRLG_L3CreateBlock(x1,y2,blksizey,BLK_D);
		if ((cbd != 0) && (dir != BLK_R)) DRLG_L3CreateBlock(x1,y1,blksizex,BLK_L);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void DRLG_L3FloorArea(int x1, int y1, int x2, int y2)
{
	int	i,j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) dungeon[i][j] = 1;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3FillDiags()
{
	int	i,j,v,rv;

	for (j = 0; j < (MDMAXY-1); j++) {
		for (i = 0; i < (MDMAXX-1); i++) {
			v = dungeon[i][j] << 3;			// x8
			v += dungeon[i+1][j] << 2;		// x4
			v += dungeon[i][j+1] << 1;		// x2
			v += dungeon[i+1][j+1];
			if (v == 6) {
				rv = random(0, 2);
				if (rv == 0) dungeon[i][j] = 1;
				else dungeon[i+1][j+1] = 1;
			}
			if (v == 9) {
				rv = random(0, 2);
				if (rv == 0) dungeon[i+1][j] = 1;
				else dungeon[i][j+1] = 1;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3FillSingles()
{
	int	i,j,v;

	for (j = 1; j < (MDMAXY-1); j++) {
		for (i = 1; i < (MDMAXX-1); i++) {
			if (dungeon[i][j] == 0) {
				v = dungeon[i-1][j-1] + dungeon[i][j-1] + dungeon[i+1][j-1];
				if (v == 3) {
					v = dungeon[i-1][j] + dungeon[i+1][j];
					if (v == 2) {
						v = dungeon[i-1][j+1] + dungeon[i][j+1] + dungeon[i+1][j+1];
						if (v == 3) dungeon[i][j] = 1;
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3FillStraights()
{
	int		i,j;
	int		xc,xs;
	int		yc,ys;
	int		k,rv;

	for (j = 0; j < (MDMAXY-1); j++) {
		xc = 0;
		for (i = 0; i < (MDMAXX-3); i++) {
			if ((dungeon[i][j] == 0) && (dungeon[i][j+1] == 1)) {
				if (xc == 0) xs = i;
				xc++;
			} else {
				if ((xc > 3) && (random(0, 2))) {
					for (k = xs; k < i; k++) {
						rv = random(0, 2);
						dungeon[k][j] = rv;
					}
				}
				xc = 0;
			}
		}
	}
	for (j = 0; j < (MDMAXY-1); j++) {
		xc = 0;
		for (i = 0; i < (MDMAXX-3); i++) {
			if ((dungeon[i][j] == 1) && (dungeon[i][j+1] == 0)) {
				if (xc == 0) xs = i;
				xc++;
			} else {
				if ((xc > 3) && (random(0, 2))) {
					for (k = xs; k < i; k++) {
						rv = random(0, 2);
						dungeon[k][j+1] = rv;
					}
				}
				xc = 0;
			}
		}
	}
	for (i = 0; i < (MDMAXX-1); i++) {
		yc = 0;
		for (j = 0; j < (MDMAXY-3); j++) {
			if ((dungeon[i][j] == 0) && (dungeon[i+1][j] == 1)) {
				if (yc == 0) ys = j;
				yc++;
			} else {
				if ((yc > 3) && (random(0, 2))) {
					for (k = ys; k < j; k++) {
						rv = random(0, 2);
						dungeon[i][k] = rv;
					}
				}
				yc = 0;
			}
		}
	}
	for (i = 0; i < (MDMAXX-1); i++) {
		yc = 0;
		for (j = 0; j < (MDMAXY-3); j++) {
			if ((dungeon[i][j] == 1) && (dungeon[i+1][j] == 0)) {
				if (yc == 0) ys = j;
				yc++;
			} else {
				if ((yc > 3) && (random(0, 2))) {
					for (k = ys; k < j; k++) {
						rv = random(0, 2);
						dungeon[i+1][k] = rv;
					}
				}
				yc = 0;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3Edges()
{
	int	i,j;

	for (j = 0; j < MDMAXY; j++) dungeon[MDMAXX-1][j] = 0;
	for (i = 0; i < MDMAXX; i++) dungeon[i][MDMAXY-1] = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static int DRLG_L3GetFloorArea()
{
	int	i,j,gfa;

	gfa = 0;
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) gfa += dungeon[i][j];
	}
	return(gfa);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3MakeMegas()
{
	int	i,j,k,v;

	for (j = 0; j < (MDMAXY-1); j++) {
		for (i = 0; i < (MDMAXX-1); i++) {
			v = dungeon[i][j] << 3;
			v += dungeon[i+1][j] << 2;
			v += dungeon[i][j+1] << 1;
			v += dungeon[i+1][j+1];
			if (v == 6) {
				k = random(0, 2);
				if (k == 0) v = 12;
				else v = 5;
			}
			if (v == 9) {
				k = random(0, 2);
				if (k == 0) v = 13;
				else v = 14;
			}
			dungeon[i][j] = L3ConvTbl[v];
		}
		dungeon[MDMAXX-1][j] = L3_DIRT;
	}
	for (i = 0; i < MDMAXX; i++) dungeon[i][MDMAXY-1] = L3_DIRT;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3River()
{
	int rx, ry, px, py;
	int dir, pdir, nodir, nodir2, dircheck;
	int river[3][100], rivercnt, riveramt;
	int i, j;
	int trys, found, bridge, lpcnt, bail;

	riveramt = 0;
	found = 0;
	trys = 0;

	while (trys < 200 && riveramt < 4) {
		found = 0;

		while (found == 0 && trys < 200) {
			rivercnt = 0;
			trys++;

			rx = 0;
			ry = 0;
			bail = 0;
			while ((dungeon[rx][ry] < 25 || dungeon[rx][ry] > 28) && (bail < 100)) {
				rx = random(0, MDMAXX);
				ry = random(0, MDMAXY);
				bail++;
				while ((dungeon[rx][ry] < 25 || dungeon[rx][ry] > 28) && (ry < MDMAXY)) {
					rx ++;
					if (rx >= MDMAXX) {
						rx = 0;
						ry++;
					}
				}
			}
			
			if (bail >= 100) return;

			switch (dungeon[rx][ry]) {
				case 25:	
					dir = WEST;
					nodir = EAST;
					river[2][rivercnt] = 40;
					break;
				case 26:
					dir = NORTH;
					nodir = SOUTH;
					river[2][rivercnt] = 38;
					break;
				case 27:
					dir = SOUTH;
					nodir = NORTH;
					river[2][rivercnt] = 41;
					break;
				case 28:
					dir = EAST;
					nodir = WEST;
					river[2][rivercnt] = 39;
					break;
			}
			river[0][rivercnt] = rx;
			river[1][rivercnt] = ry;
			rivercnt++;

			nodir2 = 4;
			dircheck = 0;
			while (dircheck < 4 && rivercnt < 100) {

				px = rx;
				py = ry;

				if (dircheck == 0) dir = random(0, 4);
				else dir = (dir + 1) & 3;
				dircheck++;

				while (dir == nodir || dir == nodir2) {
			 		dir = (dir + 1) & 3;
					dircheck++;
				}

				if (dir == NORTH && ry > 0) ry--;
				if (dir == SOUTH && ry < MDMAXY) ry++;
				if (dir == EAST  && rx < MDMAXX) rx++;
				if (dir == WEST  && rx > 0) rx--;

				if (dungeon[rx][ry] == 7) {
					dircheck = 0;
					if (dir < EAST)  river[2][rivercnt] = 17 + (BYTE)random(0, 2);
					if (dir > SOUTH) river[2][rivercnt] = 15 + (BYTE)random(0, 2);
					river[0][rivercnt] = rx;
					river[1][rivercnt] = ry;
					rivercnt++;

					if ((dir == NORTH && pdir == EAST) || (dir == WEST && pdir == SOUTH)) {
						if (rivercnt > 2) river[2][rivercnt-2] = 22;
						if (dir == NORTH) nodir2 = SOUTH;
						else	nodir2 = EAST;
					}
					if ((dir == NORTH && pdir == WEST) || (dir == EAST && pdir == SOUTH)) {
						if (rivercnt > 2) river[2][rivercnt-2] = 21;
						if (dir == NORTH) nodir2 = SOUTH;
						else	nodir2 = WEST;
					}
					if ((dir == SOUTH && pdir == EAST) || (dir == WEST && pdir == NORTH)) {
						if (rivercnt > 2) river[2][rivercnt-2] = 20;
						if (dir == SOUTH) nodir2 = NORTH;
						else	nodir2 = EAST;
					}
					if ((dir == SOUTH && pdir == WEST) || (dir == EAST && pdir == NORTH)) {
						if (rivercnt > 2) river[2][rivercnt-2] = 19;
						if (dir == SOUTH) nodir2 = NORTH;
						else	nodir2 = WEST;
					}

					pdir = dir;
				}
				else {
					rx = px;
					ry = py;
				}
			}

			if (dir == NORTH && dungeon[rx][ry-1] == 10 && dungeon[rx][ry-2] == 8) {
				river[0][rivercnt] = rx;
				river[1][rivercnt] = ry-1;
				river[2][rivercnt] = 24;
				if (pdir == EAST) river[2][rivercnt-1] = 22;
				if (pdir == WEST) river[2][rivercnt-1] = 21;
				found = 1;
			}
			if (dir == SOUTH && dungeon[rx][ry+1] == 2 && dungeon[rx][ry+2] == 8) {
				river[0][rivercnt] = rx;
				river[1][rivercnt] = ry+1;
				river[2][rivercnt] = 42;
				if (pdir == EAST) river[2][rivercnt-1] = 20;
				if (pdir == WEST) river[2][rivercnt-1] = 19;
				found = 1;
			}
			if (dir == EAST && dungeon[rx+1][ry] == 4 && dungeon[rx+2][ry] == 8) {
				river[0][rivercnt] = rx+1;
				river[1][rivercnt] = ry;
				river[2][rivercnt] = 43;
				if (pdir == NORTH) river[2][rivercnt-1] = 19;
				if (pdir == SOUTH) river[2][rivercnt-1] = 21;
				found = 1;
			}
			if (dir == WEST && dungeon[rx-1][ry] == 9 && dungeon[rx-2][ry] == 8) {
				river[0][rivercnt] = rx-1;
				river[1][rivercnt] = ry;
				river[2][rivercnt] = 23;
				if (pdir == NORTH) river[2][rivercnt-1] = 20;
				if (pdir == SOUTH) river[2][rivercnt-1] = 22;
				found = 1;
			}
		}

		if (found == 1 && rivercnt < 7) found = 0;

		if (found == 1) {

			bridge = 0;
			lpcnt = 0;
			while (bridge == 0 && lpcnt < 30) {
				lpcnt++;
				i = random(0, rivercnt);
				if (((river[2][i] == 15) || (river[2][i] == 16)) &&
					(dungeon[(river[0][i])][(river[1][i])-1] == 7) && (dungeon[(river[0][i])][(river[1][i])+1] == 7))
					bridge = 1;
				if (((river[2][i] == 17) || (river[2][i] == 18)) &&
					(dungeon[(river[0][i])-1][(river[1][i])] == 7) && (dungeon[(river[0][i])+1][(river[1][i])] == 7))
					bridge = 2;

				for (j = 0; j < rivercnt && bridge != 0; j++) {
					if ((bridge == 1) && ((river[1][i] - 1 == river[1][j]) || (river[1][i] + 1 == river[1][j])) &&
						(river[0][i] == river[0][j])) bridge = 0;
					if ((bridge == 2) && ((river[0][i] - 1 == river[0][j]) || (river[0][i] + 1 == river[0][j])) &&
						(river[1][i] == river[1][j])) bridge = 0;
				}
			}

			if (bridge != 0) {
				if (bridge == 1) river[2][i] = 44;
				else river[2][i] = 45;
				riveramt++;
				for (i = 0; i <= rivercnt; i++) dungeon[(river[0][i])][(river[1][i])] = river[2][i];
			}
			else found = 0;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int DRLG_L3SpawnEdge(int x, int y, int * totarea)
{
	BYTE i;
	static const BYTE spawntable[] = {0x00,0x0a,0x43,0x05,0x2c,0x06,0x09,0x00,0x00,0x1c,0x83,0x06,0x09,0x0a,0x05};
	
	if ((*totarea) > 40) return 1;
	if (x < 0 || y < 0 || x >= MDMAXX || y >= MDMAXY) return 1;
	
	if ((dungeon[x][y] & 0x80) != 0) return 0;
	if (dungeon[x][y] > 15) return 1;

	i = dungeon[x][y];
	dungeon[x][y] |= 0x80;
	(*totarea)++;

	if (((spawntable[i] & 0x08) != 0) && ((DRLG_L3SpawnEdge (x, y-1, totarea)) == 1)) return 1;
	if (((spawntable[i] & 0x04) != 0) && ((DRLG_L3SpawnEdge (x, y+1, totarea)) == 1)) return 1;
	if (((spawntable[i] & 0x02) != 0) && ((DRLG_L3SpawnEdge (x+1, y, totarea)) == 1)) return 1;
	if (((spawntable[i] & 0x01) != 0) && ((DRLG_L3SpawnEdge (x-1, y, totarea)) == 1)) return 1;

	if (((spawntable[i] & 0x80) != 0) && ((DRLG_L3Spawn (x, y-1, totarea)) == 1)) return 1;
	if (((spawntable[i] & 0x40) != 0) && ((DRLG_L3Spawn (x, y+1, totarea)) == 1)) return 1;
	if (((spawntable[i] & 0x20) != 0) && ((DRLG_L3Spawn (x+1, y, totarea)) == 1)) return 1;
	if (((spawntable[i] & 0x10) != 0) && ((DRLG_L3Spawn (x-1, y, totarea)) == 1)) return 1;

	return 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int DRLG_L3Spawn(int x, int y, int * totarea)
{
	BYTE i;
	static const BYTE spawntable[] = {0x00,0x0a,0x03,0x05,0x0c,0x06,0x09,0x00,0x00,0x0c,0x03,0x06,0x09,0x0a,0x05};
	
	if ((*totarea) > 40) return 1;
	if (x < 0 || y < 0 || x >= MDMAXX || y >= MDMAXY) return 1;
	
	if ((dungeon[x][y] & 0x80) != 0) return 0;
	if (dungeon[x][y] > 15) return 1;

	i = dungeon[x][y];
	dungeon[x][y] |= 0x80;
	(*totarea)++;

	if (i != 8) {

		if (((spawntable[i] & 0x08) != 0) && ((DRLG_L3SpawnEdge (x, y-1, totarea)) == 1)) return 1;
		if (((spawntable[i] & 0x04) != 0) && ((DRLG_L3SpawnEdge (x, y+1, totarea)) == 1)) return 1;
		if (((spawntable[i] & 0x02) != 0) && ((DRLG_L3SpawnEdge (x+1, y, totarea)) == 1)) return 1;
		if (((spawntable[i] & 0x01) != 0) && ((DRLG_L3SpawnEdge (x-1, y, totarea)) == 1)) return 1;
	}
	else {

		if ((DRLG_L3Spawn (x+1, y, totarea)) == 1) return 1;
		if ((DRLG_L3Spawn (x-1, y, totarea)) == 1) return 1;
		if ((DRLG_L3Spawn (x, y+1, totarea)) == 1) return 1;
		if ((DRLG_L3Spawn (x, y-1, totarea)) == 1) return 1;
	}

	return 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3Pool()
{
	int i, j, found;
	int dunx, duny;
	int totarea, poolchance;
	BYTE k;
	static const BYTE poolsub[] = {0,35,26,36,25,29,34,7,33,28,27,37,32,31,30};

	for (duny = 0; duny < MDMAXY; duny++) {
		for (dunx = 0; dunx < MDMAXX; dunx++) {
		
			if (dungeon[dunx][duny] == 8) {

				dungeon[dunx][duny] |= 0x80;
				totarea = 1;
				found = 0;

				if (dunx+1 < MDMAXX && found == 0) found = DRLG_L3Spawn (dunx+1, duny, &totarea);
				else found = 1;
				if (dunx-1 > 0 && found == 0) found = DRLG_L3Spawn (dunx-1, duny, &totarea);
				else found = 1;
				if (duny+1 < MDMAXY && found == 0) found = DRLG_L3Spawn (dunx, duny+1, &totarea);
				else found = 1;
				if (duny-1 > 0 && found == 0) found = DRLG_L3Spawn (dunx, duny-1, &totarea);
				else found = 1;

				poolchance = random(0, 100);
				for (i = duny-totarea; i < duny+totarea; i++) {
					for (j = dunx-totarea; j < dunx+totarea; j++) {

						if ((dungeon[j][i] & 0x80) != 0 && i >= 0 && i < MDMAXY && j >= 0 && j < MDMAXX) {
							
							dungeon[j][i] &= 0x7f;

							if (totarea > 4 && poolchance < 25 && found == 0) {
								k = poolsub[(dungeon[j][i])];
								if (k != 0 && k <= 37) 
									dungeon[j][i] = k;
								lavapool = TRUE;
							}
						}
					}
				}
			}
		}
	}
}
static void AcidPool()
{
	int i, j, found;
	int dunx, duny;
	int totarea, poolchance;
	BYTE k;
	static const BYTE poolsub[] = {0,133,120,125,119,107,132,7,134,122,121,118,123,131,115};

	for (duny = 0; duny < MDMAXY; duny++) {
		for (dunx = 0; dunx < MDMAXX; dunx++) {
		
			if (dungeon[dunx][duny] == 8) {

				dungeon[dunx][duny] |= 0x80;
				totarea = 1;
				found = 0;

				if (dunx+1 < MDMAXX && found == 0) found = DRLG_L3Spawn (dunx+1, duny, &totarea);
				else found = 1;
				if (dunx-1 > 0 && found == 0) found = DRLG_L3Spawn (dunx-1, duny, &totarea);
				else found = 1;
				if (duny+1 < MDMAXY && found == 0) found = DRLG_L3Spawn (dunx, duny+1, &totarea);
				else found = 1;
				if (duny-1 > 0 && found == 0) found = DRLG_L3Spawn (dunx, duny-1, &totarea);
				else found = 1;

				poolchance = random(0, 100);
				for (i = duny-totarea; i < duny+totarea; i++) {
					for (j = dunx-totarea; j < dunx+totarea; j++) {

						if ((dungeon[j][i] & 0x80) != 0 && i >= 0 && i < MDMAXY && j >= 0 && j < MDMAXX) {
							
							dungeon[j][i] &= 0x7f;

							if (totarea > 4 && poolchance < 25 && found == 0) {
								k = poolsub[(dungeon[j][i])];
								if (k != 0 && k <= 131) 
									dungeon[j][i] = k;
								lavapool = TRUE;
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
static void DRLG_L3PoolFix()
{
	int dunx, duny;

	for (duny = 0; duny < MDMAXY; duny++) {
		for (dunx = 0; dunx < MDMAXX; dunx++) {
			if ((dungeon[dunx][duny] == 8) &&
				((dungeon[dunx-1][duny-1] >= 25 && dungeon[dunx-1][duny-1] <= 41) &&
				 (dungeon[dunx-1][duny]   >= 25	&& dungeon[dunx-1][duny]   <= 41) &&
				 (dungeon[dunx-1][duny+1] >= 25	&& dungeon[dunx-1][duny+1] <= 41) &&
				 (dungeon[dunx][duny-1]   >= 25	&& dungeon[dunx][duny-1]   <= 41) &&
				 //(dungeon[dunx][duny]     >= 25	&& dungeon[dunx][duny+1]   <= 41) &&
				 (dungeon[dunx][duny+1]   >= 25	&& dungeon[dunx][duny+1]     <= 41) &&
				 (dungeon[dunx+1][duny-1] >= 25	&& dungeon[dunx+1][duny-1] <= 41) &&
				 (dungeon[dunx+1][duny]	  >= 25	&& dungeon[dunx+1][duny]   <= 41) &&
				 (dungeon[dunx+1][duny+1] >= 25	&& dungeon[dunx+1][duny+1] <= 41)))

				dungeon[dunx][duny] = 33;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void AcidPoolFix()
{
	int dunx, duny;

	for (duny = 0; duny < MDMAXY; duny++) {
		for (dunx = 0; dunx < MDMAXX; dunx++) 
		{
			if ((dungeon[dunx][duny] == 8) &&
				((dungeon[dunx-1][duny-1] >= 107 && dungeon[dunx-1][duny-1] <= 135) ||
				 (dungeon[dunx-1][duny]   >= 107	&& dungeon[dunx-1][duny]   <= 135) ||
				 (dungeon[dunx-1][duny+1] >= 107	&& dungeon[dunx-1][duny+1] <= 135) ||
				 (dungeon[dunx][duny-1]   >= 107	&& dungeon[dunx][duny-1]   <= 135) ||
				 //(dungeon[dunx][duny]     >= 25	&& dungeon[dunx][duny+1]   <= 41) &&
				 (dungeon[dunx][duny+1]   >= 107	&& dungeon[dunx][duny+1]     <= 135) ||
				 (dungeon[dunx+1][duny-1] >= 107	&& dungeon[dunx+1][duny-1] <= 135) ||
				 (dungeon[dunx+1][duny]	  >= 107	&& dungeon[dunx+1][duny]   <= 135) ||
				 (dungeon[dunx+1][duny+1] >= 107	&& dungeon[dunx+1][duny+1] <= 135)))

				dungeon[dunx][duny] = 134;
			
			if ((dungeon[dunx][duny] < 7) &&
				((dungeon[dunx+1][duny] >= 107 && dungeon[dunx+1][duny] <= 135) ||
				 (dungeon[dunx][duny-1] >= 107 && dungeon[dunx][duny-1] <= 135)))

				dungeon[dunx][duny] = 131;

		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int DRLG_L3PlaceMiniSet(const byte miniset[], int tmin, int tmax, int cx, int cy, int setview, int ldir)
{
	int sx, sy;
	int sw, sh;
	int xx, yy;
	int i, ii, numt;
	int found, trys;

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
		trys = 0;
		while ((found == 0) && (trys < 200)) {
			trys++;
			found = 1;

//			if (((sx >= SP3x1) && (sx <= SP3x2)) && ((sy >= SP3y1) && (sy <= SP3y2))) found = 0;

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
		}

		if (trys >= 200) return 1;

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
		ViewX = (sx << 1) + (DIRTEDGED2) + 1;
		ViewY = (sy << 1) + (DIRTEDGED2) + 3;
	}

	if (ldir == LVL_DOWN) {
		LvlViewX = (sx << 1) + (DIRTEDGED2) + 1;
		LvlViewY = (sy << 1) + (DIRTEDGED2) + 3;
	}

	return 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3PlaceRndSet(const byte miniset[], int rndper)
{
	int sx, sy;
	int sw, sh;
	int xx, yy;
	int ii, kk;
	int found;

	// Width and height of the mini set piece
	sw = miniset[0];
	sh = miniset[1];

	// Find a location for the mini set piece
	for (sy = 0; sy < (MDMAXY - sh); sy++) {
		for (sx = 0; sx < (MDMAXX - sw); sx++) {
			found = 1;
			ii = 2;

//			if (((sx >= SP3x1) && (sx <= SP3x2)) && ((sy >= SP3y1) && (sy <= SP3y2))) found = 0;

			for (yy = 0; ((yy < sh) && (found == 1)); yy++) {
				for (xx = 0; ((xx < sw) && (found == 1)); xx++) {
					if ((miniset[ii] != 0) && (dungeon[sx+xx][sy+yy] != miniset[ii])) found = 0;
					if (dflags[sx+xx][sy+yy] != 0) found = 0;
					ii++;
				}
			}
			
			kk = (sh * sw) + 2;
			if (miniset[kk] >= 84 && miniset[kk] <=100 && found == 1) {
				if (dungeon[sx-1][sy] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
				if (dungeon[sx+1][sy] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
				if (dungeon[sx][sy+1] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
				if (dungeon[sx][sy-1] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
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

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static unsigned char DRLG_L3PlaceAcidPool(const byte miniset[], int rndper)
{
	int sx, sy;
	int sw, sh;
	int xx, yy;
	int ii, kk;
	int found;
	unsigned char made = 0;

	// Width and height of the mini set piece
	sw = miniset[0];
	sh = miniset[1];

	// Find a location for the mini set piece
	for (sy = 0; sy < (MDMAXY - sh); sy++) {
		for (sx = 0; sx < (MDMAXX - sw); sx++) {
			found = 1;
			ii = 2;

//			if (((sx >= SP3x1) && (sx <= SP3x2)) && ((sy >= SP3y1) && (sy <= SP3y2))) found = 0;

			for (yy = 0; ((yy < sh) && (found == 1)); yy++) {
				for (xx = 0; ((xx < sw) && (found == 1)); xx++) {
					if ((miniset[ii] != 0) && (dungeon[sx+xx][sy+yy] != miniset[ii])) found = 0;
					if (dflags[sx+xx][sy+yy] != 0) found = 0;
					ii++;
				}
			}
			
			kk = (sh * sw) + 2;
			if (miniset[kk] >= 84 && miniset[kk] <=100 && found == 1) {
				if (dungeon[sx-1][sy] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
				if (dungeon[sx+1][sy] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
				if (dungeon[sx][sy+1] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
				if (dungeon[sx][sy-1] >= 84 && dungeon[sx-1][sy] <= 100) found = 0;
			}
				
			if ((found == 1) && (random(0, 100) < rndper)) {
				// Place mini set piece
				made = 1;
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
						if (miniset[kk] != 0) dungeon[sx+xx][sy+yy] = miniset[kk];
						kk++;
					}
				}
			}
		}
	}
	return made;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3Abyss()
{
	int	ax, sf, j;

	ax = MDMAXX - 1;
	sf = 0;
	while (sf == 0) {
		for (j = 0; (j < MDMAXY) && (sf == 0); j++)
			if (dungeon[ax][j] != 8) sf = 1;
		if (sf == 0) ax--;
	}

	abyssx = (ax << 1) + (DIRTEDGED2);

	for (j = 0; j < MDMAXY; j++) {
		if (dungeon[ax][j] == 8) dungeon[ax][j] = 111;
		if (dungeon[ax][j] == 2 || dungeon[ax][j] == 6) dungeon[ax][j] = 120;
		if (dungeon[ax][j] == 3 || dungeon[ax][j] == 10) dungeon[ax][j] = 112;
		if (dungeon[ax][j] < 111) dungeon[ax][j] = 7;

		if ((j & 0x0001) == 0) dungeon[ax+1][j] = 114;
		else dungeon[ax+1][j] = 113;
		if ((j & 0x0001) == 0) dungeon[ax+2][j] = 115;
		else dungeon[ax+2][j] = 116;
		if ((j & 0x0001) == 0) dungeon[ax+3][j] = 118;
		else dungeon[ax+3][j] = 117;
	}

	for (ax += 4; ax < MDMAXX; ax++)
		for (j = 0; j < MDMAXY; j++) dungeon[ax][j] = 119;

}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL WoodVertU(int i, int y)
{
	if ((dungeon[i+1][y] > 152 || dungeon[i+1][y] < 130) &&
		(dungeon[i-1][y] > 152 || dungeon[i-1][y] < 130))
	{
		if (dungeon[i][y] == 7) return TRUE;
		if (dungeon[i][y] == 10) return TRUE;
		if (dungeon[i][y] == 126) return TRUE;
		if (dungeon[i][y] == 129) return TRUE;
		if (dungeon[i][y] == 134) return TRUE;
		if (dungeon[i][y] == 136) return TRUE;
	}
	return FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL WoodVertD(int i, int y)
{
	if ((dungeon[i+1][y] > 152 || dungeon[i+1][y] < 130) &&
		(dungeon[i-1][y] > 152 || dungeon[i-1][y] < 130))
	{
		if (dungeon[i][y] == 7) return TRUE;
		if (dungeon[i][y] == 2) return TRUE;
		if (dungeon[i][y] == 134) return TRUE;
		if (dungeon[i][y] == 136) return TRUE;
	}
	return FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL WoodHorizL(int x, int j)
{
	if ((dungeon[x][j+1] > 152 || dungeon[x][j+1] < 130) &&
		(dungeon[x][j-1] > 152 || dungeon[x][j-1] < 130))
	{
		if (dungeon[x][j] == 7) return TRUE;
		if (dungeon[x][j] == 9) return TRUE;
		if (dungeon[x][j] == 121) return TRUE;
		if (dungeon[x][j] == 124) return TRUE;
		if (dungeon[x][j] == 135) return TRUE;
		if (dungeon[x][j] == 137) return TRUE;
	}
	return FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL WoodHorizR(int x, int j)
{
	if ((dungeon[x][j+1] > 152 || dungeon[x][j+1] < 130) &&
		(dungeon[x][j-1] > 152 || dungeon[x][j-1] < 130))
	{
		if (dungeon[x][j] == 7) return TRUE;
		if (dungeon[x][j] == 4) return TRUE;
		if (dungeon[x][j] == 135) return TRUE;
		if (dungeon[x][j] == 137) return TRUE;
	}
	return FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AddFenceDoors(void)
/*-----------------------------------------------------------------------**
** DESCRIPTION: Adds doors to open spaces in fence lines
** INPUT: None
** RETURN: None
/*-----------------------------------------------------------------------*/
{
	int i, j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			//Add horizontal doors
			if (dungeon[i][j] == FLOOR && 
				((dungeon[i-1][j] <= 152 && dungeon[i-1][j] >= 130) &&
				 (dungeon[i+1][j] <= 152 && dungeon[i+1][j] >= 130)))
				dungeon[i][j] = WOOD_HORIZGATE;
			//Add vertical doors
			else if (dungeon[i][j] == FLOOR && 
				((dungeon[i][j-1] <= 152 && dungeon[i][j-1] >= 130) &&
				 (dungeon[i][j+1] <= 152 && dungeon[i][j+1] >= 130)))
				dungeon[i][j] = WOOD_VERTGATE;
		}
	} 
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void FenceDoorFix(void)
/*-----------------------------------------------------------------------**
** DESCRIPTION: Fixes any free standing doors. This removes any doors which
**              are not attached to a fence line.
** INPUT: None
** RETURN: None
/*-----------------------------------------------------------------------*/
{
	int	i, j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if (dungeon[i][j] == WOOD_HORIZGATE &&
				((dungeon[i+1][j] > 152 || dungeon[i+1][j] < 130) ||
				 (dungeon[i-1][j] > 152 || dungeon[i-1][j] < 130)))
				dungeon[i][j] = FLOOR;
			else if (dungeon[i][j] == WOOD_HORIZGATE &&
					 dungeon[i+1][j] != 130 && dungeon[i-1][j] != 130 &&
					 dungeon[i+1][j] != 132 && dungeon[i-1][j] != 132 &&
					 dungeon[i+1][j] != 133 && dungeon[i-1][j] != 133 &&
					 dungeon[i+1][j] != 134 && dungeon[i-1][j] != 134 &&
					 dungeon[i+1][j] != 136 && dungeon[i-1][j] != 136 &&
					 dungeon[i+1][j] != 138 && dungeon[i-1][j] != 138 &&
					 dungeon[i+1][j] != 140 && dungeon[i-1][j] != 140)
				dungeon[i][j] = FLOOR;
			else if (dungeon[i][j] == WOOD_VERTGATE &&
			         ((dungeon[i][j+1] > 152 || dungeon[i][j+1] < 130) ||
			          (dungeon[i][j-1] > 152 || dungeon[i][j-1] < 130)))
				dungeon[i][j] = FLOOR;
			else if (dungeon[i][j] == WOOD_VERTGATE &&
					 dungeon[i][j+1] != 131 && dungeon[i][j-1] != 131 &&
					 dungeon[i][j+1] != 132 && dungeon[i][j-1] != 132 &&
					 dungeon[i][j+1] != 133 && dungeon[i][j-1] != 133 &&
					 dungeon[i][j+1] != 135 && dungeon[i][j-1] != 135 &&
					 dungeon[i][j+1] != 137 && dungeon[i][j-1] != 137 &&
					 dungeon[i][j+1] != 138 && dungeon[i][j-1] != 138 &&
					 dungeon[i][j+1] != 139 && dungeon[i][j-1] != 139)
				dungeon[i][j] = FLOOR;
		} 
	} 
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3Wood()
{
	int i,j;
	int x,y;
	int xx,yy;
	int rt, rp, skip;
	int x1,y1,x2,y2;

	// Put wood in walls/corners
	for (j = 0; j < MDMAXY-1; j++) {
		for (i = 0; i < MDMAXX-1; i++) {
			if ((dungeon[i][j] == 10) && (random(0, 2))) {
				x = i;
				while (dungeon[x][j] == 10) x++;
				x--;
				if ((x - i) > 0) {
					dungeon[i][j] = 127;
					for (xx = i+1; xx < x; xx++) {
						if (random(0, 2)) dungeon[xx][j] = 126;
						else dungeon[xx][j] = 129;
					}
					dungeon[x][j] = 128;
				}
			}
			if ((dungeon[i][j] == 9) && (random(0, 2))) {
				y = j;
				while (dungeon[i][y] == 9) y++;
				y--;
				if ((y - j) > 0) {
					dungeon[i][j] = 123;
					for (yy = j+1; yy < y; yy++) {
						if (random(0, 2)) dungeon[i][yy] = 121;
						else dungeon[i][yy] = 124;
					}
					dungeon[i][y] = 122;
				}
			}
			if ((dungeon[i][j] == 11) && (dungeon[i+1][j] == 10) && (dungeon[i][j+1] == 9) && (random(0, 2))) {
				dungeon[i][j] = 125;
				x = i+1;
				while (dungeon[x][j] == 10) x++;
				x--;
				for (xx = i+1; xx < x; xx++) {
					if (random(0, 2)) dungeon[xx][j] = 126;
					else dungeon[xx][j] = 129;
				}
				dungeon[x][j] = 128;
				y = j+1;
				while (dungeon[i][y] == 9) y++;
				y--;
				for (yy = j+1; yy < y; yy++) {
					if (random(0, 2)) dungeon[i][yy] = 121;
					else dungeon[i][yy] = 124;
				}
				dungeon[i][y] = 122;
			}
		}
	}

	// Put wood lines in
	for (j = 0; j < MDMAXY; j++) 
	{
		for (i = 0; i < MDMAXX; i++) 
		{
			if ((dungeon[i][j] == FLOOR) && (random(0, 1) == 0) && 
				(SkipThemeRoom(i, j))) 
			{
				rt = random(0, 2);
				x1 = 0;
				y1 = 0;
				//Create vertical fence
				if (rt == 0) {
					y1 = j;
					while (WoodVertU(i,y1)) y1--;
					y1++;
					y2 = j;
					while (WoodVertD(i,y2)) y2++;
					y2--;
					rp = 1;
					if (dungeon[i][y1] == FLOOR) rp = 0;
					if (dungeon[i][y2] == FLOOR) rp = 0;
					if (((y2 - y1) > 1) && (rp != 0)) {
						skip = random(0, y2 - y1 - 1) + y1 + 1;
						for (y = y1; y <= y2; y++) {
							if (y != skip) {
								if (dungeon[i][y] == FLOOR) {
									if (random(0, 2)) dungeon[i][y] = 135;
									else dungeon[i][y] = 137;
								} 
								if (dungeon[i][y] == 10) dungeon[i][y] = 131;
								if (dungeon[i][y] == 126) dungeon[i][y] = 133;
								if (dungeon[i][y] == 129) dungeon[i][y] = 133;
								if (dungeon[i][y] == 2) dungeon[i][y] = 139;
								if (dungeon[i][y] == 134) dungeon[i][y] = 138;
								if (dungeon[i][y] == 136) dungeon[i][y] = 138;
							} 
						} 
					} 
				} 
				//Create horizontal fence
				if (rt == 1) {
					x1 = i;
					while (WoodHorizL(x1,j)) x1--;
					x1++;
					x2 = i;
					while (WoodHorizR(x2,j)) x2++;
					x2--;
					rp = 1;
					if (dungeon[x1][j] == FLOOR) rp = 0;
					if (dungeon[x2][j] == FLOOR) rp = 0;
					if (((x2 - x1) > 1) && (rp != 0)) {
						skip = random(0, x2 - x1 - 1) + x1 + 1;
						for (x = x1; x <= x2; x++) {
							if (x != skip) {
								if (dungeon[x][j] == FLOOR) {
									if (random(0, 2)) dungeon[x][j] = 134;
									else dungeon[x][j] = 136;
								} 
								if (dungeon[x][j] == 9) dungeon[x][j] = 130;
								if (dungeon[x][j] == 121) dungeon[x][j] = 132;
								if (dungeon[x][j] == 124) dungeon[x][j] = 132;
								if (dungeon[x][j] == 4) dungeon[x][j] = 140;
								if (dungeon[x][j] == 135) dungeon[x][j] = 138;
								if (dungeon[x][j] == 137) dungeon[x][j] = 138;
							} 
						} 
					} 
				} 
			} 
		} 
	} 

	//Add doors to fences
	AddFenceDoors();
	FenceDoorFix();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*

void DRLG_L3FTVR(int i, int j, int x, int y, int d)
{
	if ((dTransVal[x][y] == 0) && (dungeon[i][j] == FLOOR)) {
		dTransVal[x][y] = TransVal;
		dTransVal[x+1][y] = TransVal;
		dTransVal[x][y+1] = TransVal;
		dTransVal[x+1][y+1] = TransVal;
		DRLG_L3FTVR(i+1,j, x+2,y, 1);
		DRLG_L3FTVR(i-1,j, x-2,y, 2);
		DRLG_L3FTVR(i,j+1, x,y+2, 3);
		DRLG_L3FTVR(i,j-1, x,y-2, 4);

		DRLG_L3FTVR(i-1,j-1, x-2,y-2, 5);
		DRLG_L3FTVR(i+1,j-1, x+2,y-2, 6);
		DRLG_L3FTVR(i-1,j+1, x-2,y+2, 7);
		DRLG_L3FTVR(i+1,j+1, x+2,y+2, 8);
	} else {
		if (d == 1) {
			dTransVal[x][y] = TransVal;
			dTransVal[x+1][y] = TransVal;
			dTransVal[x][y+1] = TransVal;
			dTransVal[x+1][y+1] = TransVal;
		}
		if (d == 2) {
			dTransVal[x+1][y] = TransVal;
			dTransVal[x+1][y+1] = TransVal;
		}
		if (d == 3) {
			dTransVal[x][y] = TransVal;
			dTransVal[x+1][y] = TransVal;
			dTransVal[x][y+1] = TransVal;
			dTransVal[x+1][y+1] = TransVal;
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

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*

void DRLG_L3FloodTVal()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == FLOOR) && (dTransVal[xx][yy] == 0)) {
				DRLG_L3FTVR(i,j,xx,yy,0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*

void DRLG_L3TransFix()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == UR_WALL) && (dungeon[i][j-1] == LEFT_WALL)) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if ((dungeon[i][j] == LL_WALL) && (dungeon[i+1][j] == TOP_WALL)) {
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == LEFT_WALL) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == TOP_WALL) {
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == LR_WALL) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			xx += 2;
		}
		yy += 2;
	}

	yy = DMAXY - DIRTEDGED2 - 2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DMAXX - DIRTEDGED2 - 2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dTransVal[xx-1][yy] != 0) ||
				(dTransVal[xx][yy-1] != 0) ||
				(dTransVal[xx-1][yy-1] != 0)) {
				dTransVal[xx][yy] = 1;
				dTransVal[xx+1][yy] = 1;
				dTransVal[xx][yy+1] = 1;
				dTransVal[xx+1][yy+1] = 1;
			}
			xx -= 2;
		}
		yy -= 2;
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dTransVal[i][j] != 0) dTransVal[i][j] = 1;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

int DRLG_L3Anvil()
{
	int sx, sy;
	int sw, sh;
	int xx, yy;
	int ii;
	int found, trys;

	// Width and height of the mini set piece
	sw = L3ANVIL[0];
	sh = L3ANVIL[1];

	// Random starting pos
	sx = random(0, MDMAXX-sw);
	sy = random(0, MDMAXY-sh);

	// Find a location for the mini set piece
	found = 0;
	trys = 0;
	while ((found == 0) && (trys < 200)) {
		trys++;
		found = 1;

//		if (((sx >= SP3x1) && (sx <= SP3x2)) && ((sy >= SP3y1) && (sy <= SP3y2))) found = 0;

		ii = 2;
		for (yy = 0; ((yy < sh) && (found == 1)); yy++) {
			for (xx = 0; ((xx < sw) && (found == 1)); xx++) {
				if ((L3ANVIL[ii] != 0) && (dungeon[sx+xx][sy+yy] != L3ANVIL[ii])) found = 0;
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
	}

	if (trys >= 200) return 1;

	// Place mini set piece
	ii = (sh * sw) + 2;
	for (yy = 0; yy < sh; yy++) {
		for (xx = 0; xx < sw; xx++) {
			if (L3ANVIL[ii] != 0) dungeon[sx+xx][sy+yy] = L3ANVIL[ii];
			dflags[sx+xx][sy+yy] |= SETP_BIT;
			ii++;
		}
	}

	setpc_x = sx;
	setpc_y = sy;
	setpc_w = sw;
	setpc_h = sh;

	return 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void FixL3Warp()
{
	int i, j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 125) &&
				(dungeon[i+1][j] == 125) &&
				(dungeon[i][j+1] == 125) &&
				(dungeon[i+1][j+1] == 125)) {
				dungeon[i][j] = 156;
				dungeon[i+1][j] = 155;
				dungeon[i][j+1] = 153;
				dungeon[i+1][j+1] = 154;
				return;
			}
			if ((dungeon[i][j] == 5) && (dungeon[i+1][j+1] == 7))
				dungeon[i][j] = 7;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void FixL3HallofHeroes()
{
	int i, j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 5) && (dungeon[i+1][j+1] == 7))
				dungeon[i][j] = 7;
		}
	}
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 5) && (dungeon[i+1][j+1] == 12) && (dungeon[i+1][j] == 7)) {
				dungeon[i][j] = 7;
				dungeon[i][j+1] = 7;
				dungeon[i+1][j+1] = 7;
			}
			if ((dungeon[i][j] == 5) && (dungeon[i+1][j+1] == 12) && (dungeon[i][j+1] == 7)) {
				dungeon[i][j] = 7;
				dungeon[i+1][j] = 7;
				dungeon[i+1][j+1] = 7;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

byte lockout[MDMAXX][MDMAXY];
int lockoutcnt;

void DRLG_L3LockRec(int x, int y)
{
	if (lockout[x][y] != 0) {
		lockout[x][y] = 0;
		lockoutcnt++;
		DRLG_L3LockRec(x, y-1);
		DRLG_L3LockRec(x, y+1);
		DRLG_L3LockRec(x-1, y);
		DRLG_L3LockRec(x+1, y);
	}
}

BOOL DRLG_L3Lockout()
{
	int i,j,t;
	int fx,fy;

	t = 0;
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if (dungeon[i][j] != 0) {
				lockout[i][j] = 1;
				fx = i;
				fy = j;
				t++;
			} else lockout[i][j] = 0;
		}
	}
	lockoutcnt = 0;
	DRLG_L3LockRec(fx, fy);
	if (t == lockoutcnt) return(TRUE);
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static void DRLG_L3(int entry)
{
	int x1, y1, x2, y2, sx1, sy1;
	int i, j;
	int found;
	BOOL genok;

	lavapool = FALSE;
	do {
		do {
			do {			// do this till MIN AREA is met.
				InitL3Dungeon();	// fill dungeon with fill

				x1 = random(0, MDMAXX >> 1) + (MDMAXX >> 2);	// random number 0-30
				y1 = random(0, MDMAXY >> 1) + (MDMAXY >> 2);
				x2 = x1 + 2;
				y2 = y1 + 2;
				DRLG_L3FillRoom(x1,y1,x2,y2);		// if empty, randomly fill dungeon with 1s
				DRLG_L3CreateBlock(x1,y1,2,BLK_U);	// recursively add blocks at random directions
				DRLG_L3CreateBlock(x2,y1,2,BLK_R);	// starting with the direction listed at the 
				DRLG_L3CreateBlock(x1,y2,2,BLK_D);	// end of the function
				DRLG_L3CreateBlock(x1,y1,2,BLK_L);

				if (QuestStatus(Q_ANVIL)) {		// force this whole area to be a floor
					sx1 = random(0, MDMAXX >> 2) + (MDMAXX >> 2);
					sy1 = random(0, MDMAXY >> 2) + (MDMAXY >> 2);
					DRLG_L3FloorArea(sx1, sy1, sx1+12, sy1+12);
				}

				DRLG_L3FillDiags();
				DRLG_L3FillSingles();
				DRLG_L3FillStraights();
				DRLG_L3FillDiags();
				DRLG_L3Edges();
				if (DRLG_L3GetFloorArea() >= MINFAREA) {
					genok = DRLG_L3Lockout();
				} else genok = FALSE;
			} while (! genok);
			DRLG_L3MakeMegas();

//*************************************************************
//			Place stairs
//*************************************************************
			
			if (entry == LVL_DOWN) {
				if (currlevel < HIVESTART)
					found = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 1, LVL_DOWN);
					else	// place stairs down. On first level have town warp stairs
					{
						if (currlevel != HIVESTART)						
							found = DRLG_L3PlaceMiniSet(L6UP, 1, 1, -1, -1, 1, LVL_DOWN);
						else
							found = DRLG_L3PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, 1, LVL_TWARPDN);	// JKE add hive
					}

				if (found == 0)	// Place stairs down
				{
					if (currlevel < HIVESTART)
						found = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 0, LVL_UP);
					else
						if (currlevel != HIVEEND)	// no stairs down on last level
							found = DRLG_L3PlaceMiniSet(L6DOWN, 1, 1, -1, -1, 0, LVL_UP);
				}

				if ((found == 0) && (currlevel == 9))
				{
					if (currlevel < HIVESTART)
						found = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, 0, LVL_TWARPDN); // add hive level JKE
				}
			} else {
				if (entry == LVL_UP) {
					if (currlevel < HIVESTART)
						found = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 0, LVL_DOWN);
					else
					{
						if (currlevel != HIVESTART)						
							found = DRLG_L3PlaceMiniSet(L6UP, 1, 1, -1, -1, 0, LVL_DOWN);
						else
							found = DRLG_L3PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, 0, LVL_TWARPDN);	// JKE add hive
					}
					if (found == 0) {
						if (currlevel < HIVESTART)
						{
							found = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 1, LVL_UP);
							ViewX += 2;
							ViewY -= 2;
						}
						else
						{
							if (currlevel != HIVEEND)
							{
								found = DRLG_L3PlaceMiniSet(L6DOWN, 1, 1, -1, -1, 1, LVL_UP);
								ViewX += 2;
								ViewY -= 2;
							}
						}

					if ((found == 0) && (currlevel == 9))
						if (currlevel < HIVESTART)
							found = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, 0, LVL_TWARPDN); // JKE add hive
					}
				} else {
					if (currlevel < HIVESTART)
						found = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 0, LVL_DOWN);
					else
					{
						if (currlevel != HIVESTART)						
							found = DRLG_L3PlaceMiniSet(L6UP, 1, 1, -1, -1, 0, LVL_DOWN);
						else
							found = DRLG_L3PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, 1, LVL_TWARPDN);	// JKE add hive
					}
					if (found == 0)
						if (currlevel < HIVESTART)
							found = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 0, LVL_UP);
						else
							if (currlevel != HIVEEND)
								found = DRLG_L3PlaceMiniSet(L6DOWN, 1, 1, -1, -1, 0, LVL_UP);

					if ((found == 0) && (currlevel == 9))
							found = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, 1, LVL_TWARPDN);	// JKE add hive
				}
			}

			if ((found == 0) && QuestStatus(Q_ANVIL))
				found = DRLG_L3Anvil();

		} while (found == 1);

//		if (currlevel < HIVESTART)
//			DRLG_L3Abyss();
// JKE temp hack remove lava
		if (currlevel < HIVESTART) 
			DRLG_L3Pool();
		else
		{
//			lavapool = TRUE;
			lavapool += DRLG_L3PlaceAcidPool(L6ACID3, 30);
			lavapool += DRLG_L3PlaceAcidPool(L6ACID4, 40);
			lavapool += DRLG_L3PlaceAcidPool(L6ACID1, 50);
			lavapool += DRLG_L3PlaceAcidPool(L6ACID2, 60);

			if (lavapool < 3)
				lavapool = FALSE;
		}
//			AcidPool();

	} while (!lavapool);

// JKE temp hack remove lava
	if (currlevel < HIVESTART)
		DRLG_L3PoolFix();
//	else
//		AcidPoolFix();

	if (currlevel < HIVESTART)
		FixL3Warp();
// JKE temp hack remove lava
	if (currlevel < HIVESTART)
	{
		DRLG_L3PlaceRndSet(L3ISLE1, 70);
		DRLG_L3PlaceRndSet(L3ISLE2, 70);
		DRLG_L3PlaceRndSet(L3ISLE3, 30);
		DRLG_L3PlaceRndSet(L3ISLE4, 30);
		DRLG_L3PlaceRndSet(L3ISLE1, 100);
		DRLG_L3PlaceRndSet(L3ISLE2, 100);
		DRLG_L3PlaceRndSet(L3ISLE5, 90);
	}
	else
	{
		DRLG_L3PlaceRndSet(ACIDISLE1, 70);
		DRLG_L3PlaceRndSet(ACIDISLE2, 70);
		DRLG_L3PlaceRndSet(ACIDISLE3, 30);
		DRLG_L3PlaceRndSet(ACIDISLE4, 30);
		DRLG_L3PlaceRndSet(ACIDISLE1, 100);
		DRLG_L3PlaceRndSet(ACIDISLE2, 100);
		DRLG_L3PlaceRndSet(ACIDISLE5, 90);
	}

	if (currlevel < HIVESTART)
		FixL3HallofHeroes();

// temp hack JKE remove lava
	if (currlevel < HIVESTART)
				DRLG_L3River();

	// this may make other lockouts - (only on lvl10 with anvil quest). . . .
	// this is a kludgy fix for anvil quest - the bridge to the anvil was sometimes getting
	// blocked by river, this makes the "fake" walls back into the bridge - rjs
	if (QuestStatus(Q_ANVIL)) {
		dungeon[setpc_x + 7][setpc_y + 5] = 7;
		dungeon[setpc_x + 8][setpc_y + 5] = 7;
		dungeon[setpc_x + 9][setpc_y + 5] = 7;
		if ((dungeon[setpc_x + 10][setpc_y + 5] == 17) ||
			(dungeon[setpc_x + 10][setpc_y + 5] == 18)) dungeon[setpc_x + 10][setpc_y + 5] = 45;
	}

	if (currlevel < HIVESTART)
		DRLG_PlaceThemeRooms(5, 10, FLOOR, 0, FALSE);
// temp hack JKE remove most
	if (currlevel < HIVESTART)
	{
		DRLG_L3Wood();

		DRLG_L3PlaceRndSet(L3TITE1, 10);
		DRLG_L3PlaceRndSet(L3TITE2, 10);
		DRLG_L3PlaceRndSet(L3TITE3, 10);
		DRLG_L3PlaceRndSet(L3TITE6, 20);
		DRLG_L3PlaceRndSet(L3TITE7, 20);
		//DRLG_L3PlaceRndSet(L3TITE4, 10);
		//DRLG_L3PlaceRndSet(L3TITE5, 10);
		DRLG_L3PlaceRndSet(L3TITE8, 20);
		DRLG_L3PlaceRndSet(L3TITE9, 20);
		DRLG_L3PlaceRndSet(L3TITE10,20);
		DRLG_L3PlaceRndSet(L3TITE11,30);
		DRLG_L3PlaceRndSet(L3TITE12,20);
		DRLG_L3PlaceRndSet(L3TITE13,20);

		DRLG_L3PlaceRndSet(L3CREV1, 30);
		DRLG_L3PlaceRndSet(L3CREV2, 30);
		DRLG_L3PlaceRndSet(L3CREV3, 30);
		DRLG_L3PlaceRndSet(L3CREV4, 30);
		DRLG_L3PlaceRndSet(L3CREV5, 30);
		DRLG_L3PlaceRndSet(L3CREV6, 30);
		DRLG_L3PlaceRndSet(L3CREV7, 30);
		DRLG_L3PlaceRndSet(L3CREV8, 30);
		DRLG_L3PlaceRndSet(L3CREV9, 30);
		DRLG_L3PlaceRndSet(L3CREV10,30);
		DRLG_L3PlaceRndSet(L3CREV11,30);

		DRLG_L3PlaceRndSet(L3XTRA1,25);
		DRLG_L3PlaceRndSet(L3XTRA2,25);
		DRLG_L3PlaceRndSet(L3XTRA3,25);
		DRLG_L3PlaceRndSet(L3XTRA4,25);
		DRLG_L3PlaceRndSet(L3XTRA5,25);
	}
	else		// JKE Place our tiles here.
	{
//		DRLG_L3PlaceRndSet(L6ACIDTEST,25);
		DRLG_L3PlaceRndSet(L6FILL1,20);		// JKE Tile to sub, and % to exchange.
		DRLG_L3PlaceRndSet(L6FILL2,20);
		DRLG_L3PlaceRndSet(L6FILL3,20);
		DRLG_L3PlaceRndSet(L6FILL4,20);

		DRLG_L3PlaceRndSet(L6TITE1,10);
		DRLG_L3PlaceRndSet(L6TITE2,15);
		DRLG_L3PlaceRndSet(L6TITE3,20);
		DRLG_L3PlaceRndSet(L6TITE4,25);
		DRLG_L3PlaceRndSet(L6TITE5,30);
		DRLG_L3PlaceRndSet(L6TITE6,35);
		DRLG_L3PlaceRndSet(L6TITE7,40);
		DRLG_L3PlaceRndSet(L6TITE8,45);
		DRLG_L3PlaceRndSet(L6TITE9,50);
		DRLG_L3PlaceRndSet(L6TITE10,55);
		
		DRLG_L3PlaceRndSet(L6TITE10,10);
		DRLG_L3PlaceRndSet(L6TITE9,15);
		DRLG_L3PlaceRndSet(L6TITE8,20);
		DRLG_L3PlaceRndSet(L6TITE7,25);
		DRLG_L3PlaceRndSet(L6TITE6,30);
		DRLG_L3PlaceRndSet(L6TITE5,35);
		DRLG_L3PlaceRndSet(L6TITE4,40);
		DRLG_L3PlaceRndSet(L6TITE3,45);
		DRLG_L3PlaceRndSet(L6TITE2,50);
		DRLG_L3PlaceRndSet(L6TITE1,55);

		DRLG_L3PlaceRndSet(L6FLOOR5,40);
		DRLG_L3PlaceRndSet(L6FLOOR6,45);

		DRLG_L3PlaceRndSet(L6FLOOR1,25);
		DRLG_L3PlaceRndSet(L6FLOOR2,25);
		DRLG_L3PlaceRndSet(L6FLOOR3,25);
		DRLG_L3PlaceRndSet(L6FLOOR4,25);
		DRLG_L3PlaceRndSet(L6WALL1,25);
		DRLG_L3PlaceRndSet(L6WALL2,25);
		DRLG_L3PlaceRndSet(L6WALL3,25);
		DRLG_L3PlaceRndSet(L6WALL4,25);
		DRLG_L3PlaceRndSet(L6WALL5,25);
		DRLG_L3PlaceRndSet(L6WALL6,25);
		DRLG_L3PlaceRndSet(L6WALL7,25);
		DRLG_L3PlaceRndSet(L6WALL8,25);
		DRLG_L3PlaceRndSet(L6WALL9,25);
		DRLG_L3PlaceRndSet(L6WALL10,25);
		DRLG_L3PlaceRndSet(L6WALL11,25);
		DRLG_L3PlaceRndSet(L6WALL12,25);
		DRLG_L3PlaceRndSet(L6WALL13,25);
		DRLG_L3PlaceRndSet(L6WALL14,25);
		DRLG_L3PlaceRndSet(L6CORNER1,25);
		DRLG_L3PlaceRndSet(L6CORNER2,25);
		DRLG_L3PlaceRndSet(L6CORNER3,25);
		DRLG_L3PlaceRndSet(L6CORNER4,25);

	}

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
	}

	extern void DRLG_Init_Globals();
	DRLG_Init_Globals();	
	
//	DRLG_CheckQuests(SP3x1,SP3y1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L3Pass3()
{
	int	i,j,xx,yy;
	long v1,v2,v3,v4,lv;

	// Init dungeon to dirt
	lv = L3_DIRT - 1;
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
		for (xx = 0; xx < DMAXX; xx+=2) {		//DMAXX = abyssx for abyss
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
			if (lv >= 0) {
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
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
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

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#if IS_VERSION(RETAIL)
void CreateL3Dungeon(unsigned int rseed, int entry)
{
	int		i,j;

	SetRndSeed(rseed);

	dminx = DIRTEDGED2;
	dminy = DIRTEDGED2;
	dmaxx = DMAXX - (DIRTEDGED2);
	dmaxy = DMAXY - (DIRTEDGED2);

	//DRLG_LoadL3SP();
	DRLG_InitTrans();		// initialize transparent values
	DRLG_InitSetPC();		// ? sets 4 pc values to 0
	DRLG_L3(entry);
	DRLG_L3Pass3();
	//DRLG_FreeL3SP();

	// Special lava lighting!!!	JKE
	if (currlevel < HIVESTART)
	{
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++) {
					 if ((dPiece[i][j] >= 56) && (dPiece[i][j] <= 147)) DoLighting (i, j, 7, -1);
				else if ((dPiece[i][j] >= 154) && (dPiece[i][j] <= 161)) DoLighting (i, j, 7, -1);
				else if (dPiece[i][j] == 150) DoLighting (i, j, 7, -1);
				else if (dPiece[i][j] == 152) DoLighting (i, j, 7, -1);
			}
		}
	}
	else
	{
		for (j = 0; j < DMAXY; j++) 
			for (i = 0; i < DMAXX; i++) 
					 if ((dPiece[i][j] >= 382) && (dPiece[i][j] <= 457))
						 DoLighting (i, j, 9, -1);
						 //dLight[i][j] = 1;
	} 	

	DRLG_SetPC();
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void LoadL3Dungeon(char sFileName[], int vx, int vy)
{
	int		i, j, rw, rh;
	byte	*pLevelMap, *lm;

	InitL3Dungeon();

	dminx = DIRTEDGED2;
	dminy = DIRTEDGED2;
	dmaxx = DMAXX - (DIRTEDGED2);
	dmaxy = DMAXY - (DIRTEDGED2);

	DRLG_InitTrans();

	pLevelMap = LoadFileInMemSig(sFileName,NULL,'LMPt');
	lm = pLevelMap;

	rw = *lm;
	lm+=2;
	rh = *lm;
	lm+=2;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
			} else dungeon[i][j] = FLOOR;
			lm+=2;
		}
	}

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++)
			if (dungeon[i][j] == 0) dungeon[i][j] = L3_DIRT;
	} 
	
	abyssx = DMAXX;
	DRLG_L3Pass3();

	extern void DRLG_Init_Globals();
	DRLG_Init_Globals();

	ViewX = 31;
	ViewY = 83;

	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);

	// Special lava lighting!!!
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
				 if ((dPiece[i][j] >= 56) && (dPiece[i][j] <= 147)) DoLighting (i, j, 7, -1);
			else if ((dPiece[i][j] >= 154) && (dPiece[i][j] <= 161)) DoLighting (i, j, 7, -1);
			else if (dPiece[i][j] == 150) DoLighting (i, j, 7, -1);
			else if (dPiece[i][j] == 152) DoLighting (i, j, 7, -1);
		}
	}

	DiabloFreePtr(pLevelMap);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void LoadPreL3Dungeon(char sFileName[], int vx, int vy)
{
	int		i, j, rw, rh;
	byte	*pLevelMap, *lm;

	InitL3Dungeon();

	DRLG_InitTrans();

	pLevelMap = LoadFileInMemSig(sFileName,NULL,'LMPt');
	lm = pLevelMap;

	rw = *lm;
	lm+=2;
	rh = *lm;
	lm+=2;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
			} else dungeon[i][j] = FLOOR;
			lm+=2;
		}
	}

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++)
			if (dungeon[i][j] == 0) dungeon[i][j] = L3_DIRT;
	} 

	CopyMemory(pdungeon,dungeon,sizeof(pdungeon));

	DiabloFreePtr(pLevelMap);
}
