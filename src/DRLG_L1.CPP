/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Dungeon file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L1.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
** CreateL1Dungeon
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"drlg_l1.h"
#include	"gendung.h"
#include	"engine.h"
#include	"trigs.h"
#include	"lighting.h"
#include	"monster.h"
#include	"objects.h"
#include	"quests.h"
#include	"multi.h"
#include	"Items.h"

// LEVEL 1

#define L1NGRATE	10		// cannot be higher than 100 or less than 0

#define L1BGRATE	 5
#define L1RFLOOR	20

#define L1BWALL1	30
#define L1BWALL2	15
#define L1BWALL3	 5

#define L1STATUE	10


// LEVEL 2

#define L2NGRATE	10		// cannot be higher than 100 or less than 0

#define L2BGRATE	10
#define L2RFLOOR	20

#define L2BWALL1	30
#define L2BWALL2	20
#define L2BWALL3	10

#define L2STATUE	10


// LEVEL 3

#define L3NGRATE	10		// cannot be higher than 100 or less than 0

#define L3BGRATE	15
#define L3RFLOOR	30

#define L3BWALL1	30
#define L3BWALL2	20
#define L3BWALL3	15

#define L3STATUE	10


// LEVEL 4

#define L4NGRATE	10		// cannot be higher than 100 or less than 0

#define L4BGRATE	20
#define L4RFLOOR	30

#define L4BWALL1	30
#define L4BWALL2	20
#define L4BWALL3	20

#define L4STATUE	10


Na_Krul_Struct Na_Krul;		// Info for Na-Kruls room JKE

/*-----------------------------------------------------------------------**
** Registration info
**-----------------------------------------------------------------------*/
#include "regconst.h"
char sgszRegSig2[REG_LEN] = "REGISTRATION_BLOCK";


/*-----------------------------------------------------------------------**
** File Variables
**-----------------------------------------------------------------------*/
static byte dflags[MDMAXX][MDMAXY];
static byte * pSetPiece;
static BOOL setloadflag;


/*-----------------------------------------------------------------------**
** Constant arrays
**-----------------------------------------------------------------------*/
/*
DPatsStruct	DPAT[NUMDPATS] = {	{ 8, 4, 1, 4, 0 },
								{ 8, 1, 1, 4, 0 },
								{ 4, 5, 0, 4, 0 },
								{ 4, 1, 1, 4, 0 },
								{ 2, 5, 1, 0, 0 },
								{ 2, 4, 1, 4, 0 },
								{ 1, 5, 1, 0, 0 },
								{ 1, 1, 1, 4, 0 },
								{ 15, 5, 1, 4, 0 } };
*/

static const ShadowStruct SPATS[NUMSPATS] = {
				{ 7, 13, 0, 13, _S6, 0, _S4 },
				{ 16, 13, 0, 13, _S6, 0, _S4 },
				{ 15, 13, 0, 13, _S7, 0, _S4 },
				{ 5, 13, 13, 13, _S14, _S2, _S1 },
				{ 5, 13, 1, 13, _S5, _S8, _S1 },
				{ 5, 13, 13, 2, _S5, _S2, _S10 },
				{ 5, 0, 1, 2, 0, _S8, _S10 },
				{ 5, 13, 11, 13, _S5, _S9, _S1 },
				{ 5, 13, 13, 12, _S5, _S2, _S11 },
				{ 5, 13, 11, 12, _S12, _S9, _S11 },
				{ 5, 13, 1, 12, _S5, _S8, _S11 },
				{ 5, 13, 11, 2, _S5, _S9, _S10 },
				{ 9, 13, 13, 13, _S6, _S2, _S4 },
				{ 9, 13, 1, 13, _S6, _S8, _S4 },
				{ 9, 13, 11, 13, _S13, _S9, _S4 },
				{ 8, 13, 0, 13, _S6, 0, _S1 },
				{ 8, 13, 0, 12, _S5, 0, _S11 },
				{ 8, 0, 0, 2, 0, 0, _S10 },
				{ 11, 0, 0, 13, 0, 0, _S1 },
				{ 11, 13, 0, 13, _S1, 0, _S1 },
				{ 11, 2, 0, 13, _S10, 0, _S1 },
				{ 11, 12, 0, 13, _S11, 0, _S1 },
				{ 11, 13, 11, 12, _S1, 0, _S11 },
				{ 14, 0, 0, 13, 0, 0, _S1 },
				{ 14, 13, 0, 13, _S1, 0, _S1 },
				{ 14, 2, 0, 13, _S10, 0, _S1 },
				{ 14, 12, 0, 13, _S11, 0, _S1 },
				{ 14, 13, 11, 12, _S1, 0, _S11 },
				{ 10, 0, 13, 0, 0, _S2, 0 },
				{ 10, 13, 13, 0, _S2, _S2, 0 },
				{ 10, 0, 1, 0, 0, _S8, 0 },
				{ 10, 13, 11, 0, _S2, _S9, 0 },
				{ 12, 0, 13, 0, 0, _S2, 0 },
				{ 12, 13, 13, 0, _S2, _S2, 0 },
				{ 12, 0, 1, 0, 0, _S8, 0 },
				{ 12, 13, 11, 0, _S2, _S9, 0 },
				{ 3, 13, 11, 12, _S12, 0, 0 } };

// Types for tile substitution
/*
static const byte BTYPES[NUMBLOCKS] = { 0,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,					// L1Base
		0, 0, 0, 0, 0, 0, 0,														// L1Dirt
		25, 26, 4, 28, 4, 30, 31, 6, 7, 41, 1, 2, 4, 10, 43, 40, 41, 42, 43, 14,	// L1Doors
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		// L1Big
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		79, 80, 0, 82, 0, 0, 0, 0, 0, 0, 79, 0, 80, 0, 0, 79, 80, 0,				// L1Plain
		2, 2, 2, 1, 1, 11, 25, 13, 13, 13,											// L1Blood
		1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2, 12, 0, 0, 11, 1, 11, 1,					// L1Misc
		13, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 13, 13,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// L1Shadow
		0, 0, 0, 0, 0, 0, 0, 0,														// L1New
		0, 0, 0, 0, 0, 0, 0, 0, 0,													// L1Massac
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,								// L1DrOpen
		0, 0, 0, 0, 0, 0, 0, 0,														// L1Secret
		0, 0, 0, 0, 0, 0, 0 };														// L1Dirt
*/

// Types for shadow pass
static const byte BSTYPES[NUMBLOCKS] = { 0,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,					// L1Base
		0, 0, 0, 0, 0, 0, 0,														// L1Dirt
		1, 2, 10, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 5, 14, 10, 4, 14, 4, 5,		// L1Doors
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		// L1Big
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 2, 3, 4, 1, 6, 7, 16, 17, 2, 1, 1, 2, 2, 1, 1, 2, 2,						// L1Plain
		2, 2, 2, 1, 1, 11, 1, 13, 13, 13,											// L1Blood
		1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2, 12, 0, 0, 11, 1, 11, 1,					// L1Misc
		13, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 13, 13,
		13, 13, 13, 13, 13, 13, 13, 1, 11, 2, 12, 13, 13, 13, 12, 2, 1, 2, 2,		// L1Shadow
		4, 14, 4, 10, 13, 13, 4, 4,													// L1New
		1, 1, 4, 2, 2, 13, 13, 13, 13,												// L1Massac
		25, 26, 28, 30, 31, 41, 43, 40, 41, 42, 43, 25, 41, 43, 28, 28,				// L1DrOpen
		1, 2, 25, 26, 22, 22, 25, 26,												// L1Secret
		0, 0, 0, 0, 0, 0, 0 };														// L1Dirt

// Types for tile substitution
static const byte L5BTYPES[NUMBLOCKS] = { 0,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,					// L1Base
		0, 0, 0, 0, 0, 0, 0,														// L1Dirt
		25, 26, 0, 28, 0, 30, 31, 0, 0, 0, 0, 0, 0, 0, 0, 40, 41, 42, 43, 0,		// L1Doors
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		// L1Big
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		79, 80, 0, 82, 0, 0, 0, 0, 0, 0, 79, 0, 80, 0, 0, 79, 80, 0,				// L1Plain
		2, 2, 2, 1, 1, 11, 25, 13, 13, 13,											// L1Blood
		1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2, 12, 0, 0, 11, 1, 11, 1,					// L1Misc
		13, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 13, 13,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// L1Shadow
		0, 0, 0, 0, 0, 0, 0, 0,														// L1New
		0, 0, 0, 0, 0, 0, 0, 0, 0,													// L1Massac
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,								// L1DrOpen
		0, 0, 0, 0, 0, 0, 0, 0,														// L1Secret
		0, 0, 0, 0, 0, 0, 0 };														// L1Dirt

/*-----------------------------------------------------------------------**
** Manditory mini set pieces
**-----------------------------------------------------------------------*/
static const byte STAIRSUP[] = { 4, 4,									// X size, Y size

					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,		// Pattern to look for
					D_HWALL, D_HWALL, D_HWALL, D_HWALL,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,

					0, 66, 6, 0,							// Pattern to sub
					63, 64, 65, 0,
					0, 67, 68, 0,
					0, 0, 0, 0 };

static const byte L5STAIRSUP[] = { 4, 5,									// X size, Y size

					D_DIRT,  D_DIRT,  D_DIRT,  D_DIRT,		// Pattern to look for
					D_DIRT,  D_DIRT,  D_DIRT,  D_DIRT,
					D_HWALL, D_HWALL, D_HWALL, D_HWALL,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,

					 0, 54, 23, 0,
					 0, 53, 18, 0,							// Pattern to sub
					55, 56, 57, 0,
					58, 59, 60, 0,
					0, 0, 0, 0 };

static const byte STAIRSDOWN[] = { 4, 3,									// X size, Y size

					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,		// Pattern to look for
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,

					62, 57, 58, 0,							// Pattern to sub
					61, 59, 60, 0,
					0, 0, 0, 0 };

static const byte L5STAIRSDOWN[] = { 4, 5,

					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,		// Pattern to look for
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,

					 0,  0, 52, 0,							// Pattern to sub
					 0, 48, 51, 0,
					 0, 47, 50, 0,
					45, 46, 49, 0,
					 0,  0,  0, 0 };



// Temp borrow to get this to work. JKE
static const byte WARPSTAIRS[] = { 4, 5,									// X size, Y size

					D_DIRT,  D_DIRT,  D_DIRT,  D_DIRT,		// Pattern to look for
					D_DIRT,  D_DIRT,  D_DIRT,  D_DIRT,
					D_HWALL, D_HWALL, D_HWALL, D_HWALL,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR, D_FLOOR,

					 0, 62, 23, 0,
					 0, 61, 18, 0,							// Pattern to sub
					63, 64, 65, 0,
					66, 67, 68, 0,
					0, 0, 0, 0 };

static const byte LAMPS[] = 		{ 2, 2,									// X size, Y size

					D_FLOOR, 0,								// Pattern to look for
					D_FLOOR, D_FLOOR,

					129, 0,									// Pattern to sub
					130, 128 };


static const byte PWATERIN[] = 	{ 6, 6,													// X size, Y size

					D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR, D_FLOOR,			// Pattern to look for
					D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR, D_FLOOR,			// Pattern to look for
					D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR, D_FLOOR,			// Pattern to look for
					D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR, D_FLOOR,			// Pattern to look for
					D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR, D_FLOOR,			// Pattern to look for
					D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR,  D_FLOOR, D_FLOOR,			// Pattern to look for

					    0,   0,   0,   0,   0,   0,										// Pattern to sub
					    0, 202, 200, 200,  84,   0,										// Pattern to sub
					    0, 199, 203, 203,  83,   0,										// Pattern to sub
					    0,  85, 206,  80,  81,   0,										// Pattern to sub
					    0,   0, 134, 135,   0,   0,										// Pattern to sub
					    0,   0,   0,   0,   0,   0};									// Pattern to sub


//JKE Enter new swap tiles here then call placerndset()
/*
static const byte TEMPLATE[] = { 3, 3,													// X size, Y size

					D_FLOOR, D_FLOOR, D_FLOOR,										// Pattern to look for
					D_FLOOR, D_FLOOR, D_FLOOR,
					D_FLOOR, D_FLOOR, D_FLOOR,

					0, 0, 0,														// Pattern to sub
					0, 0, 0,
					0, 0, 0};
*/

// NORMAL GRATE SUBS
static const byte ARCHLEFT[] = { 1, 1,													// X size, Y size

					D_AVW,															// Pattern to look for

					95};																	// Pattern to sub

static const byte ARCHRIGHT[] = { 1, 1,													// X size, Y size

					D_AHW,															// Pattern to look for

					96};																	// Pattern to sub

static const byte GRATELEFT[] = { 1, 3,													// X size, Y size

					D_VWALL,															// Pattern to look for
					D_VWALL,
					D_VWALL,

					91,																	// Pattern to sub
					90,
					89};

static const byte GRATERIGHT[] = { 3, 1,													// X size, Y size

					D_HWALL,D_HWALL,D_HWALL,											// Pattern to look for

					94,93,92};																	// Pattern to sub

static const byte FLOOR1[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					97};																	// Pattern to sub

static const byte FLOOR2[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					98};																	// Pattern to sub

static const byte FLOOR3[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					99};																	// Pattern to sub

static const byte FLOOR4[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					100};																	// Pattern to sub

static const byte BIGFLOOR[] = { 3, 3,													// X size, Y size

					D_FLOOR,D_FLOOR,D_FLOOR,												// Pattern to look for
					D_FLOOR,D_FLOOR,D_FLOOR,
					D_FLOOR,D_FLOOR,D_FLOOR,

					0,  0,  0,																	// Pattern to sub
					0,101,  0,
					0,  0,  0};


// BROKEN GRATE SUBS
static const byte ARCHLEFT1A[] = { 1, 1,													// X size, Y size

					D_AVW,															// Pattern to look for

					185};																	// Pattern to sub

static const byte ARCHLEFT1B[] = { 1, 1,													// X size, Y size

					D_AVW,															// Pattern to look for

					186};																	// Pattern to sub

static const byte ARCHRIGHT1A[] = { 1, 1,													// X size, Y size

					D_AHW,															// Pattern to look for

					187};																	// Pattern to sub

static const byte ARCHRIGHT1B[] = { 1, 1,													// X size, Y size

					D_AHW,															// Pattern to look for

					188};																	// Pattern to sub

static const byte GRATELEFT1BA[] = { 1, 1,													// X size, Y size

					89,															// Pattern to look for

					173};																	// Pattern to sub

static const byte GRATELEFT1BB[] = { 1, 1,													// X size, Y size

					89,															// Pattern to look for

					174};																	// Pattern to sub

static const byte GRATELEFT1MA[] = { 1, 1,													// X size, Y size

					90,															// Pattern to look for

					175};																	// Pattern to sub

static const byte GRATELEFT1MB[] = { 1, 1,													// X size, Y size

					90,															// Pattern to look for

					176};																	// Pattern to sub

static const byte GRATELEFT1TA[] = { 1, 1,													// X size, Y size

					91,															// Pattern to look for

					177};																	// Pattern to sub

static const byte GRATELEFT1TB[] = { 1, 1,													// X size, Y size

					91,															// Pattern to look for

					178};																	// Pattern to sub

static const byte GRATERIGHT1RA[] = { 1, 1,													// X size, Y size

					92,														// Pattern to look for

					179};																	// Pattern to sub

static const byte GRATERIGHT1RB[] = { 1, 1,													// X size, Y size

					92,														// Pattern to look for

					180};																	// Pattern to sub

static const byte GRATERIGHT1MA[] = { 1, 1,													// X size, Y size

					92,														// Pattern to look for

					181};																	// Pattern to sub

static const byte GRATERIGHT1MB[] = { 1, 1,													// X size, Y size

					92,														// Pattern to look for

					182};																	// Pattern to sub

static const byte GRATERIGHT1LA[] = { 1, 1,													// X size, Y size

					92,														// Pattern to look for

					183};																	// Pattern to sub

static const byte GRATERIGHT1LB[] = { 1, 1,													// X size, Y size

					92,														// Pattern to look for

					184};																	// Pattern to sub

static const byte FLOOR1B1[] = { 1, 1,													// X size, Y size

					98,															// Pattern to look for

					189};																	// Pattern to sub

static const byte FLOOR2B1[] = { 1, 1,													// X size, Y size

					98,															// Pattern to look for

					190};																	// Pattern to sub

static const byte FLOOR3B1[] = { 1, 1,													// X size, Y size

					97,															// Pattern to look for

					191};																	// Pattern to sub

static const byte FLOOR4B1[] = { 1, 1,													// X size, Y size

					D_COL,															// Pattern to look for

					192};																	// Pattern to sub

static const byte FLOOR5B1[] = { 1, 1,													// X size, Y size

					99,															// Pattern to look for

					193};																	// Pattern to sub

static const byte FLOOR6B1[] = { 1, 1,													// X size, Y size

					99,															// Pattern to look for

					194};																	// Pattern to sub

static const byte FLOOR7B1[] = { 1, 1,													// X size, Y size

					100,															// Pattern to look for

					195};																	// Pattern to sub

static const byte BIGFLOORB1[] = { 1, 1,													// X size, Y size

					101,														// Pattern to look for

					196};

static const byte BIGFLOORB2[] = { 1, 1,													// X size, Y size

					101,

					197};

static const byte BIGFLOORB3[] = { 1, 1,													// X size, Y size

					101,													// Pattern to look for

					198};



// GENERAL FLOOR RUBBLE

static const byte BIGFLOORR1[] = { 3, 3,													// X size, Y size

					D_FLOOR,D_FLOOR,D_FLOOR,												// Pattern to look for
					D_FLOOR,D_FLOOR,D_FLOOR,
					D_FLOOR,D_FLOOR,D_FLOOR,

					0,  0,  0,																	// Pattern to sub
					0,167,  0,
					0,  0,  0};

static const byte BIGFLOORR2[] = { 3, 3,													// X size, Y size

					D_FLOOR,D_FLOOR,D_FLOOR,												// Pattern to look for
					D_FLOOR,D_FLOOR,D_FLOOR,
					D_FLOOR,D_FLOOR,D_FLOOR,

					0,  0,  0,																	// Pattern to sub
					0,168,  0,
					0,  0,  0};

static const byte BIGFLOORR3[] = { 3, 3,													// X size, Y size

					D_FLOOR,D_FLOOR,D_FLOOR,												// Pattern to look for
					D_FLOOR,D_FLOOR,D_FLOOR,
					D_FLOOR,D_FLOOR,D_FLOOR,

					0,  0,  0,																	// Pattern to sub
					0,169,  0,
					0,  0,  0};

static const byte BIGFLOORR4[] = { 3, 3,													// X size, Y size

					D_FLOOR,D_FLOOR,D_FLOOR,												// Pattern to look for
					D_FLOOR,D_FLOOR,D_FLOOR,
					D_FLOOR,D_FLOOR,D_FLOOR,

					0,  0,  0,																	// Pattern to sub
					0,170,  0,
					0,  0,  0};

static const byte BIGFLOORR5[] = { 3, 3,													// X size, Y size

					D_FLOOR,D_FLOOR,D_FLOOR,												// Pattern to look for
					D_FLOOR,D_FLOOR,D_FLOOR,
					D_FLOOR,D_FLOOR,D_FLOOR,

					0,  0,  0,																	// Pattern to sub
					0,171,  0,
					0,  0,  0};

static const byte BIGFLOORR6[] = { 3, 3,													// X size, Y size

					D_FLOOR,D_FLOOR,D_FLOOR,												// Pattern to look for
					D_FLOOR,D_FLOOR,D_FLOOR,
					D_FLOOR,D_FLOOR,D_FLOOR,

					0,  0,  0,																	// Pattern to sub
					0,172,  0,
					0,  0,  0};

static const byte FLOORR1[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					163};																	// Pattern to sub

static const byte FLOORR2[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					164};																	// Pattern to sub

static const byte FLOORR3[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					165};																	// Pattern to sub

static const byte FLOORR4[] = { 1, 1,													// X size, Y size

					D_FLOOR,															// Pattern to look for

					166};																	// Pattern to sub


// BROKEN WALL 1
static const byte BROKEN_VWALL1[] = { 1, 1,

					D_VWALL,

					112};

static const byte BROKEN_HWALL1[] = { 1, 1,

					D_HWALL,

					113};

static const byte BROKEN_LRC1[] = { 1, 1,

					D_LRC,

					114};

static const byte BROKEN_ULC1[] = { 1, 1,

					D_ULC,

					115};

static const byte BROKEN_AULC1[] = { 1, 1,

					D_AULC,

					116};

static const byte BROKEN_URC1[] = { 1, 1,

					D_URC,

					117};

static const byte BROKEN_LLC1[] = { 1, 1,

					D_LLC,

					118};

static const byte BROKEN_AURC1[] = { 1, 1,

					D_AURC,

					119};

static const byte BROKEN_ALLC1[] = { 1, 1,

					D_ALLC,

					120};

static const byte BROKEN_TULC11[] = { 1, 1,

					D_TULC1,

					121};

static const byte BROKEN_AVW1[] = { 1, 1,

					D_AVW,

					122};

static const byte BROKEN_AHW1[] = { 1, 1,

					D_AHW,

					123};

static const byte BROKEN_FLOOR1[] = { 1, 1,

					D_FLOOR,

					124};

static const byte BROKEN_TULC21[] = { 1, 1,

					D_TULC2,

					125};

static const byte BROKEN_COL1[] = { 1, 1,

					D_COL,
					
					126};

static const byte BROKEN_BCAP1[] = { 1, 1,

					D_BCAP,

					127};

static const byte BROKEN_RCAP1[] = { 1, 1,

					D_RCAP,

					128};

// BROKEN WALL 2
static const byte BROKEN_VWALL2[] = { 1, 1,

					D_VWALL,

					129};

static const byte BROKEN_HWALL2[] = { 1, 1,

					D_HWALL,

					130};

static const byte BROKEN_LRC2[] = { 1, 1,

					D_LRC,

					131};

static const byte BROKEN_ULC2[] = { 1, 1,

					D_ULC,

					132};

static const byte BROKEN_AULC2[] = { 1, 1,

					D_AULC,

					133};

static const byte BROKEN_URC2[] = { 1, 1,

					D_URC,

					134};

static const byte BROKEN_LLC2[] = { 1, 1,

					D_LLC,

					135};

static const byte BROKEN_AURC2[] = { 1, 1,

					D_AURC,

					136};

static const byte BROKEN_ALLC2[] = { 1, 1,

					D_ALLC,

					137};

static const byte BROKEN_TULC12[] = { 1, 1,

					D_TULC1,

					138};

static const byte BROKEN_AVW2[] = { 1, 1,

					D_AVW,

					139};

static const byte BROKEN_AHW2[] = { 1, 1,

					D_AHW,

					140};

static const byte BROKEN_FLOOR2[] = { 1, 1,

					D_FLOOR,

					141};

static const byte BROKEN_TULC22[] = { 1, 1,

					D_TULC2,

					142};

static const byte BROKEN_COL2[] = { 1, 1,

					D_COL,
					
					143};

static const byte BROKEN_BCAP2[] = { 1, 1,

					D_BCAP,

					144};

static const byte BROKEN_RCAP2[] = { 1, 1,

					D_RCAP,

					145};

// BROKEN WALL 3
static const byte BROKEN_VWALL3[] = { 1, 1,

					D_VWALL,

					146};

static const byte BROKEN_HWALL3[] = { 1, 1,

					D_HWALL,

					147};

static const byte BROKEN_LRC3[] = { 1, 1,

					D_LRC,

					148};

static const byte BROKEN_ULC3[] = { 1, 1,

					D_ULC,

					149};

static const byte BROKEN_AULC3[] = { 1, 1,

					D_AULC,

					150};

static const byte BROKEN_URC3[] = { 1, 1,

					D_URC,

					151};

static const byte BROKEN_LLC3[] = { 1, 1,

					D_LLC,

					152};

static const byte BROKEN_AURC3[] = { 1, 1,

					D_AURC,

					153};

static const byte BROKEN_ALLC3[] = { 1, 1,

					D_ALLC,

					154};

static const byte BROKEN_TULC13[] = { 1, 1,

					D_TULC1,

					155};

static const byte BROKEN_AVW3[] = { 1, 1,

					D_AVW,

					156};

static const byte BROKEN_AHW3[] = { 1, 1,

					D_AHW,

					157};

static const byte BROKEN_FLOOR3[] = { 1, 1,

					D_FLOOR,

					158};

static const byte BROKEN_TULC23[] = { 1, 1,

					D_TULC2,

					159};

static const byte BROKEN_COL3[] = { 1, 1,

					D_COL,
					
					160};

static const byte BROKEN_BCAP3[] = { 1, 1,

					D_BCAP,

					161};

static const byte BROKEN_RCAP3[] = { 1, 1,

					D_RCAP,

					162};

// STATUES

static const byte STATUE_LEFTA[] = { 1, 1,

					D_VWALL,

					199};

static const byte STATUE_LEFTB[] = { 1, 1,

					D_VWALL,

					201};

static const byte STATUE_RIGHTA[] = { 1, 1,

					D_HWALL,

					200};

static const byte STATUE_RIGHTB[] = { 1, 1,

					D_HWALL,

					202};


// QUEST ROOMS
static byte NA_KRULS_ROOM[] = { 4, 6,					// Dimensions of the room

					115,130,  6,D_FLOOR,						// Define the room
					129,108,  1,D_FLOOR,
					  1,107,103,D_FLOOR,
					146,106,102,D_FLOOR,
					129,168,  1,D_FLOOR,
					  7,  2,  3,D_FLOOR};

static byte CORNERSTONE[] = { 5, 5,

					  4,  2,  2,  2, 6,						// Define the room
					  1,111,172,  0, 1,
					  1,172,  0,  0,25,
					  1,  0,  0,  0, 1,
					  7,  2,  2,  2, 3};
					
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
/*
static void DRLG_CheckDoor(int x, int y)
{
	byte	mf, c;

	mf = 0;
	if ((dflags[x][y] & HDOOR) == HDOOR) {
		c = dungeon[x+1][y];
		if (c == D_VWALL) mf = 1;
		if (c == D_ULC) mf = 1;
		if (c == D_URC) mf = 1;
		if (c == D_TULC1) mf = 1;
		if (c == D_DH) mf = 1;
		if (c == D_DURC) mf = 1;
		if (mf == 1) {
			dflags[x][y] ^= HDOOR;
			dflags[x-1][y] |= HDOOR;
		}
	}
	if ((dflags[x][y] & VDOOR) == VDOOR) {
		c = dungeon[x][y+1];
		if (c == D_HWALL) mf = 1;
		if (c == D_ULC) mf = 1;
		if (c == D_LLC) mf = 1;
		if (c == D_TULC2) mf = 1;
		if (c == D_DV) mf = 1;
		if (c == D_DLLC) mf = 1;
		if (mf == 1) {
			dflags[x][y] ^= VDOOR;
			dflags[x][y-1] |= VDOOR;
		}
	}
}
*/

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_PlaceDoor(int x, int y)
{
	byte	c, df;

	if ((dflags[x][y] & SETP_BIT) == 0) {
		//dungeon[x][y] = D_COL;
		//dflags[x][y] = SETP_BIT;
		//return;
		df = dflags[x][y] & SETP_MASK;
		c = dungeon[x][y];
		if (df == HDOOR) {
			if ((y != 1) && (c == D_HWALL)) dungeon[x][y] = D_DRH;
			if ((y != 1) && (c == D_LLC)) dungeon[x][y] = D_DRLLC;
			if ((y != 1) && (c == D_TULC2)) dungeon[x][y] = D_DRHT2;
			if ((y != 1) && (c == D_ULC)) dungeon[x][y] = D_DRHULC;
			if ((x != 1) && (c == D_VWALL)) dungeon[x][y] = D_DRV;
			if ((x != 1) && (c == D_TULC1)) dungeon[x][y] = D_DRVT1;
			if ((x != 1) && (c == D_URC)) dungeon[x][y] = D_DRURC;
		}
		if (df == VDOOR) {
			if ((x != 1) && (c == D_VWALL)) dungeon[x][y] = D_DRV;
			if ((x != 1) && (c == D_URC)) dungeon[x][y] = D_DRURC;
			if ((x != 1) && (c == D_TULC1)) dungeon[x][y] = D_DRVT1;
			if ((x != 1) && (c == D_ULC)) dungeon[x][y] = D_DRVULC;
			if ((y != 1) && (c == D_HWALL)) dungeon[x][y] = D_DRH;
			if ((y != 1) && (c == D_TULC2)) dungeon[x][y] = D_DRHT2;
			if ((y != 1) && (c == D_LLC)) dungeon[x][y] = D_DRLLC;
		}
		if (df == DDOOR) {
			if ((x != 1) && (y != 1) && (c == D_ULC)) dungeon[x][y] = D_DDULC;
			if ((x != 1) && (c == D_TULC1)) dungeon[x][y] = D_DRVT1;
			if ((y != 1) && (c == D_TULC2)) dungeon[x][y] = D_DRHT2;
			if ((y != 1) && (c == D_HWALL)) dungeon[x][y] = D_DRH;
			if ((x != 1) && (c == D_VWALL)) dungeon[x][y] = D_DRV;
			if ((y != 1) && (c == D_LLC)) dungeon[x][y] = D_DRLLC;
			if ((x != 1) && (c == D_URC)) dungeon[x][y] = D_DRURC;
		}
	}
	dflags[x][y] = SETP_BIT;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
/*
static int DRLG_SubInitDoor(int v1, int v2)
{
	int		rv;

	rv = random(0, 3);
	if (rv == 0) rv = v1;
	else if (rv == 1) rv = v2;
	else if (rv == 2) rv = v1 | v2;
	return(rv);
}
*/
static void DRLG_L5Shadows()
{
	int		x,y;

	for (y = 1; y < MDMAXY; y++) 
	{
		for (x = 1; x < MDMAXX; x++) 
		{
			switch (dungeon[x][y])
			{
				case 5:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;
		
				case 7:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;
			
				case 8:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;
			
				case 9:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 10:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 11:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 12:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 14:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 15:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 17:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 95:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 96:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 208;
					break;

				case 116:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 118:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 119:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 120:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 121:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 122:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 211;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 212;
					break;

				case 123:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 125:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 126:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 128:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 133:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 135:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 136:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 137:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 213;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 214;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 138:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 139:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 215;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 216;
					break;

				case 140:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 217;
					break;

				case 142:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 143:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 213;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 214;
					break;

				case 145:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 213;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 214;
					break;

				case 150:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 217;
					break;

				case 152:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 153:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 154:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 155:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 205;
					break;

				case 156:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 157:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 217;
					break;

				case 159:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 160:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 206;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 207;
					break;

				case 162:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 209;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 210;
					break;

				case 167:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 209;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 210;
					break;

				case 187:
					if (dungeon[x][y-1] == D_FLOOR)
						dungeon[x][y-1] = 208;
					break;

				case 185:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 186:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 203;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 204;
					break;

				case 192:
					if (dungeon[x-1][y] == D_FLOOR)
						dungeon[x-1][y] = 209;
					if (dungeon[x-1][y-1] == D_FLOOR)
						dungeon[x-1][y-1] = 210;
					break;
					
			
			}
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L1Shadows()
{
	int		x,y,i,patflag;
	byte	sd[2][2];
	byte	tnv3;

	for (y = 1; y < MDMAXY; y++) {
		for (x = 1; x < MDMAXX; x++) {
			if ((x == 60) && (y == 21))
				patflag = 1;
			sd[0][0] = BSTYPES[dungeon[x][y]];
			sd[1][0] = BSTYPES[dungeon[x-1][y]];
			sd[0][1] = BSTYPES[dungeon[x][y-1]];
			sd[1][1] = BSTYPES[dungeon[x-1][y-1]];
			for (i = 0; i < NUMSPATS; i++) {
				if (SPATS[i].strig == sd[0][0]) {
					patflag = 1;
					if ((SPATS[i].s1 != 0) && (SPATS[i].s1 != sd[1][1])) patflag = 0;
					if ((SPATS[i].s2 != 0) && (SPATS[i].s2 != sd[0][1])) patflag = 0;
					if ((SPATS[i].s3 != 0) && (SPATS[i].s3 != sd[1][0])) patflag = 0;
					if (patflag == 1) {
						if ((SPATS[i].nv1 != 0) && (dflags[x-1][y-1] == 0)) dungeon[x-1][y-1] = SPATS[i].nv1;
						if ((SPATS[i].nv2 != 0) && (dflags[x][y-1] == 0)) dungeon[x][y-1] = SPATS[i].nv2;
						if ((SPATS[i].nv3 != 0) && (dflags[x-1][y] == 0)) dungeon[x-1][y] = SPATS[i].nv3;
					}
				}

			}
		}
	}

	// Fix grates
	for (y = 1; y < MDMAXY; y++) {
		for (x = 1; x < MDMAXX; x++) {
			if ((dungeon[x-1][y] == _S1) && (dflags[x-1][y] == 0)) {
				tnv3 = _S1;
				if (dungeon[x][y] == 29) tnv3 = _S3;
				if (dungeon[x][y] == 32) tnv3 = _S3;
				if (dungeon[x][y] == 35) tnv3 = _S3;
				if (dungeon[x][y] == 37) tnv3 = _S3;
				if (dungeon[x][y] == 38) tnv3 = _S3;
				if (dungeon[x][y] == 39) tnv3 = _S3;
				dungeon[x-1][y] = tnv3;
			}
			if ((dungeon[x-1][y] == _S11) && (dflags[x-1][y] == 0)) {
				tnv3 = _S11;
				if (dungeon[x][y] == 29) tnv3 = _S15;
				if (dungeon[x][y] == 32) tnv3 = _S15;
				if (dungeon[x][y] == 35) tnv3 = _S15;
				if (dungeon[x][y] == 37) tnv3 = _S15;
				if (dungeon[x][y] == 38) tnv3 = _S15;
				if (dungeon[x][y] == 39) tnv3 = _S15;
				dungeon[x-1][y] = tnv3;
			}
			if ((dungeon[x-1][y] == _S10) && (dflags[x-1][y] == 0)) {
				tnv3 = _S10;
				if (dungeon[x][y] == 29) tnv3 = _S16;
				if (dungeon[x][y] == 32) tnv3 = _S16;
				if (dungeon[x][y] == 35) tnv3 = _S16;
				if (dungeon[x][y] == 37) tnv3 = _S16;
				if (dungeon[x][y] == 38) tnv3 = _S16;
				if (dungeon[x][y] == 39) tnv3 = _S16;
				dungeon[x-1][y] = tnv3;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int DRLG_PlaceMiniSet(const byte miniset[], int tmin, int tmax, int cx, int cy, int setview, int noquad, int ldir)
{
	int sx, sy;
	int	sw, sh;
	int	xx, yy;
	int	i, ii, numt;
	int found;
	int abort;

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
		abort = 0;
		while (found == 0) {
			found = 1;
			if ((cx != -1) && (sx >= (cx - sw)) && (sx <= (cx + 12))) {
				sx++;
				//sx = random(0, MDMAXX-sw);
				//sy = random(0, MDMAXY-sh);
				found = 0;
			}
			if ((cy != -1) && (sy >= (cy - sh)) && (sy <= (cy + 12))) {
				sy++;
				//sx = random(0, MDMAXX-sw);
				//sy = random(0, MDMAXY-sh);
				found = 0;
			}
			switch (noquad) {
				case 0 :
					if ((sx < cx) && (sy < cy)) found = 0;
					break;
				case 1:
					if ((sx > cx) && (sy < cy)) found = 0;
					break;
				case 2:
					if ((sx < cx) && (sy > cy)) found = 0;
					break;
				case 3:
					if ((sx > cx) && (sy > cy)) found = 0;
					break;
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
				abort++;
				if (abort > 4000) return(-1);
			}
		}

		// Place mini set piece
		ii = (sh * sw) + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) dungeon[sx+xx][sy+yy] = miniset[ii];
				ii++;
			}
		}
	}

	if (miniset == PWATERIN) {
		i = TransVal;
		TransVal = 0;
		DRLG_MRectTrans(sx+0, sy+2, sx+5, sy+4);
		TransVal = i;
		quests[Q_PWATER]._qtx = (sx << 1) + 5 + DIRTEDGED2;
		quests[Q_PWATER]._qty = (sy << 1) + 6 + DIRTEDGED2;
	}

	if (setview == 1) {
		ViewX = (sx << 1) + 3 + (DIRTEDGED2);
		ViewY = (sy << 1) + 4 + (DIRTEDGED2);
	}

	if (ldir == LVL_DOWN) {
		LvlViewX = (sx << 1) + 3 + (DIRTEDGED2);
		LvlViewY = (sy << 1) + 4 + (DIRTEDGED2);
	}

	if ((sx < cx) && (sy < cy)) return(0);
	if ((sx > cx) && (sy < cy)) return(1);
	if ((sx < cx) && (sy > cy)) return(2);
	return(3);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
/*
static void DRLG_PlaceRndSet(byte miniset[], int rndper)
{
	int sx, sy;
	int	sw, sh;
	int	xx, yy;
	int	ii;
	int found;

	// Width and height of the mini set piece
	sw = miniset[0];
	sh = miniset[1];

	// Find a location for the mini set piece
	for (sy = 0; sy < (MDMAXY - sh); sy++) {
		for (sx = 0; sx < (MDMAXX - sw); sx++) {
			found = 1;
			ii = 2;
			for (yy = 0; ((yy < sh) && (found == 1)); yy++) {
				for (xx = 0; ((xx < sw) && (found == 1)); xx++) {
					if ((miniset[ii] != 0) && (dungeon[sx+xx][sy+yy] != miniset[ii])) found = 0;
					if (dflags[sx+xx][sy+yy] != 0) found = 0;
					ii++;
				}
			}
			if ((found == 1) && (random(0, 100) < rndper)) {
				// Place mini set piece
				ii = (sh * sw) + 2;
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
						if (miniset[ii] != 0) dungeon[sx+xx][sy+yy] = miniset[ii];
						ii++;
					}
				}
			}
		}
	}
}
*/

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L1Floor()
{
	int		i,j;
	long	rv;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dflags[i][j] == 0) && (dungeon[i][j] == D_FLOOR)) {
				rv = random(0, 3);
				if (rv == 1) dungeon[i][j] = 162;
				if (rv == 2) dungeon[i][j] = 163;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L1Pass3()
{
	int	i,j,xx,yy;
	long v1,v2,v3,v4,lv;

	// Init dungeon to dirt
	lv = D_DIRT - 1;
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
			app_assert(lv >= 0); 
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

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_LoadL1SP()
{
	setloadflag = FALSE;
	if (QuestStatus(Q_BUTCHER)) {
		pSetPiece = LoadFileInMemSig("Levels\\L1Data\\rnd6.DUN",NULL,'STPC');
		setloadflag = TRUE;
	}		
	if (QuestStatus(Q_SKELKING) && (gbMaxPlayers == 1)) {
		pSetPiece = LoadFileInMemSig("Levels\\L1Data\\SKngDO.DUN",NULL,'STPC');
		setloadflag = TRUE;
	}
	if (QuestStatus(Q_LTBANNER)) {
		pSetPiece = LoadFileInMemSig("Levels\\L1Data\\Banner2.DUN",NULL,'STPC');
		setloadflag = TRUE;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_FreeL1SP() {
	DiabloFreePtr(pSetPiece);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DRLG_Init_Globals() {
	ZeroMemory(dFlags,sizeof(dFlags));
	ZeroMemory(dPlayer,sizeof(dPlayer));
	ZeroMemory(dMonster,sizeof(dMonster));
	ZeroMemory(dDead,sizeof(dDead));
	ZeroMemory(dObject,sizeof(dObject));
	ZeroMemory(dItem,sizeof(dItem));
	ZeroMemory(dMissile,sizeof(dMissile));
	ZeroMemory(dSpecial,sizeof(dSpecial));

	// Init the light values for each piece
	char cLight;
	if (lightflag == 0) {
		if (light4flag) cLight = 3;
		else cLight = 15;
	}
	else {
		cLight = 0;
	}
	FillMemory(dLight,sizeof(dLight),cLight);
}


static void DRLG_L5Doors()		//JKE
{

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			// place tops of arches
			int nPiece = dPiece[i][j];
				 if (nPiece == 77)  nPiece = 1;
			else if (nPiece == 80)  nPiece = 2;
			else continue;

			dSpecial[i][j] = nPiece;
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_InitL1Vals() {

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			// place tops of arches
			int nPiece = dPiece[i][j];
				 if (nPiece == 12)  nPiece = 1;
			else if (nPiece == 11)  nPiece = 2;
			else if (nPiece == 71)  nPiece = 1;
			else if (nPiece == 259) nPiece = 5;
			else if (nPiece == 249) nPiece = 2;
			else if (nPiece == 325) nPiece = 2;
			else if (nPiece == 321) nPiece = 1;
			else if (nPiece == 255) nPiece = 4;
			else if (nPiece == 211) nPiece = 1;
			else if (nPiece == 344) nPiece = 2;
			else if (nPiece == 341) nPiece = 1;
			else if (nPiece == 331) nPiece = 2;
			else if (nPiece == 418) nPiece = 1;
			else if (nPiece == 421) nPiece = 2;
			else continue;

			dSpecial[i][j] = nPiece;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void LoadL1Dungeon(char sFileName[], int vx, int vy)
{
	int		i,j,rw,rh;
	byte	*pLevelMap,*lm;

	dminx = DIRTEDGED2;
	dminy = DIRTEDGED2;
	dmaxx = DMAXX - (DIRTEDGED2);
	dmaxy = DMAXY - (DIRTEDGED2);

	DRLG_InitTrans();

	// Load map
	pLevelMap = LoadFileInMemSig(sFileName,NULL,'LMPt');
	lm = pLevelMap;

	// Fill with dirt
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			dungeon[i][j] = D_DIRT;
			dflags[i][j] = NODOOR;
		}
	}

	// Put map in dungeon
	rw = *lm;

	lm+=2;
	rh = *lm;
	lm+=2;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
				dflags[i][j] |= SETP_BIT;	// Don't go changin'
			} else dungeon[i][j] = D_FLOOR;
			lm+=2;
		}
	}

	// Floor subs
	DRLG_L1Floor();

	ViewX = vx;
	ViewY = vy;

	// Convert to minis
	DRLG_L1Pass3();

	DRLG_Init_Globals();
	if (currlevel < HIVESTART)			// JKE NO TOPS!!!
		DRLG_InitL1Vals();

	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);
	//SetMapItems(pLevelMap);

	// Free map
	DiabloFreePtr(pLevelMap);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void LoadPreL1Dungeon(char sFileName[], int vx, int vy)
{
	int		i,j,rw,rh;
	byte	*pLevelMap,*lm;

	dminx = DIRTEDGED2;
	dminy = DIRTEDGED2;
	dmaxx = DMAXX - (DIRTEDGED2);
	dmaxy = DMAXY - (DIRTEDGED2);

	// Load map
	pLevelMap = LoadFileInMemSig(sFileName,NULL,'LMPt');
	lm = pLevelMap;

	// Fill with dirt
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			dungeon[i][j] = D_DIRT;
			dflags[i][j] = NODOOR;
		}
	}

	// Put map in dungeon
	rw = *lm;
	lm+=2;
	rh = *lm;
	lm+=2;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
				dflags[i][j] |= SETP_BIT;	// Don't go changin'
			} else dungeon[i][j] = D_FLOOR;
			lm+=2;
		}
	}

	// Floor subs
	DRLG_L1Floor();

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
	}

	// Free map
	DiabloFreePtr(pLevelMap);
}

/*------------------------------------------------------------------------*
**
**								DRLG (5)
**                            drlg1 - test
**
*-------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*
** Defines
**------------------------------------------------------------------------*/

#define L5DIR_HORIZ		0
#define L5DIR_VERT		1

#define L5ROOM_MIN 		2
#define L5ROOM_MAX 		6

#define L5DUNX			40
#define L5DUNY			40

#define L5MIN_AREA	((L5DUNX*L5DUNY)/3) + ((L5DUNX*L5DUNY)/7)

#define L5DX			80
#define L5DY			80

byte L5dungeon[L5DX][L5DY];
byte L5ConvTbl[16] = { 22, 13, 1, 13, 2, 13, 13, 13, 4, 13, 1, 13, 2, 13, 16, 13 };

/*------------------------------------------------------------------------*
** Global variables
**------------------------------------------------------------------------*/
static BOOL HR1, HR2, HR3;
static BOOL VR1, VR2, VR3;

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static void InitL5Dungeon()
{
	int i,j;

	for (j = 0; j < L5DUNY; j++) {
		for (i = 0; i < L5DUNX; i++) {
			dungeon[i][j] = 0;
			dflags[i][j] = NODOOR;
		}
	}
}

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static void L5ClearFlags()
{
	int i,j;

	for (j = 0; j < L5DUNY; j++) {
		for (i = 0; i < L5DUNX; i++) dflags[i][j] &= SETP_TMASK;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void L5drawRoom(int x, int y, int w, int h)
{
	int i,j;


	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			dungeon[i+x][j+y] = 1;
		}
	}
}

/*---------------------------------------------------------------------*
** Check limits of 40 X 40
**---------------------------------------------------------------------*/
static BOOL L5checkRoom(int x, int y, int width, int height)
{
	int i, j;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if ((x+i < 0) || (x+i >= L5DUNX) || (y+j < 0) || (y+j >= L5DUNY)) return(FALSE);
			if (dungeon[x+i][y+j] != 0) return(FALSE);
		}
	}
	return(TRUE);
}

/*---------------------------------------------------------------------**
** Generate left and right, up and down rooms
**---------------------------------------------------------------------*/
static void L5roomGen(int x, int y, int w, int h, int dir)
{
	int rx, ry, rx2, ry2;
	int height, width;
	int cx1, cy1, cw, ch;
	int num;
	int dirProb;
	int ran;
	BOOL c, d;

	ran = random(0, 4);
	if (dir == L5DIR_VERT) {
		if (ran == 0) dirProb = L5DIR_HORIZ;
		else dirProb = L5DIR_VERT;
	} else {
		if (ran == 0) dirProb = L5DIR_VERT;
		else dirProb = L5DIR_HORIZ;
	}
	switch(dirProb) {
		case L5DIR_HORIZ :				// left/right
			// left room
			num = 0;
			do {
				width  = ((random(0, L5ROOM_MAX-L5ROOM_MIN+1) + L5ROOM_MIN) >> 1) << 1;
				height = ((random(0, L5ROOM_MAX-L5ROOM_MIN+1) + L5ROOM_MIN) >> 1) << 1;
				ry = y + (h/2) - (height/2);
				rx = x - width;
				cx1 = rx - 1;
				cy1 = ry - 1;
				cw = height + 2;
				ch = width + 1;
				c = L5checkRoom(cx1, cy1, cw, ch);
				num++;
			} while ((c == FALSE) && (num < 20));
			if (c == TRUE) L5drawRoom(rx, ry, width, height);

			// right room
			rx2 = x + w;
			cx1 = rx2;
			cy1 = ry - 1;
			ch = height + 2;
			cw = width + 1;
			d = L5checkRoom(cx1, cy1, cw, ch);
			if (d == TRUE) L5drawRoom(rx2, ry, width, height);
			if (c == TRUE) L5roomGen(rx, ry, width, height, L5DIR_VERT);
			if (d == TRUE) L5roomGen(rx2, ry, width, height, L5DIR_VERT);
			break;

		case L5DIR_VERT : 				// top/bottom
			// top room
			num = 0;
			do {
				width  = ((random(0, L5ROOM_MAX-L5ROOM_MIN+1) + L5ROOM_MIN) >> 1) << 1;
				height = ((random(0, L5ROOM_MAX-L5ROOM_MIN+1) + L5ROOM_MIN) >> 1) << 1;
				rx = x + (w/2) - (width/2);
				ry = y - height;
				cx1 = rx - 1;
				cy1 = ry - 1;
				ch = height + 1;
				cw = width + 2;
				c = L5checkRoom(cx1, cy1, cw, ch);
				num++;

			} while ((c == FALSE) && (num < 20));
			if (c == TRUE) L5drawRoom(rx, ry, width, height);

			// bottom room
			ry2 = y + h;
			cx1 = rx - 1;
			cy1 = ry2;
			ch = height + 1;
			cw = width + 2;
			d = L5checkRoom(cx1, cy1, cw, ch);
			if (d == TRUE) L5drawRoom(rx, ry2, width, height);
			if (c == TRUE) L5roomGen(rx, ry, width, height, L5DIR_HORIZ);
			if (d == TRUE) L5roomGen(rx, ry2, width, height, L5DIR_HORIZ);
			break;
	}
}

/*-----------------------------------------------------------------------*
** DRLG_GChamber(int sx, int sy, int topflag, int bottomflag, int leftflag, int rightflag)
**-----------------------------------------------------------------------*/
static void L5firstRoom()
{
	int x, y;
	int xs,xe,ys,ye;

	if (random(0, 2) == 0) { // vertical
		ys = 1;
		ye = L5DUNY - 1;
		VR1 = random(0, 2);
		VR2 = random(0, 2);
		VR3 = random(0, 2);
		if ((VR1 + VR3) <= 1) VR2 = 1;
		if (VR1) L5drawRoom(15,1,10,10);
		else ys += 17;
		if (VR2) L5drawRoom(15,15,10,10);
		if (VR3) L5drawRoom(15,29,10,10);
		else ye -= 17;
		for (y = ys; y < ye; y++) {
			dungeon[17][y] = 1;
			dungeon[18][y] = 1;
			dungeon[19][y] = 1;
			dungeon[20][y] = 1;
			dungeon[21][y] = 1;
			dungeon[22][y] = 1;
		}
		if (VR1) L5roomGen(15,1,10,10,L5DIR_HORIZ);
		if (VR2) L5roomGen(15,15,10,10,L5DIR_HORIZ);
		if (VR3) L5roomGen(15,29,10,10,L5DIR_HORIZ);
		HR1 = HR2 = HR3 = FALSE;
	} else { // horizontal 
		xs = 1;
		xe = L5DUNX - 1;
		HR1 = random(0, 2);
		HR2 = random(0, 2);
		HR3 = random(0, 2);
		if ((HR1 + HR3) <= 1) HR2 = 1;
		if (HR1) L5drawRoom(1,15,10,10);
		else xs += 17;
		if (HR2) L5drawRoom(15,15,10,10);
		if (HR3) L5drawRoom(29,15,10,10);	
		else xe -= 17;
		for (x = xs; x < xe; x++) {
			dungeon[x][17] = 1;
			dungeon[x][18] = 1;
			dungeon[x][19] = 1;
			dungeon[x][20] = 1;
			dungeon[x][21] = 1;
			dungeon[x][22] = 1;
		}
		if (HR1) L5roomGen(1,15,10,10,L5DIR_VERT);
		if (HR2) L5roomGen(15,15,10,10,L5DIR_VERT);
		if (HR3) L5roomGen(29,15,10,10,L5DIR_VERT);
		VR1 = VR2 = VR3 = FALSE;
	}
}

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static long L5GetArea()
{
	int i,j;
	long rv;

	rv = 0;
	for (j = 0; j < L5DUNY; j++) {
		for (i = 0; i < L5DUNX; i++) {
			if (dungeon[i][j] == 1) rv++;
		}
	}
	return(rv);
}

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static void L5makeDungeon()
{
	int i, j;
	int k, l;

	for (j = 0; j < L5DUNY; j++) {
		for(i = 0; i < L5DUNX; i++) {
			k = i<<1;
			l = j<<1;
			L5dungeon[k][l] = dungeon[i][j];
			L5dungeon[k][l+1] = dungeon[i][j];
			L5dungeon[k+1][l] = dungeon[i][j];
			L5dungeon[k+1][l+1] = dungeon[i][j];
		}
	}
}

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static void L5makeDmt()
{
	int i, j;
	int idx;
	int val;
	int dmtx, dmty;

	for (j = 0; j < L5DUNY; j++)
		for (i = 0; i < L5DUNX; i++) dungeon[i][j] = D_DIRT;

	dmty = 0;
	for (j = 1; j <= 77; j+=2) {
		dmtx = 0;
		for (i = 1; i <= 77; i+=2) {
			idx = L5dungeon[i][j] + (L5dungeon[i+1][j]<<1) +
				(L5dungeon[i][j+1]<<2) + (L5dungeon[i+1][j+1]<<3);

			val = L5ConvTbl[idx];
			dungeon[dmtx][dmty] = val;
			dmtx++;
		}
		dmty++;
	}

}						  

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int L5HWallOk(int i, int j)
{
	int x;
	BOOL wallok;

	x = 1;
	while ((dungeon[i+x][j] == 13) &&
			(dungeon[i+x][j-1] == 13) &&
			(dungeon[i+x][j+1] == 13) &&
			(dflags[i+x][j] == NODOOR)) x++;
	wallok = FALSE;
	if ((dungeon[i+x][j] >=  3) && (dungeon[i+x][j] <=  7)) wallok = TRUE;
	if ((dungeon[i+x][j] >= 16) && (dungeon[i+x][j] <= 24)) wallok = TRUE;
	if (dungeon[i+x][j] == 22) wallok = FALSE;
	if (x == 1) wallok = FALSE;
	if (wallok) return(x);
	else return(-1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int L5VWallOk(int i, int j)
{
	int y;
	BOOL wallok;

	y = 1;
	while ((dungeon[i][j+y] == 13) &&
			(dungeon[i-1][j+y] == 13) &&
			(dungeon[i+1][j+y] == 13) &&
			(dflags[i][j+y] == NODOOR)) y++;
	wallok = FALSE;
	if ((dungeon[i][j+y] >=  3) && (dungeon[i][j+y] <=  7)) wallok = TRUE;
	if ((dungeon[i][j+y] >= 16) && (dungeon[i][j+y] <= 24)) wallok = TRUE;
	if (dungeon[i][j+y] == 22) wallok = FALSE;
	if (y == 1) wallok = FALSE;
	if (wallok) return(y);
	else return(-1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void L5HorizWall(int i, int j, char p, int dx)
{
	int xx;
	char wt, dt;

	switch(random(0, 4)) {
		case 0:
		case 1:
			wt = 2;		// Normal solid wall
			break;
		case 2:
			wt = 12;	// Arch wall
			if (p == 2) p = 12;
			if (p == 4) p = 10;
			break;
		case 3:
			wt = 36;	// Grate wall
			if (p == 2) p = 36;
			if (p == 4) p = 27;
			break;
	}

	if (random(0, 6) == 5) dt = 12;	// Arch
	else dt = 26;		// Door

	if (wt == 12) dt = 12;		// If arch wall, force arch door

	dungeon[i][j] = p;
	for (xx = 1; xx < dx; xx++) dungeon[i+xx][j] = wt;
	xx = random(0, dx-1) + 1;
	if (dt == 12) dungeon[i+xx][j] = dt;
	else {
		dungeon[i+xx][j] = 2;
		dflags[i+xx][j] |= HDOOR;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void L5VertWall(int i, int j, char p, int dy)
{
	int yy;
	char wt, dt;

	switch(random(0, 4)) {
		case 0:
		case 1:
			wt = 1;		// Normal solid wall
			break;
		case 2:
			wt = 11;	// Arch wall
			if (p == 1) p = 11;
			if (p == 4) p = 14;
			break;
		case 3:
			wt = 35;	// Grate wall
			if (p == 1) p = 35;
			if (p == 4) p = 37;
			break;
	}

	if (random(0, 6) == 5) dt = 11;	// Arch
	else dt = 25;		// Door

	if (wt == 11) dt = 11;		// If arch wall, force arch door

	dungeon[i][j] = p;
	for (yy = 1; yy < dy; yy++) dungeon[i][j+yy] = wt; 
	yy = random(0, dy-1) + 1;
	if (dt == 11) dungeon[i][j+yy] = dt;
	else {
		dungeon[i][j+yy] = 1;
		dflags[i][j+yy] |= VDOOR;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define WALLRND 100
static void L5AddWall()
{ 
	int i, j;
	int x, y;
	
	for (j = 0; j < MDMAXY ; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if (dflags[i][j] == 0) {
				if ((dungeon[i][j] == 3) && (random(0, 100) < WALLRND)) {
					x = L5HWallOk(i, j);
					if (x != -1) L5HorizWall(i, j, 2, x);
				}
				if ((dungeon[i][j] == 3) && (random(0, 100) < WALLRND)) {
					y = L5VWallOk(i, j);
					if (y != -1) L5VertWall(i, j, 1, y);
				}
				if ((dungeon[i][j] == 6) && (random(0, 100) < WALLRND)) {
					x = L5HWallOk(i, j);
					if (x != -1) L5HorizWall(i, j, 4, x);
				}
				if ((dungeon[i][j] == 7) && (random(0, 100) < WALLRND)) {
					y = L5VWallOk(i, j);
					if (y != -1) L5VertWall(i, j, 4, y);
				}
				if ((dungeon[i][j] == 2) && (random(0, 100) < WALLRND)) {
					x = L5HWallOk(i, j);
					if (x != -1) L5HorizWall(i, j, 2, x);
				}
				if ((dungeon[i][j] == 1) && (random(0, 100) < WALLRND)) {
					y = L5VWallOk(i, j);
					if (y != -1) L5VertWall(i, j, 1, y);
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L5GChamber(int sx, int sy, int topflag, int bottomflag, int leftflag, int rightflag)
{
	int	i,j;

	if (topflag == 1) { 
		// Place top row tiles
		dungeon[sx+2][sy] = D_AHW;
		dungeon[sx+3][sy] = D_AHW;
		dungeon[sx+4][sy] = D_LRC;
		dungeon[sx+7][sy] = D_ALLC;
		dungeon[sx+8][sy] = D_AHW;
		dungeon[sx+9][sy] = D_HWALL;
	}
	if (bottomflag == 1) {		
		// Place bottom row tiles
		sy += 11;
		dungeon[sx+2][sy] = D_TULC1;
		dungeon[sx+3][sy] = D_AHW;
		dungeon[sx+4][sy] = D_AURC;
		dungeon[sx+7][sy] = D_AULC;
		dungeon[sx+8][sy] = D_AHW;
		if (dungeon[sx+9][sy] != D_ULC) dungeon[sx+9][sy] = D_DULC;
		sy -= 11;
	}

	if (leftflag == 1) { 
		// Place left column tiles
		dungeon[sx][sy+2] = D_AVW;
		dungeon[sx][sy+3] = D_AVW;
		dungeon[sx][sy+4] = D_LRC;
		dungeon[sx][sy+7] = D_AURC;
		dungeon[sx][sy+8] = D_AVW;
		dungeon[sx][sy+9] = D_VWALL;
	} 
	if (rightflag == 1) {
		// Place right column tiles
		sx += 11;
		dungeon[sx][sy+2] = D_TULC2;
		dungeon[sx][sy+3] = D_AVW;
		dungeon[sx][sy+4] = D_ALLC;
		dungeon[sx][sy+7] = D_AULC;
		dungeon[sx][sy+8] = D_AVW;
		if (dungeon[sx][sy+9] != D_ULC) dungeon[sx][sy+9] = D_DULC;
		sx -= 11;
	}

	for (j = 1; j < 11; j++) {
		for (i = 1; i < 11; i++) {
			dungeon[sx+i][sy+j] = D_FLOOR;
			dflags[sx+i][sy+j] |= SETP_TEMP;
		}
	}
	dungeon[sx+4][sy+4] = D_COL;
	dungeon[sx+7][sy+4] = D_COL;
	dungeon[sx+4][sy+7] = D_COL;
	dungeon[sx+7][sy+7] = D_COL;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void DRLG_L5GHall(int x1, int y1, int x2, int y2)
{
	int i;

	// Horiz or vert?
	if (y1 == y2) {
		// Horiz
		for (i = x1; i < x2; i++) {
			dungeon[i][y1] = D_AHW;
			dungeon[i][y1+3] = D_AHW;
		}
	} else {
		// Vert
		for (i = y1; i < y2; i++) {
			dungeon[x1][i] = D_AVW;
			dungeon[x1+3][i] = D_AVW;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void L5tileFix()
{
	int i, j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 22)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] == 13) && (dungeon[i+1][j] == 22)) dungeon[i+1][j] = 18;
			if ((dungeon[i][j] == 13) && (dungeon[i+1][j] ==  2)) dungeon[i+1][j] =  7;
	
			if ((dungeon[i][j] ==  6) && (dungeon[i+1][j] == 22)) dungeon[i+1][j] = 24;
			
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] == 22)) dungeon[i][j+1] = 24;
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] ==  1)) dungeon[i][j+1] =  6;
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] == 22)) dungeon[i][j+1] = 19;
		}
	}
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 13) && (dungeon[i+1][j] == 19)) dungeon[i+1][j] = 21;
			if ((dungeon[i][j] == 13) && (dungeon[i+1][j] == 22)) dungeon[i+1][j] = 20;
			if ((dungeon[i][j] ==  7) && (dungeon[i+1][j] == 22)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] == 13) && (dungeon[i+1][j] == 24)) dungeon[i+1][j] = 21;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 22)) dungeon[i+1][j] = 20;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 19)) dungeon[i+1][j] = 21;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] =  6;
			if ((dungeon[i][j] ==  7) && (dungeon[i+1][j] == 19)) dungeon[i+1][j] = 21;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] =  6;
			if ((dungeon[i][j] ==  3) && (dungeon[i+1][j] == 22)) dungeon[i+1][j] = 24;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] =  6;
			if ((dungeon[i][j] ==  7) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] =  6;
			if ((dungeon[i][j] ==  7) && (dungeon[i+1][j] == 24)) dungeon[i+1][j] = 21;
			if ((dungeon[i][j] ==  4) && (dungeon[i+1][j] == 16)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] ==  7) && (dungeon[i+1][j] == 13)) dungeon[i+1][j] = 17;	
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 24)) dungeon[i+1][j] = 21;	
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 13)) dungeon[i+1][j] = 17;	
	
			if ((dungeon[i][j] == 23) && (dungeon[i-1][j] == 22)) dungeon[i-1][j] = 19;
			if ((dungeon[i][j] == 19) && (dungeon[i-1][j] == 23)) dungeon[i-1][j] = 21;
			if ((dungeon[i][j] ==  6) && (dungeon[i-1][j] == 22)) dungeon[i-1][j] = 24;
			if ((dungeon[i][j] ==  6) && (dungeon[i-1][j] == 23)) dungeon[i-1][j] = 21;
		
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] =  7;
			if ((dungeon[i][j] ==  6) && (dungeon[i][j+1] == 18)) dungeon[i][j+1] = 21;
			if ((dungeon[i][j] == 18) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] =  7;	
			if ((dungeon[i][j] ==  6) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] =  7;	
			if ((dungeon[i][j] == 21) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] =  7;	
			if ((dungeon[i][j] ==  6) && (dungeon[i][j+1] == 22)) dungeon[i][j+1] = 24;	
			if ((dungeon[i][j] ==  6) && (dungeon[i][j+1] == 13)) dungeon[i][j+1] = 16;	
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] == 13)) dungeon[i][j+1] = 16;	
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] == 16)) dungeon[i][j+1] = 17;	
	
			if ((dungeon[i][j] ==  6) && (dungeon[i][j-1] == 22)) dungeon[i][j-1] =  7;
			if ((dungeon[i][j] ==  6) && (dungeon[i][j-1] == 22)) dungeon[i][j-1] = 24;
			if ((dungeon[i][j] ==  7) && (dungeon[i][j-1] == 24)) dungeon[i][j-1] = 21;
			if ((dungeon[i][j] == 18) && (dungeon[i][j-1] == 24)) dungeon[i][j-1] = 21;
		}	
	}
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] ==  4) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] =  7;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 19)) dungeon[i+1][j] = 21;
			if ((dungeon[i][j] == 18) && (dungeon[i][j+1] == 22)) dungeon[i][j+1] = 20;
		}
	}
}

static void DRLG_L5PlaceRndSet(const byte miniset[], int rndper)
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
static void DRLG_L5Subs()
{
	int		x,y,i,rv;
	byte	c;

	for (y = 0; y < MDMAXY; y++) {
		for (x = 0; x < MDMAXX; x++) {
			rv = random(0, 4);
			if (rv == 0) {
				c = dungeon[x][y];
				c = L5BTYPES[c];
				if ((c != 0) && (dflags[x][y] == 0)) {
					rv = random(0, 16);
					i = -1;
					while (rv >= 0) {
						i++;
						if (i == NUMBLOCKS) i = 0;
						if (c == L5BTYPES[i]) rv--;
					}
					if (i == 89) {
						c = dungeon[x][y-1];
						if ((L5BTYPES[c] == 79) && (dflags[x][y-1] == 0)) {
							dungeon[x][y-1] = 90;
						} else i = 79;
					}
					if (i == 91) {
						c = dungeon[x+1][y];
						if ((L5BTYPES[c] == 80) && (dflags[x+1][y] == 0)) {
							dungeon[x+1][y] = 92;
						} else i = 80;
					}
					dungeon[x][y] = i;
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void DRLG_L5SetRoom(int rx1, int ry1)
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
			} else dungeon[rx1+i][ry1+j] = D_FLOOR;
			sp+=2;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void HRLG_L5SetRoom(int rx1, int ry1)
{
	int		rw;
	int		i,j;

// Don't know what these do. JKE
	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = NA_KRULS_ROOM[0];
	setpc_h = NA_KRULS_ROOM[1];

	Na_Krul.x = ((rx1 + 3) * 2);
	Na_Krul.y = ((ry1 + 4) * 2);
	Na_Krul.Books = FALSE;
	Na_Krul.Open = FALSE;
	Na_Krul.Lever_Thrown = FALSE;


	rw = 2;
	for (j = 0; j < NA_KRULS_ROOM[1]; j++) {
		for (i = 0; i < NA_KRULS_ROOM[0]; i++) {
			if (NA_KRULS_ROOM[rw] != 0) {
				dungeon[rx1+i][ry1+j] = NA_KRULS_ROOM[rw];
				dflags[rx1+i][ry1+j] |= SETP_BIT;
			} else dungeon[rx1+i][ry1+j] = D_FLOOR;
			++rw;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void CRLG_L5SetRoom(int rx1, int ry1)
{
	int		rw;
	int		i,j;

// Don't know what these do. JKE
	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = CORNERSTONE[0];
	setpc_h = CORNERSTONE[1];


	rw = 2;
	for (j = 0; j < CORNERSTONE[1]; j++) {
		for (i = 0; i < CORNERSTONE[0]; i++) {
			if (CORNERSTONE[rw] != 0) {
				dungeon[rx1+i][ry1+j] = CORNERSTONE[rw];
				dflags[rx1+i][ry1+j] |= SETP_BIT;
			} else dungeon[rx1+i][ry1+j] = D_FLOOR;
			++rw;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void L5FillChambers()
{
	int c;

	if (HR1) DRLG_L5GChamber(0, 14, 0, 0, 0, 1);
	if (HR2) {
		if ((HR1) && (!HR3)) DRLG_L5GChamber(14, 14, 0, 0, 1, 0);
		if ((!HR1) && (HR3)) DRLG_L5GChamber(14, 14, 0, 0, 0, 1);
		if ((HR1) && (HR3)) DRLG_L5GChamber(14, 14, 0, 0, 1, 1);
		if ((!HR1) && (!HR3)) DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
	}
	if (HR3) DRLG_L5GChamber(28, 14, 0, 0, 1, 0);
	if ((HR1) && (HR2)) DRLG_L5GHall(12, 18, 14, 18);
	if ((HR2) && (HR3)) DRLG_L5GHall(26, 18, 28, 18);
	if ((HR1) && (!HR2) && (HR3)) DRLG_L5GHall(12, 18, 28, 18);

	if (VR1) DRLG_L5GChamber(14, 0, 0, 1, 0, 0);
	if (VR2) {
		if ((VR1) && (!VR3)) DRLG_L5GChamber(14, 14, 1, 0, 0, 0);
		if ((!VR1) && (VR3)) DRLG_L5GChamber(14, 14, 0, 1, 0, 0);
		if ((VR1) && (VR3)) DRLG_L5GChamber(14, 14, 1, 1, 0, 0);
		if ((!VR1) && (!VR3)) DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
	}
	if (VR3) DRLG_L5GChamber(14, 28, 1, 0, 0, 0);
	if ((VR1) && (VR2)) DRLG_L5GHall(18, 12, 18, 14);
	if ((VR2) && (VR3)) DRLG_L5GHall(18, 26, 18, 28);
	if ((VR1) && (!VR2) && (VR3)) DRLG_L5GHall(18, 12, 18, 28);

	if (currlevel == NA_KRUL_LEVEL)
	{
		if ((!VR1) && (!VR2) && (!VR3)) {
			// Horizontal
			c = 1;		// Middle room
			if ((!HR1) && (HR2) && (HR3)) {
				if (random(0, 2)) c = 2;
			}
			if ((HR1) && (HR2) && (!HR3)) {
				if (random(0, 2)) c = 0;
			}
			if ((HR1) && (!HR2) && (HR3)) {
				if (random(0, 2)) c = 0;
				else c = 2;
			}
			if ((HR1) && (HR2) && (HR3)) c = random(0, 3);
			switch (c) {
				case 0:
					HRLG_L5SetRoom(2,16);
					break;
				case 1:
					HRLG_L5SetRoom(16,16);
					break;
				case 2:
					HRLG_L5SetRoom(30,16);
					break;
			}
		} else {
			// Vertical
			c = 1;		// Middle room
			if ((!VR1) && (VR2) && (VR3)) {
				if (random(0, 2)) c = 2;
			}
			if ((VR1) && (VR2) && (!VR3)) {
				if (random(0, 2)) c = 0;
			}
			if ((VR1) && (!VR2) && (VR3)) {
				if (random(0, 2)) c = 0;
				else c = 2;
			}
			if ((VR1) && (VR2) && (VR3)) c = random(0, 3);
			switch (c) {
				case 0:
					HRLG_L5SetRoom(16,2);
					break;
				case 1:
					HRLG_L5SetRoom(16,16);
					break;
				case 2:
					HRLG_L5SetRoom(16,30);
					break;
			}
		}
	}
	if (currlevel == CORNERSTONE_LEVEL)
	{
		if ((!VR1) && (!VR2) && (!VR3)) {
			// Horizontal
			c = 1;		// Middle room
			if ((!HR1) && (HR2) && (HR3)) {
				if (random(0, 2)) c = 2;
			}
			if ((HR1) && (HR2) && (!HR3)) {
				if (random(0, 2)) c = 0;
			}
			if ((HR1) && (!HR2) && (HR3)) {
				if (random(0, 2)) c = 0;
				else c = 2;
			}
			if ((HR1) && (HR2) && (HR3)) c = random(0, 3);
			switch (c) {
				case 0:
					CRLG_L5SetRoom(2,16);
					break;
				case 1:
					CRLG_L5SetRoom(16,16);
					break;
				case 2:
					CRLG_L5SetRoom(30,16);
					break;
			}
		} else {
			// Vertical
			c = 1;		// Middle room
			if ((!VR1) && (VR2) && (VR3)) {
				if (random(0, 2)) c = 2;
			}
			if ((VR1) && (VR2) && (!VR3)) {
				if (random(0, 2)) c = 0;
			}
			if ((VR1) && (!VR2) && (VR3)) {
				if (random(0, 2)) c = 0;
				else c = 2;
			}
			if ((VR1) && (VR2) && (VR3)) c = random(0, 3);
			switch (c) {
				case 0:
					CRLG_L5SetRoom(16,2);
					break;
				case 1:
					CRLG_L5SetRoom(16,16);
					break;
				case 2:
					CRLG_L5SetRoom(16,30);
					break;
			}
		}
	}
	if (setloadflag) {
		if ((!VR1) && (!VR2) && (!VR3)) {
			// Horizontal
			c = 1;		// Middle room
			if ((!HR1) && (HR2) && (HR3)) {
				if (random(0, 2)) c = 2;
			}
			if ((HR1) && (HR2) && (!HR3)) {
				if (random(0, 2)) c = 0;
			}
			if ((HR1) && (!HR2) && (HR3)) {
				if (random(0, 2)) c = 0;
				else c = 2;
			}
			if ((HR1) && (HR2) && (HR3)) c = random(0, 3);
			switch (c) {
				case 0:
					DRLG_L5SetRoom(2,16);
					break;
				case 1:
					DRLG_L5SetRoom(16,16);
					break;
				case 2:
					DRLG_L5SetRoom(30,16);
					break;
			}
		} else {
			// Vertical
			c = 1;		// Middle room
			if ((!VR1) && (VR2) && (VR3)) {
				if (random(0, 2)) c = 2;
			}
			if ((VR1) && (VR2) && (!VR3)) {
				if (random(0, 2)) c = 0;
			}
			if ((VR1) && (!VR2) && (VR3)) {
				if (random(0, 2)) c = 0;
				else c = 2;
			}
			if ((VR1) && (VR2) && (VR3)) c = random(0, 3);
			switch (c) {
				case 0:
					DRLG_L5SetRoom(16,2);
					break;
				case 1:
					DRLG_L5SetRoom(16,16);
					break;
				case 2:
					DRLG_L5SetRoom(16,30);
					break;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L5FTVR(int i, int j, int x, int y, int d)
{
	if ((dTransVal[x][y] == 0) && (dungeon[i][j] == D_FLOOR)) {
		dTransVal[x][y] = TransVal;
		dTransVal[x+1][y] = TransVal;
		dTransVal[x][y+1] = TransVal;
		dTransVal[x+1][y+1] = TransVal;
		DRLG_L5FTVR(i+1,j, x+2,y, 1);
		DRLG_L5FTVR(i-1,j, x-2,y, 2);
		DRLG_L5FTVR(i,j+1, x,y+2, 3);
		DRLG_L5FTVR(i,j-1, x,y-2, 4);

		DRLG_L5FTVR(i-1,j-1, x-2,y-2, 5);
		DRLG_L5FTVR(i+1,j-1, x+2,y-2, 6);
		DRLG_L5FTVR(i-1,j+1, x-2,y+2, 7);
		DRLG_L5FTVR(i+1,j+1, x+2,y+2, 8);
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

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L5FloodTVal()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == D_FLOOR) && (dTransVal[xx][yy] == 0)) {
				DRLG_L5FTVR(i,j,xx,yy,0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L5TransFix()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == D_DURC) && (dungeon[i][j-1] == D_DV)) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if ((dungeon[i][j] == D_DLLC) && (dungeon[i+1][j] == D_DH)) {
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == D_DV) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == D_DH) {
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == D_DLRC) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			xx += 2;
		}
		yy += 2;
	}
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void DRLG_L5DirtFix()
{
	int i, j;

	if (currlevel < CRYPTSTART)
	{
		for (j = 0; j < MDMAXY-1 ; j++) {
			for (i = 0; i < MDMAXX-1 ; i++) {
				if ((dungeon[i][j] ==  21) && (dungeon[i+1][j] != 19)) dungeon[i][j] = 202;
				if ((dungeon[i][j] ==  19) && (dungeon[i+1][j] != 19)) dungeon[i][j] = 200;
				if ((dungeon[i][j] ==  24) && (dungeon[i+1][j] != 19)) dungeon[i][j] = 205;

				if ((dungeon[i][j] ==  18) && (dungeon[i][j+1] != 18)) dungeon[i][j] = 199;
				if ((dungeon[i][j] ==  21) && (dungeon[i][j+1] != 18)) dungeon[i][j] = 202;
				if ((dungeon[i][j] ==  23) && (dungeon[i][j+1] != 18)) dungeon[i][j] = 204;
			}
		}
	}
	else
	{
		for (j = 0; j < MDMAXY - 1 ; j++) {
			for (i = 0; i < MDMAXX - 1 ; i++) {
//				if ((dungeon[i][j] ==  21) && (dungeon[i+1][j] != 19)) 
//					dungeon[i][j] = 85;
//				if ((dungeon[i][j] ==  19) && (dungeon[i+1][j] != 19)) 
//					dungeon[i][j] = 83;
//				if ((dungeon[i][j] ==  24) && (dungeon[i+1][j] != 19)) 
//					dungeon[i][j] = 88;

//				if ((dungeon[i][j] ==  18) && (dungeon[i][j+1] != 18)) 
//					dungeon[i][j] = 82;
//				if ((dungeon[i][j] ==  21) && (dungeon[i][j+1] != 18)) 
//					dungeon[i][j] = 85;
//				if ((dungeon[i][j] ==  23) && (dungeon[i][j+1] != 18)) 
				if (dungeon [i][j] == 19)
					dungeon[i][j] = 83;
				if (dungeon [i][j] == 21)
					dungeon[i][j] = 85;
				if (dungeon [i][j] == 23)
					dungeon[i][j] = 87;
				if (dungeon [i][j] == 24)
					dungeon[i][j] = 88;
				if (dungeon [i][j] == 18)
					dungeon[i][j] = 82;

			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void DRLG_L5CornerFix()
{
	int i, j;

	for (j = 1; j < MDMAXY-1; j++) {
		for (i = 1; i < MDMAXX-1; i++) {
			if (!(dflags[i][j] & SETP_BIT)) {
				if ((dungeon[i][j] == 17) && (dungeon[i-1][j] == 13) && (dungeon[i][j-1] == 1)) {
					dungeon[i][j] = 16;
					dflags[i][j-1] &= SETP_BIT;
				}
			}
			if ((dungeon[i][j] == 202) && (dungeon[i+1][j] == 13) && (dungeon[i][j+1] == 1)) {
				dungeon[i][j] = 8;
			}
		}
	}
}


static void Statues(int chance)
{
	DRLG_L5PlaceRndSet(STATUE_LEFTA, chance);
	DRLG_L5PlaceRndSet(STATUE_LEFTB, chance);
	
	DRLG_L5PlaceRndSet(STATUE_RIGHTA, chance);
	DRLG_L5PlaceRndSet(STATUE_RIGHTB, chance);
}

static void BrokenWall1(int chance)
{
	DRLG_L5PlaceRndSet(BROKEN_VWALL1, chance);
	DRLG_L5PlaceRndSet(BROKEN_HWALL1, chance);
	DRLG_L5PlaceRndSet(BROKEN_LRC1, chance);
	DRLG_L5PlaceRndSet(BROKEN_ULC1, chance);
	DRLG_L5PlaceRndSet(BROKEN_AULC1, chance);
	DRLG_L5PlaceRndSet(BROKEN_URC1, chance);
	DRLG_L5PlaceRndSet(BROKEN_LLC1, chance);
	DRLG_L5PlaceRndSet(BROKEN_AURC1, chance);
	DRLG_L5PlaceRndSet(BROKEN_ALLC1, chance);
	DRLG_L5PlaceRndSet(BROKEN_TULC11, chance);
	DRLG_L5PlaceRndSet(BROKEN_AVW1, chance);
	DRLG_L5PlaceRndSet(BROKEN_AHW1, chance);
	DRLG_L5PlaceRndSet(BROKEN_FLOOR1, chance);
	DRLG_L5PlaceRndSet(BROKEN_TULC21, chance);
	DRLG_L5PlaceRndSet(BROKEN_COL1, chance);
	DRLG_L5PlaceRndSet(BROKEN_BCAP1, chance);
	DRLG_L5PlaceRndSet(BROKEN_RCAP1, chance);
}

static void BrokenWall2(int chance)
{
	DRLG_L5PlaceRndSet(BROKEN_VWALL2, chance);
	DRLG_L5PlaceRndSet(BROKEN_HWALL2, chance);
	DRLG_L5PlaceRndSet(BROKEN_LRC2, chance);
	DRLG_L5PlaceRndSet(BROKEN_ULC2, chance);
	DRLG_L5PlaceRndSet(BROKEN_AULC2, chance);
	DRLG_L5PlaceRndSet(BROKEN_URC2, chance);
	DRLG_L5PlaceRndSet(BROKEN_LLC2, chance);
	DRLG_L5PlaceRndSet(BROKEN_AURC2, chance);
	DRLG_L5PlaceRndSet(BROKEN_ALLC2, chance);
	DRLG_L5PlaceRndSet(BROKEN_TULC12, chance);
	DRLG_L5PlaceRndSet(BROKEN_AVW2, chance);
	DRLG_L5PlaceRndSet(BROKEN_AHW2, chance);
	DRLG_L5PlaceRndSet(BROKEN_FLOOR2, chance);
	DRLG_L5PlaceRndSet(BROKEN_TULC22, chance);
	DRLG_L5PlaceRndSet(BROKEN_COL2, chance);
	DRLG_L5PlaceRndSet(BROKEN_BCAP2, chance);
	DRLG_L5PlaceRndSet(BROKEN_RCAP2, chance);
}

static void BrokenWall3(int chance)
{
	DRLG_L5PlaceRndSet(BROKEN_VWALL3, chance);
	DRLG_L5PlaceRndSet(BROKEN_HWALL3, chance);
	DRLG_L5PlaceRndSet(BROKEN_LRC3, chance);
	DRLG_L5PlaceRndSet(BROKEN_ULC3, chance);
	DRLG_L5PlaceRndSet(BROKEN_AULC3, chance);
	DRLG_L5PlaceRndSet(BROKEN_URC3, chance);
	DRLG_L5PlaceRndSet(BROKEN_LLC3, chance);
	DRLG_L5PlaceRndSet(BROKEN_AURC3, chance);
	DRLG_L5PlaceRndSet(BROKEN_ALLC3, chance);
	DRLG_L5PlaceRndSet(BROKEN_TULC13, chance);
	DRLG_L5PlaceRndSet(BROKEN_AVW3, chance);
	DRLG_L5PlaceRndSet(BROKEN_AHW3, chance);
	DRLG_L5PlaceRndSet(BROKEN_FLOOR3, chance);
	DRLG_L5PlaceRndSet(BROKEN_TULC23, chance);
	DRLG_L5PlaceRndSet(BROKEN_COL3, chance);
	DRLG_L5PlaceRndSet(BROKEN_BCAP3, chance);
	DRLG_L5PlaceRndSet(BROKEN_RCAP3, chance);
}



static void FloorRubble(int chance)
{
	DRLG_L5PlaceRndSet(BIGFLOORR1, chance);
	DRLG_L5PlaceRndSet(BIGFLOORR2, chance);
	DRLG_L5PlaceRndSet(BIGFLOORR3, chance);
	DRLG_L5PlaceRndSet(BIGFLOORR4, chance);
	DRLG_L5PlaceRndSet(BIGFLOORR5, chance);
	DRLG_L5PlaceRndSet(BIGFLOORR6, chance);

	DRLG_L5PlaceRndSet(FLOORR1, chance);
	DRLG_L5PlaceRndSet(FLOORR2, chance);
	DRLG_L5PlaceRndSet(FLOORR3, chance);
	DRLG_L5PlaceRndSet(FLOORR4, chance);

}



static void BrokenGrate(int chance)
{
	DRLG_L5PlaceRndSet(ARCHLEFT1A, chance);
	DRLG_L5PlaceRndSet(ARCHRIGHT1A, chance);

	DRLG_L5PlaceRndSet(ARCHLEFT1B, chance);
	DRLG_L5PlaceRndSet(ARCHRIGHT1B, chance);


	DRLG_L5PlaceRndSet(GRATELEFT1BA, chance);
	DRLG_L5PlaceRndSet(GRATELEFT1BB, chance);

	DRLG_L5PlaceRndSet(GRATELEFT1MA, chance);
	DRLG_L5PlaceRndSet(GRATELEFT1MB, chance);

	DRLG_L5PlaceRndSet(GRATELEFT1TA, chance);
	DRLG_L5PlaceRndSet(GRATELEFT1TB, chance);

	DRLG_L5PlaceRndSet(GRATERIGHT1RA, chance);
	DRLG_L5PlaceRndSet(GRATERIGHT1RB, chance);

	DRLG_L5PlaceRndSet(GRATERIGHT1MA, chance);
	DRLG_L5PlaceRndSet(GRATERIGHT1MB, chance);

	DRLG_L5PlaceRndSet(GRATERIGHT1LA, chance);
	DRLG_L5PlaceRndSet(GRATERIGHT1LB, chance);

	DRLG_L5PlaceRndSet(FLOOR1B1, chance);
	DRLG_L5PlaceRndSet(FLOOR2B1, chance);
	DRLG_L5PlaceRndSet(FLOOR3B1, chance);
	DRLG_L5PlaceRndSet(FLOOR4B1, chance);
	DRLG_L5PlaceRndSet(FLOOR5B1, chance);
	DRLG_L5PlaceRndSet(FLOOR6B1, chance);
	DRLG_L5PlaceRndSet(FLOOR7B1, chance);

	DRLG_L5PlaceRndSet(BIGFLOORB1, chance);
	DRLG_L5PlaceRndSet(BIGFLOORB2, chance);
	DRLG_L5PlaceRndSet(BIGFLOORB3, chance);


}

static void NormalGrate(int chance)
{
	DRLG_L5PlaceRndSet(FLOOR1, chance);
	DRLG_L5PlaceRndSet(FLOOR2, chance);
	DRLG_L5PlaceRndSet(FLOOR3, chance);
	DRLG_L5PlaceRndSet(FLOOR4, chance);
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void DRLG_L5(int entry)
{
	long area, minarea;
	BOOL doneflag;
	int i,j;
	int xx,yy;

	doneflag = FALSE;
	
	switch(currlevel) {
		case 1:
			minarea = 533;	//(L5DUNX*L5DUNY)/3;
			break;
		case 2:
			minarea = 693;	//((L5DUNX*L5DUNY)/3) + ((L5DUNX*L5DUNY)/10);
			break;
		case 3:
		case 4:
			minarea = 761;	//((L5DUNX*L5DUNY)/3) + ((L5DUNX*L5DUNY)/7);
			break;
		default:
			minarea = 761;	// Crypt levels
	}
	while (!doneflag) {
		DRLG_InitTrans();
		do{
			InitL5Dungeon();
			L5firstRoom();
			area = L5GetArea();
		} while (area < minarea);
		L5makeDungeon();
//if (currlevel < CRYPTSTART)
		L5makeDmt();
		L5FillChambers();
//if (currlevel < CRYPTSTART)
		L5tileFix();

		L5AddWall();
		L5ClearFlags();
		DRLG_L5FloodTVal();

		doneflag = TRUE;



		if (QuestStatus(Q_PWATER)) {
			if (entry == LVL_DOWN) {
				if (DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 1, -1, LVL_DOWN) < 0) doneflag = FALSE;
			} else {
				if (DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 0, -1, LVL_DOWN) < 0) doneflag = FALSE;
				ViewY--;
			}
		}

		if (QuestStatus(Q_LTBANNER)) {	//JKE This might prove a problem if the quest comes up.
			if (entry == LVL_DOWN) {
				if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 1, -1, LVL_DOWN) < 0) doneflag = FALSE;
			} else {
				if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 0, -1, LVL_DOWN) < 0) doneflag = FALSE;
				if (entry == LVL_UP) {
					ViewX = (setpc_x << 1) + 4 + DIRTEDGED2;
					ViewY = (setpc_y << 1) + 12 + DIRTEDGED2;
				} else
					ViewY--;
			}
		} else {
			if (entry == LVL_DOWN) 
			{
				if (currlevel < CRYPTSTART)
				{
					if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 1, -1, LVL_DOWN) < 0) doneflag = FALSE;
					if (DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 0, -1, LVL_UP) < 0) doneflag = FALSE;
				}
				else
				{
					if (currlevel == CRYPTSTART)	// JKE set town warp up
					{
						if (DRLG_PlaceMiniSet(WARPSTAIRS, 1, 1, 0, 0, 0, -1, LVL_TWARPDN) < 0) doneflag = FALSE;
						if (DRLG_PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, 0, -1, LVL_UP) < 0) doneflag = FALSE;
					}
					else
					{

						if (DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 1, -1, LVL_DOWN) < 0) doneflag = FALSE;
						if (currlevel !=CRYPTEND)	// no stairs down on level 20 JKE
							if (DRLG_PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, 0, -1, LVL_UP) < 0) doneflag = FALSE;
					}
					++ViewY;
				}
			} else {
				if (entry == LVL_UP){
					if (currlevel < CRYPTSTART)
					{
						if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 0, -1, LVL_DOWN) < 0) doneflag = FALSE;
						if (DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 1, -1, LVL_UP) < 0) doneflag = FALSE;
						ViewY--;
					}
					else
					{
						if (currlevel == CRYPTSTART)	// JKE set town warp up
						{
							if (DRLG_PlaceMiniSet(WARPSTAIRS, 1, 1, 0, 0, 0, -1, LVL_TWARPDN) < 0) doneflag = FALSE;
							if (DRLG_PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, 1, -1, LVL_UP) < 0) doneflag = FALSE;
						}
						else
						{
							if (DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 1, -1, LVL_DOWN) < 0) doneflag = FALSE;
							if (currlevel !=CRYPTEND)	// no stairs down on level 20 JKE
								if (DRLG_PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, 1, -1, LVL_UP) < 0) doneflag = FALSE;
						}
					ViewY +=3;
					}
				} else {
					if (currlevel < CRYPTSTART)
					{
						if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 0, -1, LVL_DOWN) < 0) doneflag = FALSE;
						if (DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 0, -1, LVL_UP) < 0) doneflag = FALSE;
					}
					else
					{
						if (currlevel == CRYPTSTART)	// JKE set town warp up
						{
							if (DRLG_PlaceMiniSet(WARPSTAIRS, 1, 1, 0, 0, 1, -1, LVL_TWARPDN) < 0) doneflag = FALSE;
							if (DRLG_PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, 0, -1, LVL_UP) < 0) doneflag = FALSE;
						}
						else
						{
							if (DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 1, -1, LVL_DOWN) < 0) doneflag = FALSE;
							if (currlevel !=CRYPTEND)	// no stairs down on level 20 JKE
								if (DRLG_PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, 0, -1, LVL_UP) < 0) doneflag = FALSE;
						}
					}
				}
			}

//			if (currlevel == NA_KRUL_LEVEL)
//				if (DRLG_PlaceMiniSet(NA_KRULS_ROOM, 1, 1, 0, 0, 0, -1, LVL_DOWN) < 0) 
//					doneflag = FALSE;
		}
	}



	// Fix stair trans
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if (dungeon[i][j] == 64) {
				xx = (i << 1) + DIRTEDGED2;
				yy = (j << 1) + DIRTEDGED2;
				DRLG_CopyTrans(xx, yy+1, xx, yy);
				DRLG_CopyTrans(xx+1, yy+1, xx+1, yy);
			}
		}
	}

	// Finish/fix trans
//	if (currlevel < CRYPTSTART)
		DRLG_L5TransFix();

	// Fix dirt transparency floor corners
//	if (currlevel < CRYPTSTART)
		DRLG_L5DirtFix();

//	if (currlevel < CRYPTSTART)
		DRLG_L5CornerFix();

	// Put in doors
//	if (currlevel < CRYPTSTART)		JKE Turn off crypt doors
		for (j = 0; j < MDMAXY; j++) {
			for (i = 0; i < MDMAXX; i++)
				if ((dflags[i][j] & SETP_MASK) != NODOOR) DRLG_PlaceDoor(i,j);
		}

	// Random subs
	if (currlevel < CRYPTSTART)		// These are to turn off stuff in the crypt JKE
		DRLG_L5Subs();
	else
	{

		Statues(L1STATUE);
		
		DRLG_L5PlaceRndSet(ARCHLEFT, 95);
		DRLG_L5PlaceRndSet(ARCHRIGHT, 95);
		DRLG_L5PlaceRndSet(GRATELEFT, 100);
		DRLG_L5PlaceRndSet(GRATERIGHT, 100);
		DRLG_L5PlaceRndSet(BIGFLOOR, 60);

		DRLG_L5Shadows();

		switch (currlevel)
		{
			case CRYPTSTART:
				BrokenWall1(L1BWALL1);
				BrokenWall2(L1BWALL2);
				BrokenWall3(L1BWALL3);
				DRLG_L5Shadows();
				NormalGrate(L1NGRATE);
				BrokenGrate(L1BGRATE);
				FloorRubble(L1RFLOOR);
				break;
		case CRYPTSTART + 1:
				NormalGrate(L2NGRATE);
				BrokenGrate(L2BGRATE);
				FloorRubble(L2RFLOOR);
				BrokenWall1(L2BWALL1);
				BrokenWall2(L2BWALL2);
				BrokenWall3(L2BWALL3);
				DRLG_L5Shadows();
				break;
		case CRYPTSTART + 2:
				NormalGrate(L3NGRATE);
				BrokenGrate(L3BGRATE);
				FloorRubble(L3RFLOOR);
				BrokenWall1(L3BWALL1);
				BrokenWall2(L3BWALL2);
				BrokenWall3(L3BWALL3);
				DRLG_L5Shadows();
				break;
		default:
				NormalGrate(L4NGRATE);
				BrokenGrate(L4BGRATE);
				FloorRubble(L4RFLOOR);
				BrokenWall1(L4BWALL1);
				BrokenWall2(L4BWALL2);
				BrokenWall3(L4BWALL3);
				DRLG_L5Shadows();
				break;
		}

	}


	// Create shadows
	if (currlevel < CRYPTSTART)
		DRLG_L1Shadows();
//	else
//		DRLG_L5Shadows();

	// Create mini set pieces
	if (currlevel < CRYPTSTART)
		DRLG_PlaceMiniSet(LAMPS, 5, 10, 0, 0, 0, -1, LVL_NODIR);

	// Floor subs
	if (currlevel < CRYPTSTART)
		DRLG_L1Floor();

//	if (currlevel >= CRYPTSTART)		// Place all our specifics here JKE
//	{
//		DRLG_L5Doors();
//	}


	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
	}

	DRLG_Init_Globals();
	// Check for any mini quest pieces
	DRLG_CheckQuests(setpc_x, setpc_y);

}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
/*
byte L5TileType(int t)
{
	return(BTYPES[t]);
}
*/

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
void CreateL5Dungeon(unsigned int rseed, int entry)
{
	int x,y;

	SetRndSeed(rseed);
	
	dminx = DIRTEDGED2;
	dminy = DIRTEDGED2;
	dmaxx = DMAXX - (DIRTEDGED2);
	dmaxy = DMAXY - (DIRTEDGED2);

	Na_Krul.x = 0;
	Na_Krul.y = 0;
	Na_Krul.Books = FALSE;
	Na_Krul.Open = FALSE;
	Na_Krul.LeverX = 0;
	Na_Krul.LeverY = 0;
	Na_Krul.Lever_Thrown = FALSE;
	Na_Krul.MIndex = 0;
	
	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL1SP();
	DRLG_L5(entry);
	DRLG_L1Pass3();
	DRLG_FreeL1SP();
	if (currlevel < HIVESTART)			// JKE NO TOPS!!!!!
		DRLG_InitL1Vals();
	else
		DRLG_L5Doors();
	DRLG_SetPC();
	
	for (y = dminy; y < dmaxy; ++y)
	{
		for (x = dminx; x < dmaxx; ++x)
		{
			if (dPiece[x][y] == 290)
			{
				Na_Krul.x = x;
				Na_Krul.y = y;
			}
			if (dPiece[x][y] == 317)
			{
				CornerStone.x = x;
				CornerStone.y = y;
			}
		}
	}
}
